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
  // expect 0x0 for upper cmd bits and 0x0 for lower bits
  const uint8_t expected= 0;//0x00|0x00<<4 = 0;
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(DSA_1, Release, data));
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
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(DSA_2, Deploy, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data, decodedId, decodedCmd));
  ASSERT_EQ(decodedCmd, Deploy);
  ASSERT_EQ(decodedId, DSA_2);
}

TEST(EncodeMsg, MT1On)
{
  uint8_t data;
  uint8_t expectedData=16+1; // binary 00010001
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MT_1, On, data));
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
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MT_2|MT_3, On, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data, decodedId, decodedCmd));
  ASSERT_EQ(decodedCmd, On);
  ASSERT_EQ(decodedId, MT_2|MT_3);
}
