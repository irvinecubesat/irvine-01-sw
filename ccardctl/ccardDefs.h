#ifndef __CCARD_DEFS_H__
#define __CCARD_DEFS_H__
namespace IrvCS
{
#define MSG_TYPE_OFFSET_BITS 16
#define MSG_ID_OFFSET_BITS 0
#define MSG_CMD_OFFSET_BITS 8

#define DSA1_RELEASE_STATUS_BIT 0
#define DSA1_DEPLOY_STATUS_BIT  1
#define DSA2_RELEASE_STATUS_BIT 2
#define DSA2_DEPLOY_STATUS_BIT  3

/**
 * Timeout for each release operation in seconds
 **/  
#define TIMEOUT_RELEASE 45

/**
 * Timeout for Emergency Release
 **/
#define TIMEOUT_EMERGENCY_RELEASE 15

/**
 * Wait in seconds between Release retries
 **/
#define RELEASE_WAIT 5
  
/**
 * Timeout for Deploy operation in seconds
 **/
#define TIMEOUT_DEPLOY 10

  enum MsgTypes
  {
    MsgDsa=0,
    MsgMt=1,
    MaxTypes
  };
  
  /**
   * DSA commands to release and deploy
   */
  enum DsaCmd
  {
    Release=0,     
    Deploy=1,
    SetTimer=2,      // set timer bit
    SetTimerOn=3,    // enable timer use
    SetTimerOff=4,   // disable timer use
    ResetTimer=5,  // reset timer and DSA to initial value (1)
    CmdUnknown
  };

  /**
   * DSA 1 and DSA 2 ID's.  Values are chosen for encoding/decoding efficiency
   * when writing to the i2c output register.  Only one should be set at a given
   * time
   * 
   * Used as the shift offset.
   *
   * Note:  Original slide documentation had DSA1 and DSA2 reversed.
   */
  enum DsaId
  {
    DSA_2=0,
    DSA_1=2,
    DSA_UNKNOWN
  };

  /**
   * Magnetorquer state bit is 0 for on and 1 for off
   **/
  enum MtState
  {
    Off=0,
    On=1
  };

  /**
   * Magnetorquer ID is the first 3 bits representing x, y, and z
   * respectively.  Multiple bits may be set at a given time
   **/
  enum MtId
  {
    MT_1=1,
    MT_2=2,
    MT_3=4
  };

  enum CCardError
  {
    CC_OK=0,
    CC_Timeout=-122
  };
}
#endif  
