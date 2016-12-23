#handles the deploy command
handleDeploy(){
  getGPIO $DSA1_RLS_B
  local rls1=$?
  getGPIO $DSA2_RLS_B
  local rls2=$?
  getGPIO $DSA1_DPLY_B
  local dply1=$?
  getGPIO $DSA2_DPLY_B
  local dply2=$?

  dep1(){
    if [ "$rls1" = "0" ]; then
      echo "CANNOT DEPLOY DSA 1, NOT RELEASED"
    elif [ "$dply1" = "1" ]; then
      echo "DSA 1 IS ALREADY DEPLOYED"
    else
      setGPIO $DSA1_DPLY_B 1
      echo "DEPLOYED DSA 1"
    fi
  }

  dep2(){
    if [ "$rls2" = "0" ]; then
      echo "CANNOT DEPLOY DSA 2, NOT RELEASED"
    elif [ "$dply2" = "1" ]; then
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
