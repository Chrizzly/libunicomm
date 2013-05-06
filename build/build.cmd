@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem build.cmd
rem
rem Unified Communication protocol C++ library.
rem 
rem Unicomm build automation auxiliary script.
rem 
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2009, (c) Dmitry Timoshenko

call deps.cmd

if %ERRORLEVEL% neq 0 goto :eof

rem //////////////////////////////////////////////////////////////////////////
rem Invoke boost build engine

cd..

bjam.exe %*

if %ERRORLEVEL% neq 0 (
  set ERROR_MESSAGE=Boost bjam returned an error
  goto :eof
)

rem Compiler or maybe someboby else doesn't set ERRORLEVEL properly - depending on 
rem compilation result. %ERRORLEVEL% is always zero.
rem echo Unicomm is successfully built!

rem goto :eof

