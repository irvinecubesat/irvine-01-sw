#ifndef __CCARDI2CX_HH__
#define __CCARDI2CX_HH__

namespace IrvCS
{
  /**
   * Manage I2C expander state
   **/
  class CCardI2CX
  {
  public:
    /**
     * Initialize the I2C expander state
     **/
    CCardI2CX(uint8_t state);

    ~CCardI2CX();
  };
}

#endif
