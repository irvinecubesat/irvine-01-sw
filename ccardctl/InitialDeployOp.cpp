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

  OpStatus release(DsaId id){
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
      Sleep(5);
    }

    //Emergency Release of 45+15 seconds

      DBG_print(LOG_INFO, "Attempting Emergency Release");
    int relStat = controller_ ->
    performDsaOperation(id, Release, EMERGENCY_RELEASE_TIMEOUT);
    if(relStat == StatOk){
      DBG_print(LOG_INFO, "Success");
      return StatOk;
    }

    DBG_print(LOG_INFO, "Failure");
    return relStat;
  }

  OpStatus deploy(DsaId id){
    OpStatus status = controller_->performDsaOperation(id, Deploy, DSA_TIMEOUT)
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
    Sleep(5);
        // Release DSA2 3 times with timeout 45 sec
        // Emergency release 15 sec if necessary
    DBG_print(LOG_INFO, "Attempting to release DSA 2");
    OpStatus rel2 = release(DSA_2);
    Sleep(5);

        // Deploy DSA1 timeout 10 sec
    DBG_print(LOG_INFO, "Attempting to deploy DSA 1");
    OpStatus dep1 = deploy(DSA_1);
    Sleep(5);

        // Deploy DSA2 timeout 10 sec
    DBG_print(LOG_INFO, "Attempting to deploy DSA 2");
    OpStatus dep2 = deploy(DSA_2);
    Sleep(5);

    DBG_print(LOG_INFO, "Completed Initial Deploy Operation with status %d", status);

    return status;
  }
}
