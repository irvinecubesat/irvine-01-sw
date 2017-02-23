#include <CCardMsgCodec.h>
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
  uint8_t data;
  // expect 0x1 for upper cmd bits and 0x1 for lower bits
  const uint8_t expected= 16;//0x00|0x01<<4 = 0+16;
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(Release, DSA_1, data));
  ASSERT_EQ(expected, data);
}

/**
 * Test Encode and decode DSA command
 **/
TEST(EncodeAndDecodeMsg, DeployDsa2)
{
  uint8_t data;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(Deploy, DSA_2, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data, decodedCmd, decodedId));
  ASSERT_EQ(decodedCmd, Deploy);
  ASSERT_EQ(decodedId, DSA_2);
}

TEST(EncodeMsg, MT1On)
{
  uint8_t data;
  uint8_t expectedData=16+1; // binary 00010001
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(On, MT_1, data));
  ASSERT_EQ(expectedData, data);
  
}

/**
 * Test Encode and decode MT command
 **/
TEST(EncodeAndDecodeMsg, MT_2_and_3_on)
{
  uint8_t data;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(On, MT_2|MT_3, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data, decodedCmd, decodedId));
  ASSERT_EQ(decodedCmd, On);
  ASSERT_EQ(decodedId, MT_2|MT_3);
}
