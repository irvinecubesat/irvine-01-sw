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
  encodeMsgData(const uint8_t msgType, const uint8_t id, const uint8_t cmd,
                uint32_t &data)
  {
    data = id;
    data |= (cmd<<MSG_CMD_OFFSET_BITS) | (msgType<<MSG_TYPE_OFFSET_BITS);
    return 0;
  }


  uint8_t CCardMsgCodec::
  decodeMsgData(const uint32_t data,
                uint8_t &msgType, uint8_t &id, uint8_t &cmd)
  {
    msgType = (data>>MSG_TYPE_OFFSET_BITS) & 0xFF;
    cmd = (data>>MSG_CMD_OFFSET_BITS) & 0xFF; // command is in the higher bit
    id = data & 0xFF;            // filter out the higher bits

    return 0;
  }
  
}
