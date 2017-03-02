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
  uint32_t data;
  // expect 0x0 for upper cmd bits and 0x02 for lower bits (DSA_1 is now 02
  const uint8_t expected=2;//0x0000100<<4 = 0;
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MsgDsa, DSA_1, Release, data));
  ASSERT_EQ(expected, data);
}

/**
 * Test Encode and decode DSA command
 **/
TEST(EncodeAndDecodeMsg, DeployDsa2)
{
  uint32_t data;
  uint8_t decodedType=0;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MsgDsa, DSA_2, Deploy, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data,
                                            decodedType, decodedId, decodedCmd));
  ASSERT_EQ(decodedType, MsgDsa);
  ASSERT_EQ(decodedCmd, Deploy);
  ASSERT_EQ(decodedId, DSA_2);
}

TEST(EncodeMsg, MT1On)
{
  uint32_t data;
  uint32_t expectedData=1<<MSG_TYPE_OFFSET_BITS|1<<MSG_ID_OFFSET_BITS
    |1<<MSG_CMD_OFFSET_BITS;
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MsgMt, MT_1, On, data));
  ASSERT_EQ(expectedData, data);
  
}

/**
 * Test Encode and decode MT command
 **/
TEST(EncodeAndDecodeMsg, MT_2_and_3_on)
{
  uint32_t data;
  uint8_t decodedType=0;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MsgMt, MT_2|MT_3, On, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data,
                                            decodedType, decodedId, decodedCmd));
  ASSERT_EQ(decodedType, MsgMt);
  ASSERT_EQ(decodedCmd, On);
  ASSERT_EQ(decodedId, MT_2|MT_3);
}

/**
 * Test Encode and decode MT command
 **/
TEST(EncodeAndDecodeMsg, DsaResetDSA1)
{
  uint32_t data;
  uint8_t decodedType=0;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MsgDsa, DSA_1, ResetTimer, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data,
                                            decodedType, decodedId, decodedCmd));
  ASSERT_EQ(decodedType, MsgDsa);
  ASSERT_EQ(decodedCmd, ResetTimer);
  ASSERT_EQ(decodedId, DSA_1);
}

/**
 * Test Encode and decode MT command
 **/
TEST(EncodeAndDecodeMsg, DsaSetTimerDSA2)
{
  uint32_t data;
  uint8_t decodedType=0;
  uint8_t decodedCmd=0;
  uint8_t decodedId=0;
  
  ASSERT_EQ(0, CCardMsgCodec::encodeMsgData(MsgDsa, DSA_2, SetTimer, data));
  ASSERT_EQ(0, CCardMsgCodec::decodeMsgData(data,
                                            decodedType, decodedId, decodedCmd));
  ASSERT_EQ(decodedType, MsgDsa);
  ASSERT_EQ(decodedCmd, SetTimer);
  ASSERT_EQ(decodedId, DSA_2);
}
