#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <sstream>
#include <syslog.h>

#include "Mutex.h"
#include "MutexLock.h"
#include "CCardI2CX.h"

namespace IrvCS
{
  /**
   * Guard i2c calls from getting called concurrently
   **/
  static Mutex gI2cAccessMutex;
  
  CCardI2CX::CCardI2CX():addr_(0x38), initialized_(false),
                         isPoweredOn_(false)
  {
    int gpioInitStatus=initGpios();

    int powerStatus=0;

    if (0 != gpioInitStatus)
    {
      syslog(LOG_ERR, "Encountered %d errors initializing GPIO's", gpioInitStatus);
    }

    //
    // For irvine-02, payload power is always turned on
    //

    if (0 != setPayloadPower(pl5VGpio_, 1))
    {
      powerStatus++;
    }

    if (0 != setPayloadPower(pl3VGpio_,1))
    {
      powerStatus++;
    }

    if (powerStatus == 0)
    {
      syslog(LOG_NOTICE, "%s Initialized", __FILENAME__);
      initialized_=true;
    } else
    {
      syslog(LOG_ERR, "%s:  Unable to initialize", __FILENAME__);
    }
  }

  int CCardI2CX::initGpios()
  {
    int errors=0;
    if (!pl3VGpio_.initialize(102, "PL 3V Pwr"))
    {
      syslog(LOG_ERR, "Unable to initialize 3V PL GPIO)");
      errors++;
    }

    if (!pl5VGpio_.initialize(103, "PL 5V Pwr"))
    {
      syslog(LOG_ERR, "Unable to initialize 5V PL GPIO)");
      errors++;
    }
    return errors;
  }

  CCardI2CX::~CCardI2CX()
  {
    syslog(LOG_NOTICE, "%s Cleaning up", __FILENAME__);
    // close any resources
    if (i2cdev_ >= 0)
    {
      close(i2cdev_);
    }
  }

  int CCardI2CX::powerOn()
  {
    int powerStatus=0;

    pwrTimestamp_=time(NULL);

    if (isPoweredOn_)
    {
      // Already on.
      syslog(LOG_DEBUG, "5V PL Pwr Access");
      return 0;
    }

    syslog(LOG_INFO, "Powering on C-Card");
    //
    // power on 5V payload for C-Card
    //
    if (0 != setPayloadPower(pl5VGpio_, 1))
    {
      return --powerStatus;
    }
    isPoweredOn_=true;

    const char *i2cbus="/dev/i2c-1"; 

    // initialize i2c bus
    syslog(LOG_INFO, "Initializing %s",i2cbus);
    i2cdev_=open(i2cbus, O_RDWR);

    if (i2cdev_<0)
    {
      syslog(LOG_ERR, "Unable to open i2c device %s:  %s (%d)",
                i2cbus, strerror(errno), errno);
      return --powerStatus;
    }
      
    if (ioctl(i2cdev_, I2C_SLAVE, addr_) < 0)
    {
      syslog(LOG_ERR, "Unable to initialize %02x on %s:  %s (%d)",
                addr_, i2cbus, strerror(errno), errno);
      return --powerStatus;
    }

    // Read input register to make sure we can access valid data
    int inputRegister=0;
    if (0 > (inputRegister=getRegister(RegInput)))
    {
      return --powerStatus;
    }

    __s32 result=i2c_smbus_write_byte_data(i2cdev_, RegConfig, DSA_CONFIG_PORTS);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to write data to register %02x:  %s (%d)",
                RegConfig, strerror(result*-1), result);
      return --powerStatus;
    }

    if (reset() < 0)
    {
      syslog(LOG_ERR, "Unable to initialize state");
      return --powerStatus;
    }

    return powerStatus;
  }

  /**
   * Perform the specified DSA command/operation.
   * @param id the id of the DSA
   * @param cmd the command to perform (Release or Deploy)
   * @param timeoutSec the timeout in seconds
   * @return StatOk if successful
   * @return OpStatus (<0)
   */
  OpStatus CCardI2CX::performDsaOperation(DsaId id, DsaCmd cmd, int timeoutSec)
  {
    if (cmd != Deploy && cmd != Release)
    {
      return StatInvalidInput;
    }
    
    int status=dsaPerform(id, cmd, timeoutSec);
    
    if (status < 0)
    {
      return static_cast<OpStatus>(status);
    }
    
    return StatOk;
  }

  /**
   * Get the sensor status for the given DSA/Cmd
   **/
  int CCardI2CX::getSensorStatus(DsaId id, DsaCmd cmd)
  {
    uint8_t statusBit=DSA_SENSE_OFFSET+id+((cmd==Release)?0:1);

    return (dsaSenseBits_<<statusBit) & 0x1;
  }

  /**
   * Get the status bits
   **/
  int CCardI2CX::getSensorStatusBits()
  {
    int retVal=getRegister(RegInput);

    if (0 > retVal)
    {
      return retVal;
    }
    
    return retVal & 0xFF;
  }

  int CCardI2CX::setPayloadPower(Gpio &pwrGpio, uint8_t onOrOff)
  {
    int retVal=0;
    const char *opString=(onOrOff==0?"Disabled":"Enabled");
    std::stringstream stm;
    stm<<pwrGpio;
    retVal=pwrGpio.set(onOrOff);
    if (retVal != 0)
    {
      syslog(LOG_ERR, "Unable to set %s to %d (%d)", stm.str().c_str(), onOrOff, 
                retVal);
    } else
    {
      syslog(LOG_INFO, "%s --> %s", stm.str().c_str(), opString);
    }
    return retVal;
  }

  int CCardI2CX::setI2Cstate(uint8_t state)
  {
    // set the initial output states
    int result=i2c_smbus_write_byte_data(i2cdev_, (int)RegOutput, state);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to set register %02x with %02x:  %s (%d)",
                RegOutput, state, strerror(result*-1), result);
    } else
    {
      syslog(LOG_INFO, "CCardI2CX SET --> %02x", state);
    }
    return result;
  }

  int CCardI2CX::setState(uint8_t state)
  {
    MutexLock lock(gI2cAccessMutex);

    return setI2Cstate(state);
  }

  int CCardI2CX::getRegister(const RegisterType regType)
  {
    MutexLock lock(gI2cAccessMutex);

    int result=i2c_smbus_read_byte_data(i2cdev_, regType);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to get register %02x:  %s (%d)",
                RegOutput, strerror(result*-1), result);
    }

    return result;
  }

  
  int CCardI2CX::getState(uint8_t &state)
  {
    int result=0;
    uint8_t outputRegister=0;

    outputRegister=getRegister(RegOutput);

    state=(uint8_t)(result&DSA_CMD_MASK);
    return 0;
  }

  static void setDeployState(int bitOffset, int gpioResult, uint8_t &deployState)
  {
    uint8_t stateBit=1<<bitOffset;
    if (gpioResult == 0)
    {
      deployState &= ~stateBit;
      return;
    } else if (gpioResult > 0)
    {
      deployState |= stateBit;
    } else // set  error bit which is in the first upper 2 bits.
    {
      deployState |= 1<<(bitOffset+4);
    }
  }

  uint8_t CCardI2CX::getDsaDeployState()
  {
    uint8_t deployState=0;
    int retrieveStatus=0;

    uint8_t inputRegister=0;
    inputRegister=getRegister(RegInput);
    inputRegister << DSA_SENSE_OFFSET;
    
    return deployState;
  }

  int8_t CCardI2CX::getStates(uint8_t &portState, uint8_t &deployState)
  {
    deployState=getDsaDeployState();
    return getState(portState);
  }
  
  bool CCardI2CX::isOk()
  {
    return initialized_;
  }

  int CCardI2CX::reset()
  {
    return setI2Cstate(portState_.reset());
  }

  int CCardI2CX::dsaPerform(DsaId id, DsaCmd cmd, int timeoutSec)
  {
    int status=CC_OK;
    int dsaIndex=0;
    int timeCount=0;            // incremented every second we wait

    syslog(LOG_INFO, "Performing DSA-%d %s", 
              id==DSA_1?1:2, cmd == Release?"Release":"Deploy");

    // first reset everything
    portState_.reset();
    status=portState_.setDsa(id, cmd);
    
    int setStatus=setState(status);
    if (0 < setStatus)
    {
      syslog(LOG_WARNING, "%s Unable to set expander value to %02x",
             __FILENAME__, status);
      return setStatus;
    }

    if (cmd != Deploy && cmd != Release)
    {
      return StatInvalidInput;
    }
    
    // make sure this matches the DsaId enumeration
    const char *dsaIdStr[]={
      "DSA_1",
      "",
      "DSA_2",
      "DSA_Unknown"
    };

    //
    // Wait for DSA sense to change
    //
    syslog(LOG_INFO, "Waiting %d sec for %s Sensor to change", 
              timeoutSec, dsaIdStr[id]);

    while (true)
    {
      int retrieveStatus= getSensorStatusBits();
      if (0 > retrieveStatus)
      {
        syslog(LOG_ERR, "Unable to retrive sensor status");
        break;
      }
      dsaSenseBits_=retrieveStatus&0xFF;

      int sensorStatus=getSensorStatus(id, cmd);
      
      if (cmd == Release && sensorStatus == 1)
      {
        syslog(LOG_INFO, "Released %s at %d sec", dsaIdStr[id], timeCount+1);
        break;
      } else if (cmd == Deploy && sensorStatus == 1)
      {
        syslog(LOG_INFO, "Deployed %s at %d sec", dsaIdStr[id], timeCount+1);
        break;
      } else if (timeCount++ > timeoutSec)
      {
        syslog(LOG_WARNING, "%s operation timed out after %d sec",
                  dsaIdStr[id], timeCount);
        status=StatTimeOut;
        break;
      }
      sleep(1);
    }
  cleanup:
    reset();

    return status;
  }
}
