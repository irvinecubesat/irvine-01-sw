#!/bin/bash
#
# Script to generate etags for a given set of directories.
# This allows you to use emacs tags to locate variable and function definitions.
#

usage()
{
    echo "Usage:  $0 {src root 1} {src root 2} ..."
    echo
    echo "Example:  mketags.sh irvine-01-sw"
}

if [ $# -eq 0 ]; then
    usage;
fi

# automaticall add the toolchain path
dirs="$* /opt/toolchain/toolchain-arm-linux/include"
for src in $dirs; do
    find $src -name "*.cpp" -print -or -name "*.h" -print | xargs etags --append
done
