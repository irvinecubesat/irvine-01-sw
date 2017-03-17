#include "InitialDeployOp.h"
#include <polysat/debug.h>

namespace IrvCS
{
  //
  // @TODO Add implementation and unit tests
  //
  InitialDeployOp::InitialDeployOp(DsaController *controller):controller_(controller)
  {
  }

  InitialDeployOp::~InitialDeployOp()
  {
  }

  OpStatus InitialDeployOp::execute()
  {
    DBG_print(LOG_INFO, "Executing Initial Deploy Operation");
    OpStatus status=StatErr;

    //@TODO Implement
    //
    // Release DSA1 3 times with timeout 45 sec
    // Emergency release additional 15 sec if necessary
    // Release DSA2 3 times with timeout 45 sec
    // Emergency release 15 sec if necessary
    // 
    // Deploy DSA1 timeout 10 sec
    // Deploy DSA2 timeout 10 sec
    //
    // Sleep 5 seconds between each operation
    //
    DBG_print(LOG_ERR, "IMPLEMENT ME");
  
    DBG_print(LOG_INFO, "Completed Initial Deploy Operation with status %d", status);

    return status;
  }
}
