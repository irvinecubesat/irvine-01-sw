#ifndef __DSA_OP_CONTEXT__
#define __DSA_OP_CONTEXT__

#include <sys/socket.h>
#include <polysat/polysat.h>
#include "OpContext.h"
#include "CCardI2CX.h"

namespace IrvCS
{
  class DsaOpContext:public OpContext
  {
  public:
    DsaOpContext(Process *proc, struct sockaddr_in *src, CCardI2CX *expander,
                 volatile bool &inProgress):proc_(proc), expander_(expander),
                                   inProgress_(inProgress) 
      {
        inProgress_=true;
        memcpy(&src_, src, sizeof(struct sockaddr_in));
      }

    ~DsaOpContext()
      {
        inProgress_=false;
      }

    OpStatus setStatus(OpStatus status)
      {
        CCardStatus cCardStatus;

        cCardStatus.status=status;

        expander_->getStates(cCardStatus.portStatus, cCardStatus.dsaDeployState);

        // send the response
        PROC_cmd_sockaddr(proc_->getProcessData(), CCARD_RESPONSE,
                          &cCardStatus, sizeof(cCardStatus), &src_);
      }
   
  private:

    Process *proc_;
    struct sockaddr_in src_;
    CCardI2CX *expander_;
    volatile bool &inProgress_;
    
  };
}

#endif
