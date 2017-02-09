#!/bin/bash
#
# Program to monitor pings and log the result
#
LOG_DIR=~/logs
logFile="$LOG_DIR"/pingMon.log
hostIp=192.168.0.100

if [ ! -d $LOG_DIR ]; then
    mkdir -p $LOG_DIR
fi

usage()
{
    cat <<EOF
Usage:  pingMon.sh [options] {ip address} 

       ping the given IP address, logging to the file

Options:

  -l {log file}    The log file to log output to

EOF
    exit 1
}

log()
{
    output="$(date +%Y%m%d_%H%M%S) $*"
    echo $output |tee -a $logFile
}

while getopts "l:h" arg; do
    case $arg in
	    l)
	        logFile=$OPTARG
	        ;;
        h)
	        usage
	        ;;
	    *)
	        usage
    esac
done
shift $((OPTIND-1))

if [ -z "$1" ]; then
    echo
    echo "Please provide the IP address to monitor"
    echo
    usage
    exit 1
fi

log "[I] Starting Ping test"
ping $hostIp 2>&1 |tee -a $logFile&
pid=$!

cleanup()
{
    log "[I] Terminating test"
}
trap cleanup EXIT
wait $(($pid))


