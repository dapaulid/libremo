#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                            U N I T   T E S T                                   \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
cd $BUILDDIR
rm -f coverage.info
lcov --zerocounters --directory src --directory test
bin/unit_tests
