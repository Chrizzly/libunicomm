///////////////////////////////////////////////////////////////////////////////
// auto_sence.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file auto_sence.hpp Automatic platform and compiler selection. */

#ifndef SMART_AUTO_SENSE_HPP_
#define SMART_AUTO_SENSE_HPP_

//////////////////////////////////////////////////////////////////////////
//  Here try to autodetect the platform being used

#if defined (SOLARIS) || defined (__SVR4)
# define SMART_SOLARIS
# define SMART_UNIX
#elif defined (__linux__)
# define SMART_LINUX
# define SMART_UNIX

# ifdef __s390__
#   define SMART_LINUX_390
# endif
#elif defined (__FreeBSD__)
# define SMART_FREEBSD
# define SMART_UNIX
#elif defined (__CYGWIN__)
# define SMART_CYGWIN
#elif defined (_WIN32)
# define SMART_WIN32
# define SMART_WIN
#elif defined (_WIN64)
# define SMART_WIN64
# define SMART_WIN
#elif defined (macintosh) || (defined(__APPLE__) && defined(__MACH__))
# define SMART_MACOS
#else
# error Code requires port to host OS!
#endif

//////////////////////////////////////////////////////////////////////////
//  This section attempts to autodetect the compiler being used

#if defined (__BORLANDC__)
// bcb is actually not supported
# define SMART_BORLAND
# ifndef SMART_TOOLSET_NAME
#   define SMART_TOOLSET_NAME "bcb"
# endif // SMART_TOOLSET_NAME
#elif defined (_MSC_VER)
# define SMART_VISUAL_CPP _MSC_VER

# ifndef SMART_TOOLSET_NAME
#   if (_MSC_VER == 1300)
// vc7:
#     define SMART_TOOLSET_NAME "vc7"
#   elif (_MSC_VER == 1310)
// vc71:
#     define SMART_TOOLSET_NAME "vc71"
#   elif (_MSC_VER == 1400)
// vc80:
#     define SMART_TOOLSET_NAME "vc80"
#   elif (_MSC_VER == 1500)
// vc90:
#     define SMART_TOOLSET_NAME "vc90"
#   elif (_MSC_VER >= 1600)
// vc10:
#     define SMART_TOOLSET_NAME "vc100"
#   endif // _MSC_VER == 1300
# endif // SMART_TOOLSET_NAME

#elif defined (__INTEL_COMPILER)
# define SMART_INTEL
# ifndef SMART_TOOLSET_NAME
#   define SMART_TOOLSET_NAME "iw"
# endif // SMART_TOOLSET_NAME
#elif defined (SMART_SOLARIS)
# if defined (__SUNPRO_CC) && (__SUNPRO_CC >=0x500)
#   define SMART_SUNCC5
# elif defined (__SUNPRO_CC) && (__SUNPRO_CC <0x500)
#   define SMART_SUNCC
# elif defined (_EDG_RUNTIME_USES_NAMESPACES)
#   define SMART_SOLARIS_KAICC
# elif defined (__GNUG__)
#   define SMART_GNUG
# else
#   error Code requires port to current development environment
# endif
#elif defined (__GNUG__) || defined (__BEOS__) || defined (__linux__) || defined (__CYGWIN__)
# define SMART_GNUG
# ifndef SMART_TOOLSET_NAME
#   define SMART_TOOLSET_NAME "gcc"
# endif // SMART_TOOLSET_NAME
#else
# error Code requires port to current development environment
#endif

//////////////////////////////////////////////////////////////////////////
// Get the current configuration

#if defined (_DEBUG) || !defined (NDEBUG)
# define SMART_DEBUG
# define SMART_CONFIG_NAME "debug-"
#else 
# define SMART_CONFIG_NAME ""
#endif // SMART_DEBUG

//////////////////////////////////////////////////////////////////////////
// Platform specific

// Allow use of features specific to Windows XP or later.
// Change this to the appropriate value to target other versions of Windows.
#if defined(SMART_VISUAL_CPP) && !defined(_WIN32_WINNT)
# define _WIN32_WINNT 0x0501
#endif // _WIN32_WINNT

#define SMART_VERSION "1_01"

#endif // SMART_AUTO_SENSE_HPP_
