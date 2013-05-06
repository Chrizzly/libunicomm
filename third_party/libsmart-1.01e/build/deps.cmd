@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem deps.cmd
rem
rem Smart multipurpose C++ library.
rem
rem Sets up smart dependencies.
rem
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2009, (c) Dmitry Timoshenko

rem dependency location files
set boost_root_file=.\deps\boost_root-win
set boost_include_path_file=.\deps\boost_include_path-win
set boost_library_path_file=.\deps\boost_library_path-win
set xercesc_root_file=.\deps\xercesc_root-win

rem ///////////////////////////////////////////////////////////////////////////
rem Boost Library configuration
rem for details see http://www.boost.org

rem Path to boost source tree setup

if "%SMART_USE_PREBUILT_BOOST%"=="true" goto use_prebult_boost

if not defined BOOST_ROOT (
  rem Specify path to the boost root directory
  rem Check whether boost_root dependency file exists
  if not exist "%boost_root_file%" (
    set ERROR_MESSAGE=File [%boost_root_file%] containing Boost Library location doesn't exist
    goto handle_error
  )

  set /P BOOST_ROOT=< "%boost_root_file%"
)

if not defined BOOST_ROOT (
  set ERROR_MESSAGE=Boost Library location is undefined, please specify the path which library is available through
  goto handle_error
)

if not exist "%BOOST_ROOT%" (
  set ERROR_MESSAGE=Given [%BOOST_ROOT%] Boost Library location doesn't exist
  goto handle_error
)

rem add trailing slash if it's neccessary
if not "%BOOST_ROOT:~-1%"=="\" (
  if not "%BOOST_ROOT:~-1%"=="/" (
    set BOOST_ROOT=%BOOST_ROOT%\
  )
)

goto skip_prebult_boost

:use_prebult_boost
rem Path to a boost pre-built version setup: INCLUDE

if not defined BOOST_INCLUDE_PATH (
  rem Specify path to the boost root directory
  rem Check whether boost_root dependency file exists
  if not exist "%boost_include_path_file%" (
    set ERROR_MESSAGE=File [%boost_include_path_file%] containing Boost Library headers location doesn't exist
    goto handle_error
  )

  set /P BOOST_INCLUDE_PATH=< "%boost_include_path_file%"
)

if not defined BOOST_INCLUDE_PATH (
  set ERROR_MESSAGE=Boost Library location is undefined, please specify the path which library is available through
  goto handle_error
)

if not exist "%BOOST_INCLUDE_PATH%" (
  set ERROR_MESSAGE=Given [%BOOST_INCLUDE_PATH%] Boost Library headers location doesn't exist
  goto handle_error
)

rem add trailing slash if it's neccessary
if not "%BOOST_INCLUDE_PATH:~-1%"=="\" (
  if not "%BOOST_INCLUDE_PATH:~-1%"=="/" (
    set BOOST_INCLUDE_PATH=%BOOST_INCLUDE_PATH%\
  )
)

rem Path to a boost pre-built version setup: LIBRARY

if not defined BOOST_LIBRARY_PATH (
  rem Specify path to the boost root directory
  rem Check whether boost_root dependency file exists
  if not exist "%boost_library_path_file%" (
    set ERROR_MESSAGE=File [%boost_library_path_file%] containing Boost Library binaries location doesn't exist
    goto handle_error
  )

  set /P BOOST_LIBRARY_PATH=< "%boost_library_path_file%"
)

if not defined BOOST_LIBRARY_PATH (
  set ERROR_MESSAGE=Boost Library location is undefined, please specify the path which library is available through
  goto handle_error
)

if not exist "%BOOST_LIBRARY_PATH%" (
  set ERROR_MESSAGE=Given [%BOOST_LIBRARY_PATH%] Boost Library binaries location doesn't exist
  goto handle_error
)

rem add trailing slash if it's neccessary
if not "%BOOST_LIBRARY_PATH:~-1%"=="\" (
  if not "%BOOST_LIBRARY_PATH:~-1%"=="/" (
    set BOOST_LIBRARY_PATH=%BOOST_LIBRARY_PATH%\
  )
)

:skip_prebult_boost

rem ///////////////////////////////////////////////////////////////////////////
rem Xerces-C Library configuration
rem for details see http://xerces.apache.org/xerces-c/

if defined XERCESCROOT (
  if not defined XERCESC_ROOT (
    set XERCESC_ROOT=%XERCESCROOT%
  )
) else (
  rem Specify path to the xerces-c root directory
  rem Check whether xercesc_root dependency file exists
  if not defined XERCESC_ROOT (
    if not exist "%xercesc_root_file%" (
      set ERROR_MESSAGE=File [%xercesc_root_file%] containing Xerces-C Library location doesn't exist
      goto ok
    )
    
    set /P XERCESC_ROOT=< "%xercesc_root_file%"
  ) 
)

if not defined XERCESC_ROOT (
  set ERROR_MESSAGE=Xerces-C Library location is undefined, please specify the library location first
  goto ok
)

if not exist "%XERCESC_ROOT%" (
  set ERROR_MESSAGE=Given [%XERCESC_ROOT%] Xerces-C Library location doesn't exist
  goto ok
)

rem Xerces-C
if not "%XERCESC_ROOT:~-1%"=="\" (
  if not "%XERCESC_ROOT:~-1%"=="/" (
    set XERCESC_ROOT=%XERCESC_ROOT%\
  )
)

rem ///////////////////////////////////////////////////////////////////////////
rem Everything is ok

:ok

echo.
echo Environment is properly set...
echo.

if not "%SMART_USE_PREBUILT_BOOST%"=="true" (
  echo Boost:          %BOOST_ROOT%
) else (
  echo Boost headers:  %BOOST_INCLUDE_PATH%
  echo Boost binaries: %BOOST_LIBRARY_PATH%
)

if defined XERCESC_ROOT (
  echo Xerces-C:       %XERCESC_ROOT%
) else (
  echo WARNING:        %ERROR_MESSAGE%
  echo WARNING:        Dependent smart libraries won't be built.
)

echo.
echo Ready to build smart...
echo.
set ERRORLEVEL=0

rem ///////////////////////////////////////////////////////////////////////////
rem Finalize current batch file
goto :eof

rem ///////////////////////////////////////////////////////////////////////////
rem An error handler
:handle_error
set ERRORLEVEL=1

