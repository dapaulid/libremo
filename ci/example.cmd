@echo off
pushd .

echo ================================================================================
echo                               E X A M P L E                                     
echo ================================================================================
cd build
bin\Debug\example || goto :failed
echo ================================================================================

:success
popd
goto :EOF

:failed
popd
exit /b 1
