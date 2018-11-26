#include <arpa/inet.h>
#include "BeaconStatus.h"
#include <iostream>

// angular rate 12/20
#define GYRO_RAW_TO_DEGSEC(raw) ((int32_t)ntohl(raw))/(float(1<<20));

// Temperature 10/6
#define TEMP_RAW_TO_KELVIN(raw) ((int32_t)ntohs(raw))/(float)(1<<6)

// Voltage 16/16
#define V_RAW_TO_VOLTS(raw) ((int32_t)ntohl(raw))/(float(1<<16))

// Amps 15/16
#define A_RAW_TO_AMPS(raw) ((int32_t)ntohl(raw))/(float(1<<16))

namespace IrvCS
{
  BeaconStatus::BeaconStatus(const BeaconData *data)
  {
    id_=data->id;
    ldc_=ntohs(data->ldc);

    for(int i = 0; i < 3; i++)
    {
      gyro_[i] = GYRO_RAW_TO_DEGSEC(data->gyro[i]);
    }
    for(int i = 0; i < 3; i++)
    {
      mag_[i] = ((int32_t)ntohl(data->mag[i]));
    }

    tempDaughterA_ = TEMP_RAW_TO_KELVIN(data->daughter_aTmpSensor);    
    tempThreeVpl_ = TEMP_RAW_TO_KELVIN(data->threeV_plTmpSensor);
    tempNz_ = TEMP_RAW_TO_KELVIN(data->tempNz); 
    volt3V_ = V_RAW_TO_VOLTS(data->threeVPwrSensor.volt);
    curr3V_ = A_RAW_TO_AMPS(data->threeVPwrSensor.current);
    volt5Vpl_ = V_RAW_TO_VOLTS(data->fiveV_plPwrSensor.volt);
    curr5Vpl_ = A_RAW_TO_AMPS(data->fiveV_plPwrSensor.current);
  }

  BeaconStatus::~BeaconStatus()
  {
  }

/**
 * Output Beacon Status in human friendly attr = value form
 **/
  std::ostream& operator<<(std::ostream &os, const BeaconStatus &beaconStatus)
  {
    return os<<"ID="<<beaconStatus.id_<<std::endl
             <<"LDC="<<beaconStatus.ldc_<<std::endl
             <<"Gyro_X="<<beaconStatus.gyro_[0]<<std::endl
             <<"Gyro_Y="<<beaconStatus.gyro_[1]<<std::endl
	     <<"Gyro_Z="<<beaconStatus.gyro_[2]<<std::endl 
             <<"Mag_X="<<beaconStatus.mag_[0]<<std::endl 
             <<"Mag_Y="<<beaconStatus.mag_[1]<<std::endl 
             <<"Mag_Z="<<beaconStatus.mag_[2]<<std::endl 
	     <<"Temp_K="<<beaconStatus.tempDaughterA_<<std::endl
	     <<"3V_Payload_Temp_C="<<beaconStatus.tempThreeVpl_<<std::endl
	     <<"External_Temp_C="<<beaconStatus.tempNz_<<std::endl
	     <<"3V_Sensor_Voltage="<<beaconStatus.volt3V_<<std::endl
	     <<"3V_Sensor_Current="<<beaconStatus.curr3V_<<std::endl
	     <<"5V_Payload_Voltage="<<beaconStatus.volt5Vpl_<<std::endl 
	     <<"5V_Payload_Current="<<beaconStatus.curr5Vpl_<< std::endl 

	     ;
  }
  
}
