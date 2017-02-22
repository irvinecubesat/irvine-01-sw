#ifndef __CCARD_MESSAGES_H__
#define __CCARD_MESSAGES_H__

#include <stdint.h>
extern "C"
{

#define CCARD_CMD 0x02
#define CCARD_RESPONSE 0xF2
  
  /**
   * Provide card status.  Provide the port register.
   **/
  struct CCardStatus
  {
    uint8_t pinStatus;
  }__attribute__((packed));

  /**
   * Single command structure to hold either DSA or MT commands.
   *
   * DSA Command: 
   *   - Upper 4 bits is reserved for cmd, release or deploy (0 or 1), 
   *   - lowest bit holds which panel (0 or 1).  The timer is automatically
   *     triggered for each command unless otherwise noted.
   *
   * MT Command:
   *   - First 3 bits indicate the x, y, and z values of the magnetorquers.
   **/
  struct CCardMsg
  {
    uint8_t data;
  }__attribute__((packed));

}
#endif
