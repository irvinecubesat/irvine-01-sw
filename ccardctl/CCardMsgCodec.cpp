#include "CCardMsgCodec.h"

namespace IrvCS
{
  /**
   * Create the DSA command data with programmer-friendly API.
   * @param cmd the command to execute
   * @param id The ID of the device to execute the command on
   * @return 0 for success, nonzero otherwise
   **/
  uint8_t CCardMsgCodec::
  encodeMsgData( const uint8_t id, const uint8_t cmd, uint8_t &data)
  {
    data = id;
    data |= (cmd<<4);
    
    return 0;
  }


  uint8_t CCardMsgCodec::
  decodeMsgData(const uint8_t data, uint8_t &id, uint8_t &cmd)
  {
    cmd = data>>4;              // command is in the higher bit
    id = data & 0xF;            // filter out the higher bits

    return 0;
  }
  
}
