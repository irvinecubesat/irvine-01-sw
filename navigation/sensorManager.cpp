/* TODO 
* -Add integration functions for gyro and accelerometer
* -Add CMake files for autobuild with toolchain 
* -Remove global structs 
* -Add frontend bash script for value retrieval and feedback
* -Add more comments
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <iostream>
#include <string>

//specify size of buffer where output from command will be stored
const int BUFFER_SIZE = 2048;
double RAW_VALUES[9];

//create structs for storage value readouts and future values
struct Accelerometer {
	double x;
	double y;
	double z;
	
	//TODO: Use trapazoid rule to calculate integrated values over x sample period
	double integ_x;
	double integ_y;
	double integ_z;
} accelValues;

struct Gyro {
	double x;
	double y;
	double z;

	double integ_x;
	double integ_y;
	double integ_z;
} gyroValues;

struct Magetometer {
	double x;
	double y;
	double z;
} magValues;


//store RAW_VALUES into appropriate structs for readability purposes
void store() {
	accelValues.x = RAW_VALUES[0];
	accelValues.y = RAW_VALUES[1];
	accelValues.z = RAW_VALUES[2];
	
	gyroValues.x = RAW_VALUES[3];
	gyroValues.y = RAW_VALUES[4];
	gyroValues.z = RAW_VALUES[5];
	
	magValues.x = RAW_VALUES[6];
	magValues.y = RAW_VALUES[7];
	magValues.z = RAW_VALUES[8];
}

//Get methods for values
double getAccelX() {
	std::cout << accelValues.x << std::endl;
	return accelValues.x;
}
double getAccelY() {
	std::cout << accelValues.y << std::endl;
	return accelValues.y;
}
double getAccelZ() {
	std::cout << accelValues.z << std::endl;
	return accelValues.z;
}

/*
	double getIntegAccelX(double time, double sampleRate) { 
		... 
		return accelValues.integ_x;
	}
*/

double getGyroX() {
	std::cout << gyroValues.x << std::endl;
	return gyroValues.x;
}
double getGyroY() {
	std::cout << gyroValues.y << std::endl;
	return gyroValues.y;
}
double getGyroZ() {
	std::cout << gyroValues.z << std::endl;
	return gyroValues.z;
}

double getMagX() {
	std::cout << magValues.x << std::endl;
	return magValues.x;
}
double getMagY() {
	std::cout << magValues.y << std::endl;
	return magValues.y;
}
double getMagZ() {
	std::cout << magValues.z << std::endl;
	return magValues.z;
}


//entry point
int main(int argc, char *argv[]) {
	//Run adcs-sensor status to get raw readouts and store to FILE pointer
	FILE *command; 
	//printf("[LOG]: Init \n");
	command = popen("/usr/bin/adcs-sensors-status", "r");
	
	//On failure, return error code
	if(command == NULL) {
		printf("[ERROR]: Can't retrieve sensor values \n");
		return 1;
	}
	//printf("[LOG]: Output from adcs-sensor-status \n");
	
	//Read through output file and split into string array deliminated by newline char
	char buffer[BUFFER_SIZE];
	std::string lines[9];
	int i = 0;
	while(fgets(buffer, sizeof(buffer)-1, command) != NULL) {
		lines[i] = buffer;
		//std::cout << lines[i];
		i++;
	}
	
	//Loop through each line and pull out the raw value 
	//std::cout<< "[LOG]: Raw Values: " << std::endl;
	for(int n = 0; n < 9; n++) {
		bool flag = false;
		int b = 0;
		std::string s = "";
		for(int i = 0; i < lines[n].length(); i++) {
			if(lines[n][i] == '=') {
			 	flag = true;
				continue;
			}
			if(flag == true && lines[n][i] == ' ') flag = false;
			if(flag != true) continue;
			s+= lines[n][i];
		}
		RAW_VALUES[n] = atof(s.c_str());	
	}


	//Print raw values
	//for(int i = 0; i < 9; i++) {
	//	std::cout << RAW_VALUES[i] << std::endl;
	//}

	store();

	//std::cout << std::endl;
	//Print values requested by command line args	
	for(int i = 1; i < argc; i++) {
		try {
			std::cout << RAW_VALUES[atoi(argv[i])] << std::endl;

		} catch(int e) {
			std::cout << "[ERROR]: Bad input params, failed." << std::endl;
		} 
	}

	return 0;
}


