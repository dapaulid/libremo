#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                              C O V E R A G E                                   \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
cd $BUILDDIR
# capture coverage
lcov --capture --directory src --directory test --output-file coverage.info --quiet --rc lcov_branch_coverage=1
# only keep info about files in src directory
# necessary to correctly handle header files
lcov --extract coverage.info '*/src/*' --output-file coverage.info --quiet --rc lcov_branch_coverage=1
# output summary
lcov --list coverage.info --rc lcov_branch_coverage=1
