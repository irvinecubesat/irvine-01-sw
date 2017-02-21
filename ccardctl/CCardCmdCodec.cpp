#include "CCardCmdCodec.h"

namespace IrvCS
{
  CCardCmdCodec::CCardCmdCodec()
  {
  }

  CCardCmdCodec::~CCardCmdCodec()
  {
  }
  
  /**
   * Create the DSA command data with programmer-friendly API.
   * @param cmd the DSA command to execute
   * @param id The ID of the DSA to execute the command on
   * @return 0 for success, nonzero otherwise
   **/
  uint8_t CCardCmdCodec::
  encodeDsaCmd( const DsaCmd cmd, const DsaId id, uint8_t &data)
  {
    data=0;
    data |= (cmd<<id) | 1<<7;
    
    return 0;
  }

  
  /**
   * Decode the DSA command data into programmer-friendly info.
   * @param data the incoming data
   * @param cmd the dsa command to extract
   * @param id the affected DSA ID
   **/
  uint8_t CCardCmdCodec::
  decodeDsaCmd(const uint8_t data, DsaCmd &cmd, DsaId &id)
  {
    return 0;
  }
  
}
