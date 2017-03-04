#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <syslog.h>
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
  CCardI2CX::CCardI2CX():addr_(0x38), initialized_(false)
  {

    // initialize GPIO's
    const int pl5Vpin=103;
    if (0 != initGPIO(0, pl5Vpin, &pl5VGpio_))
    {
      DBG_print(LOG_ERR, "Unable to initialize 5V PL Power gpio %d\n", pl5Vpin);
    }

    //
    // power on 5V payload
    //
    if (0 != setGPIO(&pl5VGpio_, OUT, 1))
    {
      DBG_print(LOG_ERR, "Unable to turn on 5V PL Power\n", pl5Vpin);
    }
    
    const int dsa1SensePin[2]={58,59}; // HW gpio 0, 1
    const int dsa2SensePin[2]={60,62}; // HW gpio 2,4
    for (int i = 0; i < 2; i++)
    {
      if (0 != initGPIO(0, dsa1SensePin[i], &(dsa1SenseGpio_[i])))
      {
        DBG_print(LOG_ERR, "Unable to initialize GPIO %d", dsa1SensePin[i]);
      }
      if (0 != setGPIO(&(dsa1SenseGpio_[i]), IN, 0))
      {
        DBG_print(LOG_ERR, "Unable to set GPIO %d as input", dsa1SensePin[i]);
      }   
          
      if (0 != initGPIO(0, dsa2SensePin[i], &(dsa2SenseGpio_[i])))
      {
        DBG_print(LOG_ERR, "Unable to initialize GPIO %d", dsa2SensePin[i]);
      }

      if (0 != setGPIO(&(dsa2SenseGpio_[i]), IN, 0))
      {
        DBG_print(LOG_ERR, "Unable to set GPIO %d as input", dsa2SensePin[i]);
      }   
    }

    const char *i2cbus="/dev/i2c-1"; 

    // initialize i2c bus
    syslog(LOG_INFO, "Initializing %s",i2cbus);
    i2cdev_=open(i2cbus, O_RDWR);

    if (i2cdev_<0)
    {
      syslog(LOG_ERR, "Unable to open i2c device %s:  %s (%d)",
             i2cbus, strerror(errno), errno);
      return;
    }
      
    if (ioctl(i2cdev_, I2C_SLAVE, addr_) < 0)
    {
      syslog(LOG_ERR, "Unable to initialize %02x on %s:  %s (%d)",
             addr_, i2cbus, strerror(errno), errno);
      return;
    }

    // Read input register to make sure we can access valid data
    __s32 data=i2c_smbus_read_byte_data(i2cdev_, REG_INPUT_PORT);

    if (data < 0)
    {
      syslog(LOG_ERR, "Unable to read data from register %02x:  %s (%d)",
             REG_INPUT_PORT,strerror(data*-1), data);
      return;
    }

    // set register 3 to 0 to set all ports to output
    __s32 result=i2c_smbus_write_byte_data(i2cdev_, REG_CONFIG, 0x00);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to write data to register %02x:  %s (%d)",
             REG_CONFIG, strerror(result*-1), result);
      return;
    }

    if (reset() < 0)
    {
      DBG_print(LOG_ERR, "Unable to initialize state");
      return;
    }
    
    syslog(LOG_NOTICE, "%s Initialized", __FILENAME__);
    initialized_=true;
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
      syslog(LOG_ERR, "Unable to get register %02x with %02x:  %s (%d)",
             REG_OUTPUT_PORT, state, strerror(result*-1), result);
      return result;
    }

    state=(uint8_t)(result&0xFF);
    return 0;
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

    status=portState_.setDsa(id, cmd);
    
    status=setState(status);
    if (0 != status)
    {
      DBG_print(DBG_LEVEL_WARN, "%s Unable to set expander value to %02x", status);
      return status;
    }

    if (cmd == Deploy || cmd == Release)
    {
      portState_.setDsa(id, SetTimer);

      status=portState_.setDsa(id,SetTimer);
      int setStatus=setState(status);
      if (0 > setStatus)
      {
        DBG_print(DBG_LEVEL_WARN, "%s Unable to set expander value to %02x", status);
        return setStatus;
      }
      //
      // Wait for DSA sense to change
      //
      const char *dsaIdStr=NULL;
      gpio *senseArray=NULL;
      if (id == DSA_1)
      {
        senseArray=dsa1SenseGpio_;
        dsaIdStr="DSA_1";
      } else if (id == DSA_2)
      {
        senseArray=dsa2SenseGpio_;
        dsaIdStr="DSA_2";
      } else
      {
        DBG_print(LOG_ERR, "Invalid DSA ID:  %d", id);
        return -1;
      }

      DBG_print(LOG_INFO, "Waiting for %s Sensor to change", dsaIdStr);

      while (true)
      {
        int gpioRelease=readGPIO(&(senseArray[0]));
        int gpioDeploy=readGPIO(&(senseArray[1]));
        // check both for now since we're not sure which pin is which
        if (cmd == Release && (gpioRelease>0 || gpioDeploy>1))
        {
          DBG_print(LOG_INFO, "Released %s at %d sec (%d,%d)", dsaIdStr, timeCount+1,
                    gpioRelease, gpioDeploy);
          break;
        } else if (gpioRelease>0 && gpioDeploy>0)
        {
          DBG_print(LOG_INFO, "Deployed %s at %d sec", dsaIdStr, timeCount+1);
          break;
        } else if (timeCount++ > timeoutSec)
        {
          DBG_print(LOG_WARNING, "%s operation timed out after %d sec (%d,%d)",
                    dsaIdStr, timeCount, gpioRelease, gpioDeploy);
          break;
        }
        sleep(1);
      }
      reset();
    }
    return status;
  }
}
