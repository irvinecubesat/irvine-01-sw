#include <CCardI2CPortState.h>
#include <gtest/gtest.h>

using namespace IrvCS;
/**
 * I2C Expander Register 1 bits (see CCardI2CPortState.h for definitions).
 *
 * DSA and Timer bits are Active Low --> "On" is bit set to 0 
 * MT bits are Active High           --> "On" is bit set to 1
 *
 *  |Op Key   | Description   |
 *  |---------|---------------|
 *  |T        | Timer Enable  |
 *  |M        | Magnetorquer  |
 *  |R        | Release DSA   |
 *  |D        | Deploy DSA    |
 *  |---------|---------------|
 *
 *          MSB             LSB
 * Bit   ->   7 6 5 4 3 2 1 0
 * Op    ->   T M M M D R D R
 * id    ->     3 2 1 1 1 2 2
 *
 *            1 0 0 0 1 1 1 1       <---  Binary     All "Off"
 *            ^^^^^^^ ^^^^^^^                        Initial State
 *              0x80 +  0xF  = 0x8F <---  Hex
 *
 **/

/**
 * Test Setting DSA only
 **/
TEST(Initialization,InitialPortState)
{
  CCardI2CPortState portState;
  uint8_t expectedInitialState=0xF|1<<7; // 10001111
  ASSERT_EQ(expectedInitialState, portState.getState());
  ASSERT_EQ(0, portState.getMtState());
}

TEST(DSACommand,DSA1Release)
{
  CCardI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x8b; // 10001011
  uint8_t expectedState2=(uint8_t)0xb;  // 00001011
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_1, Release));
  ASSERT_EQ(expectedState2, portState.setDsa(DSA_1, SetTimer));
}

TEST(DSACommand, DSA1Deploy)
{
  CCardI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x87; // 10000111
  uint8_t expectedState2=(uint8_t)0x7;  // 00000111
  
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_1, Deploy));
  ASSERT_EQ(expectedState2, portState.setDsa(DSA_1, SetTimer));
}

TEST(DSACommand, DSA2Release)
{
  CCardI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x8e;  // 10001110
  uint8_t expectedState2=(uint8_t)0xe;   // 00001110
  
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_2, Release));
  ASSERT_EQ(expectedState2, portState.setDsa(DSA_2, SetTimer));
}

/**
 * Do DSA 1 Release/Deploy, then DSA2 Release Deploy
 **/
TEST(DSACommand, DSA1ReleaseDSA2Release)
{
  CCardI2CPortState portState;
  uint8_t expectedStateR1=(uint8_t)0x8b; // 10000011
  uint8_t expectedStateR1Timer=(uint8_t)0xb;  // 00001011
  uint8_t expectedStateD1=(uint8_t)0x87; // 10000011
  uint8_t expectedStateD1Timer=(uint8_t)0x7;
  uint8_t expectedStateR2=(uint8_t)0x8e;     // 10001110
  uint8_t expectedStateR2Timer=(uint8_t)0xe; // 00001110
  uint8_t expectedStateD2=(uint8_t)0x8d;      // 10001101
  uint8_t expectedStateD2Timer=(uint8_t)0xd;  // 00001101

  // DSA 1
  ASSERT_EQ(expectedStateR1, portState.setDsa(DSA_1, Release));
  ASSERT_EQ(expectedStateR1Timer, portState.setDsa(DSA_1, SetTimer));
  ASSERT_EQ(expectedStateD1, portState.setDsa(DSA_1, Deploy));
  ASSERT_EQ(expectedStateD1Timer, portState.setDsa(DSA_1, SetTimer));
  
  // DSA 2
  ASSERT_EQ(expectedStateR2, portState.setDsa(DSA_2, Release));
  ASSERT_EQ(expectedStateR2Timer, portState.setDsa(DSA_2, SetTimer));
  ASSERT_EQ(expectedStateD2, portState.setDsa(DSA_2, Deploy));
  ASSERT_EQ(expectedStateD2Timer, portState.setDsa(DSA_2, SetTimer));
}

/**
 * Test reseting DSA1.  First set DSA1 to Release, then reset it
 **/
TEST(DSACommand, DSA1ReleaseThenReset)
{
  CCardI2CPortState portState;
  const uint8_t expectedState=(uint8_t)0x8b;   // complement of 0x0100 = 1011
  const uint8_t expectedState2=(uint8_t)0x0b;   // complement of 0x0100 = 1011
  const uint8_t originalState=(uint8_t)0x8F; // 10001111b - original state
  ASSERT_EQ(expectedState, portState.setDsa(DSA_1, Release));
  ASSERT_EQ(expectedState2, portState.setDsa(DSA_1, SetTimer));
  ASSERT_EQ(originalState, portState.setDsa(DSA_1, ResetTimer));
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
 * Set DSA2 Deploy and MT 3 to On, then turn MT3 off
 **/
TEST(MTandDSACommand, DSA1Deploy_MT3)
{
  CCardI2CPortState portState;

  //01101101b - set 1st MT off and and 2d and 3rd MT on, 
  uint8_t expectedState=(uint8_t)0x6D; 
  portState.setDsa(DSA_2,Deploy);
  portState.setDsa(DSA_2,SetTimer);
  ASSERT_EQ(expectedState, portState.setMt(0x7,0x06));
  expectedState=(uint8_t)0x4D; //01001101b - turn off 2rd MT bit only
  ASSERT_EQ(expectedState, portState.setMt(0x2,0x00));
  ASSERT_EQ(0x4, portState.getMtState());
}

/**
 * Test DSA and MT ops
 **/
TEST(DSAandMT, DSA2DeployMT3On)
{
  CCardI2CPortState portState;
  uint8_t expectedState=(uint8_t)0x8D; // 10001101b
  uint8_t expectedState2=(uint8_t)0xD; // 00001101b
  
  ASSERT_EQ(expectedState, portState.setDsa(DSA_2, Deploy));
  ASSERT_EQ(expectedState2, portState.setDsa(DSA_2, SetTimer));

  uint8_t expectedState3=(uint8_t)0x4D; // 01001101b

  ASSERT_EQ(expectedState3, portState.setMt(0x7, 0x04));
  
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
