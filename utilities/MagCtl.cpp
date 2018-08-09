#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>

int bus;
char *busAdr = "/dev/i2c-1";
char device = 0x39;
char buffer[2] = {0}; 
char enable = 0x10; //only MT01 FWD
int dutyCycle = 2;
int pulseCycle = 1;
int count = 0;

void startI2C() {
	if((bus = open(busAdr, O_RDWR))<0) perror("Failed to open bus");
	if(ioctl(bus, I2C_SLAVE, device) < 0) perror("Failed to find device");
}

void i2cwrite() {
	if(write(bus, buffer, 2) != 1);// printf("Failed to write");
}

int main() {
	startI2C();		//enable line
	buffer[0] = 0x03; 	//set register to Config 
	buffer[1] = 0x00;	//set all pins to outputs
	i2cwrite();		//send setting
	buffer[0] = 0x01;	//set register to Output ports for all other cmds
	
	while(1) { 		//generate software PWM with I2C at 400 KBPS
		count++;
		if(count%dutyCycle < pulseCycle) buffer[1] = enable; 
		else buffer[1] = 0x00;

		i2cwrite();
		//usleep(10000);
	}

	return 0; 
}
