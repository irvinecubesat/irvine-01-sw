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

/**
 * i2c expander registers
 **/
#define REG_INPUT_PORT  0x00
#define REG_OUTPUT_PORT 0x01
#define REG_CONFIG      0x03

namespace IrvCS
{
  /**
   * Guard i2c calls from getting called concurrently
   **/
  static Mutex gI2cAccessMutex;
  
  CCardI2CX::CCardI2CX():addr_(0x38), initialized_(false), enableTimer_(false),
                         isPoweredOn_(false)
  {
    int gpioInitStatus=initGpios();

    int powerStatus=0;

    if (0 != gpioInitStatus)
    {
      syslog(LOG_ERR, "Encountered %d errors initializing GPIO's", gpioInitStatus);
    }
    
    //
    // Power off 3V and 5V payload power for C-Card at startup
    //
    if (0 != setPayloadPower(pl5VGpio_, 0))
    {
      powerStatus++;
    }

    if (0 != setPayloadPower(pl3VGpio_,0))
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

    //
    // GPIO 0-2 -> PA26-28 -> offset by PIO_A_BASE 32 ---> 58, 59, 60
    // GPIO 4   -> PB16    -> offset by PIO_B_BASE 64 ---> 80
    //
    const int dsa1SensePin[2]={58,59}; // HW gpio 0, 1
    const int dsa2SensePin[2]={60,80}; // HW gpio 2, 4
    for (int i = 0; i < 2; i++)
    {
      if (!dsa1SenseGpio_[i].initialize(dsa1SensePin[i]))
      {
        syslog(LOG_ERR, "Unable to initialize GPIO %d", dsa1SensePin[i]);
      }

      if (!dsa2SenseGpio_[i].initialize(dsa2SensePin[i]))
      {
        syslog(LOG_ERR, "Unable to initialize GPIO %d", dsa2SensePin[i]);
      }
    }
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
    __s32 data=i2c_smbus_read_byte_data(i2cdev_, REG_INPUT_PORT);

    if (data < 0)
    {
      syslog(LOG_ERR, "Unable to read data from register %02x:  %s (%d)",
                REG_INPUT_PORT,strerror(data*-1), data);
      return --powerStatus;
    }

    // set register 3 to 0 to set all ports to output
    __s32 result=i2c_smbus_write_byte_data(i2cdev_, REG_CONFIG, 0x00);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to write data to register %02x:  %s (%d)",
                REG_CONFIG, strerror(result*-1), result);
      return --powerStatus;
    }

    if (reset() < 0)
    {
      syslog(LOG_ERR, "Unable to initialize state");
      return --powerStatus;
    }

    return powerStatus;
  }

  int CCardI2CX::idleCheck()
  {
    if (!isPoweredOn_ || ((time(NULL)-pwrTimestamp_) < C_CARD_IDLE_THRESHOLD))
    {
      return 0;
    }
    syslog(LOG_INFO, "Switching to Idle mode");
    if (0 != setPayloadPower(pl5VGpio_, 0))
    {
      return -1;
    }
    isPoweredOn_=false;

    return 1;
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
   *
   * @param id the id of the DSA
   * @param cmd the command to perform (Release or Deploy)
   * @return 0 if off
   * @return 1 if on
   * @return <0 if error
   **/
  int CCardI2CX::getSensorStatus(DsaId id, DsaCmd cmd)
  {
    bool successful=false;

    Gpio *senseArray=NULL;
    if (id == DSA_1)
    {
      senseArray=dsa1SenseGpio_;
    } else if (id == DSA_2)
    {
      senseArray=dsa2SenseGpio_;
    } else                      // invalid/unknown dsa value
    {
      syslog(LOG_WARNING, "Uknown DSA Id:  %d", id);
      return StatInvalidInput;
    }
    int gpioVal=senseArray[cmd].get();
    if (gpioVal < 0)
    {
      return gpioVal;
    }
    
    // Odd == 1, even == 0
    return gpioVal&1;
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
    int result=i2c_smbus_write_byte_data(i2cdev_, REG_OUTPUT_PORT, state);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to set register %02x with %02x:  %s (%d)",
                REG_OUTPUT_PORT, state, strerror(result*-1), result);
    } else
    {
      syslog(LOG_INFO, "CCardI2CX SET --> %02x", state);
    }
  }

  int CCardI2CX::setState(uint8_t state)
  {
    MutexLock lock(gI2cAccessMutex);
    int pwrStatus=powerOn();
    if (pwrStatus < 0)
    {
      return StatErr;
    }
    return setI2Cstate(state);
  }

  int CCardI2CX::getState(uint8_t &state)
  {
    MutexLock lock(gI2cAccessMutex);
    int pwrStatus=powerOn();
    if (pwrStatus < 0)
    {
      return StatErr;
    }

    // get output register state
    int result=i2c_smbus_read_byte_data(i2cdev_, REG_OUTPUT_PORT);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to get register %02x with %02x:  %s (%d)",
                REG_OUTPUT_PORT, state, strerror(result*-1), result);
      return result;
    }

    state=(uint8_t)(result&0xFF);
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
    int gpioDsa1Release=dsa1SenseGpio_[0].get();
    int gpioDsa1Deploy= dsa1SenseGpio_[1].get();
    int gpioDsa2Release=dsa2SenseGpio_[0].get();
    int gpioDsa2Deploy= dsa2SenseGpio_[1].get();
    setDeployState(DSA1_RELEASE_STATUS_BIT, gpioDsa1Release, deployState);
    setDeployState(DSA1_DEPLOY_STATUS_BIT, gpioDsa1Deploy, deployState);
    setDeployState(DSA2_RELEASE_STATUS_BIT, gpioDsa2Release, deployState);
    setDeployState(DSA2_DEPLOY_STATUS_BIT, gpioDsa2Deploy, deployState);
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

  int CCardI2CX::mtPerform(uint8_t idBits, uint8_t cmd)
  {
    uint8_t status=portState_.setMt(idBits, cmd);
    
    int setStatus=setState(status);
    if (0 != setStatus)
    {
      syslog(LOG_WARNING, "%s Unable to set expander value to %02x",
             __FILENAME__, status);
      return setStatus;
    }
    return status;
  }
  
  int CCardI2CX::dsaPerform(DsaId id, DsaCmd cmd, int timeoutSec)
  {
    int status=CC_OK;
    int dsaIndex=0;
    int timeCount=0;            // incremented every second we wait

    if (cmd == SetTimerOn)
    {
      syslog(LOG_INFO, "Timer --> Enabled");
      enableTimer_=true;
      return portState_.getState();
    } else if (cmd == SetTimerOff)
    {
      syslog(LOG_INFO, "Timer --> Disabled");
      enableTimer_=false;
      return portState_.getState();
    }

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
      "DSA_2",
      "",
      "DSA_1",
      "DSA_Unknown"
    };

    setPayloadPower(pl3VGpio_, 1);
    if (enableTimer_)
    {
      status=portState_.setDsa(id,SetTimer);
      int setStatus=setState(status);
      if (0 > setStatus)
      {
        syslog(LOG_WARNING,
               "%s Unable to set timer bit (%02x)", __FILENAME__, status);
        status=StatDeviceAccess;
        goto cleanup;
      }
    }

    //
    // Wait for DSA sense to change
    //
    syslog(LOG_INFO, "Waiting %d sec for %s Sensor to change", 
              timeoutSec, dsaIdStr[id]);

    while (true)
    {
      int sensorStatus = getSensorStatus(id, cmd);

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
    setPayloadPower(pl3VGpio_, 0);

    return status;
  }
}
