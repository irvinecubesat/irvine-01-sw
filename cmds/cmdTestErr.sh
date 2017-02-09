#!/bin/sh
#
#  Test cmd execution error
#
msg="Executed cmd:  $0 $*"
echo "$msg">/var/log/cmdTest.log 
echo "$msg"
( >&2 echo "something went wrong with $0 $*")
exit 5
