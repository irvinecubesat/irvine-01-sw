#!/bin/sh
#
# Collect DSA Deploy telemetry
#

gpioRoot=/sys/class/gpio/
getGpio()
{
    gpioDir=$gpioRoot/gpio${1}
    cat "${gpioDir}/value"
}
outputState()
{
    echo "$1=$(getGpio "$2")"
}

outputState "dsa1_release" 58
outputState "dsa1_deploy" 59
outputState "dsa2_release" 60
outputState "dsa2_deploy" 80
