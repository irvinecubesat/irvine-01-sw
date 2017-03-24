#include "InitialDeployOp.h"
#include <polysat/debug.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

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

  OpStatus InitialDeployOp::release(DsaId id){
    //Release 3 times, 45 seconds timeout each, return if successful
    for (int i = 0; i < 3; i++) {

      DBG_print(LOG_INFO, "Attempt %d", (i+1));
      int relStat = controller_ ->
      performDsaOperation(id, Release, RELEASE_TIMEOUT);
      if(relStat == StatOk){
        DBG_print(LOG_INFO, "Success");
        return StatOk;
      }
      DBG_print(LOG_INFO, "Failure");
      sleep(5);
    }

    //Emergency Release of 45+15 seconds

      DBG_print(LOG_INFO, "Attempting Emergency Release");
    OpStatus relStat = controller_ ->
    performDsaOperation(id, Release, EMERGENCY_RELEASE_TIMEOUT);
    if(relStat == StatOk){
      DBG_print(LOG_INFO, "Success");
      return StatOk;
    }

    DBG_print(LOG_INFO, "Failure");
    return relStat;
  }

  OpStatus InitialDeployOp::deploy(DsaId id){
    OpStatus status = controller_->performDsaOperation(id, Deploy, DSA_TIMEOUT);
    if(status == StatOk){
      DBG_print(LOG_INFO, "Success");
      return StatOk;
    }else{
      DBG_print(LOG_INFO, "Failure");
      return status;
    }
  }

  OpStatus InitialDeployOp::execute()
  {
    DBG_print(LOG_INFO, "Executing Initial Deploy Operation");
    OpStatus status=StatOk;


        // Release DSA1 3 times with timeout 45 sec
        // Emergency release additional 15 sec if necessary
    DBG_print(LOG_INFO, "Attempting to release DSA 1");
    OpStatus rel1 = release(DSA_1);
    sleep(5);
        // Release DSA2 3 times with timeout 45 sec
        // Emergency release 15 sec if necessary
    DBG_print(LOG_INFO, "Attempting to release DSA 2");
    OpStatus rel2 = release(DSA_2);
    sleep(5);

        // Deploy DSA1 timeout 10 sec
    DBG_print(LOG_INFO, "Attempting to deploy DSA 1");
    OpStatus dep1 = deploy(DSA_1);
    sleep(5);

        // Deploy DSA2 timeout 10 sec
    DBG_print(LOG_INFO, "Attempting to deploy DSA 2");
    OpStatus dep2 = deploy(DSA_2);
    sleep(5);

    if(dep2!=StatOk || dep1!=StatOk || rel1!=StatOk || rel2!=StatOk){
      status = StatTimeOut;
    }

    DBG_print(LOG_INFO, "Completed Initial Deploy Operation with status %d", status);

    return status;
  }
}
