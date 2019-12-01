@echo off
pushd .

echo ================================================================================
echo                              P R E B U I L D                                    
echo ================================================================================
mkdir build
cd build
cmake .. || goto :failed

:success
popd
goto :EOF

:failed
popd
exit /b 1
