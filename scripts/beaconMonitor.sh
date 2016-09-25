#!/bin/bash
#
# Run this on the host computer to monitor beacon packets
#
sudo tcpdump -i eth1 -X port 50000 2>&1|tee -a ~/logs/beaconMonitor.log
