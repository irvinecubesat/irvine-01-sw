#!/bin/sh
#
# Collect DSA Deploy telemetry
#

outputState()
{
    echo "$1=$(gpiotest $2 in)"
}

outputState "D1-R" 58
outputState "D1-D" 59
outputState "D2-R" 60
outputState "D2-D" 80
