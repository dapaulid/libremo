#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                              E X A M P L E                                     \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
cd $BUILDDIR
bin/example
echo -e "\e[1;36m================================================================================\e[0m"