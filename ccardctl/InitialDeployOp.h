#ifndef __INITIAL_DEPLOY_OP_HH__
#define __INITIAL_DEPLOY_OP_HH__

#include "DsaController.h"
#include "ccardDefs.h"

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
    DsaController *controller_;
  };
}
#endif
