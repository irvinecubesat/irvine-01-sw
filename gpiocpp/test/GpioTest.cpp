#include <gtest/gtest.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fstream>

#define GPIO_BASE "gpio/"
#include "../Gpio.cpp"

using namespace IrvCS;

/**
 * Text fixture for this test.  Each TEST_F creates and destroys this fixture.
 **/
class GpioEnv: public ::testing::Test
{
protected:
  virtual void SetUp()
    {
      openlog("GpioTest", LOG_PERROR, LOG_USER);
      int status=system("mkdir -p " GPIO_BASE);
      if (0 !=status)
      {
        printf("Status %d creating %s\n", WEXITSTATUS(status),
               GPIO_BASE);
      }

      // make the gpio100 directory
      
      status=system("mkdir -p " GPIO_BASE "gpio100");
      if (0 !=status)
      {
        printf("Status %d creating %s\n", WEXITSTATUS(status),
               GPIO_BASE "gpio100");
      }
}
  
  virtual void TearDown()
    {
      int status=system("rm -rf " GPIO_BASE);
      if (status != 0)
      {
        printf("Status %d encountered removing %s",
               WEXITSTATUS(status), GPIO_BASE);
      }
    }
};

TEST_F(GpioEnv, TestInit)
{
  Gpio gpio(100);

  ASSERT_EQ(true, gpio.initialized());
  struct stat statBuf={0};

  ASSERT_EQ(0, stat(GPIO_BASE, &statBuf));
}

TEST_F(GpioEnv, TestSet)
{
  Gpio gpio(100);

  ASSERT_EQ(0, gpio.set(1));
  
  struct stat statBuf={0};

  ASSERT_EQ(0, stat(GPIO_BASE "gpio100/value", &statBuf));

  // verify the gpio value
  uint8_t value;
  std::ifstream ifs(GPIO_BASE "gpio100/value", std::ifstream::in);
  ifs>>value;
  ASSERT_EQ('1', value);
}

/**
 * Test sets, then gets
 **/
TEST_F(GpioEnv, TestGet)
{
  Gpio gpio(100);

  ASSERT_EQ(0, gpio.set(1));
  ASSERT_EQ(1, gpio.get());
  ASSERT_EQ(0, gpio.set(0));
  ASSERT_EQ(0, gpio.get());
}

