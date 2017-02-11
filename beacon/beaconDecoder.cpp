#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>

/**
 * Read binary from stdin and output human readable ascii for
 * Beacon decoding
 **/
int main(int argc, char *argv[])
{
  std::stringstream stm;
  unsigned char buf[512]={0};
  int numChars=0;

  
  (void)freopen(NULL, "rb", stdin);
  do
  {
    numChars=read(0, buf, sizeof(buf));
    stm<<buf;
  } while (numChars == sizeof(buf));

  std::cout<<stm.str().c_str()<<std::endl;

}
    
