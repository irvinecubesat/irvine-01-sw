#ifndef __CCARD_DEFS_H__
#define __CCARD_DEFS_H__
namespace IrvCS
{
  /**
   * DSA commands to release and deploy
   */
  enum DsaCmd
  {
    Release=1,
    Deploy=2
  };

  /**
   * DSA 1 and DSA 2 ID's.  Values are chosen for encoding/decoding efficiency
   * when writing to the i2c output register.  Only one should be set at a given
   * time
   * 
   * Used as the shift offset.
   */
  enum DsaId
  {
    DSA_1=0,
    DSA_2=2
  };

  /**
   * Magnetorquer Cmd bit is 0 for on and 1 for off
   **/
  enum MtCmd
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
}
#endif  
