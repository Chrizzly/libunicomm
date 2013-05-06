@echo off

rem //////////////////////////////////////////////////////////////////////////
rem build.cmd
rem
rem Unified Communication protocol C++ library.
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

set UNICOMM_SSL=false

:begin_loop
if "%~1"=="" goto break_loop

rem //////////////////////////////////////////////////////////////////////////
rem Set variable to tell that we use ssl in order to set envrionment
rem appropriately
if "%~1"=="debug-ssl" set UNICOMM_SSL=true
if "%~1"=="release-ssl" set UNICOMM_SSL=true
if "%~1"=="variant=debug-ssl" set UNICOMM_SSL=true
if "%~1"=="variant=release-ssl" set UNICOMM_SSL=true
if "%~1"=="--use-complex-xml" set UNICOMM_USE_COMPLEX_XML=true
if "%~1"=="UNICOMM_USE_COMPLEX_XML" set UNICOMM_USE_COMPLEX_XML=true
if "%~1"=="define=UNICOMM_USE_COMPLEX_XML" set UNICOMM_USE_COMPLEX_XML=true
if "%~1"=="--use-pre-built-boost" set UNICOMM_USE_PREBUILT_BOOST=true
if "%~1"=="--help" goto print_help
if "%~1"=="-h" goto print_help
if "%~1"=="/?" goto print_help

rem if "%UNICOMM_SSL%"=="true" (
rem   if "%UNICOMM_USE_COMPLEX_XML%"=="true" (
rem     if "%UNICOMM_USE_PREBUILT_BOOST%"=="true" goto break_loop
rem   )
rem )

shift
goto begin_loop

:break_loop

rem //////////////////////////////////////////////////////////////////////////
rem Do all the work here

echo.
echo /////////////////////////////////////////////////////////////////////////
echo Start prepearing environment to build unicomm...

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

echo.
echo Unicomm building finished: ERRORLEVEL = '%ERRORLEVEL%'

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
echo Please, fix it and try again
echo.
echo ERROR: Unicomm building failed
echo.

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal

set ERRORLEVEL=1
goto :eof

:print_help
echo.
echo Usage: %script_name% [options] [properties] [targets]
echo.
echo To succesfully build unicomm it's neccessary to provide information about
echo dependencies location on build machine. Assign correct values to
echo following variables:
echo   BOOST_ROOT only in case of source tree boost usage
echo   BOOST_INCLUDE_PATH only in case of pre-built boost usage
echo   BOOST_LIBRARY_PATH only in case of pre-built boost usage
echo   OPENSSL_ROOT in case if you wish to use ssl with unicomm
echo   XERCESC_ROOT if you are intended to use XML engine with the unicomm
echo.
echo or you may use appropriate files located in
echo 'UNICOMM_ROOT/build/deps' subdirectory.
echo.
echo Invoked without parameters causes release in
echo static and shared linkage configurations will be built.
echo To build all possible variants use '.\build-complete.cmd' command.
echo You are also enabled to specify what configuration you need exactly.
echo.
echo NOTE: All the parameters specified are just passed to boost bjam engine.
echo       So you are able to use any syntax bjam allows.
echo       By default, on Windows platforms msvc toolset is used. On Linux platform
echo       gcc is used if not specified explicitly.
echo.
echo Use something like this:
echo   .\build.cmd variant=debug-ssl,release-ssl link=shared toolset=msvc-8.0
echo   .\build.cmd debug link=shared toolset=msvc
echo   .\build.cmd variant=debug,release link=shared toolset=msvc-10.0
echo   .\build-complete.cmd toolset=gcc
echo   .\build-complete.cmd
echo.
echo The options with no '--' prefix are bjam options and can be used
echo in different forms like
echo.
echo   .\build.cmd variant=debug variant=release
echo   .\build.cmd variant=debug,release
echo   .\build.cmd debug release
echo.
echo More details available in boost.build's documentation.
echo.
echo Defaults for the options are specified in brackets if there are.
echo.
echo Options:
echo.
echo   -h, --help                Display this help and exit
echo.
echo   --prefix=^<PREFIX^>         Install unicomm files here. By default is 
echo                             not used. To force the installation to the 
echo                             prefix location specify the option or target
echo                             'unicomm-install' (see below) explicitly.
echo                             Binaries are installed into ^<PREFIX^>/lib and
echo                             headers into ^<PREFIX^>/include/unicomm, i.e.
echo                             if '--prefix=c:\unicomm' the libraries will be
echo                             copyied into 'c:\unicomm\lib' and headers
echo                             will appear in 'c:\unicomm\include\unicomm'.
echo                             If 'unicomm-install' target and '--prefix' 
echo                             option is absent binaries installed into 
echo                             'UNICOMM_ROOT/lib' subdirectory.
echo                             [c:\unicomm].
echo.
echo   --use-pre-built-boost=^<BOOST_VER^>
echo                             Designates to use pre-built boost libraries
echo                             instead of using boost source tree. You should
echo                             define either BOOST_ROOT or BOOST_INCLUDE_PATH
echo                             in couple with BOOST_LIBRARY_PATH depending on
echo                             the option presence. To make the boost build 
echo                             to find pebuilt libraries it's necessary to 
echo                             specify exact version of boost e.g. 
echo                             'build.cmd --use-pre-built-boost=1.53' will
echo                             use 1.53 to resolve the names.
echo.
echo   --clean                   Cleans the given configuration. This will remove 
echo                             unicomm related targets within the configuration 
echo                             specified. The targets declared by dependencies 
echo                             (e.g. boost) are not affected. For instance 
echo                             'build.cmd --clean' removes all the destination 
echo                             files regarding to 'variant=release' and 
echo                             'link=static,shared' i.e. removes the default 
echo                             configuration's main targets.
echo.
echo   --clean-all               Cleans the given configuration and all it's 
echo                             dependencies. This will remove unicomm related 
echo                             targets within the configuration specified and 
echo                             all the dependencies are built.
echo.
echo   --win-ver=^<WIN_VER^>       Used to specify Windows system version 
echo                             explicitly.
echo                             [0x501] - Windows XP.
echo.
echo   --use-complex-xml         Makes the XML engine to be available.
echo                             An alternate way to designate to use XML 
echo                             is to specify 'define=UNICOMM_USE_COMPLEX_XML' 
echo                             parameter in command line.
echo.
echo Properties:
echo.
echo   toolset=^<TOOLSET^>         Use given toolset.
echo                             The toolset identifier string can be one of
echo                             gcc^|msvc. There are only gcc and msvc 
echo                             are guaranteed to work properly with unicomm.
echo                             [automatically detected]
echo.
echo   define=^<MACRO^>            Define the specified macro.
echo                             Please, see unicomm manuals for macroses being 
echo                             used by unicomm: http://libunicomm.org?page=manuals.
echo.
echo   variant=^<VARIANT^>         Select one or more build variants.
echo                             debug^|release^|debug-ssl^|release-ssl are possible.
echo                             [release]
echo.
echo   link=^<LINK^>               Whether to build static or shared libraries.
echo                             Allowed values static^|shared.
echo                             [static,shared]
echo.
echo Targets:
echo.
echo   unicomm-install           Installs unicomm to the prefix location.
echo   http-install              Build and install http sample.
echo   term-install              Build and install term sample.
echo   echo-install              Build and install echo sample.
echo.
echo NOTE: Samples installed to the 'UNICOMM_ROOT/out/samples/boost-build' 
echo       subdirectory.
echo.

rem //////////////////////////////////////////////////////////////////////////
rem pop environment
endlocal
