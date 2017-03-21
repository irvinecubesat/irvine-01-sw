#ifndef __INITIAL_DEPLOY_OP_HH__
#define __INITIAL_DEPLOY_OP_HH__

#include "DsaController.h"
#include "ccardDefs.h"

#define RELEASE_TIMEOUT 45
#define EMERGENCY_RELEASE_TIMEOUT 60
#define DSA_TIMEOUT 10

namespace IrvCS
{
  /**
   *
   **/
  class InitialDeployOp
  {
  public:
    InitialDeployOp(DsaController *controller);

    ~InitialDeployOp();

    /**
     * Execute the operation
     * @return OpStatus
     **/
    OpStatus execute();

  private:
    OpStatus release(DsaId id);
    OpStatus deploy(DsaId id);
    DsaController *controller_;
  };
}
#endif
