@echo off
pushd .

echo ================================================================================
echo                             U N I T   T E S T                                   
echo ================================================================================
cd build
bin\Debug\unit_tests || goto :failed

:success
popd
goto :EOF

:failed
popd
exit /b 1
