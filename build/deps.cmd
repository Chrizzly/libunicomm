@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem deps.cmd
rem
rem Unified Communication protocol C++ library.
rem
rem Sets up the unicomm dependencies.
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
set smart_root_file=.\deps\smart_root-win
set openssl_root_file=.\deps\openssl_root-win
set xercesc_root_file=.\deps\xercesc_root-win

rem ///////////////////////////////////////////////////////////////////////////
rem Boost Library configuration
rem for details see http://www.boost.org

rem Path to boost source tree setup

if "%UNICOMM_USE_PREBUILT_BOOST%"=="true" goto use_prebult_boost

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
  set ERROR_MESSAGE=BOOST_ROOT: Boost Library location is undefined
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
  set ERROR_MESSAGE=BOOST_INCLUDE_PATH: Boost Library location is undefined
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
  set ERROR_MESSAGE=BOOST_LIBRARY_PATH: Boost Library location is undefined
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
rem Smart Library configuration
rem for details see http://smartxx.org

if not defined SMART_ROOT (
  rem Specify path to the smart root directory
  rem Check whether smart_root dependency file exists
  if not exist "%smart_root_file%" (
    set ERROR_MESSAGE=File [%smart_root_file%] containing Smart Library location doesn't exist
    goto handle_error
  )

  set /P SMART_ROOT=< "%smart_root_file%"
)

if not defined SMART_ROOT (
  set ERROR_MESSAGE=Smart Library location is undefined, please specify the path which library is available through
  goto handle_error
)

call .\unwind_path.cmd "%SMART_ROOT%" SMART_ROOT

if not exist "%SMART_ROOT%" (
  set ERROR_MESSAGE=Given [%SMART_ROOT%] Smart Library location doesn't exist
  goto handle_error
)

rem add trailing slash if it's neccessary
if not "%SMART_ROOT:~-1%"=="\" (
  if not "%SMART_ROOT:~-1%"=="/" (
    set SMART_ROOT=%SMART_ROOT%\
  )
)

rem ///////////////////////////////////////////////////////////////////////////
rem OpenSSL Library configuration
rem for details see http://www.openssl.org

REM echo UNICOMM_SSL = %UNICOMM_SSL%
REM set OPENSSL_ROOT=asdf

rem Have no idea why this works that way... hm
rem Inside this commented if OPENSSL_ROOT 
rem and may be other variables (didn't check) could not be set properly
rem microsoft bug?

if not "%UNICOMM_SSL%"=="true" goto skip_ssl

rem if "%UNICOMM_SSL%"=="true" (
  if not defined OPENSSL_ROOT (
    rem Specify path to the openssl root directory
    rem Check whether openssl_root dependency file exists
    if not exist "%openssl_root_file%" (
      set ERROR_MESSAGE=File [%openssl_root_file%] containing OpenSSL Library location doesn't exist
      goto handle_error
    )

    set /P OPENSSL_ROOT=< "%openssl_root_file%"
  )

  if not defined OPENSSL_ROOT (
    set ERROR_MESSAGE=OpenSSL Library location is undefined, please specify the path which library is available through
    goto handle_error
  )
  
  if not exist "%OPENSSL_ROOT%" (
    set ERROR_MESSAGE=Given [%OPENSSL_ROOT%] OpenSSL Library location doesn't exist
    goto handle_error
  )
rem )

rem add trailing slash if it's neccessary
if not "%OPENSSL_ROOT:~-1%"=="\" (
  if not "%OPENSSL_ROOT:~-1%"=="/" (
    set OPENSSL_ROOT=%OPENSSL_ROOT%\
  )
)

:skip_ssl

rem ///////////////////////////////////////////////////////////////////////////
rem Xerces-C Library configuration
rem for details see http://xerces.apache.org/xerces-c/

if not "%UNICOMM_USE_COMPLEX_XML%"=="true" goto skip_xercesc

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
      goto handle_error
    )
    
    set /P XERCESC_ROOT=< "%xercesc_root_file%"
  ) 
)

if not defined XERCESC_ROOT (
  set ERROR_MESSAGE=Xerces-C Library location is undefined, please specify the library location first
  goto handle_error
)

if not exist "%XERCESC_ROOT%" (
  set ERROR_MESSAGE=Given [%XERCESC_ROOT%] Xerces-C Library location doesn't exist
  goto handle_error
)

rem add trailing slash if it's neccessary
if not "%XERCESC_ROOT:~-1%"=="\" (
  if not "%XERCESC_ROOT:~-1%"=="/" (
    set XERCESC_ROOT=%XERCESC_ROOT%\
  )
)

:skip_xercesc

rem ///////////////////////////////////////////////////////////////////////////
rem Everything is ok

echo.
echo Environment is properly set...
echo.

if not "%UNICOMM_USE_PREBUILT_BOOST%"=="true" (
  echo Boost:          %BOOST_ROOT%
) else (
  echo Boost headers:  %BOOST_INCLUDE_PATH%
  echo Boost binaries: %BOOST_LIBRARY_PATH%
)

rem echo Smart:          %SMART_ROOT%

if "%UNICOMM_SSL%"=="true" (
  echo OpenSSL:        %OPENSSL_ROOT%
)

if "%UNICOMM_USE_COMPLEX_XML%"=="true" (
  echo Xerces-C:       %XERCESC_ROOT%
)

echo.
echo Ready to build unicomm...
echo.
set ERRORLEVEL=0

rem ///////////////////////////////////////////////////////////////////////////
rem Finalize current batch file
goto :eof

rem ///////////////////////////////////////////////////////////////////////////
rem An error handler
:handle_error
set ERRORLEVEL=1

