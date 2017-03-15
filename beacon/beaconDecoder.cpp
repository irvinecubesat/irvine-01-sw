#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include "BeaconStatus.h"

using namespace IrvCS;

/**
 * Read binary from stdin and output human readable ascii for
 * Beacon decoding
 **/
int main(int argc, char *argv[])
{
  int status=0;
  std::stringstream stm;
  unsigned char buf[512]={0};
  int numChars=0;

  
  (void)freopen(NULL, "rb", stdin);

  numChars=read(0, buf, sizeof(buf));
  if (numChars == sizeof(buf) || numChars > 227 )
  {
    std::cout << "Error:  Incoming stream is larger than expected ("
              <<numChars<<")"<<std::endl;
    status=1;
  } else
  {
    BeaconStatus beaconStatus((const BeaconData *)buf);
    std::cout << beaconStatus<<std::endl;
  }

  return status;
}
    
