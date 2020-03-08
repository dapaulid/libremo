#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                          S T R E S S   T E S T                                 \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
REMO_COLORS=always test/stress.py --repetitions=1000 --exit-on-error $BUILDDIR/bin/unit_tests
