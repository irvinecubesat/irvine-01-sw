#include "CCardI2CPortState.h"

/*
 * See header file for method documentation
 */
namespace IrvCS
{
  CCardI2CPortState::CCardI2CPortState()
  {
    //
    // Initial state should be DSA bits on (active low),
    // MT bits off (active high)
    // 
    reg1State_= DSA_MASK & ~MT_MASK;
  }

  CCardI2CPortState::~CCardI2CPortState()
  {
    // not much to do here yet...
  }

  uint8_t CCardI2CPortState::setDsa(DsaId id, DsaCmd cmd)
  {
    // since we defined the id and DsaCmd values appropriately, we
    // use these values directly in our bitmask computation
    // The id determines the offset in the bits and the cmd is the bits
    // int the offset
    uint8_t dsaOffset = id;
    uint8_t dsaCmdBits=cmd;
    if (cmd == Reset) 
    {
      // set command bits for dsaID to 1 plus timer bit
      // TODO:  Determine if this is the correct action for reset
      reg1State_|=(3<<dsaOffset)|DSA_ENABLE_TIMER; // 3 is 11 in binary
    } else
    {
      // active low, so we need the complement of the bits
      uint8_t dsaBits=(1<<(dsaOffset+dsaCmdBits))|DSA_ENABLE_TIMER;
      reg1State_&=~dsaBits;
    }

    return reg1State_;
  }

  uint8_t CCardI2CPortState::setMt(uint8_t idBits, MtState state)
  {
    if (On == state)
    {
      reg1State_ |= (idBits<<MT_OFFSET);
    } else
    {
      reg1State_ &= ~(idBits<<MT_OFFSET);
    }
    
    return reg1State_;
  }

  uint8_t CCardI2CPortState::getState()
  {
    return reg1State_;
  }

  uint8_t CCardI2CPortState::getMtState()
  {
    return (reg1State_&MT_MASK)>>MT_OFFSET;
  }
}
