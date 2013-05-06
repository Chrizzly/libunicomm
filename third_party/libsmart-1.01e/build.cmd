@echo off

rem //////////////////////////////////////////////////////////////////////////
rem build.cmd
rem 
rem Smart multipurpose C++ library.
rem 
rem Try .\build.cmd --help for more details.
rem
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2009, (c) Dmitry Timoshenko

rem //////////////////////////////////////////////////////////////////////////
rem push environment
setlocal

rem any not known error is unknown error
set ERROR_MESSAGE=Unknown error

rem get script location
set script_dir=%~dp0
set script_drive=%~d0

rem get current directory
set cur_dir=%CD%

rem get script name
set script_name=%~nx0

rem //////////////////////////////////////////////////////////////////////////
rem Go to the script location directory if it's necessary

rem add trailing slash if it's neccessary
if not "%cur_dir:~-1%"=="\" (
  if not "%cur_dir:~-1%"=="/" (
    set cur_dir=%cur_dir%\
  )
)

if not "%script_dir:~-1%"=="\" (
  if not "%script_dir:~-1%"=="/" (
    set script_dir=%script_dir%\
  )
)

rem echo.
rem echo We are here:     %cur_dir%
rem echo And script here: %script_dir%

rem goto script directory if it's neccessary
if not "%cur_dir%"=="%script_dir%" (
  echo ...Changing location to %script_dir%...
  %script_drive%
  cd %script_dir%
  
rem doesn't work due to cd doesn't set the ERRORLEVEL properly
rem if %ERRORLEVEL% neq 0 (
rem set ERROR_MESSAGE=Can't change location to [%script_dir%]
rem goto handle_error
rem )
)

rem So, intended to check that we are at the expected place by another way

set new_cur_dir=%CD%

rem add trailing slash if it's neccessary
if not "%new_cur_dir:~-1%"=="\" (
  if not "%new_cur_dir:~-1%"=="/" (
    set new_cur_dir=%new_cur_dir%\
  )
)

if not "%new_cur_dir%"=="%script_dir%" (
  set ERROR_MESSAGE=Can't change location to [%script_dir%]
  goto handle_error
)

rem //////////////////////////////////////////////////////////////////////////
rem Determines the configuration specified by command line

:begin_loop
if "%~1"=="" goto break_loop

rem //////////////////////////////////////////////////////////////////////////
rem Set variable to tell that we use ssl in order to set envrionment
rem appropriately
if "%~1"=="--use-pre-built-boost" set SMART_USE_PREBUILT_BOOST=true
if "%~1"=="--help" goto print_help
if "%~1"=="-h" goto print_help
if "%~1"=="/?" goto print_help

shift
goto begin_loop

:break_loop

rem //////////////////////////////////////////////////////////////////////////
rem Do all the work here

echo.
echo /////////////////////////////////////////////////////////////////////////
echo Start prepearing environment to build smart...

cd ./build

if not "%*"=="" (
  echo %*
)

call ./build.cmd %*

if %ERRORLEVEL% neq 0 (
  cd ..
  goto handle_error
)

cd ..

rem //////////////////////////////////////////////////////////////////////////
rem And go back if it's neccessary - endlocal restore invironment
rem if not "%cur_dir%"=="%script_dir%" (
rem  echo ...Going back to [%cur_dir%]...
rem   cd %cur_dir%
rem   echo We are again here: %CD%
rem )

echo.
echo Smart building finished: ERRORLEVEL = '%ERRORLEVEL%'

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal
goto :eof

rem ///////////////////////////////////////////////////////////////////////////
rem An error handler
:handle_error
echo.
echo ---------------------------
echo Error has been encountered:
echo   %ERROR_MESSAGE%
echo.
echo Please, correct the situation and try again
echo.
echo error: Smart building failed

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal

set ERRORLEVEL=1
goto :eof

:print_help
echo.
echo Usage: %script_name% [OPTION]
echo.
echo To succesfully build unicomm it's neccessary to provide information about
echo dependencies location on build machine. Assign correct values to
echo following variables:
echo   BOOST_ROOT only in case of source tree boost usage, default
echo   BOOST_INCLUDE_PATH only in case of pre-built boost usage
echo   BOOST_LIBRARY_PATH only in case of pre-built boost usage
echo   XERCESC_ROOT if you are intended to build smart_complex library
echo.
echo or you may use appropriate files in
echo SMART_ROOT/build/dependencies directory.
echo.
echo Invoked without any parameter causes release in
echo static linkage configurations will be built.
echo To build all possible variants use ./build_unicomm.sh script.
echo You are also enabled to specify what configuration you need exactly.
echo.
echo NOTE: All the parameters specified are just passed to boost bjam engine.
echo       So you are able to use any syntax bjam allows.
echo       By default, on Windows platforms msvc toolset is used. On Linux platform
echo       gcc is used if not specified explicitly.
echo.
echo Something like this:
echo   ./build.sh variant=debug,release toolset=gcc
echo.
echo The options with no '--' prefix are bjam options and can be used
echo in different forms like
echo.
echo   ./build.sh debug release
echo.
echo 'variant' is ommited in last line.
echo More details available in boost.build's documentation.
echo.
echo Defaults for the options are specified in brackets if there are.
echo.
echo Configuration:
echo.
echo   -h, --help                Display this help and exit
echo.
echo   --use-pre-built-boost=^<BOOST_VER^>
echo                             Designates to use pre-built boost libraries
echo                             instead of using boost source tree. You 
echo                             should define either BOOST_ROOT or 
echo                             BOOST_INCLUDE_PATH in couple with 
echo                             BOOST_LIBRARY_PATH depending on the option 
echo                             presence. To make the boost build to find 
echo                             pebuilt libraries it's necessary to specify 
echo                             exact version of boost e.g. './build.sh 
echo                             --use-pre-built-boost=1.46.1' will use 1.46.1 
echo                             to resolve the names.
echo. 
echo   --clean                   Cleans the given configuration. This will remove 
echo                             smart related targets within the configuration 
echo                             specified. The targets declared by dependencies 
echo                             (e.g. boost) are not affected. For instance 
echo                             './build.sh --clean' removes all the destination 
echo                             files regarding to 'variant=release' and 
echo                             'link=static,shared' i.e. removes the default 
echo                             configuration's main targets.
echo.
echo   --clean-all               Cleans the given configuration and all it's 
echo                             dependencies. This will remove smart related 
echo                             targets within the configuration specified and 
echo                             all the dependencies are built unlike '--clean' 
echo                             which removes only the smart related targets.
echo.
echo   toolset=^<TOOLSET^>         Use given toolset.
echo                             The toolset identifier string can be one of
echo                             gcc^|msvc^|intel. There are only gcc and msvc 
echo                             are guaranteed to work properly with smart.
echo                             [automatically detected]
echo.
echo   define=^<MACRO^>            Define the specified macro.
echo                             Please, see smart manuals for macroses being used
echo                             by smart.
echo.
echo   variant=^<VARIANT^>         Select one or more the build variants.
echo                             debug^|release are possible.
echo                             [release]
echo.

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal
