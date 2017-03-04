#include "CCardI2CPortState.h"
#include <syslog.h>
#include <string>
#include <sstream>

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
    (void)reset();
  }

  CCardI2CPortState::~CCardI2CPortState()
  {
    // not much to do here yet...
  }


  uint8_t CCardI2CPortState::reset()
  {
    reg1State_= DSA_MASK & ~MT_MASK;
    return reg1State_;
  }
  
  uint8_t CCardI2CPortState::setDsa(DsaId id, DsaCmd cmd)
  {
    // since we defined the id and DsaCmd values appropriately, we
    // use these values directly in our bitmask computation
    // The id determines the offset in the bits and the cmd is the bits
    // int the offset
    uint8_t dsaOffset = id;
    uint8_t dsaCmdBits=cmd;

    if (cmd == ResetTimer) 
    {
      // set command bits for dsaID to 1 plus timer bit
      // TODO:  Determine if this is the correct action for reset
      reg1State_|=(3<<dsaOffset)|DSA_ENABLE_TIMER; // 3 is 11 in binary
    } else if (cmd == SetTimer)
    {
      // turn on timer
      reg1State_&= ~(DSA_ENABLE_TIMER); // enable timer
    } else
    {
      uint8_t dsaBits=(1<<(dsaOffset|dsaCmdBits));
      // First to turn timer and DSA's off
      reg1State_|= DSA_MASK;
      // set only the DSA bit (no timer).
      // active low, so we need the complement of the bits
      reg1State_&=~dsaBits;
    }

    return reg1State_;
  }

  uint8_t CCardI2CPortState::setMt(uint8_t idMask, uint8_t mtBits)
  {
    // flip all bits off, then assign the mtBits
    reg1State_ &= ~(idMask<<MT_OFFSET);
    reg1State_ |= ((mtBits&idMask)<<MT_OFFSET);
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

  std::string CCardI2CPortState::printBinary(const uint8_t data, const uint8_t nbits)
  {
    std::stringstream stm;
    for (int i=nbits-1; i >=0; i--)
    {
      stm<<((data>>i)&1);
    }
    return stm.str();
  }
  
  std::string CCardI2CPortState::stateToString(const uint8_t state)
  {
    std::stringstream stm;
    stm<<"D1("<<(state&DSA1_RELEASE?1:0)
       <<","<<(state&DSA1_DEPLOY?1:0)
       <<") D2("<<(state&DSA2_RELEASE?1:0)
       <<","<<(state&DSA2_DEPLOY?1:0)
       <<") DT("<<(state&DSA_ENABLE_TIMER?1:0)
       <<") M("<<printBinary((state&MT_MASK)>>MT_OFFSET, 3)<<")";
    return stm.str();
  }
}
