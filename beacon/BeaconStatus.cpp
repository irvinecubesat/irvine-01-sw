#include <arpa/inet.h>
#include "BeaconStatus.h"

namespace IrvCS
{
  BeaconStatus::BeaconStatus(const BeaconData *data)
  {
    id_=data->id;
    ldc_=ntohs(data->ldc);
    
    for(int i = 0; i < 3; i++) {
	gyro_[i] = ((int32_t)ntohl(data->gyro[i]))/(1024.0*1024.0);
    }
    for(int i = 0; i < 3; i++) {
	mag_[i] = ((int32_t)ntohl(data->mag[i]));
    }
    tempDaughterA_ = (int32_t)ntohl(data->daughter_aTmpSensor); 
    tempThreeVpl_ = (int32_t)ntohl(data->threeV_plTmpSensor);
    tempNz_ = (int32_t)ntohl(data->tempNz); 
    volt3V_ = (int32_t)ntohl(data->threeVPwrSensor.volt);
    curr3V_ = (int32_t)ntohl(data->threeVPwrSensor.current);
    volt5Vpl_ = (int32_t)ntohl(data->fiveV_plPwrSensor.volt);
    curr5Vpl_ = (int32_t)ntohl(data->fiveV_plPwrSensor.current);
    //
    // @TODO convert the rest of the members to host byte order and appropriate
    //       units.  See adc-sensors-util for example of int/uint->float
    //       conversion.
    //
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
	     <<"Temp_C="<<beaconStatus.tempDaughterA_<<std::endl
	     <<"3V_Payload_Temp_C="<<beaconStatus.tempThreeVpl_<<std::endl
	     <<"External_Temp_C="<<beaconStatus.tempNz_<<std::endl
	     <<"3V_Sensor_Voltage="<<beaconStatus.volt3V_<<std::endl
	     <<"3V_Sensor_Current="<<beaconStatus.curr3V_<<std::endl
	     <<"5V_Payload_Voltage="<<beaconStatus.volt5Vpl_<<std::endl 
	     <<"5V_Payload_Current="<<beaconStatus.curr5Vpl_<< std::endl 

	     ;
  }
  
}
