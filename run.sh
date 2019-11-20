#!/bin/bash -e

BUILDTYPE="Debug"
BUILDDIR="build/$BUILDTYPE"

if [ ! -d "$BUILDDIR" ]; then
	echo -e "\e[1;36m================================================================================\e[0m"
	echo -e "\e[1;36m                             P R E B U I L D                                    \e[0m"
	echo -e "\e[1;36m================================================================================\e[0m"
 	mkdir -p $BUILDDIR
	cd $BUILDDIR
	cmake -DCMAKE_BUILD_TYPE=$BUILDTYPE ../..
else
	cd $BUILDDIR
fi

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                                B U I L D                                       \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
cmake --build .

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                            U N I T   T E S T                                   \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
rm -f coverage.info
lcov --zerocounters --directory src
bin/unit_tests

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                              C O V E R A G E                                   \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
lcov --capture --quiet --directory src --output-file coverage.info --rc lcov_branch_coverage=1
lcov --summary coverage.info --rc lcov_branch_coverage=1

echo -e "\e[1;36m================================================================================\e[0m"
echo -e "\e[1;36m                              E X A M P L E                                     \e[0m"
echo -e "\e[1;36m================================================================================\e[0m"
bin/example
echo -e "\e[1;36m================================================================================\e[0m"