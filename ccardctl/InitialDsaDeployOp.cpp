#include <unistd.h>
#include <syslog.h>
#include "InitialDsaDeployOp.h"

namespace IrvCS
{
  //
  // @TODO Add implementation and unit tests
  //
  InitialDsaDeployOp::InitialDsaDeployOp(DsaController *controller):controller_(controller)
  {
  }

  InitialDsaDeployOp::~InitialDsaDeployOp()
  {
  }

  OpStatus InitialDsaDeployOp::release(DsaId id){
    //Release 3 times, 45 seconds timeout each, return if successful
    for (int i = 0; i < 3; i++) {

      syslog(LOG_INFO, "Attempt %d", (i+1));
      int relStat = controller_ ->
        performDsaOperation(id, Release, DSA_RELEASE_TIMEOUT);
      if(relStat == StatOk){
        syslog(LOG_INFO, "Success");
        return StatOk;
      }
      syslog(LOG_WARNING, "Timed out");
      sleep(DSA_RELEASE_WAIT);
    }

    syslog(LOG_INFO, "Attempting Emergency Release");
    OpStatus relStat = controller_ ->
      performDsaOperation(id, Release, DSA_EMERGENCY_RELEASE_TIMEOUT);
    if(relStat == StatOk){
      syslog(LOG_INFO, "Success");
      return StatOk;
    }

    syslog(LOG_WARNING, "Timed out");
    return relStat;
  }

  OpStatus InitialDsaDeployOp::deploy(DsaId id){
    OpStatus status = controller_->performDsaOperation(id, Deploy,
                                                       DSA_DEPLOY_TIMEOUT);
    if(status == StatOk){
      syslog(LOG_INFO, "Success");
      return StatOk;
    }else{
      syslog(LOG_WARNING, "Timed out");
      return status;
    }
  }

  OpStatus InitialDsaDeployOp::execute()
  {
    syslog(LOG_INFO, "Executing Initial DSA Release/Deploy Operation");
    OpStatus status=StatErr;

    // Release DSA1 3 times with timeout 45 sec
    // Emergency release additional 15 sec if necessary
    syslog(LOG_INFO, "Attempting to release DSA 1");
    OpStatus rel1 = release(DSA_1);

    // Release DSA2 3 times with timeout 45 sec
    // Emergency release 15 sec if necessary
    syslog(LOG_INFO, "Attempting to release DSA 2");
    OpStatus rel2 = release(DSA_2);
    sleep(DSA_OP_WAIT);

    // Deploy DSA1 timeout 10 sec
    syslog(LOG_INFO, "Attempting to deploy DSA 1");
    OpStatus dep1 = deploy(DSA_1);
    sleep(DSA_OP_WAIT);

    // Deploy DSA2 timeout 10 sec
    syslog(LOG_INFO, "Attempting to deploy DSA 2");
    OpStatus dep2 = deploy(DSA_2);
    sleep(DSA_OP_WAIT);

    if (dep1+dep2+rel1+rel2 == 0)
    {
      status=StatOk;
    } else if(dep2!=StatOk || dep1!=StatOk || rel1!=StatOk || rel2!=StatOk)
    {
      status = StatTimeOut;
    } 

    if (status == StatOk)
    {
      syslog(LOG_INFO, "Successfully Completed Initial DSA Deploy Operation",
             status);
    } else
    {
      syslog(LOG_WARNING, "Completed Initial DSA Deploy Operation with status %d",
             status);
    }
    

    return status;
  }
}
