#include <CCardI2CPortState.h>
#include <gtest/gtest.h>

using namespace IrvCS;

/**
 * Test Setting DSA only
 **/
TEST(Initialization,InitialPortState)
{
  CCardI2CPortState portState;
  uint8_t expectedInitialState=0xF|1<<7;
  ASSERT_EQ(expectedInitialState, portState.getState());
  ASSERT_EQ(0, portState.getMtState());
}

TEST(DSAStates,DSA1Release)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0xE; // complement of 0x1 = 1110b
  
  ASSERT_EQ(expectedState, portState.setDsa(DSA_1, Release));
}

TEST(DSAStates, DSA1Deploy)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0xD; // complement of 0x2 = 1101b
  
  ASSERT_EQ(expectedState, portState.setDsa(DSA_1, Deploy));
}

TEST(DSAStates, DSA2Release)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0xB; // complement of 0x4 = 1011b
  
  ASSERT_EQ(expectedState, portState.setDsa(DSA_2, Release));
}

TEST(MTStates, EnableMt1_2_3)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0xFF; // 11111111b
  ASSERT_EQ(expectedState, portState.setMt(0x7,On));
}

TEST(MTStates, EnableMt1)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0x9F; // 10011111b
  ASSERT_EQ(expectedState, portState.setMt(0x1,On));
}

/**
 * Set DSA1 Deploy and MT 3 to On, then turn MT3 off
 **/
TEST(MTandDSAStates, DSA1Deploy_MT3)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0x4D; //01001101b
  portState.setDsa(DSA_1,Deploy);
  ASSERT_EQ(expectedState, portState.setMt(0x4, On));
  expectedState=(uint8_t)0x0D; //00001101b
  ASSERT_EQ(expectedState, portState.setMt(0x4, Off));
  ASSERT_EQ(0, portState.getMtState());
}

/**
 * Test update API
 **/
TEST(UpdateAPI, DSA1DeployMT3On)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0xD; // complement of 0x2 = 00001101b
  
  ASSERT_EQ(expectedState, portState.update(MsgDsa, DSA_1, Deploy));

  uint8_t expectedState2=(uint8_t)0x4D; // 01001101b

  ASSERT_EQ(expectedState2, portState.update(MsgMt, MT_3, 1));
  
}
