#ifndef __CCARD_MESSAGES_H__
#define __CCARD_MESSAGES_H__

#include <stdint.h>
extern "C"
{

#define CCARD_CMD 0x02
#define CCARD_RESPONSE 0xF2
  
  /**
   * Provide card status.  Provide the port register.
   **/
  struct CCardStatus
  {
    uint8_t portStatus;
    int8_t status;
  }__attribute__((packed));

  /**
   * Single command structure to hold commands 32-bits
   *
   * See CCardMsgCodec for the encoding/decoding details
   **/
  struct CCardMsg
  {
    uint32_t data;
  }__attribute__((packed));

}
#endif
