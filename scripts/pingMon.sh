#!/bin/bash
#
# Program to monitor pings and log the result
#
LOG_DIR=~/logs
logFile="$LOG_DIR"/pingMon.log
hostIp=${SAT_IP-192.168.0.100}
message="Ping Monitoring"

if [ ! -d $LOG_DIR ]; then
    mkdir -p $LOG_DIR
fi

usage()
{
    cat <<EOF
Usage:  pingMon.sh [options] {ip address} 

       ping the given IP address (default $hostIp), logging to the file.  
       Use control-c to stop the test.  Default IP may be specified by
       the SAT_IP environment variable.

Options:

    -m {message}   A message to help identify the test scenario
    -l {log file}    The log file to log output to

EOF
    exit 1
}

log()
{
    output="$(date +%Y%m%d_%H%M%S) $*"
    echo "$output" |tee -a $logFile
}

while getopts "l:m:h" arg; do
    case $arg in
	    l)
	        logFile=$OPTARG
	        ;;
        m)
            message="$OPTARG"
            ;;
        h)
	        usage
	        ;;
	    *)
	        usage
    esac
done
shift $((OPTIND-1))

hostIp=${1-$SAT_IP}

if [ -z "$hostIp" ]; then
    echo
    echo "Please provide the IP address to monitor"
    echo
    usage
    exit 1
fi

log "[I] ------ Starting $message for $hostIp ------"
ping "$hostIp" 2>&1 |tee -a "$logFile"&
pid=$!

cleanup()
{
    log "[I] ------ Stopping $message for $hostIp ------"
}
trap cleanup EXIT
wait $pid


