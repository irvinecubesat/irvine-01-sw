/*
 * Remote command execution utility
 */
#include <string>
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
 * cmdExec usage
 **/
void usage(char *argv[])
{
  printf("Usage:  %s [options]\n\n", argv[0]);
  printf("        Execute a command on the target cmdd\n");
  printf("        Command will be automatically prefixed with 'cmd'\n");
  printf("\n");
  printf("Options:\n\n");
  printf(" -d {log level}     set log level\n");
  printf(" -h {host IP}       target host IP\n");
  printf(" -s                 get cmdd status\n");
  printf(" -c {cmd}           execute command\n");
  printf(" -a {args}          arguments for command.  Quote for multiple arguments\n");
  printf(" -i {cmd ID}        Integer ID used to track command execution and return (optional)\n");
  printf(" -t {timeout ms}    Timeout in ms.  Default 120000 (2 min)\n");
  printf("\n");
  exit(1);
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
    printf("response code incorrect, Got 0x%02X expected 0x%02X\n", 
           resp.cmd, CMD_STATUS_RESPONSE);
    return 5;
   }

  printf("Cmds:  %d\n", ntohl(resp.status.cmdCount));
  printf("Errs:  %d\n", ntohl(resp.status.errCount));
  return 0;
}

static int execCmd(const std::string &host, uint32_t cmdId, const std::string &cmd,
                   const std::string &args, uint32_t timeout)
{
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
    printf("response code incorrect, Got 0x%02X expected 0x%02X\n", 
           resp.cmd, CMD_EXEC_RESPONSE);
    return 5;
   }

  if (IRV_CMD_PROTO_ID != ntohl(resp.cmdResp.protocolId))
  {
    printf("Invalid protocolId:  %08x != %08x\n",
           ntohl(resp.cmdResp.protocolId),
           IRV_CMD_PROTO_ID);
    return 5;
  }

  if (IRV_CMD_VERSION != ntohl(resp.cmdResp.version))
  {
    printf("Incoming protocol version mismatch (%d != %d)\n",
           ntohl(resp.cmdResp.version), IRV_CMD_VERSION);
    return 5;
  }

  if (cmdId != ntohl(resp.cmdResp.cmdId))
  {
    printf("Wrong cmd ID received:  %d != %d.\n",
           ntohl(resp.cmdResp.cmdId), cmdId);
    return 5;
  }
  printf("cmd:  %s\n", resp.cmdResp.cmd);
  printf("status:  %d\n", ntohl(resp.cmdResp.status));
  if (0 == resp.cmdResp.status)
  {
    printf("msg:  %s\n", resp.cmdResp.msg);
  } else
  {
    printf("msg:  %s\n", resp.cmdResp.msg);
    printf("err:  %s\n", resp.cmdResp.err);
  }
  return resp.cmdResp.status;
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
  enum Action
  {
    ExecCmd,
    GetStatus
  };
  Action action=GetStatus;

  while ((opt=getopt(argc,argv,"d:h:c:a:si:t:")) != -1)
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
    status=execCmd(host, cmdId, cmd, args, timeout);
    break;
  }
  return status;
}
