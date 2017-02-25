#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <syslog.h>
#include <linux/i2c-dev.h>

#include "CCardI2CX.h"

/**
 * i2c expander registers
 **/
#define REG_INPUT_PORT  0x00
#define REG_OUTPUT_PORT 0x01
#define REG_CONFIG      0x03

namespace IrvCS
{
  CCardI2CX::CCardI2CX(uint8_t state):addr_(0x38), initialized_(false)
  {
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

    if (setState(state))
    {
      syslog(LOG_ERR, "Unable to set the initial state");
    }

    syslog(LOG_NOTICE, "%s Initialized", __FILE__);
    initialized_=true;
  }
  
  CCardI2CX::~CCardI2CX()
  {
    syslog(LOG_NOTICE, "%s Cleaning up", __FILE__);
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
      syslog(LOG_ERR, "Unable to set register %02x with %02x:  %s (%d)",
             REG_OUTPUT_PORT, state, strerror(result*-1), result);
    }
    return result;
  }

  int CCardI2CX::getState(uint8_t &state)
  {
    // set the initial output states
    int result=i2c_smbus_read_byte_data(i2cdev_, REG_OUTPUT_PORT);
    if (result < 0)
    {
      syslog(LOG_ERR, "Unable to set register %02x with %02x:  %s (%d)",
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
}
