/*
 * C-Card Controller
 */
#include <getopt.h>
#include <string>
#include <iostream>
#include <syslog.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <polysat/polysat.h>
#include "cCardMessages.h"
#include "CCardMsgCodec.h"
#include "ccardDefs.h"
#include "CCardI2CX.h"
#include "InitialDeployOp.h"

#define DBG_LEVEL_DEBUG DBG_LEVEL_ALL

static Process *gProc=NULL;

static uint8_t gPortState=0;
static IrvCS::CCardI2CX *gI2cExpander=NULL;

using namespace IrvCS;

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
  if (cmd >= CmdUnknown)
  {
    return CmdUnknown;
  }
  
  return static_cast<DsaCmd>(cmd);
}

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
    static int counter=0;
    status.status=0;
    if (NULL == gI2cExpander)
    {
      DBG_print(LOG_ERR, "%s gI2cExpander is NULL", __FILENAME__);
      status.status=-1;
    } else
    {
      int getStatus=0;
      // refresh state every 100 polls
      if (0==(counter % 100))
      {
        getStatus=gI2cExpander->getState(gPortState);
      }
      if (getStatus==0)
      {
        status.portStatus=gPortState;
      } else
      {
        status.status=getStatus;
      }
      status.dsaDeployState=gI2cExpander->getDsaDeployState();
    }
    syslog(LOG_DEBUG, "Sending status message response %02x", status.portStatus);

    PROC_cmd_sockaddr(gProc->getProcessData(), CMD_STATUS_RESPONSE,
                      &status, sizeof(status), src);
  }

  /**
   * Process the CCard cmd message
   **/
  void ccard_cmd(int socket, unsigned char cmd, void *data, size_t dataLen,
                 struct sockaddr_in *src)
  {
    if (NULL == gI2cExpander)
    {
      syslog(LOG_ERR, "%s gI2cExpander is NULL", __FILENAME__);
      return;
    }
    CCardMsg *msg=(CCardMsg *)data;
    if (dataLen != sizeof(CCardMsg))
    {
      DBG_print(DBG_LEVEL_WARN, "Incoming size is incorrect (%d != %d)",
                dataLen, sizeof(CCardMsg));
      return;
    }
    CCardStatus status;
    status.status=0;

    int setStatus=0;
    uint8_t msgType=0;
    uint8_t devId=0;
    uint8_t msgCmd=0;
    int timeout=5;
    uint32_t hostData=ntohl(msg->data);
    IrvCS::CCardMsgCodec::decodeMsgData(hostData, msgType, devId, msgCmd);
    DsaId dsaId=DSA_UNKNOWN;
    DsaCmd dsaCmd=CmdUnknown;
    switch (msgType)
    {
    case IrvCS::MsgDsa:
      dsaId = id2DsaId(devId);
      if (DSA_UNKNOWN == dsaId)
      {
        DBG_print(LOG_ERR, "Unknown DSA ID:  %d", devId);
        status.status=-1;
        break;
      }
      dsaCmd = cmd2DsaCmd(msgCmd);
      if (CmdUnknown == dsaCmd)
      {
        DBG_print(LOG_ERR, "Uknown DSA Cmd:  %d", cmd);
        status.status=-1;
        break;
      }
      if (dsaCmd == Release)
      {
        timeout=TIMEOUT_RELEASE;
      } else if (dsaCmd == Deploy)
      {
        timeout=TIMEOUT_DEPLOY;
      }
      setStatus=gI2cExpander->dsaPerform(dsaId, dsaCmd, timeout);
      if (setStatus < 0)
      {
        status.status=setStatus;
      } else
      {
        status.portStatus=gPortState=(uint8_t)setStatus;
      }
      break;
    case IrvCS::MsgMt:
      setStatus=gI2cExpander->mtPerform(devId, msgCmd);
      if (setStatus < 0)
      {
        status.status=setStatus;
      } else
      {
        status.portStatus=gPortState=(uint8_t)setStatus;
      }
      break;
    default:
      DBG_print(LOG_WARNING, "%s Unknown msg type:  %d",
                __FILENAME__, msgType);
    }

    status.dsaDeployState=gI2cExpander->getDsaDeployState();

    PROC_cmd_sockaddr(gProc->getProcessData(), CCARD_RESPONSE,
                      &status, sizeof(status), src);
  }
}

static int executeInitialDeploymentOp(void *arg)
{
  IrvCS::DsaController *dsaController=static_cast<IrvCS::DsaController *>(arg);

  DBG_print(LOG_NOTICE, "Performing Initial Deployment Operation");
  IrvCS::InitialDeployOp deployOp(dsaController);

  deployOp.execute();

  return EVENT_REMOVE;
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
           <<" -D {file}       Set the initial deploy operation flag file."<<std::endl
           <<"                 If file exists, skip initial deployment op."<<std::endl
           <<"                 Must be in persistent location which is cleared before launch."<<std::endl
           <<" -T {seconds}    Set the initial deploy time in seconds"<<std::endl
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
  int initDeployDelayTime=-1;
  bool initDeployFlag=false;
  struct stat statBuf;
  
  int logLevel=DBG_LEVEL_INFO;

  while ((opt=getopt(argc,argv,"sd:h")) != -1)
  {
    switch (opt)
    {
    case 'D':
      // @TODO we could register a ppod deployment event callback
      // which would then make this file unnecessary.
      // Not sure how to test it to see if it works.
      if (0 == stat(optarg, &statBuf))
      {
        initDeployFlag = false;
      } else
      {
        initDeployFlag = true;
      }
      break;
    case 'T':
      initDeployDelayTime=strtol(optarg, NULL, 10);
      break;
    case 's':
      syslogOption=LOG_PERROR;
      openlog("ccardctl", syslogOption, LOG_USER);
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

  gProc = new Process("ccardctl");
  DBG_setLevel(logLevel);

  DBG_print(DBG_LEVEL_INFO, "Starting up ccardctl");
  IrvCS::CCardI2CX i2cX;

  gI2cExpander = &i2cX;

  gProc->AddSignalEvent(SIGINT, &sigint_handler, gProc);

  void *initDeployEvt=NULL;
  EventManager *events=gProc->event_manager();

  if (initDeployFlag)
  {
    DBG_print(LOG_NOTICE, "Scheduling Initial Deployment in %d seconds",
              initDeployDelayTime);
    initDeployEvt=EVT_sched_add(PROC_evt(gProc->getProcessData()),
                                EVT_ms2tv(initDeployDelayTime*1000),
                                executeInitialDeploymentOp,
                                &i2cX);

  }

  DBG_print(DBG_LEVEL_INFO, "%s Ready to process messages",__FILENAME__);
  
  events->EventLoop();

  //
  // Cleanup
  //
  if (initDeployEvt)
  {
    EVT_sched_remove(PROC_evt(gProc->getProcessData()), initDeployEvt);
  }

  delete gProc;
  gI2cExpander=NULL;

  return status;
}
