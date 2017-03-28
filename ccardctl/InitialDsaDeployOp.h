#ifndef __INITIAL_DEPLOY_OP_HH__
#define __INITIAL_DEPLOY_OP_HH__

#include "DsaController.h"
#include "ccardDefs.h"

namespace IrvCS
{
  /**
   *
   **/
  class InitialDsaDeployOp
  {
  public:
    InitialDsaDeployOp(DsaController *controller);

    ~InitialDsaDeployOp();

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
