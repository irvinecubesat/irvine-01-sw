#!/bin/sh
# deployment script for irvine-01 cubesat deployable solar arrays

DSA1_RLS_B=296
DSA1_DPLY_B=297
DSA2_RLS_B=298
DSA2_DPLY_B=299
DSA_EN_BAR=303
DPLY_SENSE_1A=0
DPLY_SENSE_1B=1
DPLY_SENSE_2A=2
DPLY_SENSE_2B=4


# TODO: CHANGE THIS TO CORRECT VALUE WHEN POSSIBLE

GPIO_PATH=${GPIO_PATH-"/sys/class/gpio"}

# call    setGPIO GPIONUM VALUE
# returns void
setGPIO(){
  local gpio=$1
  local val=$2
  local path="$GPIO_PATH/gpio$gpio/value"
  echo "$val"> "$path"
}

# call    getGPIO GPIONUM
# returns $gpio_out
getGPIO(){
  local path="$GPIO_PATH/gpio$1/value"
  local ret=$(cat $path)
  gpio_out=$ret
}

#reads from gpio and stores in gpio_out, then echos the value
printGPIO(){
  getGPIO $2
  echo "$1: $gpio_out"
}

#handles the deploy command
handleDeploy(){
  getGPIO $DSA1_RLS_B
  local rls1=$gpio_out
  getGPIO $DSA2_RLS_B
  local rls2=$gpio_out
  getGPIO $DSA1_DPLY_B
  local dply1=$gpio_out
  getGPIO $DSA2_DPLY_B
  local dply2=$gpio_out

  dep1(){
    if [ $rls1 = "0" ]; then
      echo "CANNOT DEPLOY DSA 1, NOT RELEASED"
    elif [ $dply1 = "1" ]; then
      echo "DSA 1 IS ALREADY DEPLOYED"
    else
      setGPIO $DSA1_DPLY_B 1
      echo "DEPLOYED DSA 1"
    fi
  }

  dep2(){
    if [ $rls2 = "0" ]; then
      echo "CANNOT DEPLOY DSA 2, NOT RELEASED"
    elif [ $dply2 = "1" ]; then
      echo "DSA 2 IS ALREADY DEPLOYED"
    else
      setGPIO $DSA2_DPLY_B 1
      echo "DEPLOYED DSA 2"
    fi
  }

  if [ -z $1 ]; then
    dep1
    dep2
  else
    if [ $1 = "1" ]; then
      dep1
    elif [ $1 = "2" ]; then
      dep2
    fi
  fi
}

#handles the release command
handleRelease(){
  getGPIO $DSA1_RLS_B
  local rls1=$gpio_out
  getGPIO $DSA2_RLS_B
  local rls2=$gpio_out

  rel1(){
    if [ $rls1 = "1" ]; then
      echo "DSA 1 IS ALREADY RELEASED"
    else
      setGPIO $DSA1_RLS_B 1
      echo "RELEASED DSA 1"
    fi
  }

  rel2(){
    if [ $rls2 = "1" ]; then
      echo "DSA 2 IS ALREADY RELEASED"
    else
      setGPIO $DSA2_RLS_B 1
      echo "RELEASED DSA 2"
    fi
  }

  if [ -z $1 ]; then
    rel1
    rel2
  else
    if [ $1 = "1" ]; then
      rel1
    elif [ $1 = "2" ]; then
      rel2
    fi
  fi
}

#reads the status of all 4 deploy sense variables
readDsaStatus(){
  local successes=0

  sense(){
    printGPIO $1 $2
    if [ "$gpio_out" = "0" ]; then
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
