#ifndef __CCARD_MSG_CODEC_HH__
#define __CCARD_MSG_CODEC_HH__
#include <stdint.h>
#include "ccardDefs.h"
namespace IrvCS
{
  /**
   * Encoder/decoder for the C Card controller cmd messages.
   * Provides utilities to generate commands and process them.  
   *
   * Also provides utility functions to decode the results of queries into
   * human readable format.
   *
   * The command is 8 bits, with the higher 4 bits containing the
   * command and the lower bits containing the device id.
   *
   * Within the 
   **/
  class CCardMsgCodec
  {
  public:
    /**
     * Create the command data with programmer-friendly API.
     * @param cmd the command to execute
     * @param id The ID of the DSA to execute the command on
     * @return 0 for success, nonzero otherwise
     **/
    static uint8_t encodeMsgData(const uint8_t msgType, const uint8_t id, const uint8_t cmd,
                                  uint32_t &data);

    /**
     * Decode the command data into programmer-friendly info.
     * @param data the incoming data
     * @param cmd the dsa command to extract
     * @param id the affected device
     **/
    static uint8_t decodeMsgData(const uint32_t data,
                                 uint8_t &msgType, uint8_t &id, uint8_t &cmd );
    
  };
  
}

#endif
