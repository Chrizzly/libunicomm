///////////////////////////////////////////////////////////////////////////////
// auto_link.hpp
//
// Copyright (c) 2008, 2009 Dmitry Timoshenko, Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file auto_link.hpp Automatic linkage utility. */

//#ifndef SMART_AUTO_LINK_HPP_
//#define SMART_AUTO_LINK_HPP_

#include <smart/config/auto_sence.hpp>

#ifndef SMART_NO_AUTO_LINK

#ifndef SMART_SHORT_LIB_NAME
# error SMART_SHORT_LIB_NAME is not defined!
#endif // SMART_SHORT_LIB_NAME

#if defined (SMART_VISUAL_CPP) || defined (SMART_BORLAND)
# define SMART_FULL_LIB_NAME "libsmart_" SMART_SHORT_LIB_NAME "-" SMART_TOOLSET_NAME "-" SMART_CONFIG_NAME SMART_VERSION ".lib"
#endif // defined SMART_VISUAL_CPP || defined SMART_BORLAND

#if defined (SMART_VISUAL_CPP)
# pragma comment(lib, SMART_FULL_LIB_NAME)
//# pragma message ("Smart Autolink, linking: " SMART_FULL_LIB_NAME "...")
#elif defined (SMART_BORLAND)
# pragma link SMART_FULL_LIB_NAME
#elif defined (SMART_GNUG)
//# pragma message("GNU C++ is used!") // Check, what should be added here!
#else // unresolved compiler
# error Unable to resolve used compiler
#endif // defined SMART_VISUAL_CPP
#endif // SMART_NO_AUTO_LINK

//#endif // SMART_AUTO_LINK_HPP_

