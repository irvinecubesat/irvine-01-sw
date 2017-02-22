#include "../CCardMsgCodec.h"
#include <gtest/gtest.h>

using namespace IrvCS;

/*
 * Unit test for CCardMsgCodec
 */

/**
 * Test encoding the Release DSA1 command
 **/
TEST(EncodeMsg, ReleaseDsa1)
{
  CCardMsgCodec codec;
  uint8_t data;
  // expect 0x1 for upper cmd bits and 0x1 for lower bits
  const uint8_t expected= 16;//0x00|0x01<<4 = 0+16;
  ASSERT_EQ(0, codec.encodeMsgData(Release, DSA_1, data));
  ASSERT_EQ(expected, data);
}

/**
 * Test Encode and decode
 **/
TEST(EncodeAndDecodeMsg, DeployDsa2)
{
  CCardMsgCodec codec;
  uint8_t data;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, codec.encodeMsgData(Deploy, DSA_2, data));
  codec.decodeMsgData(data, decodedCmd, decodedId);
  ASSERT_EQ(decodedCmd, Deploy);
  ASSERT_EQ(decodedId, DSA_2);
}
