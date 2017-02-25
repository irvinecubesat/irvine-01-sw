#ifndef __CCARDI2CX_HH__
#define __CCARDI2CX_HH__

namespace IrvCS
{
  /**
   * Manage I2C expander state.  @see https://www.kernel.org/doc/Documentation/i2c/dev-interface
   **/
  class CCardI2CX
  {
  public:
    /**
     * Initialize the I2C expander state
     **/
    CCardI2CX(uint8_t state);
    
    ~CCardI2CX();

    /**
     * Set the state
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
     * Determine if our expander is ok.
     **/
    bool isOk();

  private:
    int i2cdev_;
    int addr_;
    bool initialized_;
  };
}

#endif
