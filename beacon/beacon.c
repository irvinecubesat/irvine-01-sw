#include <polysat/polysat.h>
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
#define BEACON_MESSAGE        "Hello World!"

static void send_beacon_packet(ProcessData *proc, char *data, int len)
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

static int assemble_beacon(void *arg)
{
   ProcessData *proc = (ProcessData*)arg;

   if (proc)
      send_beacon_packet(proc, BEACON_MESSAGE, strlen(BEACON_MESSAGE));

   return EVENT_KEEP;
}

// Simple SIGINT handler example
static int sigint_handler(int signum, void *arg)
{
   // Cause the event loop to exit
   EVT_exit_loop(PROC_evt(arg));

   return EVENT_KEEP;
}

int main(void)
{
   ProcessData *proc;
   void *beacon_evt;

   // Initialize the process
   proc = PROC_init("beacon");
   if (!proc)
      return -1;

   // Schedule an event to run periodically.  The event generates the
   //   beacon.
   beacon_evt = EVT_sched_add(PROC_evt(proc),
      EVT_ms2tv(17 * 1000), assemble_beacon, proc);

   // Add an event-loop based signal handler call back for SIGINT signal
   PROC_signal(proc, SIGINT, &sigint_handler, proc);

   // Enter the main event loop.
   EVT_start_loop(PROC_evt(proc));

   // Clean up after the event loop exits
   if (beacon_evt)
      EVT_sched_remove(PROC_evt(proc), beacon_evt);

   // Clean up, whenever we exit event loop
   PROC_cleanup(proc);

   return 0;
}


