#!/bin/sh
#
# cmd logging library.  Put the following at the top of the program:
#
# instdir=$(cd $(dirname $0); pwd)
# . ${instdir}/log.sh
# log_init $0
#

#
# logFile may be overridden in the calling program
#
if [ -w /var/log ]; then
  logFile=/var/log/cmds.log
else
  logFile=/tmp/cmds.log
fi

#
# @param program name
#
log_init()
{
  log_prog=$1
}

#
# Common log function that logs to stderr if [E] starts the message or
# stdout otherwise.  Logs output to $logFile, adding a datestamp
#
# Keep logging minimal to save on characters.  
#
# @param Start your output with "[E]" for an error
log()
{
    if [ "${1%%]*}" = "[E" ]; then
        ( >&2 echo $* )      # echo to stderr
    else
        echo $*              # echo anything else to stdout
    fi
    # add a date stamp to the output going to log file
    output="$(date +%Y%m%d_%H%M%S) $log_prog: $*"
    echo $output >> $logFile
}
