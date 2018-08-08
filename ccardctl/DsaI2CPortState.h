#ifndef __DSA_I2C_PORT_STATE__
#define __DSA_I2C_PORT_STATE__

#include <stdint.h>
#include <string>

#include "ccardDefs.h"

namespace IrvCS
{
  /**
   * Define the port bits of the tca9554.  
   **/
#define DSA1_RELEASE  1<<0
#define DSA1_DEPLOY   1<<1
#define DSA2_RELEASE  1<<2
#define DSA2_DEPLOY   1<<3
#define DSA1_RELEASE_SENSE 1<<4
#define DSA1_DEPLOY_SENSE  1<<5
#define DSA2_RELEASE_SENSE 1<<6
#define DSA2_DEPLOY_SENSE  1<<7

/**
 * Mask for DSA command values
 **/
#define DSA_CMD_MASK 0x0F

/**
 * Sense value mask
 **/  
#define DSA_SENSE_MASK 0xF0

/**
 * Upper 4 bits are input ports, lower are output
 **/
#define DSA_CONFIG_PORTS 0xF0

#define DSA_SENSE_OFFSET 4


  /**
   * Compute DSA I2C expander Port state for Register 1.  
   *
   * Ports on the expander (on i2c slave address 0x38) are mapped as follows:
   *
   * 0-3 are output ports for initiating release and deploy operations:
   *
   * 0 - DSA1 Release
   * 1 - DSA1 Deploy
   * 2 - DSA2 Release
   * 3 - DSA2 Deploy
   *
   * 4-7 are input ports containing the DSA release/deploy sensors:
   * 
   * 4 DSA1 Release Sense
   * 5 DSA1 Deploy Sense
   * 6 DSA2 Release Sense
   * 7 DSA2 Deploy Sense
   *
   * Note:  Release and Deploy sensors need to be verified.
   *
   * Configuration register (Register 3) should be set to 0xF0
   *
   * See  <a href="http://www.ti.com/lit/ds/symlink/tca9554a.pdf">
   * The TCA9554a I2C expander spec sheet for more information
   * </a>
   *
   **/
  class DsaI2CPortState
  {
  public:
    /**
     * Initialize the register values to what we want the initial
     * port state to be.  MT's should be 0. DSA's bits should be 1.
     **/
    DsaI2CPortState();

    virtual ~DsaI2CPortState();

    /**
     * Reset the state to initial values
     **/
    uint8_t reset();
    
    /**
     * DSA values are active low - set the value to 0 to 
     * trigger the command.
     * @param id the DSA ID
     * @param cmd the DSA command to execute
     * @return the computed expander register 1 value
     **/
    uint8_t setDsa(DsaId id, DsaCmd cmd);

    /**
     * Gets the computed port state
     * @return the computed expander register 1 value
     **/
    uint8_t getState();

    /**
     * Convert data into a human readable string
     **/
    static std::string stateToString(const uint8_t data, const uint8_t dsaDeployState);

    /**
     * Print number to binary format string
     **/
    static std::string printBinary(const uint8_t data, const uint8_t nbits);

  private:
    uint8_t reg1State_;
  };
}
#endif
