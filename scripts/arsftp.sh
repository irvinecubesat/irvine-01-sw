#!/bin/bash
#
# Wrapper for arsftp to run on gateway machine.  Sets the LD_LIBRARY_PATH.
#
# Usage:  arsftp.sh cmd (e.g. arsftp.sh get ....)
#
log()
{
  echo $*
}

arsftpPath=/opt/toolchain//toolchain-arm-linux/bin/arsftp
arsftpCmds=$(ls ${arsftpPath}-* | awk -F- '{print $NF}')

if [ ! -x $arsftpPath ]; then
  log "[E] $arsftpPath not found.  Install the toolchain via the irvine-01-sw project"
  exit 1
fi

usage()
{
  cat <<EOF
  Usage:  arsftp.sh cmd [args]

  run arsftp-{cmd} with given arguments

  Possible commands are:

$(echo "$arsftpCmds" | awk '{print "  "$1}')
EOF

  exit 1
}
cmd=$1
shift 
if [ -z "$cmd" ]; then
  log "[E] Please provide a cmd to execute."
  usage
else
  if ! ( echo $arsftpCmds |fgrep -qw "$cmd" ) then
    log "[E] Invalid cmd: $cmd"
    usage
  fi	
fi

export LD_LIBRARY_PATH=/opt/toolchain//toolchain-arm-linux/lib
${arsftpPath}-${cmd} $*

