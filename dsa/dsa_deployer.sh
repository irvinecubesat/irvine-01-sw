#!/bin/sh
# deploy script for irvine-01 cubesat deployable solar arrays

instDir=$(cd $(dirname "$0");pwd)
. $instDir/scripts/gpio.sh
. $instDir/scripts/deploy.sh
. $instDir/scripts/release.sh


#TODO: CHANGE BEFORE LAUNCH
GPIO_PATH="$instDir/gpio"


DSA1_RLS_B=296
DSA1_DPLY_B=297
DSA2_RLS_B=298
DSA2_DPLY_B=299
DSA_EN_BAR=303
DPLY_SENSE_1A=0
DPLY_SENSE_1B=1
DPLY_SENSE_2A=2
DPLY_SENSE_2B=4

#reads the status of all 4 deploy sense variables
readDsaStatus(){
  local successes=0

  sense(){
    printGPIO $1 $2
    if [ "$?" = "0" ]; then
      ((successes++))
    fi
  }

  sense "Sensor_1A" $DPLY_SENSE_1A
  sense "Sensor_1B" $DPLY_SENSE_1B
  sense "Sensor_2A" $DPLY_SENSE_2A
  sense "Sensor_2B" $DPLY_SENSE_2B

  if [ $successes = 4 ]; then
    echo "ALL ARE OPEN"
  fi
}

#COMMANDS
if [ -z $1 ]||[ $1 = "-h" ]; then
  echo "Commands:"
  echo "sh dsa_deployer.sh ReadDSA"
  echo "sh dsa_deployer.sh Release"
  echo "sh dsa_deployer.sh Release [1 or 2]"
  echo "sh dsa_deployer.sh Deploy"
  echo "sh dsa_deployer.sh Deploy [1 or 2]"
  echo "sh dsa_deployer.sh CLEAR"

  echo "GPIO_PATH:  $GPIO_PATH"
  exit
elif [ $1 = "ReadDSA" ]; then
  readDsaStatus
  exit
elif [ $1 = "Release" ]; then
  handleRelease $2
  exit
elif [ $1 = "Deploy" ]; then
  handleDeploy $2
  exit
elif [ $1 = "CLEAR" ]; then
  setGPIO $DSA1_RLS_B 0
  setGPIO $DSA1_DPLY_B 0
  setGPIO $DSA2_RLS_B 0
  setGPIO $DSA2_DPLY_B 0
  setGPIO $DSA_EN_BAR 0
  setGPIO $DPLY_SENSE_1A 0
  setGPIO $DPLY_SENSE_1B 0
  setGPIO $DPLY_SENSE_2A 0
  setGPIO $DPLY_SENSE_2B 0
  echo "GPIOS CLEARED."
  exit
elif [ $1 = "PRINT" ]; then
  printGPIO "DSA1_RLS_B    " $DSA1_RLS_B
  printGPIO "DSA1_DPLY_B   " $DSA1_DPLY_B
  printGPIO "DSA2_RLS_B    " $DSA2_RLS_B
  printGPIO "DSA2_DPLY_B   " $DSA2_DPLY_B
  printGPIO "DSA_EN_BAR    " $DSA_EN_BAR
  printGPIO "DPLY_SENSE_1A " $DPLY_SENSE_1A
  printGPIO "DPLY_SENSE_1B " $DPLY_SENSE_1B
  printGPIO "DPLY_SENSE_2A " $DPLY_SENSE_2A
  printGPIO "DPLY_SENSE_2B " $DPLY_SENSE_2B
  exit
else
  echo "Invalid Arguments"
  echo "sh dsa_deployer -h for commands"
  exit
fi
