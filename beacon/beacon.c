#include <polysat/polysat.h>
#include <polysat_pkt/sys_manager_cmd.h>
#include <polysat_pkt/sys_manager_structs.h>
#include <polysat_pkt/status-structs.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BEACON_PKT_ID         1
#define BEACON_DST_PORT       2
#define BEACON_DST_IP_STR     "224.0.0.1"
#define BEACON_MESSAGE        "IRVINE-01"

static ProcessData *gProc=NULL;


typedef struct {
 uint16_t id;
 uint16_t ldc;			/* Long Duration Timer (about 4min 20 sec/tick) */

 uint8_t gyro[3];
 uint8_t mag[3];
uint8_t gps[3];
uint8_t temp[6];
uint8_t volt[2];

} __attribute__((packed)) BeaconData;

/**
* Retrieve data from system manager and populate the
* beacon structure.
* (Doxygen comment style)
* @param ip The IP Address where the system manager is running
* @param data What the beacon will send
* @return 0 for success
* @return non-zero if error
**/
int retrieveSystemStatus(const char *ip, BeaconData *data)
{
 struct {
   uint8_t cmd;
   struct SMStatus_v5 sms;
 } __attribute__((packed)) resp;


 int len;
 char cmd = SYS_MANAGER_STATUS;
 uint8_t cmd_resp = CMD_STATUS_RESPONSE;

 len = socket_send_packet_and_read_response(ip, "sys_manager", &cmd,
					     sizeof(cmd), &resp, sizeof(resp), 5000);


 if (len != sizeof(resp))
 {
   return len;
 }

 if (resp.cmd != cmd_resp)
 {
   printf("response code incorrect.  Got 0x%02X expected 0x%02X\n",
	   resp.cmd, cmd_resp);
   return 5;
 }

 //
 // Fill in BeaconData elements with sys manager info in the
 // response.
 //

data->ldc = resp.sms.LDC;

data->temp[0] = resp.sms.tempNz;
data->temp[1] = resp.sms.tempPz;
data->temp[2] = resp.sms.tempNx;
data->temp[3] = resp.sms.tempPx;
data->temp[4] = resp.sms.tempNy;
data->temp[5] = resp.sms.tempPy;

data->gps[0] = resp.sms.latitude;
data->gps[1] = resp.sms.longitude;
data->gps[2] = resp.sms.altitude;

data->volt[0] = resp.sms.threeV_plPwrSensor;
data->volt[1] = resp.sms.fiveV_plPwrSensor;

 return 0;
}

/**
* Retrieve data from system manager and populate the
* beacon structure.
* (Doxygen comment style)
* @param ip The IP Address where the system manager is running
* @param data What the beacon will send
* @return 0 for success
* @return non-zero if error
**/
int retrieveAdcsSensorsInfo(const char *ip, BeaconData *data)
{
 struct {
   uint8_t cmd;
   struct SMStatus_v5 sms;
 } __attribute__((packed)) resp;


 int len;
 char cmd = ADCS_SENSORS_INFO;
 uint8_t cmd_resp = CMD_SENSORS_RESPONSE;

 len = socket_send_packet_and_read_response(ip, "adcs_sensors", &cmd,
					     sizeof(cmd), &resp, sizeof(resp), 5000);


 if (len != sizeof(resp))
 {
   return len;
 }

 if (resp.cmd != cmd_resp)
 {
   printf("response code incorrect.  Got 0x%02X expected 0x%02X\n",
	   resp.cmd, cmd_resp);
   return 5;
 }

 //
 // Fill in BeaconData elements with adcs sensors info in the
 // response.
 //

data->gyro[0] = resp.sms.gyro.x;
data->gyro[1] = resp.sms.gyro.y;
data->gyro[2] = resp.sms.gyro.z;

data->mag[0] = resp.sms.mag.x;
data->mag[1] = resp.sms.mag.y;
data->mag[2] = resp.sms.mag.z;

 return 0;
}


/**
* Respond to status commands to let watchdog know we're alive.
**/
void beacon_status(int socket, unsigned char cmd, void * data, size_t dataLen,
                  struct sockaddr_in * src)
{
  char status = 0;



  PROC_cmd_sockaddr(gProc, CMD_STATUS_RESPONSE, &status,
                    sizeof(status), src);
}


static void send_beacon_packet(ProcessData *proc, BeaconData *data, int len)
{
  struct sockaddr_in dest;



  // Select IPv4 Socket type
  dest.sin_family = AF_INET;
  // Convert the port from host ("readable") order to network
  dest.sin_port = htons(BEACON_DST_PORT);
  // Allow socket to receive messages from any IP address
  dest.sin_addr.s_addr = inet_addr(BEACON_DST_IP_STR);



  // Send the beacon
  PROC_cmd_sockaddr(proc, BEACON_PKT_ID, data, len, &dest);
}


/**
* Send Beacon runs periodically to send the beacon
*/
static int send_beacon(void *arg)
{
  ProcessData *proc = (ProcessData*)arg;

  /*
   * Make sure all data in BeanData is in network byte order
   */
  BeaconData beaconData;	/* struct to send in beacon */
  if (proc)
  {
    // Fill in Beacon Data
     int stat = retrieveSystemStatus("127.0.0.1", &beaconData);


     if (stat != 0)
     {
       DBG_print(DBG_LEVEL_INFO, "There were problems retrieving system status\n");
     }

stat = retrieveAdcsSensorsInfo(“127.0.01”, &beaconData);

 if (stat != 0)
     {
       DBG_print(DBG_LEVEL_INFO, "There were problems retrieving system status\n");
     }

     DBG_print(DBG_LEVEL_INFO, "Sending beacon:  %s\n", BEACON_MESSAGE);
     send_beacon_packet(proc, &beaconData , sizeof(BeaconData));
  }

  return EVENT_KEEP;
}


// Simple SIGINT handler example
static int sigint_handler(int signum, void *arg)
{
  // Cause the event loop to exit
  EVT_exit_loop(PROC_evt(arg));



  return EVENT_KEEP;
}

/**
* @TODO add argument processing using getopt
**/
int main(void)
{
  void *beacon_evt;



  // Initialize the process
  gProc = PROC_init("beacon");
  if (!gProc)
     return -1;
  DBG_print(DBG_LEVEL_INFO, "Beacon starting:  %s\n", BEACON_MESSAGE);

  // Schedule an event to run periodically.  The event generates the
  //   beacon.
  beacon_evt = EVT_sched_add(PROC_evt(gProc),
     EVT_ms2tv(17 * 1000), send_beacon, gProc);

  // Add an event-loop based signal handler call back for SIGINT signal
  PROC_signal(gProc, SIGINT, &sigint_handler, gProc);

  // Enter the main event loop.
  EVT_start_loop(PROC_evt(gProc));


  // Clean up after the event loop exits
  if (beacon_evt)
     EVT_sched_remove(PROC_evt(gProc), beacon_evt);


  DBG_print(DBG_LEVEL_INFO, "Cleaning up\n");
 
  // Clean up, whenever we exit event loop
  PROC_cleanup(gProc);

  return 0;
}
