#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "Gpio.h"

#include <syslog.h>

/**
 * cmdExec usage
 **/
void usage(char *argv[])
{
  std::cout<<"Usage:  "<<argv[0]<<" [options] gpio"<<std::endl
           <<"        Set or get gpio"<<std::endl
           <<std::endl
           << "Options:"<<std::endl
           <<std::endl
           <<" -s val    set gpio"<<std::endl
           <<" -g        get gpio"<<std::endl
           <<" -e        Print syslog to stderr"<<std::endl
           <<" -h        this message"<<std::endl
           <<std::endl;
  exit(-1);
}

/**
 * Main function
 **/
int main(int argc, char *argv[])
{
  enum Operation
  {
    Set,
    Get,
    Unknown
  };
  Operation operation=Unknown;
  uint32_t value=0;
  int gpio=0;
  int opt=0;
  int logOption=0;
  
  while ((opt=getopt(argc,argv,"hs:ge")) != -1)
  {
    switch (opt)
    {
    case 's':
      operation=Set;
      value=strtoul(optarg, NULL, 10);
      break;
    case 'g':
      operation=Get;
      break;
    case 'e':
      logOption=LOG_PERROR;
      break;
    case 'h':
      usage(argv);
      break;
    default:
      usage(argv);
    }
  }
  if (optind >= argc)
  {
    std::cout<<"Provide gpio after the arguments"<<std::endl;
    usage(argv);
  }
  openlog("gpioUtil", logOption, LOG_USER);
  
  gpio=strtol(argv[optind], NULL, 10);

  IrvCS::Gpio obj(gpio);
  int status=0;

  switch (operation)
  {
  case Get:
    status=obj.get();
    std::cout <<status<<std::endl;
    break;
  case Set:
    status=obj.set(value);
    break;
  case Unknown:
    std::cout<<"No operation specified"<<std::endl;
  }

  if (status >= 0)
  {
    return 0;
  }
  return -1;
  
}
