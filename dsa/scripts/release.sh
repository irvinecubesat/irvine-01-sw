#handles the release command
handleRelease(){
  getGPIO $DSA1_RLS_B
  local rls1=$?
  getGPIO $DSA2_RLS_B
  local rls2=$?

  rel1(){
    if [ "$rls1" = "1" ]; then
      echo "DSA 1 IS ALREADY RELEASED"
    else
      setGPIO $DSA1_RLS_B 1
      echo "RELEASED DSA 1"
    fi
  }

  rel2(){
    if [ "$rls2" = "1" ]; then
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
