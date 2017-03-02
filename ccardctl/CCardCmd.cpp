/*
 * Execute C-card commands
 */
#include <string>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <polysat/polysat.h>
#include <arpa/inet.h>

#include "CCardMsgCodec.h"
#include "CCardI2CPortState.h"
#include "cCardMessages.h"
#include "ccardDefs.h"

/**
 * Wait at most 15 sec for commands to execute
 **/
#define WAIT_MS 15000

/**
 * Status returned if there is a cmd execution error
 **/
#define CMD_ERR_STATUS -1

/**
 * cmdExec usage
 **/
void usage(char *argv[])
{
  std::cout<<"Usage:  "<<argv[0]<<" [options]"<<std::endl
           <<"        Execute a C-Card command"<<std::endl
           <<"        ID's may be:  D1, D2, M1, M2, or M3"<<std::endl
           <<"        Where D=DSA and M=Magnetorquer"<<std::endl
           <<std::endl
           << "Options:"
           <<std::endl
           <<std::endl
           <<" -d {log level}     set log level"<<std::endl
           <<" -h {host IP}       target host IP"<<std::endl
           <<" -s                 get C-Card status"<<std::endl
           <<" -D {DSA id}        Execute DSA Deploy for {DSA id}"<<std::endl
           <<" -R {DSA id}        Execute DSA Release for {DSA id}"<<std::endl
           <<" -T {DSA id}        Reset Timer and DSA state for {DSA id}"<<std::endl
           <<" -M {MT id}-{0|1}   Set MT state of 0 or 1 for {MT id}"<<std::endl
           <<" -m                 Change all mt values (use mask 0x7)"<<std::endl
           <<" -t {timeout ms}    Timeout in ms. (Default "<< WAIT_MS<<")"<<std::endl
           <<" -H                 This message"
           <<std::endl<<std::endl;
  exit(CMD_ERR_STATUS);
}

/**
 * Print out the status
 **/
static void outputStatus(uint8_t status)
{
  std::cout<<"0x"<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)status
           <<" -> "
           <<IrvCS::CCardI2CPortState::stateToString(status)
           <<std::endl;
}

static int getCCardStatus(const std::string &host, uint32_t timeout)
{
  struct
  {
    uint8_t cmd;
    CCardStatus status;
  }__attribute__((packed)) resp;

  struct
  {
    uint8_t cmd;
  }__attribute__((packed)) send;

  send.cmd=1;

  int len;

  // send packet and wait for response
  if ((len = socket_send_packet_and_read_response(host.c_str(), "ccardctl", &send, 
                                                  sizeof(send), &resp,
                                                  sizeof(resp), timeout)) <= 0)
  {
    return len;
  }
 
  if (resp.cmd != CMD_STATUS_RESPONSE)
  {
    std::cerr<<"response code incorrect, Got "<<resp.cmd<<" received  "<<CMD_STATUS_RESPONSE;
    return CMD_ERR_STATUS;
  }

  outputStatus(resp.status.portStatus);

  return 0;
}


static int sendCcardMsg(const std::string &host, uint32_t data, uint32_t timeout)
{
  int status=0;
  struct
  {
    uint8_t cmd;
    CCardStatus status;
  }__attribute__((packed)) resp;

  struct
  {
    uint8_t cmd;
    CCardMsg msg;
  }__attribute__((packed)) send;

  send.cmd=CCARD_CMD;
  send.msg.data=htonl(data);

  int len;

  // send packet and wait for response
  if ((len = socket_send_packet_and_read_response(host.c_str(), "ccardctl", &send, 
                                                  sizeof(send), &resp,
                                                  sizeof(resp), timeout)) <= 0)
  {
    return len;
  }
 
  if (resp.cmd != CCARD_RESPONSE)
  {
    std::cerr<<"response code incorrect, Got "<<resp.cmd
            <<" expected "<< CCARD_RESPONSE<<std::endl;
    return CMD_ERR_STATUS;
  }

  outputStatus(resp.status.portStatus);

  return status;
}

static IrvCS::DsaId parseDsaId(const char *arg)
{
  IrvCS::DsaId dsaId=IrvCS::DSA_UNKNOWN;
  if (!strcmp("D1", arg))
  {
    dsaId=IrvCS::DSA_1;
  } else if (!strcmp("D2", optarg))
  {
    dsaId=IrvCS::DSA_2;
  }
  return dsaId;
}

int addMtBits(const char *arg, uint8_t &mtMask, uint8_t &mtBits)
{
  int status=0;
  int bit=-1;
  int onOrOff=0;
  if (!strncmp("M1", arg, 2))
  {
    bit=0;
  } else if (!strncmp("M2", arg, 2))
  {
    bit=1;
  } else if (!strncmp("M3", arg, 2))
  {
    bit=2;
  } else
  {
    printf("Unrecognized argument format:  %s\n", arg);
    return CMD_ERR_STATUS;
  }
  
  if (strlen(arg) >= 4)
  {
    onOrOff=strtol(&arg[3], NULL, 10);
  }
  if (0 == onOrOff)
  {
    mtBits &= ~(1<<bit);
  } else
  {
    mtBits |= 1<<bit;
  }

  mtMask |= 1<<bit;

  return 0;
}
/**
 * Main function
 **/
int main(int argc, char *argv[])
{
  int opt=0;
  int logLevel=DBG_LEVEL_INFO;
  std::string host="127.0.0.1";
  std::string cmd;
  std::string args;
  uint32_t cmdId=0;
  uint32_t timeout=5000;

  enum Action
  {
    GetStatus,
    DsaCommand,
    MtCommand
  };
  Action action=GetStatus;

  IrvCS::DsaId dsaId;
  IrvCS::DsaCmd dsaCmd;
  uint8_t mtBits=0;
  uint8_t mtMask=0;

  while ((opt=getopt(argc,argv,"d:h:sD:R:T:M:mt:H")) != -1)
  {
    switch (opt)
    {
    case 'd':
      logLevel=strtol(optarg, NULL, 10);
      break;
    case 'D':
      dsaId=parseDsaId(optarg);
      dsaCmd=IrvCS::Deploy;
      action=DsaCommand;
      break;
    case 'R':
      dsaId=parseDsaId(optarg);
      dsaCmd=IrvCS::Release;
      action=DsaCommand;
      break;
    case 'T':
      dsaId=parseDsaId(optarg);
      dsaCmd=IrvCS::ResetTimer;      // Reset timer
      action=DsaCommand;
      break;
    case 'M':
      addMtBits(optarg, mtMask, mtBits);
      action=MtCommand;
      break;
    case 'm':
      mtMask=0x7;               // 
      break;
    case 't':
      timeout=strtol(optarg,NULL, 10);
      break;
    case 'h':
      host=optarg;
      break;
    case 's':
      action=GetStatus;
      break;
    case 'H':
      usage(argv);
      break;
    default:
      usage(argv);
      break;
    }
  }

  if (DsaCommand == action && IrvCS::DSA_UNKNOWN == dsaId)
  {
    printf("Unsupported DSA ID:  %s\n", optarg);
    usage(argv);
  }

  DBG_setLevel(logLevel);

  int status=0;
  uint32_t msgData=0;
  switch(action)
  {
  case GetStatus:
    status=getCCardStatus(host, timeout);
    break;
  case DsaCommand:
    IrvCS::CCardMsgCodec::encodeMsgData((uint8_t)IrvCS::MsgDsa, 
                                        (uint8_t)dsaId, (uint8_t)dsaCmd, 
                                        msgData);
    
    break;
  case MtCommand:
    IrvCS::CCardMsgCodec::encodeMsgData((uint8_t)IrvCS::MsgMt, 
                                        (uint8_t)mtMask, (uint8_t)mtBits, 
                                        msgData);
    break;
  }
  if (action != GetStatus)
  {
    int sendStatus = sendCcardMsg(host, msgData, timeout);
    if (sendStatus != 0)
    {
      status=CMD_ERR_STATUS;
      printf("Error %d sending C-Card Message\n", sendStatus);
    }
  }
  return status;
}
