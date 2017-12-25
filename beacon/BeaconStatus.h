#ifndef __BEACON_STATUS_HH
#define __BEACON_STATUS_HH

#include "beacon.h"
#include <iostream>
#include <string>

namespace IrvCS
{
  /**
   * Class to hold converted Beacon Data in real world units
   **/
  class BeaconStatus
  {
  public:
    /**
     * Convert incoming BeaconData into real-world units
     **/
    BeaconStatus(const BeaconData *data);

    /**
     * Destructor to clean up allocated resources (if any)
     **/
    virtual~BeaconStatus();
    
    /**
     * Send data to an output stream in a human friendly format
     **/
    friend std::ostream& operator<<(std::ostream &os, const BeaconStatus &bp);

    /*
     * Beacon Data fields.  Raw data is in network byte order and many of
     * the values represent floating numbers in integer or unsigned integers.
     */

    /**
     * Status ID.  This should be "IRV-01" as defined by BEACON_ID
     **/
    std::string id_;

    /**
     * Long Duration Timer.  LDC*256=uptime in seconds.  
     **/
    uint16_t ldc_;

    float gyro_[3];             /* degrees per second */
    float mag_[3];              /* nT */
    float tempDaughterA_;       /* C */
    float tempThreeVpl_;        /* 3 V payload temp C */
    float tempNz_;              /* Outside temp in C */

    float volt3V_;              /* 3V sensor Voltage */
    float curr3V_;              /* 3V sensor Current */
    float volt5Vpl_;            /* 5V payload Voltage */
    float curr5Vpl_;             /* 5V payload Current */
  };
}
#endif 
