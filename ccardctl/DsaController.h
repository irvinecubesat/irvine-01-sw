#ifndef __DSA_CONTROLLER__HH
#define __DSA_CONTROLLER__HH

#include "ccardDefs.h"

namespace IrvCS
{
  /**
   * Abstract class to represent the Deployable Solar Array controller.
   **/
  class DsaController
  {
  public:
    DsaController()
    {
    }
    
    virtual ~DsaController()
    {
    }

    /**
     * Perform the specified DSA command/operation.
     * @param id the id of the DSA
     * @param cmd the command to perform (Release or Deploy)
     * @param timeoutSec the timeout in seconds
     * @return StatOk if successful
     * @return OpStatus (<0)
     */
    virtual OpStatus performDsaOperation(DsaId id, DsaCmd cmd, int timeoutSec) = 0;

    /**
     * Get the sensor status for the given DSA/Cmd
     *
     * @param id the id of the DSA
     * @param cmd the command to perform (Release or Deploy)
     * @return 0 if off
     * @return 1 if on
     * @return <0 if error
     **/
    virtual int getSensorStatus(DsaId id, DsaCmd cmd) = 0;

  };
}
#endif
