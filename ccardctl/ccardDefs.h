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
   * when writing to the i2c output register
   * 
   * Used as the shift offset.
   */
  enum DsaId
  {
    DSA_1=0,
    DSA_2=2
  };
}
#endif  
