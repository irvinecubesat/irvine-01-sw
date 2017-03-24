#ifndef __DSA_OP__HH
#define __DSA_OP__HH

#include "DsaController.h"
#include "OpContext.h"
#include <Thread.h>

namespace IrvCS
{
  /**
   * Perform DSA release/deploy operations in a thread
   **/
  class DsaOp:public Thread
  {
  public:

    /**
     * Create DsaOp with all necessary info.
     * @param dsaId the ID of the DSA
     * @param dsaCmd the DSA Cmd - Release or Deploy
     * @param controller the DSA Controller
     * @param context The response processor to send the response to.
     **/
    DsaOp(const DsaId dsaId, const DsaCmd dsaCmd,
          DsaController &controller, OpContext *context);
   
    virtual ~DsaOp();

    /**
     * Run the thread
     **/
    void *run();
      
  private:

    DsaId id_;
    DsaCmd cmd_;
    int timeout_;

    DsaController &controller_;
    OpContext *context_;
  };

}

#endif
