@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem compress.cmd
rem
rem Unified Communication protocol C++ library.
rem
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2009, (c) Dmitry Timoshenko

rem //////////////////////////////////////////////////////////////////////////
rem push environment
rem setlocal

cd %script_dir%

set archive_fn=%1
set gzip_fn=%archive_fn%.tar.gz
set gzip_temp_fn=_%gzip_fn%
set zip_fn=%archive_fn%.zip

rem //////////////////////////////////////////////////////////////////////////
rem Build command line

shift

:begin_loop
if "%~1"=="" goto break_loop

rem creating files list to pack
set files_list=%files_list% %1

shift
goto begin_loop

:break_loop


rem shift
echo.
echo *** PROCESSING: %archive_fn% ***
rem Creating tar
echo --- GZIP: TAR ---
rem echo 7z a -ttar %gzip_fn% %files_list%
7z a -ttar %gzip_fn% %files_list%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Creating TAR: %gzip_fn%
	goto handle_error
)

rem Creating gzip

echo --- GZIP: GZIP ---
rem echo 7z a -tgzip %gzip_temp_fn% %gzip_fn%
7z a -tgzip %gzip_temp_fn% %gzip_fn%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Creating GZIP: %gzip_temp_fn%
	goto handle_error
)

rem Renaming file
rem echo move /Y %gzip_temp_fn% %gzip_fn%
echo --- GZIP: RENAMING ---
move /Y %gzip_temp_fn% %gzip_fn%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Renaming GZIP: from %gzip_temp_fn% to %gzip_fn%
	goto handle_error
)

rem Creating zip
echo --- ZIP ---
rem 7z -tgzip %zip_file%.tar.gz %zip_file%.tar.gz
set zip_fn=%archive_fn%.zip

rem echo zip -9 -q -r -b %script_dir% %zip_fn% %files_list%
zip -9 -q -r -b %script_dir% %zip_fn% %files_list%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Creating ZIP: %zip_fn%
	goto handle_error
)

rem making binaries folder
set bin_dir=%zip_dir%binaries

rem echo mkdir %bin_dir%
if not exist "%bin_dir%" (
	echo --- CREATING BINARIES DIRECTORY ---
	mkdir "%bin_dir%"
	
	if %ERRORLEVEL% neq 0 (
		set ERROR_MESSAGE=Creating binary targer folder: %bin_dir%
		goto handle_error
	)
)

rem moving gzip to binaries folder
rem echo move /Y %gzip_fn% %bin_dir%/%gzip_fn%
echo --- MOVIING BINARIES: GZIP ---
move /Y %gzip_fn% %bin_dir%/%gzip_fn%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Moving GZIP failed: from %gzip_fn% to %bin_dir%/%gzip_fn%
	goto handle_error
)

rem moving zip to binaries folder
rem echo move /Y %zip_fn% %bin_dir%/%zip_fn%
echo --- MOVIING BINARIES: ZIP ---
move /Y %zip_fn% %bin_dir%/%zip_fn%

if %ERRORLEVEL% neq 0 (
	set ERROR_MESSAGE=Moving ZIP failed: from %zip_fn% to %bin_dir%/%zip_fn%
	goto handle_error
)

rem Ok!
echo.
echo Files [%gzip_fn%, %zip_fn%] successfully created

echo.
echo *** FINISHED: ERRORLEVEL = '%ERRORLEVEL%' ***

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
rem endlocal
goto :eof

rem ///////////////////////////////////////////////////////////////////////////
rem An error handler
:handle_error
echo.
echo ERROR: Error encountered while compressing files [%ERROR_MESSAGE%]
echo.

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
rem endlocal

set ERRORLEVEL=1
