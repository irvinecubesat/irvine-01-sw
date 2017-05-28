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

outputState "D1-R" 58
outputState "D1-D" 59
outputState "D2-R" 60
outputState "D2-D" 80
