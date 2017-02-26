/*
 * C-Card Controller
 */
#include <getopt.h>
#include <string>
#include <iostream>
#include <syslog.h>
#include <stdlib.h>
#include <polysat/polysat.h>
#include "cCardMessages.h"
#include "CCardI2CPortState.h"
#include "CCardMsgCodec.h"
#include "CCardI2CX.h"

#define DBG_LEVEL_DEBUG DBG_LEVEL_ALL

static Process *gProc=NULL;

static IrvCS::CCardI2CPortState *gPortState=NULL;
static IrvCS::CCardI2CX *gI2cExpander=NULL;

namespace IrvCS{
  
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
      if (gPortState=NULL)
      {
        syslog(LOG_ERR, "%s gPortState is NULL", __FILENAME__);
        return;
      }
      
      CCardStatus status;
      status.portStatus=gPortState->getState();

      PROC_cmd_sockaddr(gProc->getProcessData(), CMD_STATUS_RESPONSE,
                        &status, sizeof(status), src);
    }

    /**
     * Process the CCard cmd message
     **/
    void ccard_cmd(int socket, unsigned char cmd, void *data, size_t dataLen,
                   struct sockaddr_in *src)
    {
      if (NULL==gI2cExpander)
      {
        syslog(LOG_ERR, "%s gI2cExpander is NULL", __FILENAME__);
        return;
      }
      CCardMsg msg;
      CCardStatus status;

      uint8_t msgType=0;
      uint8_t devId=0;
      uint8_t msgCmd=0;
      uint32_t hostData=ntohl(msg.data);
      CCardMsgCodec::decodeMsgData(hostData, msgType, devId, msgCmd);

      status.portStatus=gPortState->update(msgType, devId, msgCmd);

      PROC_cmd_sockaddr(gProc->getProcessData(), CCARD_RESPONSE,
                        &status, sizeof(status), src);
    }
  }

}

/**
 * Usage
 **/
void usage(char *argv[])
{
  std::cout<<"Usage:  "<<argv[0]<<" [options]"
           <<std::endl<<std::endl
           <<"        C-card controller daemon"<<std::endl<<std::endl
           <<"Options:"<<std::endl<<std::endl
           <<" -d {log level}  set log level"<<std::endl
           <<" -s              syslog output to stderr"<<std::endl
           <<" -h              this message"<<std::endl
           <<std::endl;
  exit(1);
}

static int sigint_handler(int signum, void *arg)
{
  Process *proc=(Process *)arg;
  EVT_exit_loop(PROC_evt(proc->getProcessData()));
  
  return EVENT_KEEP;
}

int main(int argc, char *argv[])
{
  int status=0;
  int syslogOption=0;
  int opt;
  
  int logLevel=DBG_LEVEL_INFO;

  while ((opt=getopt(argc,argv,"sd:h")) != -1)
  {
    switch (opt)
    {
    case 's':
      syslogOption=LOG_PERROR;
      break;
    case 'd':
      logLevel=strtol(optarg, NULL, 10);
      break;
    case 'h':
      usage(argv);
      break;
    default:
      usage(argv);
    }
  }

  DBG_setLevel(logLevel);

  openlog("ccardctl", syslogOption, LOG_USER);

  IrvCS::CCardI2CPortState portState;
  IrvCS::CCardI2CX i2cX(portState.getState());

  //
  // Turn on payload power at GPIO 103
  //
  int sysStatus=system("/usr/bin/gpiotest 103 o 1");
  int exitStatus=WEXITSTATUS(sysStatus);
  if (exitStatus != 0)
  {
    syslog(LOG_ERR, "Unable to power on GPIO 103 for 3.3V Payload");
  }
  
  gProc=new Process("ccardctl");
  gPortState=&portState;
  gI2cExpander = &i2cX;

  gProc->AddSignalEvent(SIGINT, &sigint_handler, gProc);

  EventManager *events=gProc->event_manager();

  syslog(LOG_INFO, "%s ready to process messages",__FILENAME__);
  
  events->EventLoop();

  delete gProc;
  gPortState=NULL;
  gI2cExpander=NULL;
  return status;
}
