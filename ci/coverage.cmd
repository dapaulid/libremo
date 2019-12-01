@echo off
pushd .

echo ================================================================================
echo                               C O V E R A G E                                   
echo ================================================================================
cd build
echo Coverage not available under Windows.

:success
popd
goto :EOF

:failed
popd
exit /b 1
