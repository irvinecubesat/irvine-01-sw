#include "../CCardCmdCodec.h"
#include <gtest/gtest.h>

using namespace IrvCS;

/*
 * Unit test for CCardCmdCodec
 */
TEST(EncodeCmd, ReleaseDsa1)
{
  CCardCmdCodec codec;
  uint8_t data;
  // expect 0 for upper cmd bits and 0x1 for lower bits
  const uint8_t expected=0x01|1<<7;
  ASSERT_EQ(0, codec.encodeDsaCmd(Release, DSA_1, data));

  ASSERT_EQ(expected, data);
}
