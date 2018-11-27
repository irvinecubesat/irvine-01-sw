#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "BeaconStatus.h"

using namespace IrvCS;

void usage()
{
  std::cout <<"Usage:  beaconDecoder [options]\n\n"<<
    "Options:\n"<<
    " -x  Read input in hex \n"<<std::endl;
}

#define MAX_BEACON 227

/**
 * Read the binary beacon data
 **/
int readBinData(unsigned char *buf, uint32_t size)
{
  FILE *fp=freopen(NULL, "rb", stdin);
  if (NULL == fp)
  {
    return -1;
  }
  int numChars=0;

  numChars=read(0, buf, size);
  if (numChars > sizeof(BeaconData) )
  {
    std::cout << "Error:  Incoming stream is larger than expected ("
              <<numChars<<")"<<std::endl;
    return -1;
  }

  fclose(fp);

  return 0;
}

/**
 * Read the hex ascii beacon data
 **/
int readHexData(unsigned char *buf, uint32_t size)
{
  FILE *fp=freopen(NULL, "r", stdin);
  if (NULL == fp)
  {
    return -1;
  }
  int bufPos=0;
  char inBuf[2]={0};
  int counter=0;
  
  long byteVal=0;
  while (read(0, inBuf, 1) != 0)
  {
    if (isxdigit(inBuf[0]))
    {
      unsigned long val=strtoul(inBuf, NULL, 16);
      if (counter%2)
      {
        byteVal|=val;
        buf[bufPos]=byteVal;
        ++bufPos;
        if (bufPos == size)
        {
          break;
        }
      } else
      {
        byteVal=val<<4;
      }
      counter++;
    }
  }

  fclose(fp);
  if (bufPos != size)
  {
    std::cout <<"Did not read enough characters for beacon: "
              <<bufPos<<" != "<<size<<std::endl;
    return -1;
  }

  return 0;
}

/**
 * Read binary from stdin and output human readable ascii for
 * Beacon decoding
 **/
int main(int argc, char *argv[])
{
  int status=0;
  int opt=0;
  bool hexInput=false;
  
  while ((opt = getopt(argc, argv, "xih")) != -1) {
    switch (opt)
    {
    case 'x':
      hexInput=true;
      break;
    case 'h':
      usage();
      exit(0);
      break;
    default:
      usage();
      exit(1);
    }
  }
  
  std::stringstream stm;
  unsigned char buf[sizeof(BeaconData)]={0};

  if (MAX_BEACON < sizeof(BeaconData))
  {
    std::cerr<<"Beacon Data must not exceed "<<MAX_BEACON<<" bytes"<<std::endl;
    exit(-1);
  }

  if (!hexInput)
  {
    if (0 != readBinData(buf, sizeof(buf)))
    {
      exit(1);
    }
  } else
  {
    if (0 != readHexData(buf, sizeof(buf)))
    {
      exit(1);
    }
  }

  BeaconStatus beaconStatus((const BeaconData *)buf);
  std::cout << beaconStatus<<std::endl;


  return status;
}
    
