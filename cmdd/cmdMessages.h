#ifndef __CMD_MESSAGES__
#define __CMD_MESSAGES__

#include <stdint.h>

/*
 * Define maximum lengths of items.  Keep in mind that there is a maximum
 * packet length of 227.  Note that all char buffers are NULL terminated.
 */
#define IRV_CMD_PROTO_ID 0xAE7FCD91
#define IRV_CMD_VERSION 1

#define IRV_CMD_MAX   24
#define IRV_CMD_ARG_MAX 128
#define IRV_MSG_MAX   128
#define IRV_ERR_MAX   64

#define CMD_EXEC_CMD 0x02
#define CMD_EXEC_RESPONSE 0xF2

/*
 * Command execution status values
 */
enum IrvCmdStatus
{
  SUCCESS=0,
  GEN_ERR=1,
  TIMEOUT=2
};
  
extern "C"
{
  
  /**
   * Provide command processor health status.  Keep track of number of
   * commands, errors, etc.  Values get reset when process gets restarted.
   **/
  struct CmdProcessorStatus
  {
    uint32_t cmdCount;
    uint32_t errCount;
  } __attribute__((packed));

  /**
   * Command to execute.  This command does not contain the 
   * interface prefix that we will automatically append to the command
   * to find the executable program/script.
   **/
  struct CmdProcessorCmd
  {
    /**
     * protocol ID ensures that this command is valid and not corrupted
     **/
    uint32_t protocolId;
    uint32_t version;
    uint32_t cmdId;            // sequence number

    char cmd[IRV_CMD_MAX];
    char arg[IRV_CMD_ARG_MAX];
  } __attribute__((packed));

  /**
   * CmdProcessorResponse.
   **/
  struct CmdProcessorResp
  {
    uint32_t protocolId;
    uint32_t version;
    uint32_t cmdId;            // Matches the corresponding cmd's ID
    
    /**
     * Command execution status.  See 
     * 0 - SUCCESS = success, msg contains results (if any)
     * 1 - GEN_ERR = error, msg contains error message
     * 2 - TIMEOUT = timeout - exceeded minimum time for execution
     * Other values may be defined by the particular command
     **/
    int status;
    char cmd[IRV_CMD_MAX];      /// command that was executed
    char msg[IRV_MSG_MAX];      /// results of command execution
    char err[IRV_ERR_MAX];      /// stderr output
  } __attribute__((packed));
}
#endif
