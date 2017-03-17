#include <InitialDeployOp.h>
#include <gtest/gtest.h>

using namespace IrvCS;

//
// @TODO add unit tests to test timing, retries, emergency release, etc.
//
// @TODO use fixture for test controller
//

TEST(InitialDeployOp,executeSuccess)
{
  class TestController:public DsaController
  {
  public:
    TestController(){}
    
    OpStatus performDsaOperation(DsaId id, DsaCmd cmd, int timeoutSec)
      {
        return StatOk;
      }

    int getSensorStatus(DsaId id, DsaCmd cmd)
      {
        return 1;
      }

  };

  TestController controller;

  InitialDeployOp op(&controller);

  ASSERT_EQ(StatOk, op.execute());
}

TEST(InitialDeployOp,executeTimeout)
{
  class TestController:public DsaController
  {
  public:
    TestController(){}
    
    OpStatus performDsaOperation(DsaId id, DsaCmd cmd, int timeoutSec)
      {
        return StatTimeOut;
      }
    int getSensorStatus(DsaId id, DsaCmd cmd)
      {
        return StatTimeOut;
      }
  };

  TestController controller;

  InitialDeployOp op(&controller);

  ASSERT_EQ(StatTimeOut, op.execute());
}
