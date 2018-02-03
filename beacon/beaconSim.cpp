#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "BeaconStatus.h"

using namespace IrvCS;
/**
 * Simulated Beacon generator
 **/
int main(int argc, char *argv[])
{
  BeaconData data;
  // clear everything out
  memset(&data, 0, sizeof(BeaconData));
  
  strncpy(data.id, "IRV-02", strlen("IRV-02"));

  data.ldc=12345;
  data.gyro[0]=3956343040;
  data.gyro[1]=512033280;
  data.gyro[2]=2576942336;

  // TODO:  Populate with actual telemetry data
  data.mag[0]=12344333;
  data.mag[1]=8827373;
  data.mag[2]=1823838;;

  data.daughter_aTmpSensor=828;
  data.threeV_plTmpSensor=12323;
  data.threeV_plTmpSensor=34322;
  data.fiveV_plPwrSensor.volt=8273;
  data.fiveV_plPwrSensor.current=746;

  unsigned char buf[sizeof(BeaconData)];
  memcpy(buf, &data, sizeof(BeaconData));

  fwrite(buf, 1, sizeof(BeaconData), stdout);
}
