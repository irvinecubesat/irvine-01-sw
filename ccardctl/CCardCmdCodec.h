#ifndef __CCARD_CMD_CODEC_HH__
#define __CCARD_CMD_CODEC_HH__
#include <stdint.h>
namespace IrvCS
{
  enum DsaCmd
  {
    Release=1,
    Deploy=2
  };

  /**
   * DSA 1 and DSA 2 ID's
   */
  enum DsaId
  {
    DSA_1=0,
    DSA_2=2
  };
  
  /**
   * Encoder/decoder for the C Card controller cmd messages.
   * Provides utilities to generate commands and process them.  
   *
   * Also provides utility functions to decode the results of queries into
   * human readable format.
   *
   **/
  class CCardCmdCodec
  {
  public:
    CCardCmdCodec();
    virtual ~CCardCmdCodec();

    /**
     * Create the DSA command data with programmer-friendly API.
     * @param cmd the DSA command to execute
     * @param id The ID of the DSA to execute the command on
     * @return 0 for success, nonzero otherwise
     **/
    uint8_t encodeDsaCmd( const DsaCmd cmd, const DsaId id, uint8_t &data);

    /**
     * Decode the DSA command data into programmer-friendly info.
     * @param data the incoming data
     * @param cmd the dsa command to extract
     * @param id the affected DSA ID
     **/
    uint8_t decodeDsaCmd(const uint8_t data, DsaCmd &cmd, DsaId &id);
    
  private:
  };
  
}

#endif
