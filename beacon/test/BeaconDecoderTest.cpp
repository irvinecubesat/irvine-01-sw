#include <arpa/inet.h>
#include <BeaconStatus.h>
#include <gtest/gtest.h>

using namespace IrvCS;

/*
 * Unit tests for beacon decoding via the BeaconStatus class.  
 *
 * Note: the incoming buffer is in network byte order.  The test data
 * used for this test is in host byte order.  So we need to convert
 * any long (32-bit) integers tob network byte order using the htonl
 * function (host to network long) andb any short (16-bit) integers
 * using the htons function (host to network short).  Char arrays do
 * not need to be converted since each array element is 1 byte and
 * there is no numeric conversion needed.
 *
 * Type "man htonl" on your development machine to learn more.
 */

/**
 * Test decoding the ID and LDC
 **/
TEST(BeaconDecode, DecodeIDandLDC)
{
  /*
   * Create beacon data in network byte order
   */
  BeaconData beaconData;
  strcpy(beaconData.id, BEACON_ID);
  beaconData.ldc=htons(100);

  BeaconStatus bStatus(&beaconData);

  ASSERT_EQ(bStatus.id_, "IRV-02");
  ASSERT_EQ(bStatus.ldc_, 100);
}

/**
 * test decoding gyro.  Retrieve telemetry data from adcs-sensor-telemetry
 * for this test and compare with adcs-sensor-status output
 **/
TEST(BeaconDecode, DecodeGyro)
{
  BeaconData beaconData;

  beaconData.gyro[0]=htonl(393216);
  beaconData.gyro[1]=htonl(4294443008);
  beaconData.gyro[2]=htonl(1572864);

  BeaconStatus bStatus(&beaconData);

  ASSERT_NEAR(bStatus.gyro_[0], 0.375, 0.00001);
  ASSERT_NEAR(bStatus.gyro_[1], -0.5, 0.00001);
  ASSERT_NEAR(bStatus.gyro_[2], 1.5, 0.00001);
}

/**
 * Test decoding magnetometer data.
 *
 * Retrieve telemetry data from adcs-sensor-telemetry
 * for this test and compare with adcs-sensor-status output *
 **/
TEST(BeaconDecode, DecodeMag)
{
  BeaconData beaconData;

  beaconData.mag[0]=htonl(393216);
  beaconData.mag[1]=htonl(4294443008);
  beaconData.mag[2]=htonl(1572864);

  BeaconStatus bStatus(&beaconData);

  ASSERT_NEAR(bStatus.mag_[0], 0.375, 0.00001);
  ASSERT_NEAR(bStatus.mag_[1], -0.5, 0.00001);
  ASSERT_NEAR(bStatus.mag_[2], 1.5, 0.00001);
  
}

/**
 * Test decoding temperature variables
 **/
TEST(BeaconDecode, DecodeTemp)
{
  BeaconData beaconData;

  beaconData.daughter_aTmpSensor = htons(18788);
  beaconData.threeV_plTmpSensor = htons(18820);
  beaconData.tempNz = htons(18724);
  BeaconStatus bStatus(&beaconData);

  ASSERT_NEAR(bStatus.tempDaughterA_,20.5, 0.00001);
  ASSERT_NEAR(bStatus.tempThreeVpl_,21.000000, 0.00001);
  ASSERT_NEAR(bStatus.tempNz_,19.5, 0.00001);

}

/**
 * test decoding power - voltage and current variables
 * @TODO create the data for this test
 **/
TEST(BeaconDecode, DecodePower)
{
  BeaconData beaconData;
  beaconData.threeVPwrSensor.volt=htonl(217088);
  beaconData.threeVPwrSensor.current=htonl(1312);
  beaconData.fiveV_plPwrSensor.volt=htonl(331776);
  beaconData.fiveV_plPwrSensor.current=htonl(1536);
    
  BeaconStatus bStatus(&beaconData);

  ASSERT_NEAR(bStatus.volt3V_,3.3125, 0.00001);
  ASSERT_NEAR(bStatus.curr3V_,0.039062, 0.00001);
  ASSERT_NEAR(bStatus.volt5Vpl_,5.0625, 0.00001);
  ASSERT_NEAR(bStatus.curr5Vpl_,0.023438, 0.00001);
  
}
