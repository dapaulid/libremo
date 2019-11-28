#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                              C O V E R A G E                                   \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
cd $BUILDDIR
lcov --capture --quiet --directory src --output-file coverage.info --rc lcov_branch_coverage=1
lcov --summary coverage.info --rc lcov_branch_coverage=1
