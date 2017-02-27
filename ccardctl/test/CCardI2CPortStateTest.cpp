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
  ASSERT_EQ(expectedState, portState.setMt(0x7,0x7));
}

TEST(MTStates, EnableMt1)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0x9F; // 10011111b
  ASSERT_EQ(expectedState, portState.setMt(0x7,0x1));
}

/**
 * Set DSA1 Deploy and MT 3 to On, then turn MT3 off
 **/
TEST(MTandDSAStates, DSA1Deploy_MT3)
{
  CCardI2CPortState portState;

  //01101101b - set 1st MT off and and 2d and 3rd MT on, 
  uint8_t expectedState=(uint8_t)0x6D; 
  portState.setDsa(DSA_1,Deploy);
  ASSERT_EQ(expectedState, portState.setMt(0x7,0x06));
  expectedState=(uint8_t)0x4D; //01001101b - turn off 2rd MT bit only
  ASSERT_EQ(expectedState, portState.setMt(0x2,0x00));
  ASSERT_EQ(0x4, portState.getMtState());
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

  ASSERT_EQ(expectedState2, portState.update(MsgMt, 0x7, 0x04));
  
}

TEST(PrintBinary, MTValues)
{
  CCardI2CPortState portState;
  ASSERT_EQ(CCardI2CPortState::printBinary(7,3), "111");
  ASSERT_EQ(CCardI2CPortState::printBinary(6,3), "110");
  ASSERT_EQ(CCardI2CPortState::printBinary(4,3), "100");
  ASSERT_EQ(CCardI2CPortState::printBinary(3,3), "011");
  ASSERT_EQ(CCardI2CPortState::printBinary(2,3), "010");
  ASSERT_EQ(CCardI2CPortState::printBinary(1,3), "001");
}
