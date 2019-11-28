#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                             P R E B U I L D                                    \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
mkdir -p $BUILDDIR
cd $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=$BUILDTYPE ../..
