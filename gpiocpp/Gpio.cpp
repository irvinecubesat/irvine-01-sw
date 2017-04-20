#include "Gpio.h"
#include <syslog.h>
#include <fstream>

//
// Utilize the user space for gpio
//

// allow GPIO_BASE to be overridden for unit test
#ifndef GPIO_BASE
#define GPIO_BASE "/sys/class/gpio/"
#endif

#define MAX_GPIO_PATH 128
#define GPIO_EXPORT GPIO_BASE"export"
#define GPIO_UNEXPORT GPIO_BASE"unexport"
#define GPIO_NODE GPIO_BASE "gpio%d"
#define GPIO_DIRECTION GPIO_NODE "/direction"
#define GPIO_VALUE GPIO_NODE "/value"

namespace IrvCS
{
  enum GpioDirection
  {
    In=0,
    Out=1
  };
  

  Gpio::Gpio():gpio_(-1),label_("")
  {
  }

  Gpio::Gpio(int16_t gpio, const std::string &label):label_(label)
  {
    if (!initialize(gpio))
    {
      syslog(LOG_ERR, "Unable to initialize %d", gpio);
    }
  }

  Gpio::~Gpio()
  {
    //
    // export the gpio
    //
    std::ofstream os(GPIO_UNEXPORT, std::ofstream::out);

    os<<gpio_;

    if (os.fail())
    {
      syslog(LOG_ERR, "Unable to write %d to %s", gpio_, GPIO_UNEXPORT);
    }
  }

  bool Gpio::initialize(int16_t gpio, const std::string &label)
  {
    bool retVal=false;
    label_=label;

    //
    // export the gpio
    //
    std::ofstream os(GPIO_EXPORT, std::ios::out);

    os<<gpio;

    if (os.fail())
    {
      syslog(LOG_ERR, "Unable to write %d to %s", gpio, GPIO_EXPORT);
      goto fail;
    }
    gpio_=gpio;
    return true;
    
    fail:
    gpio_=-1;
    return false;
  }

  /**
   * Print out the GPIO info
   **/
  std::ostream& operator<<(std::ostream &os, const Gpio &gpio)
  {
    return os <<gpio.label_<<"("<<gpio.gpio_<<")";
  }

  bool Gpio::initialized()
  {
    return gpio_>= 0;
  }
  
  static int8_t setDirection(uint8_t gpio, GpioDirection dir)
  {
    char outBuf[MAX_GPIO_PATH]={0};
    snprintf(outBuf, MAX_GPIO_PATH-1, GPIO_DIRECTION, gpio);
    std::ofstream dirOs(outBuf, std::ofstream::out);
    if (dir == In)
    {
      dirOs <<"in";
    } else
    {
      dirOs << "out";
    }
    
    if (dirOs.fail())
    {
      syslog(LOG_ERR, "Unable to set direction to %d for %d via %s", dir, gpio,
             outBuf);
      return -1;
    }
    return 0;
  }
  
  int8_t Gpio::set(uint8_t val)
  {
    char outBuf[MAX_GPIO_PATH]={0};
    snprintf(outBuf, MAX_GPIO_PATH-1, GPIO_VALUE, gpio_);
    std::ofstream valOs(outBuf, std::ofstream::out);

    // set means using direction output, then writing to the gpio value
    if (0 != setDirection(gpio_, Out))
    {
      goto fail;
    }

    if (0 == val)
    {
      valOs<<"0";
    } else
    {
      valOs<<"1";
    }
    
    if (valOs.fail())
    {
      syslog(LOG_ERR, "Unable to write %d to %s", val, outBuf);
      goto fail;
    }
    
    return 0;
  fail:
    return -1;
  }

  int8_t Gpio::get()
  {
    int8_t outVal;
    char readVal;
    char outBuf[MAX_GPIO_PATH]={0};
    snprintf(outBuf, MAX_GPIO_PATH-1, GPIO_VALUE, gpio_);
    std::ifstream valIs(outBuf, std::ios::out);
    
    // set means using direction input, then reading the gpio value
    if (0 != setDirection(gpio_, In))
    {
      goto fail;
    }

    valIs>>readVal;

    if (valIs.fail())
    {
      syslog(LOG_ERR, "Unable to read from %s", outBuf);
      goto fail;
    }
    if (readVal=='0')
    {
      outVal=0;
    } else
    {
      outVal=1;
    }
    return outVal;
  fail:
    return -1;
  }


}
