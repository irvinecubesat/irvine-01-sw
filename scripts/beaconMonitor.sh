#!/bin/bash
#
# Run this on the host computer to monitor beacon packets
#
# On ground network, run this command to set the default route on the board:
# /sbin/route add default gw 192.168.0.1
#
sudo tcpdump -i eth1 -s0 -X -vvv net 224.0.0.1/32 2>&1|tee -a ~/logs/beaconMonitor.log
