@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem publish.cmd
rem
rem Unified Communication protocol C++ library.
rem
rem USAGE: publish target_library_root
rem 
rem NOTE: Don't use trailing back slash
rem
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2009, (c) Dmitry Timoshenko

rem //////////////////////////////////////////////////////////////////////////
rem push environment
setlocal

echo.
echo /////////////////////////////////////////////////////////////////////////
echo Publishing unicomm...

rem any not known error is unknown error
set ERROR_MESSAGE=Unknown error

rem get script location
set script_dir=%~dp0
set script_drive=%~d0
set target_dir=%~f1

rem echo target_dir = %target_dir%

set zip_file=%~nx1
set zip_dir=%~dp1
set zip_drive=%~d1

rem fixme: REMOVE TRAILING SLASH FROM %~dp1

rem check if f
rem add trailing slash if it's neccessary
REM if not "%target_dir:~-1%"=="\" (
  REM if not "%target_dir:~-1%"=="/" (
    REM set target_dir=%target_dir%\
  REM )
REM )

if not exist "%target_dir%" (
	md %target_dir%
	
	if %ERRORLEVEL% neq 0 (
		set ERROR_MESSAGE=Error creating [%target_dir%]
		goto handle_error
	)
) 

set zip_file_full_name=%zip_dir%%zip_file%.zip

if exist "%zip_file_full_name%" (
	echo File [%zip_file_full_name%] already exists, delete before proceed?
	echo.
	del /P %zip_file_full_name%
)

rem remove trailing slash if it's neccessary
if "%script_dir:~-1%"=="\" (
	set script_dir=%script_dir:~0,-1%
)

echo.
echo Source directory: %script_dir%
echo Target directory: %target_dir%
REM echo zip_file  = %zip_file%
REM echo zip_dir   = %zip_dir%
REM echo zip_drive = %zip_drive%
echo.

rem copying files
%script_drive%
cd %script_dir%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Can't change directory to %script_dir%
	goto handle_error
)

xcopy %script_dir% %target_dir% /E /Q /EXCLUDE:exclude_files

rem check errors
if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Error encountered while copying files
	goto handle_error
)

rem changing location
%zip_drive%
cd %zip_dir%
rem echo Current directory: %cd%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Can't change directory to %zip_dir%
	goto handle_error
)

rem compress files
REM if "%~$PATH:9"=="" (
	REM set ERROR_MESSAGE=ZIP archiver is not found on the system
	REM goto handle_error	
REM )

rem Creating tar

set gzip_file_name=%zip_file%.tar.gz
set gzip_file_full_name=%zip_dir%%gzip_file_name%

if exist "%gzip_file_full_name%" (
	echo File [%gzip_file_full_name%] already exists, delete before proceed?
	echo.
	del /P %gzip_file_full_name%
)

rem 7z a -ttar libsmart-1.01.tar.gz ./libsmart-1.01/
7z a -r -ttar %gzip_file_name% %zip_dir%%zip_file%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Error encountered while compressing files: ZIP archiver is absent?
	goto handle_error
)

rem Creating gzip

set gzip_temp_file_name=_%gzip_file_name%

7z a -tgzip %gzip_temp_file_name% %gzip_file_name%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Error encountered while compressing files.
	goto handle_error
)

rem Renaming file
move /Y %gzip_temp_file_name% %gzip_file_name%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Error encountered while compressing files.
	goto handle_error
)

rem Creating zip
rem 7z -tgzip %zip_file%.tar.gz %zip_file%.tar.gz
rem echo -9 -q -r -m -b %zip_dir% %zip_file%.zip %zip_file%
zip -9 -q -r -m -b %zip_dir% %zip_file%.zip %zip_file%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Error encountered while compressing files: ZIP archiver is absent?
	goto handle_error
)

echo.
echo File [%zip_file_full_name%] successfully compressed

set user_input=
echo.
set /P user_input=Would you like to publish binaries? (y/n): 

if not "%user_input%"=="" set user_input=%user_input:~0,1%

rem /I makes the IF comparison case-insensitive
if /I not '%user_input%'=='Y' (
	endlocal
	goto success
)

rem //////////////////////////////////////////////////////////////////////////
rem prepearing binaries
echo.
echo /////////////////////////////////////////////////////////////////////////
echo Prepearing unicomm binaries...
echo.

rem Windows binaries SHARED DLL
echo.
echo /////////////////////////////////////////////////////////////////////////
echo // Windows binaries SHARED DLL 
echo /////////////////////////////////////////////////////////////////////////
echo.
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd unicomm-vc80-1_00.dll ./lib/unicomm-vc80-1_00.dll ./lib/unicomm-vc80-1_00.exp ./lib/unicomm-vc80-1_00.lib
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd unicomm-vc80-debug-1_00.dll ./lib/unicomm-vc80-debug-1_00.dll ./lib/unicomm-vc80-debug-1_00.exp ./lib/unicomm-vc80-debug-1_00.lib ./lib/unicomm-vc80-debug-1_00.pdb
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd unicomm-vc80-ssl-1_00.dll ./lib/unicomm-vc80-ssl-1_00.dll ./lib/unicomm-vc80-ssl-1_00.exp ./lib/unicomm-vc80-ssl-1_00.lib
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd unicomm-vc80-ssl-debug-1_00.dll ./lib/unicomm-vc80-ssl-debug-1_00.dll ./lib/unicomm-vc80-ssl-debug-1_00.exp ./lib/unicomm-vc80-ssl-debug-1_00.lib ./lib/unicomm-vc80-ssl-debug-1_00.pdb

rem Windows binaries STATIC LIB
echo.
echo.
echo /////////////////////////////////////////////////////////////////////////
echo // Windows binaries STATIC LIB
echo /////////////////////////////////////////////////////////////////////////
echo.
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-vc80-1_00.lib ./lib/libunicomm-vc80-1_00.lib 
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-vc80-debug-1_00.lib ./lib/libunicomm-vc80-debug-1_00.lib ./lib/libunicomm-vc80-debug-1_00.pdb
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-vc80-ssl-1_00.lib ./lib/libunicomm-vc80-ssl-1_00.lib
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-vc80-ssl-debug-1_00.lib ./lib/libunicomm-vc80-ssl-debug-1_00.lib ./lib/libunicomm-vc80-ssl-debug-1_00.pdb

rem Linux binaries SHARED
echo.
echo /////////////////////////////////////////////////////////////////////////
echo // Linux binaries SHARED
echo /////////////////////////////////////////////////////////////////////////
echo.
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-1_00.so.1.00 ./lib/libunicomm-gcc42-1_00.so.1.00
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-debug-1_00.so.1.00 ./lib/libunicomm-gcc42-debug-1_00.so.1.00
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-ssl-1_00.so.1.00 ./lib/libunicomm-gcc42-ssl-1_00.so.1.00
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-ssl-debug-1_00.a ./lib/libunicomm-gcc42-ssl-debug-1_00.a

rem Linux binaries STATIC
echo.
echo /////////////////////////////////////////////////////////////////////////
echo // Linux binaries STATIC
echo /////////////////////////////////////////////////////////////////////////
echo.
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-1_00.a ./lib/libunicomm-gcc42-1_00.a
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-debug-1_00.a ./lib/libunicomm-gcc42-debug-1_00.a
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-ssl-1_00.a ./lib/libunicomm-gcc42-ssl-1_00.a
call %script_dir%/safe_call.cmd %script_dir%/compress.cmd libunicomm-gcc42-ssl-debug-1_00.so.1.00 ./lib/libunicomm-gcc42-ssl-debug-1_00.so.1.00

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Error encountered while compressing files?
	goto handle_error
)

:success
echo.
echo SUCCESS
echo ---------------------------
echo Unicomm publishing finished: ERRORLEVEL = '%ERRORLEVEL%'

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal
goto :eof

rem ///////////////////////////////////////////////////////////////////////////
rem An error handler
:handle_error
echo.
echo FAILURE
echo ---------------------------
echo Error has been encountered:
echo   %ERROR_MESSAGE%
echo.
echo Please, correct the situation and try again
echo.
echo error: Unicomm publishing failed

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal

set ERRORLEVEL=1
