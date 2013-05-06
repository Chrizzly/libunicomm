rem //////////////////////////////////////////////////////////////////////////
rem push environment
setlocal

echo.
echo Safe call to [%1]
echo.

shift
:begin_loop
if "%~0"=="" goto break_loop

set params=%params% %0

shift
goto begin_loop

:break_loop

rem echo params = %params%
call %params%

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal
