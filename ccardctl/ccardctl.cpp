/*
 * C-Card Controller
 */
#include <polysat/polysat.h>
#include "cCardMessages.h"

static Process *gProc=NULL;

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
    status.data=gOutputPinStatus;

    PROC_cmd_sockaddr(gProc->getProcessData(), CCARD_RESPONSE,
                      &status, sizeof(status), src);
  }

  /**
   * Process the CCard command
   **/
  void ccard_cmd(int socket, unsigned char cmd, void *data, size_t dataLen,
                 struct sockaddr_in *src)
  {
    CCardCmd cmd;

    status.data=gOutputPinStatus;

    PROC_cmd_sockaddr(gProc->getProcessData(), CCARD_RESPONSE,
                      &status, sizeof(status), src);
  }
}
