@echo off

if not exist build (
	call ci\prebuild.cmd || goto :failed
)
call ci\build.cmd || goto :failed
call ci\test.cmd || goto :failed
call ci\coverage.cmd || goto :failed
call ci\example.cmd || goto :failed

:success
goto :EOF

:failed
exit /b 1
