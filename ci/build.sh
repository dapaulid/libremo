#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                                B U I L D                                       \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
cd $BUILDDIR
cmake --build .
