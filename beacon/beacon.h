#ifndef __BEACON_HH__
#define __BEACON_HH__

#include <polysat_pkt/sys_manager_structs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BEACON_ID "IRV-02"
#define ID_LEN 7

/**
 * Packet needs to fit in 227 bytes
 **/
typedef struct {
  char id[ID_LEN]; /* "IRV-02" To make it easy recognize           */

  uint16_t ldc;    /* Long Duration Timer (about 4min 15 sec/tick) */

  uint32_t gyro[3]; 
  uint32_t mag[3];  
  uint16_t daughter_aTmpSensor;
  uint16_t threeV_plTmpSensor;
  uint16_t tempNz;

  /* Power Data is 8 bytes */
  struct PowerData threeVPwrSensor;
  struct PowerData fiveV_plPwrSensor;

  // TODO:  Add DSA release/deployment state
} __attribute__((packed)) BeaconData;

#ifdef __cplusplus
}
#endif

#endif
