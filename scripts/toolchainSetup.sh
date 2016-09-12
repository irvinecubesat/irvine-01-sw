#!/bin/bash
#
# Script to set up the toolchain in a central location.  Run this a sudo
#
#

usage()
{
    cat <<EOF

Usage:  sudo $0 -v {version}

Setup the toolchain for irvine cubesat.  Run this with sudo to put it in a 
central location

EOF

    exit 1    
}

TOOLCHAIN_ROOT=${TOOLCHAIN_ROOT-/opt/toolchain}
TOOLCHAIN_PROJ=${TOOLCHAIN_PROJ-toolchain-arm-linux}

while getopts "v:" arg; do
    case $arg in
	v)
	    TOOLCHAIN_VER=$OPTARG
	    ;;
	*)
	    usage
	    ;;
    esac
done

if [ -z "$TOOLCHAIN_VER" ]; then
    echo "[E] Specify the -v option for tool chain version"
    usage
fi

TOOLCHAIN_PKG_URL=https://github.com/irvinecubesat/toolchain-arm-linux/archive/${TOOLCHAIN_VER}.tar.gz

mkdir -p ${TOOLCHAIN_ROOT};

if [ $? -ne 0 ]; then
    echo "[E] Unable to make $TOOLCHAIN_ROOT.  Run this as sudo."
    usage
    exit 2
fi

# Retrieve and extract toolchain and create a link from the versioned directory
# to the well-known directory
(
    cd ${TOOLCHAIN_ROOT};
    wget -O - $TOOLCHAIN_PKG_URL|tar xvzf -;
    rm -f ${TOOLCHAIN_PROJ}
    ln -s ${TOOLCHAIN_PROJ}-${TOOLCHAIN_VER} ${TOOLCHAIN_PROJ}
)

echo
echo "[I] Installed ${TOOLCHAIN_PROJ}-${TOOLCHAIN_VER} to ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PROJ}"
