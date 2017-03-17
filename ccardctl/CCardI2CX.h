#ifndef __CCARDI2CX_HH__
#define __CCARDI2CX_HH__
extern "C"
{
#include <gpioapi.h>
}

#include "DsaController.h"
#include "CCardI2CPortState.h"

namespace IrvCS
{
  /**
   * Manage I2C expander state.  @see https://www.kernel.org/doc/Documentation/i2c/dev-interface
   **/
  class CCardI2CX:public DsaController
  {
  public:
    /**
     * Initialize the I2C expander state
     **/
    CCardI2CX();
    
    ~CCardI2CX();

    //
    // DsaController interface methods
    //
    /**
     * Perform the specified DSA operation.
     * @param id the id of the DSA
     * @param cmd the command to perform (Release or Deploy)
     * @param timeoutSec the timeout in seconds
     * @return the current register value (>=0) if successful
     * @return OpStatus (<0)
     */
    OpStatus performDsaOperation(DsaId id, DsaCmd cmd, int timeoutSec);

    /**
     * Get the sensor status for the given DSA/Cmd
     *
     * @param id the id of the DSA
     * @param cmd the command to perform (Release or Deploy)
     * @return 0 if off
     * @return 1 if on
     * @return <0 if error
     **/
    virtual int getSensorStatus(DsaId id, DsaCmd cmd);

    //
    // lower level methods not for CCardCtl use only
    //
    
    /**
     * Turn on or off the 3V payload power
     * @param onOrOff 1 or 0
     * @return 0 if successful
     * @return <0 if failed
     **/
    int enable3VPayload(int onOrOff);

    /**
     * Set the state directly
     * @return 0 if successful,
     * @return < 0 if error
     **/
    int setState(uint8_t state);

    /**
     * Get the state
     * @return 0 if successful,
     * @return < 0 if error
     **/
    int getState(uint8_t &state);

    /**
     * Get the DSA deploy state.  
     * Use the lower 4 bits to represent DSA1 Release/Deploy state and 
     * the DSA2 Release/Deploy states.  Use the upper 4 bits to flag error bits.
     * @return the dsa deploy state in the lower 4 bits
     * @return <0 if error
     **/
    uint8_t getDsaDeployState();
    
    /**
     * Determine if our expander is ok.
     **/
    bool isOk();

    /**
     * Reset state to initial conditions
     **/
    int reset();

    /**
     * Perform operation for DSA with given timeout
     * @param id the id of the DSA
     * @param cmd the command to perform
     * @param timeoutSec the timeout in seconds
     * @return the current register value (>=0) if successful
     * @return OpStatus (<0)
     **/
    int dsaPerform(DsaId id, DsaCmd cmd, int timeoutSec=5);

    /**
     * Perform operation for MT
     **/
    int mtPerform(uint8_t idBits, uint8_t cmd);

  private:
    CCardI2CPortState portState_;
    int i2cdev_;
    int addr_;
    bool initialized_;
    gpio pl3VGpio_;
    gpio pl5VGpio_;
    gpio dsa1SenseGpio_[2];
    gpio dsa2SenseGpio_[2];
    bool enableTimer_;
  };
}

#endif
