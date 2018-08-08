#ifndef __CCARDI2CX_HH__
#define __CCARDI2CX_HH__

#include <time.h>
#include <Gpio.h>
#include "DsaController.h"
#include "DsaI2CPortState.h"

namespace IrvCS
{
  typedef enum {
    RegInput=0x00,
    RegOutput=0x01,
    RegPolarity=0x02,
    RegConfig=0x03
  } RegisterType;
  
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
    int getSensorStatus(DsaId id, DsaCmd cmd);

    //
    // lower level methods not for CCardCtl use only
    //
    
    /**
     * Turn on or off the 3V payload power
     * @param onOrOff 1 or 0
     * @return 0 if successful
     * @return <0 if failed
     **/
    int setPayloadPower(Gpio &pwrGpio, uint8_t onOrOff);

    /**
     * Set the state directly
     * @return 0 if successful,
     * @return < 0 if error
     **/
    int setState(uint8_t state);

    /**
     * Get the specified register
     * @return register value or -1 for error
     *
     **/
    int getRegister(const RegisterType regType);
    
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
     * Get the current controller states
     * @param portState the state of the I2C register 1
     * @param deployState the deployment states
     **/
    int8_t getStates(uint8_t &portState, uint8_t &deployState);

    /**
     * Determine if our expander is ok.
     **/
    bool isOk();

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
     * @return OpStatus
     **/
    int mtPerform(uint8_t idBits, uint8_t cmd);

    /**
     * Check to see if there is any activity within a time interval.  
     * Switch to lower power mode if no activity.
     * @return 0 if no change in state
     * @return 1 if system has switched to idle mode
     * @return -1 error encountered
     **/
    int idleCheck();

  private:

    /**
     * Get the sensor status the DSA sensors
     *
     * @return status bits
     * @return <0 if error
     **/
    int getSensorStatusBits();

    /**
     * Reset state to initial conditions
     **/
    int reset();

    /**
     * Set the state directly with no power on call
     * @return 0 if successful,
     * @return < 0 if error
     **/
    int setI2Cstate(uint8_t state);

    int initGpios();
    /**
     * Power on if needed and reset state
     * @return 0 if power is on
     * @return <1 if errors encountered
     **/
    int powerOn();

    bool isPoweredOn_;
    time_t pwrTimestamp_;
    
    DsaI2CPortState portState_;
    int i2cdev_;
    int addr_;
    bool initialized_;
    Gpio pl3VGpio_;
    Gpio pl5VGpio_;
    uint8_t dsaSenseBits_;
  };
}

#endif
