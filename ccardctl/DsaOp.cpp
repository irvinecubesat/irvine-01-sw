#include <syslog.h>
#include "DsaOp.h"

namespace IrvCS
{
  DsaOp::DsaOp(const DsaId dsaId, const DsaCmd dsaCmd,
               DsaController &controller, OpContext *context)
    :Thread(Detached), id_(dsaId),cmd_(dsaCmd),
     controller_(controller), context_(context)
  {
    if (cmd_ == Release)
    {
      timeout_=DSA_RELEASE_TIMEOUT;
    } else 
    {
      timeout_=DSA_DEPLOY_TIMEOUT;
    }

  }

  /**
   * Run the DSA Operation in the thread
   **/
  void *DsaOp::run()
  {
    syslog(LOG_INFO, "Executing DSA Operation");
    OpStatus status=controller_.performDsaOperation(id_, cmd_, timeout_);

    if (status == StatOk)
    {
      syslog(LOG_INFO, "Completed DSA Operation");
    } else if (status == StatTimeOut)
    {
      syslog(LOG_WARNING, "DSA Operation did not finish");
    } else 
    {
      syslog(LOG_ERR, "DSA Operation status %d", status);
    }
    context_->setStatus(status);

    delete this;
  }
  
  DsaOp::~DsaOp()
  {
    if (NULL != context_)
    {
      delete context_;
    }
  }
}
