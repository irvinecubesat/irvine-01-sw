/*
 * Remote command execution utility
 */
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <polysat/polysat.h>
#include <arpa/inet.h>

#include "cmdMessages.h"

/**
 * Wait at most 2 min for commands to execute
 **/
#define WAIT_MS 120000

/**
 * Status returned if there is a cmd execution error
 **/
#define CMD_ERR_STATUS 99

/**
 * cmdExec usage
 **/
void usage(char *argv[])
{
  std::cout<<"Usage:  "<<argv[0]<<"<<%s [options]"<<std::endl
           <<"        Execute a command on the target cmdd"<<std::endl
           <<"        Command will be automatically prefixed with 'cmd'"<<std::endl
           <<std::endl
           << "Options:"
           <<std::endl
           <<std::endl
           <<" -d {log level}     set log level"<<std::endl
           <<" -h {host IP}       target host IP"<<std::endl
           <<" -s                 get cmdd status"<<std::endl
           <<" -c {cmd}           execute command"<<std::endl
           <<" -a {args}          arguments for command.  Quote for multiple arguments"<<std::endl
           <<" -p                 pass through the status, stderr, and stdout as output of this program"<<std::endl
           <<" -i {cmd ID}        Integer ID used to track command execution and return (optional)"<<std::endl
           <<" -t {timeout ms}    Timeout in ms. (Default "<< WAIT_MS<<")"
           <<std::endl<<std::endl;
  exit(CMD_ERR_STATUS);
}

static int getCmdStatus(const std::string &host, uint32_t timeout)
{
  struct
  {
    uint8_t cmd;
    CmdProcessorStatus status;
  }__attribute__((packed)) resp;

  struct
  {
    uint8_t cmd;
  }__attribute__((packed)) send;

  send.cmd=1;

  int len;

  // send packet and wait for response
  if ((len = socket_send_packet_and_read_response(host.c_str(), "cmdd", &send, 
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

  std::cout<<"Cmds:  "<<ntohl(resp.status.cmdCount)<<std::endl;
  std::cout<<"Errs:  "<< ntohl(resp.status.errCount)<<std::endl;
  return 0;
}

static int execCmd(const std::string &host, uint32_t cmdId, const std::string &cmd,
                   const std::string &args, uint32_t timeout, bool passThrough)
{
  int status=0;
  struct
  {
    uint8_t cmd;
    CmdProcessorResp cmdResp;
  }__attribute__((packed)) resp;

  struct
  {
    uint8_t cmd;
    CmdProcessorCmd execCmd;
  }__attribute__((packed)) send;

  send.cmd=CMD_EXEC_CMD;
  send.execCmd.protocolId=htonl(IRV_CMD_PROTO_ID);
  send.execCmd.version=htonl(IRV_CMD_VERSION);
  send.execCmd.cmdId=htonl(cmdId);
  strncpy(send.execCmd.cmd, cmd.c_str(), sizeof(send.execCmd.cmd));
  strncpy(send.execCmd.arg, args.c_str(), sizeof(send.execCmd.arg));

  int len;

  // send packet and wait for response
  if ((len = socket_send_packet_and_read_response(host.c_str(), "cmdd", &send, 
                                                  sizeof(send), &resp,
                                                  sizeof(resp), timeout)) <= 0)
  {
    return len;
  }
 
  if (resp.cmd != CMD_EXEC_RESPONSE)
  {
    std::cerr<<"response code incorrect, Got "<<resp.cmd
            <<" expected "<< CMD_EXEC_RESPONSE<<std::endl;
    return CMD_ERR_STATUS;
  }

  if (IRV_CMD_PROTO_ID != ntohl(resp.cmdResp.protocolId))
  {
    std::cerr<<"Invalid protocolId:  "<<ntohl(resp.cmdResp.protocolId)
             <<" != "<< IRV_CMD_PROTO_ID<<std::endl;
    return CMD_ERR_STATUS;
  }

  if (IRV_CMD_VERSION != ntohl(resp.cmdResp.version))
  {
    std::cerr<<"Incoming protocol version mismatch ("<< ntohl(resp.cmdResp.version) 
        << " != "<< IRV_CMD_VERSION<<")"<<std::endl;
    return CMD_ERR_STATUS;
  }

  if (cmdId != ntohl(resp.cmdResp.cmdId))
  {
    std::cerr<<"Wrong cmd ID received:  "<<ntohl(resp.cmdResp.cmdId) 
             <<" != " <<cmdId <<std::endl;
    return CMD_ERR_STATUS;
  }
  if (passThrough)
  {
    if (strlen(resp.cmdResp.msg)>0)
	{
	  std::cout<<resp.cmdResp.msg;
	} 
    if (strlen(resp.cmdResp.err)> 0)
	{
	  std::cerr<<resp.cmdResp.err;
	}
  }
  else
  {
    std::cout<<"cmd:  "<< resp.cmdResp.cmd<<std::endl;
    status=ntohl(resp.cmdResp.status);
    std::cout<<"status:  "<<status<<std::endl;
      
    if (strlen(resp.cmdResp.msg) > 0)
	{
	  std::cout<<"msg:  "<< resp.cmdResp.msg;
	}
    if (strlen(resp.cmdResp.err)>0)
	{
	  std::cout<<"err:  "<<resp.cmdResp.err;
	}
  }
  return status;
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
  uint32_t timeout=120000;
  bool passThrough=false;		// pass through status, stdout/err to
  // calling program
  enum Action
  {
    ExecCmd,
    GetStatus
  };
  Action action=GetStatus;

  while ((opt=getopt(argc,argv,"d:h:c:a:si:t:p")) != -1)
  {
    switch (opt)
    {
    case 'd':
      logLevel=strtol(optarg, NULL, 10);
      break;
    case 'i':
      cmdId=strtol(optarg, NULL, 10);
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
    case 'c':
      action=ExecCmd;
      cmd=optarg;
      break;
    case 'a':
      args=optarg;
      break;
    case 'p':
      passThrough=true;
      break;
    default:
      usage(argv);
    }
  }
  
  DBG_setLevel(logLevel);

  int status=0;
  switch(action)
  {
  case GetStatus:
    status=getCmdStatus(host, timeout);
    break;
  case ExecCmd:
    status=execCmd(host, cmdId, cmd, args, timeout, passThrough);
    break;
  }
  return status;
}
