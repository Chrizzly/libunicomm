@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem build-complete.cmd
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

rem Start building
call build.cmd variant=debug,debug-ssl,release,release-ssl link=shared,static %*

