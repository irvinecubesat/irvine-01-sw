#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <polysat/debug.h>

#include "CCardI2CX.h"

/**
 * i2c expander registers
 **/
#define REG_INPUT_PORT  0x00
#define REG_OUTPUT_PORT 0x01
#define REG_CONFIG      0x03

#define DBG_LEVEL_DEBUG DBG_LEVEL_ALL

namespace IrvCS
{
  CCardI2CX::CCardI2CX():addr_(0x38), initialized_(false), enableTimer_(true)
  {
    if (0 != pl3VGpio_.initialize(102))
    {
      DBG_print(LOG_ERR, "Unable to initialize 3V PL GPIO)");
    }

    if (0 != pl5VGpio_.initialize(103))
    {
      DBG_print(LOG_ERR, "Unable to initialize 5V PL GPIO)");
    }
    //
    // power on 5V payload for C-Card
    //
    if (0 != pl5VGpio_.set(1))
    {
      DBG_print(LOG_ERR, "Unable to turn on 5V PL Power");
    }

    if (0 != enable3VPayload(0))
    {
      DBG_print(LOG_ERR, "Unable to power off 3V PL Power");
    }

    //
    // GPIO 0-2 -> PA26-28 -> offset by PIO_A_BASE 32 ---> 58, 59, 60
    // GPIO 4   -> PB16    -> offset by PIO_B_BASE 64 ---> 80
    //
    const int dsa1SensePin[2]={58,59}; // HW gpio 0, 1
    const int dsa2SensePin[2]={60,80}; // HW gpio 2, 4
    for (int i = 0; i < 2; i++)
    {
      if (0 != dsa1SenseGpio_[i].initialize(dsa1SensePin[i]))
      {
        DBG_print(LOG_ERR, "Unable to initialize GPIO %d", dsa1SensePin[i]);
      }

      if (0 != dsa2SenseGpio_[i].initialize(dsa2SensePin[i]))
      {
        DBG_print(LOG_ERR, "Unable to initialize GPIO %d", dsa2SensePin[i]);
      }
    }

    const char *i2cbus="/dev/i2c-1"; 

    // initialize i2c bus
    DBG_print(LOG_INFO, "Initializing %s",i2cbus);
    i2cdev_=open(i2cbus, O_RDWR);

    if (i2cdev_<0)
    {
      DBG_print(LOG_ERR, "Unable to open i2c device %s:  %s (%d)",
                i2cbus, strerror(errno), errno);
      return;
    }
      
    if (ioctl(i2cdev_, I2C_SLAVE, addr_) < 0)
    {
      DBG_print(LOG_ERR, "Unable to initialize %02x on %s:  %s (%d)",
                addr_, i2cbus, strerror(errno), errno);
      return;
    }

    // Read input register to make sure we can access valid data
    __s32 data=i2c_smbus_read_byte_data(i2cdev_, REG_INPUT_PORT);

    if (data < 0)
    {
      DBG_print(LOG_ERR, "Unable to read data from register %02x:  %s (%d)",
                REG_INPUT_PORT,strerror(data*-1), data);
      return;
    }

    // set register 3 to 0 to set all ports to output
    __s32 result=i2c_smbus_write_byte_data(i2cdev_, REG_CONFIG, 0x00);
    if (result < 0)
    {
      DBG_print(LOG_ERR, "Unable to write data to register %02x:  %s (%d)",
                REG_CONFIG, strerror(result*-1), result);
      return;
    }

    if (reset() < 0)
    {
      DBG_print(LOG_ERR, "Unable to initialize state");
      return;
    }
    
    DBG_print(LOG_NOTICE, "%s Initialized", __FILENAME__);
    initialized_=true;
  }

  CCardI2CX::~CCardI2CX()
  {
    DBG_print(LOG_NOTICE, "%s Cleaning up", __FILENAME__);
    // close any resources
    if (i2cdev_ >= 0)
    {
      close(i2cdev_);
    }
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
    if (cmd != Deploy || cmd != Release)
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
      DBG_print(LOG_WARNING, "Uknown DSA Id:  %d", id);
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

  int CCardI2CX::enable3VPayload(uint8_t onOrOff)
  {
    int retVal=0;
    const char *opString=(onOrOff==0?"Disabled":"Enabled");
    retVal=pl3VGpio_.set(onOrOff);
    if (retVal != 0)
    {
      DBG_print(LOG_ERR, "Unable to set 3V power to %d - status", onOrOff, 
                retVal);
    } else
    {
      DBG_print(LOG_INFO, "3.3V Payload --> %s", opString);
    }
    return retVal;
  }

  int CCardI2CX::setState(uint8_t state)
  {
    // set the initial output states
    int result=i2c_smbus_write_byte_data(i2cdev_, REG_OUTPUT_PORT, state);
    if (result < 0)
    {
      DBG_print(LOG_ERR, "Unable to set register %02x with %02x:  %s (%d)",
                REG_OUTPUT_PORT, state, strerror(result*-1), result);
    } else
    {
      DBG_print(LOG_INFO, "CCardI2CX SET --> %02x", state);
    }
    return result;
  }

  int CCardI2CX::getState(uint8_t &state)
  {
    // get output register state
    int result=i2c_smbus_read_byte_data(i2cdev_, REG_OUTPUT_PORT);
    if (result < 0)
    {
      DBG_print(LOG_ERR, "Unable to get register %02x with %02x:  %s (%d)",
                REG_OUTPUT_PORT, state, strerror(result*-1), result);
      return result;
    }

    state=(uint8_t)(result&0xFF);
    return 0;
  }

  static void setDeployState(int bitOffset, int gpioResult, uint8_t &deployState)
  {
    int offset=0;
    if (gpioResult == 0)
    {
      return;
    }
    if (gpioResult < 0)
    {
      offset=4;
    }
    deployState |= 1<<(bitOffset+offset);
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
  
  bool CCardI2CX::isOk()
  {
    return initialized_;
  }

  int CCardI2CX::reset()
  {
    return setState(portState_.reset());
  }

  int CCardI2CX::mtPerform(uint8_t idBits, uint8_t cmd)
  {
    uint8_t status=portState_.setMt(idBits, cmd);
    
    int setStatus=setState(status);
    if (0 != setStatus)
    {
      DBG_print(DBG_LEVEL_WARN, "%s Unable to set expander value to %02x",
                status);
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
      DBG_print(LOG_INFO, "Timer --> Enabled");
      enableTimer_=true;
      return portState_.getState();
    } else if (cmd == SetTimerOff)
    {
      DBG_print(LOG_INFO, "Timer --> Disabled");
      enableTimer_=false;
      return portState_.getState();
    }
    // first reset everything
    portState_.reset();
    status=portState_.setDsa(id, cmd);
    
    int setStatus=setState(status);
    if (0 < setStatus)
    {
      DBG_print(DBG_LEVEL_WARN, "%s Unable to set expander value to %02x",
                status);
      return setStatus;
    }

    if (cmd != Deploy || cmd != Release)
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

    enable3VPayload(1);
    if (enableTimer_)
    {
      status=portState_.setDsa(id,SetTimer);
      int setStatus=setState(status);
      if (0 > setStatus)
      {
        DBG_print(DBG_LEVEL_WARN,
                  "%s Unable to set timer bit (%02x)", status);
        status=StatDeviceAccess;
        goto cleanup;
      }
    }

    //
    // Wait for DSA sense to change
    //
    DBG_print(LOG_INFO, "Waiting %d sec for %s Sensor to change", 
              timeoutSec, dsaIdStr[id]);

    while (true)
    {
      int sensorStatus = getSensorStatus(id, cmd);

      if (cmd == Release && sensorStatus == 1)
      {
        DBG_print(LOG_INFO, "Released %s at %d sec", dsaIdStr[id], timeCount+1);
        break;
      } else if (cmd == Deploy && sensorStatus == 1)
      {
        DBG_print(LOG_INFO, "Deployed %s at %d sec", dsaIdStr[id], timeCount+1);
        break;
      } else if (timeCount++ > timeoutSec)
      {
        DBG_print(LOG_WARNING, "%s operation timed out after %d sec",
                  dsaIdStr, timeCount);
        status=StatTimeOut;
        break;
      }
      sleep(1);
    }
  cleanup:
    reset();
    enable3VPayload(0);

    return status;
  }
}
