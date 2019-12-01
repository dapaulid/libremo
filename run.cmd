@echo off

if not exist build (
	ci\prebuild.cmd || goto :failed
)
ci\build.cmd || goto :failed
ci\test.cmd || goto :failed
ci\coverage.cmd || goto :failed
ci\example.cmd || goto :failed

:success
goto :EOF

:failed
exit /b 1
