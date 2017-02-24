#ifndef __CCARD_I2C_PORT_STATE__
#define __CCARD_I2C_PORT_STATE__

#include <stdint.h>

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
#define MT01_ENABLE   1<<4
#define MT02_ENABLE   1<<5
#define MT03_ENABLE   1<<6
#define DSA_ENABLE_TIMER 1<<7

  /**
   * Mask for DSA-only values
   **/
#define DSA_MASK 0xF|DSA_ENABLE_TIMER

  /**
   * Offset of the MT values
   **/
#define MT_OFFSET 4

  /**
   * Mask for MT-only values
   **/
#define MT_MASK 7<<MT_OFFSET
  

  /**
   * Compute I2C expander Port state for Register 1.  As both DSA and MT
   * are controlled via one register, this class ensures that DSA and MT 
   * settings are independently set.  In addition, it automatically set the
   * timer bit 7 when DSA commands are set.
   *
   * See  <a href="http://www.ti.com/lit/ds/symlink/tca9554a.pdf">
   * The TCA9554a I2C expander spec sheet for more information
   * </a>
   *
   **/
  class CCardI2CPortState
  {
  public:
    /**
     * Initialize the register values to what we want the initial
     * port state to be.  MT's should be 0. DSA's bits should be 1.
     **/
    CCardI2CPortState();

    virtual ~CCardI2CPortState();

    /**
     * DSA values are active low - set the value to 0 to 
     * trigger the command.
     * @param id the DSA ID
     * @param cmd the DSA command to execute
     * @return the computed expander register 1 value
     **/
    uint8_t setDsa(DsaId id, DsaCmd cmd);

    /**
     * Set the Magnetorquers' state.  
     * MT's are active high - set the state bit to 1 to enable.
     *
     * @param idBits 3 bits with each bit corresponds to a magnetorquer.
     * @param state the state to set the magnetorquers
     * @return the computed expander register 1 value
     **/
    uint8_t setMt(uint8_t idBits, MtState state);

    /**
     * Gets the computed port state
     * @return the computed expander register 1 value
     **/
    uint8_t getState();

    /**
     * Get MT Bits.
     * @return only the MT bits
     **/
    uint8_t getMtState();

  private:
    uint8_t reg1State_;
  };
}
#endif
