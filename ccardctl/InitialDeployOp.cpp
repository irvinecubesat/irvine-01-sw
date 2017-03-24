#include <unistd.h>
#include <syslog.h>
#include "InitialDeployOp.h"

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
    syslog(LOG_INFO, "Executing Initial DSA Release/Deploy Operation");
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
    syslog(LOG_ERR, "IMPLEMENT ME");
    
    for (int i = 0; i < 10;i++)
    {
      syslog(LOG_INFO, "Simulated processing... count=%d", i);
      sleep(1);
    }

    syslog(LOG_INFO, "Completed Initial Deploy Operation with status %d", status);

    return status;
  }
}
