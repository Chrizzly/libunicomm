@echo off

rem ///////////////////////////////////////////////////////////////////////////
rem build_all_configurations.cmd
rem
rem Smart multipurpose C++ library.
rem
rem Try .\build.cmd --help for more details.
rem
rem NOTE: Smart is only built as static libraries.
rem
rem Distributed under the Boost Software License, Version 1.0.
rem (See accompanying file LICENSE_1_0.txt or copy at 
rem http://www.boost.org/LICENSE_1_0.txt)
rem 
rem 2009, (c) Dmitry Timoshenko

rem Start building
call build.cmd variant=debug,release link=static %*

