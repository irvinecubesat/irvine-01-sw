/**
	TODO: 
	-allow more range configuration e.g. 30% > 70% > 20%
	-combine throttleUp and throttleDown into one function using phase shift

**/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

FILE *command;	
char buffer[100];
char address = 0x26;	//thruster i2c slave address
float throttle=0.0;
char msbByte1; 
char lsbByte2;
char mask = 0xFF;


void sendCmd(int val) {	//decompose 10bit data for thruster comamnd into two bytes with appropriate padding bits as specified in datasheet 
	msbByte1 = (mask & (char)(val>>6));
	lsbByte2 = (mask & (char)(val<<2));
	//printf("\r\n cmd1: %02X cmd2: %02X \r\n", cmd1, cmd2); 
	sprintf(buffer, "/usr/sbin/i2cset -y 1 0x%02X 0x%02X 0x%02X", address, msbByte1, lsbByte2);
	//printf("\r\n%s\r\n", buffer);
	command = popen(buffer, "r"); //send the command over i2c

}


void throttleUp(float percent, int t_millis) { //continious throttle up to (p) percent of max power over (t) millis using cyclic function
	int i;
	for(i = 0; i <= t_millis; i+=10) { 
		float s = (float)i/1000.0;
		throttle = 1023.0 * percent * sin((double)(M_PI * (500.0/(float)t_millis) * s));
		//printf("\r\ntime %f throttle %f", s, throttle);  
		sendCmd((int)throttle);
		usleep(10000);
	}
}

void throttleDown(float percent, int t_millis) { //continious throttle down from (p) percent of max power over (t) millis using cyclic function
        int i;
        for(i = 0; i <= t_millis; i+=10) {
                float s = (float)i/1000.0;
                throttle = 1023.0 * percent * cos((double)(M_PI * (500.0/(float)t_millis) * s));
                //printf("\r\ntime %f throttle %f", s, throttle);
                sendCmd((int)throttle);
                usleep(10000);
    }
}

int main() {
	//sendCmd(0x303);
	throttleUp(1.0, 3000);	//throttle up thruster to 100% power over 3 seconds
	throttleDown(1.0, 3000); //throttle down thruster from 100% power over 3 seconds
	return 0;
}


