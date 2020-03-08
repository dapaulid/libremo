@echo off
pushd .

echo ================================================================================
echo                           S T R E S S   T E S T                                 
echo ================================================================================
test\stress.py --repetitions=1000 --exit-on-error build\bin\Debug\unit_tests || goto :failed

:success
popd
goto :EOF

:failed
popd
exit /b 1
