@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem unwind_path.cmd
rem
rem Unified Communication protocol C++ library.
rem
rem Expose relative path to absolute.
rem
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2012, (c) Dmitry Timoshenko

cd .\deps
set %2=%~dpnx1
cd ..
