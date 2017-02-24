/*
 * C-Card Controller
 */
#include <stdlib.h>
#include <polysat/polysat.h>
#include "cCardMessages.h"
#include "CCardI2CPortState.h"
#include "CCardMsgCodec.h"
#include "CCardI2CX.h"

namespace IrvCS{
  
  static Process *gProc=NULL;

  CCardI2CPortState gPortState;

  CCardI2CX i2cX(gPortState.getState());

  static uint8_t gOutputPinStatus=0xFF;

#define DBG_LEVEL_DEBUG DBG_LEVEL_ALL

  extern "C"
  {
    /**
     * Process the status command.  Provide the cached values since
     * this is called frequently by the watchdog.  We could periodically
     * refresh the value here.
     **/
    void ccard_status(int socket, unsigned char cmd, void *data, size_t dataLen,
                      struct sockaddr_in *src)
    {
      CCardStatus status;
      status.pinStatus=gPortState.getState();

      PROC_cmd_sockaddr(gProc->getProcessData(), CCARD_RESPONSE,
                        &status, sizeof(status), src);
    }

    /**
     * Process the CCard command
     **/
    void ccard_cmd(int socket, unsigned char cmd, void *data, size_t dataLen,
                   struct sockaddr_in *src)
    {
      CCardMsg msg;
      CCardStatus status;

      uint8_t msgType=0;
      uint8_t devId=0;
      uint8_t msgCmd=0;
      CCardMsgCodec::decodeMsgData(msg.data, msgType, devId, msgCmd);

      // TODO set i2c

      PROC_cmd_sockaddr(gProc->getProcessData(), CCARD_RESPONSE,
                        &status, sizeof(status), src);
    }
  }
}
int main(int argc, char *argv[])
{
  int status=0;
  return status;
}
