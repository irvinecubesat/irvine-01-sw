#include <DsaI2CPortState.h>
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
 * Op    ->   S S S S D R D R
 * id    ->           1 1 2 2
 *
 *            0 0 0 0 1 1 1 1       <---  Binary     All "Off"
 *            ^^^^^^^ ^^^^^^^                        Initial State
 *              0x00 +  0xF  = 0x0F <---  Hex
 *
 **/

/**
 * Test Setting DSA only
 **/
TEST(Initialization,InitialPortState)
{
  DsaI2CPortState portState;
  uint8_t expectedInitialState=0xF; // 000001111
  ASSERT_EQ(expectedInitialState, portState.getState());
}

TEST(DSACommand,DSA1Release)
{
  DsaI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x0e; // 00001110
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_1, Release));
}

TEST(DSACommand, DSA1Deploy)
{
  DsaI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x0d; // 00001101
  
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_1, Deploy));
}

TEST(DSACommand, DSA2Release)
{
  DsaI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x0b;  // 00001011
  
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_2, Release));
}

TEST(DSACommand, DSA2Deploy)
{
  DsaI2CPortState portState;
  uint8_t expectedState1=(uint8_t)0x07;  // 00000111
  
  ASSERT_EQ(expectedState1, portState.setDsa(DSA_2, Deploy));
}

/**
 * Do DSA 1 Release/Deploy, then DSA2 Release Deploy
 **/
TEST(DSACommand, DSA1ReleaseDSA2Release)
{
  DsaI2CPortState portState;
  uint8_t expectedStateR1=(uint8_t)0x0e; // 00001110
  uint8_t expectedStateD1=(uint8_t)0x0d; // 00001101
  uint8_t expectedStateR2=(uint8_t)0x0b;     // 00001011
  uint8_t expectedStateD2=(uint8_t)0x07;     // 00000111

  // DSA 1

  ASSERT_EQ(expectedStateR1, portState.setDsa(DSA_1, Release));
  ASSERT_EQ(expectedStateD1, portState.setDsa(DSA_1, Deploy));
  
  // DSA 2
  ASSERT_EQ(expectedStateR2, portState.setDsa(DSA_2, Release));
  ASSERT_EQ(expectedStateD2, portState.setDsa(DSA_2, Deploy));
}




