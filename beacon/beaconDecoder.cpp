#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
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
  const int MAX_BEACON=227;
  unsigned char buf[MAX_BEACON]={0};
  int numChars=0;

  if (MAX_BEACON < sizeof(BeaconData))
  {
    std::cerr<<"Beacon Data must not exceed "<<MAX_BEACON<<" bytes"<<std::endl;
    exit(-1);
  }
  
  FILE *fp=freopen(NULL, "rb", stdin);

  numChars=read(0, buf, sizeof(buf));
  if (numChars > sizeof(BeaconData) )
  {
    std::cout << "Error:  Incoming stream is larger than expected ("
              <<numChars<<")"<<std::endl;
    status=1;
  } else
  {
    BeaconStatus beaconStatus((const BeaconData *)buf);
    std::cout << beaconStatus<<std::endl;
  }

  if (NULL != fp)
  {
    fclose(fp);
  }

  return status;
}
    
