@echo off
pushd .

echo ================================================================================
echo                                 B U I L D                                       
echo ================================================================================
cd build
cmake --build . || goto :failed

:success
popd
goto :EOF

:failed
popd
exit /b 1
