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
    //
    // @TODO Add all members of Beacon Status to the output
    //
    return os<<"ID = "<<beaconStatus.id_<<std::endl
             <<"LDC = "<<beaconStatus.ldc_<<std::endl
             <<"Gyro_X = "<<beaconStatus.gyro_[0]<<std::endl
             <<"Gyro_Y = "<<beaconStatus.gyro_[1]<<std::endl
	     <<"Gyro Z = "<<beaconStatus.gyro_[2]<<std::endl 
             <<"Mag_X = "<<beaconStatus.mag_[0]<<std::endl 
             <<"Mag_Y = "<<beaconStatus.mag_[1]<<std::endl 
             <<"Mag Z = "<<beaconStatus.mag_[2]<<std::endl 
	
	     ;
  }
  
}
