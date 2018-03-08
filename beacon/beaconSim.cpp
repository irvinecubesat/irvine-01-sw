#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
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

  data.mag[0]=htonl(4294943596);
  data.mag[1]=htonl(4294959196);
  data.mag[2]=htonl(42600);

  data.daughter_aTmpSensor=htonl(18828);
  data.threeV_plTmpSensor=htonl(18820);
  data.tempNz=htonl(12676);
  data.threeVPwrSensor.volt=htonl(217088);
  data.threeVPwrSensor.current=htonl(1312);
  data.fiveV_plPwrSensor.volt=htonl(331776);
  data.fiveV_plPwrSensor.current=htonl(1536);

  unsigned char buf[sizeof(BeaconData)];
  memcpy(buf, &data, sizeof(BeaconData));

  fwrite(buf, 1, sizeof(BeaconData), stdout);
}
