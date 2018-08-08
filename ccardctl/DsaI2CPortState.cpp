#include "DsaI2CPortState.h"
#include <syslog.h>
#include <string>
#include <sstream>

/*
 * See header file for method documentation
 */
namespace IrvCS
{
  DsaI2CPortState::DsaI2CPortState()
  {
    //
    // Initial state should be DSA bits on (active low),
    // MT bits off (active high)
    // 
    (void)reset();
  }

  DsaI2CPortState::~DsaI2CPortState()
  {
    // not much to do here yet...
  }


  uint8_t DsaI2CPortState::reset()
  {
    reg1State_= DSA_CMD_MASK;
    return reg1State_;
  }
  
  uint8_t DsaI2CPortState::setDsa(DsaId id, DsaCmd cmd)
  {
    // since we defined the id and DsaCmd values appropriately, we
    // use these values directly in our bitmask computation
    // The id determines the offset in the bits and the cmd is the bits
    // int the offset
    uint8_t dsaOffset = id;
    uint8_t dsaCmdBits=cmd;

    uint8_t dsaBits=(1<<(dsaOffset|dsaCmdBits));
    // First to turn DSA's off
    reg1State_|= DSA_CMD_MASK;
    // set only the DSA bit
    // active low, so we need the complement of the bits
    reg1State_&=~dsaBits;

    return reg1State_;
  }

  uint8_t DsaI2CPortState::getState()
  {
    return reg1State_;
  }

  std::string DsaI2CPortState::printBinary(const uint8_t data, const uint8_t nbits)
  {
    std::stringstream stm;
    for (int i=nbits-1; i >=0; i--)
    {
      stm<<((data>>i)&1);
    }
    return stm.str();
  }
  
  std::string DsaI2CPortState::stateToString(const uint8_t state, uint8_t dsaDeployState)
  {
    const char* okMarker="*";
    const char* dsa1Released=(dsaDeployState & (1<<DSA1_RELEASE_STATUS_BIT))?okMarker:"";
    const char* dsa1Deployed=(dsaDeployState & (1<<DSA1_DEPLOY_STATUS_BIT))?okMarker:"";
    const char *dsa2Released=(dsaDeployState & (1<<DSA2_RELEASE_STATUS_BIT))?okMarker:"";
    const char *dsa2Deployed=(dsaDeployState & (1<<DSA2_DEPLOY_STATUS_BIT))?okMarker:"";
    std::stringstream stm;
    stm<<"D1("<<(state&DSA1_RELEASE?1:0)<<dsa1Released
       <<","<<(state&DSA1_DEPLOY?1:0)<<dsa1Deployed
       <<") D2("<<(state&DSA2_RELEASE?1:0)<<dsa2Released
       <<","<<(state&DSA2_DEPLOY?1:0)<<dsa2Deployed
       <<")";
    return stm.str();
  }
}
