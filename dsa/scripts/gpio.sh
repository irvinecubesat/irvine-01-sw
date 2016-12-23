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
# returns 2 if GPIO doesn't exist
getGPIO(){
  local path="$GPIO_PATH/gpio$1/value"
  local ret=$(cat $path)
  if [ -z "$ret" ]; then
      return 2
  fi
  return $ret
}

#reads from gpio and stores in gpio_out, then echos the value
printGPIO(){
  getGPIO $2
  local gpioVal=$?
  echo "$1: $gpioVal"
  return $gpioVal
}
