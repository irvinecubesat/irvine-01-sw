/*
 * Cmd processing daemon.  
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <sstream>
#include <sys/stat.h>

#include <polysat/polysat.h>

#include "cmdMessages.h"

static Process *gProc=NULL;
static std::vector<std::string> gCmdDirectories;
static uint32_t gCmdCount=0;
static uint32_t gErrCount=0;

#define CMD_PREFIX "cmd"
#define DBG_LEVEL_DEBUG DBG_LEVEL_ALL

extern "C"
{
  /**
   * Process the status command
   **/
  void cmdd_status(int socket, unsigned char cmd, void *data, size_t dataLen,
                   struct sockaddr_in *src)
  {
    CmdProcessorStatus status;
    status.cmdCount=htonl(gCmdCount);
    status.errCount=htonl(gErrCount);

    PROC_cmd_sockaddr(gProc->getProcessData(), CMD_STATUS_RESPONSE,
                      &status, sizeof(status), src);
  }

  struct ChildDoneStruct
  {
    CmdProcessorResp *resp;
    struct sockaddr_in *src;      
  };

  /**
   * Child death callback.  Called when the child has exited.
   **/
  void childDoneCb(struct ProcChild *child, void *arg)
  {
    ChildDoneStruct *doneStruct=(ChildDoneStruct *)arg;
    CmdProcessorResp *resp = doneStruct->resp;

    DBG_print(DBG_LEVEL_INFO, "Done executing cmd %s", resp->cmd);
    
    resp->status=htonl(WEXITSTATUS(child->exitStatus));
    gCmdCount++;

    //
    // Send the response
    //
    PROC_cmd_sockaddr(gProc->getProcessData(), CMD_EXEC_RESPONSE,
                      resp, sizeof(*resp), doneStruct->src);
  }

  /**
   * Append child output to message
   **/
  int childStdOutCb(struct ProcChild *child, int lastchance,
                    void *arg, char *buff, int len)
  {
    DBG_print(DBG_LEVEL_DEBUG, "Received Stdout callback (lastchance:  %d)",
      lastchance);
    CmdProcessorResp *resp=(CmdProcessorResp *)arg;
    if (len >= sizeof(resp->msg))
    {
      DBG_print(DBG_LEVEL_WARN, "cmd message has been truncated in the response");
    } else
    {
      resp->msg[len]='\0';
    }
    strncpy(resp->msg, buff, len);
    DBG_print(DBG_LEVEL_DEBUG, "msg is %s", resp->msg);
    if (lastchance == 1)
    {
      CHLD_close_stdin(child); 
    }
    return 0;
  }

  /**
   * Append child error output to message
   **/
  int childStdErrCb(struct ProcChild *child, int lastchance,
                    void *arg, char *buff, int len)
  {
    DBG_print(DBG_LEVEL_DEBUG, "Received Stderr callback");
    CmdProcessorResp *resp=(CmdProcessorResp *)arg;
    if (len >= sizeof(resp->err))
    {
      DBG_print(DBG_LEVEL_WARN, "cmd err has been truncated in the response");
    } else
    {
      resp->err[len]='\0';
    }
    strncpy(resp->err, buff, len);
    if (lastchance == 1)
    {
      CHLD_close_stdin(child);
    }
    return 0;
  }


  /**
   * Process the command.
   **/
  void cmdd_exec_cmd(int socket, unsigned char cmd, void *data, size_t dataLen,
                     struct sockaddr_in *src)
  {
    CmdProcessorCmd *procCmd = (CmdProcessorCmd *)data;
    if (dataLen != sizeof(CmdProcessorCmd))
    {
      DBG_print(DBG_LEVEL_WARN, "Incoming size is incorrect (%d != %d)",
                dataLen, sizeof(CmdProcessorCmd));
      return;
    }
    if (IRV_CMD_PROTO_ID != ntohl(procCmd->protocolId))
    {
      DBG_print(DBG_LEVEL_WARN, "Incoming protocol mismatch (%08x != %08x)",
                procCmd->protocolId, IRV_CMD_PROTO_ID);
      return;
    }
    if (IRV_CMD_VERSION != ntohl(procCmd->version))
    {
      DBG_print(DBG_LEVEL_WARN, "Incoming protocol version mismatch (%d != %d)",
                procCmd->version, IRV_CMD_VERSION);
      return;
    }
    uint32_t cmdId=ntohl(procCmd->cmdId);

    DBG_print(DBG_LEVEL_INFO, "Executing cmd id %d:  %s %s", cmdId,
              procCmd->cmd, procCmd->arg);

    CmdProcessorResp cmdResp;

    ChildDoneStruct doneStruct;
    doneStruct.resp=&cmdResp;
    doneStruct.src=src;

    cmdResp.protocolId=procCmd->protocolId;
    cmdResp.version=procCmd->version;
    cmdResp.cmdId=procCmd->cmdId;
    strncpy(cmdResp.cmd, procCmd->cmd, sizeof(cmdResp.cmd));
    strcpy(cmdResp.msg, "");
    strcpy(cmdResp.err, "");
    cmdResp.status=htonl(1);

    std::string pathToCmd;

    std::vector<std::string>::iterator it;
    for (it = gCmdDirectories.begin(); it != gCmdDirectories.end(); it++)
    {
      std::stringstream cmdStm;
      cmdStm<<(*it)<<"/"<<CMD_PREFIX<<procCmd->cmd;

      // make sure the cmd exists
      struct stat statBuf;
      if (0 == stat(cmdStm.str().c_str(), &statBuf))
      {
        pathToCmd=cmdStm.str();
        break;
      }
    }

    if (pathToCmd.empty())
    {
      std::string msg=std::string(procCmd->cmd)+" not found";
      DBG_print(DBG_LEVEL_FATAL, "%s", msg.c_str());
      gErrCount++;
      strncpy(cmdResp.err, msg.c_str(), sizeof(cmdResp.err));
      PROC_cmd_sockaddr(gProc->getProcessData(), CMD_EXEC_RESPONSE,
                        &cmdResp, sizeof(cmdResp), src);
      return;
    }

    DBG_print(DBG_LEVEL_INFO, "Executing child process:  %s", pathToCmd.c_str());
    ProcChild *childProc=PROC_fork_child(gProc->getProcessData(),
                                         "%s %s", pathToCmd.c_str(),
                                         procCmd->arg);

    if (NULL == childProc)
    {
      std::string msg=pathToCmd+" - Unable to execute";
      DBG_print(DBG_LEVEL_FATAL, "%s", msg.c_str());
      strncmp(cmdResp.err, msg.c_str(), sizeof(cmdResp.err));
      PROC_cmd_sockaddr(gProc->getProcessData(), CMD_EXEC_RESPONSE,
                        &cmdResp, sizeof(cmdResp), src);
    }
    
    CHLD_death_notice(childProc, childDoneCb, &doneStruct);
    CHLD_stdout_reader(childProc, childStdOutCb, &cmdResp);
    CHLD_stderr_reader(childProc, childStdErrCb, &cmdResp);
  }
}

/**
 * cmdd usage
 **/
void usage(char *argv[])
{
  printf("Usage:  %s [options]\n\n", argv[0]);
  printf("        Process commands from network using scripts/executables from\n");
  printf("        cmd directory\n\n");
  printf("Options:\n\n");
  printf(" -D {cmd path} set the colon-separated directory path(s) to execute commands\n" );
  printf(" -d {log level}     set log level\n");
  printf("\n");
  exit(1);
}

static int sigint_handler(int signum, void *arg)
{
  Process *proc=(Process *)arg;
  EVT_exit_loop(PROC_evt(proc->getProcessData()));
  
  return EVENT_KEEP;
}

void parseDirectoryPaths(char *inPaths, std::vector<std::string> &dirs)
{
  char *ptr=inPaths;
  char *item=NULL;

  while (true)
  {
    item = strtok(ptr, ":");
    ptr=NULL;
    if (item != NULL)
    {
      DBG_print(DBG_LEVEL_INFO, "Adding cmd directory:  %s", item);
      dirs.push_back(item);
    } else
    {
      DBG_print(DBG_LEVEL_DEBUG, "item is NULL");
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  int opt=0;
  int logLevel=DBG_LEVEL_INFO;

  while ((opt=getopt(argc,argv,"D:d:")) != -1)
  {
    switch (opt)
    {
    case 'D':
      parseDirectoryPaths(optarg, gCmdDirectories);
      break;
    case 'd':
      logLevel=strtol(optarg, NULL, 10);
      break;
    default:
      usage(argv);
    }
  }
  if (gCmdDirectories.size() == 0)
  {
    const char *msg="Provide the directory path(s) for commands with -D";
    printf("%s\n", msg);
    DBG_print(DBG_LEVEL_FATAL, msg);
    exit(-1);
  }
  DBG_print(DBG_LEVEL_DEBUG, "Creating new process");
  gProc = new Process("cmdd");

  DBG_setLevel(logLevel);

  DBG_print(DBG_LEVEL_DEBUG, "Adding signal event");
  
  gProc->AddSignalEvent(SIGINT, &sigint_handler, gProc);

  EventManager *events=gProc->event_manager();

  DBG_print(DBG_LEVEL_INFO, "cmdd ready to process commands");
  
  events->EventLoop();

  delete gProc;
  
}
