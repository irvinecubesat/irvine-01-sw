#ifndef __OPCONTEXT_HH__
#define __OPCONTEXT_HH__

#include "ccardDefs.h"

namespace IrvCS
{
  /**
   * OpContext holds information about the context of the operation including
   * who initiated, the current status, completion, etc.
   **/
  class OpContext
  {
  public:
    OpContext(){}

    virtual ~OpContext(){}

    /**
     * Set the status of the current operation.  Set to StatOk if the current operation
     * is complete.
     * @param status The status of the current operation
     * @return StatOk if setting the status was successful
     **/
    virtual OpStatus setStatus(OpStatus status)=0;

  };
}

#endif
