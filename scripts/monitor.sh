#!/bin/sh
#
# Monitor vital signs run this during test
#
# Run this on the CubeSat Board
#

# monitor interval is first argument
interval=${1-30}
LOG_DIR=/data/monitor
LOG_FILE=$LOG_DIR/monitor_$(date +%Y%m%d_%H%M%S).log
if [ ! -e $LOG_DIR ]; then
  mkdir -p $LOG_DIR
fi

finishFlag=0
finishUp()
{
  # only print end message once
  if [ $finishFlag -eq 0 ]; then
    {
      echo "####"
      echo "# Test end: $(date)"
      echo "####" 
    }|tee -a $LOG_FILE
    finishFlag=1
  fi
  exit 0
}

collectStats()
{
  sys-status|grep -e "PwrS" -e "fuel" -e "TmpSensor" -e "LDC" -e "processes" -e "proc_*" -e "mem"
  echo "Critical Process Events:"
  echo "select * from Events;"|sqlite3 ../CubeSatDB.000.sqlite
}

trap finishUp 0 SIGHUP SIGINT SIGTERM

{
  echo "####"
  echo "# Test Start:  $(date) (monitor interval $interval)"
  echo "# Logging every $interval seconds to $LOG_FILE"
  echo "####"
} |tee -a $LOG_FILE

while /bin/true; do
  collectStats|tee -a $LOG_FILE
  echo "####"|tee -a $LOG_FILE
  sleep $interval 
done 


