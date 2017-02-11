#!/bin/bash
export LD_LIBRARY_PATH=/opt/toolchain/toolchain-arm-linux/lib
BIN_DIR=/opt/toolchain/toolchain-arm-linux/bin/
CLK_STATUS=${BIN_DIR}/clksync-status
CLK_ADJUST=${BIN_DIR}/clksync-sync-offset
logFile=/tmp/clockSync.log

hostIp=${SAT_IP-192.168.0.100}

syncThresholdSec=30

usage()
{
    cat <<EOF
Usage:  $0 [options]

Options:

  -t {IP address}  The IP address of the target system (default $hostIp)
  -l {log file}    The log file to log output to
  -m               Monitor the clock sync status and sync if necessary
  -q               Query clock difference (default)
  -s               sync the clock status

EOF

exit 1
}

log()
{
  output="$(date +%Y%m%d_%H%M%S) $*"
  if [ "${1:0:2}" = "[D" ]; then
    return
  fi
  echo $output
  echo $output>> ${logFile}
}

cmd=query

unset monitor
while getopts "t:l:msqh" arg; do
    case $arg in
        t)
	    hostIp=$OPTARG
	    ;;
	l)
	    logFile=$OPTARG
	    ;;
        s)
            cmd=sync
            ;;
	m)
	    cmd=monitor
	    ;;
        q)
            cmd=query 
            ;;
        h)
	    usage
	    ;;
	*)
	    usage
    esac
done

getTimeOffset()
{
  log "[D] Retrieving status from $hostIp"
  statusOutput=$(${CLK_STATUS} -h ${hostIp})
  status=$?

  if [ ${status} -ne 0 ]; then
    log "[E] Unable to contact ${hostIp}"
    return 1
  fi

  offsetSec=$(echo "$statusOutput"|awk '/local_offset_sec:/{print $2}')
  offsetUsec=$(echo "$statusOutput"|awk '/local_offset_usec:/ {print $2}')
  return 0
}

adjustTargetTime()
{
    sec=$1
    usec=$2
    log "[I] Adjusting ${hostIp} time by $offsetSec seconds and $offsetUsec usec"

    $CLK_ADJUST -h $hostIp -s $sec -u $usec -i 1
    status=$?

    if [ $status -ne 0 ]; then
	log "[E] Unable to adjust clock time for $hostIp"
	return 1
    fi

    log "[I] Successfully adjusted clock.  Clock will take effect on reboot within 20 minutes"
    return 0
}

case $cmd in 
  query)
    getTimeOffset
    log "[I] offset:  $offsetSec sec $offsetUsec usec"
  ;;
  sync)
    getTimeOffset
    if [ $? -eq 0 ]; then
      adjustTargetTime $offsetSec $offsetUsec
    fi
  ;;
  monitor)
    state=UNKNOWN
    while /bin/true; do
      previousState=$state
      getTimeOffset
      if [ $? -eq 0 ]; then
	  if [ $offsetSec -gt $syncThresholdSec ]; then
            state=OFF_SYNC
	    log "[I] offset $offsetSec exceeds $syncThresholdSec.  Synchronizing clock"
	    adjustTargetTime $offsetSec $offsetUsec
            # when time is adjusted, wait 20 min before checking sync
            sleeptime=1200
	  else
            state=SYNC
            sleeptime=300
          fi
      else
        state=DISCONNECTED
        # try every minute if we're disconnected
        sleeptime=60
      fi
      if [ "$state" != "$previousState" ]; then
        log "[I] State changed from $previousState to $state"
      fi 
      sleep $sleeptime
    done
  ;;
esac
exit 0
