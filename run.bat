@echo off
pushd .

if not exist build (
	echo ================================================================================
	echo                              P R E B U I L D                                    
	echo ================================================================================
    mkdir build
	cd build
	cmake .. || goto :prebuild_failed
) else (
	cd build
)
echo ================================================================================
echo                                 B U I L D                                       
echo ================================================================================
cmake --build . || goto :build_failed

echo ================================================================================
echo                             U N I T   T E S T                                   
echo ================================================================================
bin\Debug\unit_tests

echo ================================================================================
echo                               E X A M P L E                                     
echo ================================================================================
bin\Debug\example
echo ================================================================================

:success
echo.
echo All done!
goto :end

:prebuild_failed
echo.
echo Error: Prebuild failed
goto :end

:build_failed
echo.
echo Error: Build failed
goto :end

:end
popd
