#!/bin/sh
#
# script to run solve-field with default arguments for flight
#
# Configuration may be adjusted in astrometry.cfg
#
solve-field --config /usr/local/astrometry/etc/astrometry.cfg --no-fits2fits --no-verify --no-plots $*
      
