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
    reg1State_= DSA_MASK & ~MT_MASK;
  }

  CCardI2CPortState::~CCardI2CPortState()
  {
    // not much to do here yet...
  }

  static DsaId id2DsaId(uint8_t id)
  {
    DsaId dsaId=DSA_UNKNOWN;
    if (id == DSA_1)
    {
      dsaId=DSA_1;
    } else if (id == DSA_2)
    {
      dsaId=DSA_2;
    }

    return dsaId;
  }

  static DsaCmd cmd2DsaCmd(uint8_t cmd)
  {
    DsaCmd dsaCmd=CmdUnknown;
    if (Deploy == cmd)
    {
      dsaCmd = Deploy;
    } else if (Release == cmd)
    {
      dsaCmd = Release;
    }
    return dsaCmd;
  }
    
  uint8_t CCardI2CPortState::
  update(const uint8_t msgType, const uint8_t id, const uint8_t cmd)
  {
    switch(msgType)
    {
    case MsgDsa:
    {
      DsaId dsaId = id2DsaId(id);
      if (DSA_UNKNOWN == dsaId)
      {
        syslog(LOG_ERR, "Unknown DSA ID:  %d", id);
        return reg1State_;
      }
      DsaCmd dsaCmd = cmd2DsaCmd(cmd);
      if (CmdUnknown == dsaCmd)
      {
        syslog(LOG_ERR, "Uknown DSA Cmd:  %d", cmd);
        return reg1State_;
      }
      return setDsa(dsaId, dsaCmd);
    }
    break;
    case MsgMt:
    {
      return setMt(id, cmd);
    }
    break;
    default:
      syslog(LOG_WARNING, "%s Unknown msg type:  %d", __FILENAME__, msgType);
    }

    return getState();
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
    } else
    {
      // active low, so we need the complement of the bits
      uint8_t dsaBits=(1<<(dsaOffset+dsaCmdBits))|DSA_ENABLE_TIMER;
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
    stm<<"D1:  R="<<(state&DSA1_RELEASE?1:0)
       <<" D="<<(state&DSA1_DEPLOY?1:0)
       <<" D2:  R="<<(state&DSA2_RELEASE?1:0)
       <<" D="<<(state&DSA2_DEPLOY?1:0)
       <<" DT="<<(state&DSA_ENABLE_TIMER?1:0)
       <<" M="<<printBinary((state&MT_MASK)>>MT_OFFSET, 3);
    return stm.str();
  }
}
