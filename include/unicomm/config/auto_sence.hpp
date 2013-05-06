///////////////////////////////////////////////////////////////////////////////
// auto_sence.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Automatic platform and compiler selection. 
// Also necessary settings are perfomed.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifndef UNI_AUTO_SENSE_HPP_
#define UNI_AUTO_SENSE_HPP_

/** @file auto_sence.hpp Automatic platform and compiler selection. */

#include <smart/config/auto_sence.hpp>

//////////////////////////////////////////////////////////////////////////
//  Here try to auto detect the platform being used

#if defined (SOLARIS) || defined (__SVR4)
# define UNI_SOLARIS
# define UNI_UNIX
#elif defined (__linux__)
# define UNI_LINUX
# define UNI_UNIX

# ifdef __s390__
#   define UNI_LINUX_390
# endif
#elif defined (__FreeBSD__)
# define UNI_FREEBSD
# define UNI_UNIX
#elif defined (__CYGWIN__)
# define UNI_CYGWIN
#elif defined (_WIN32)
# define UNI_WIN32
# define UNI_WIN
#elif defined (_WIN64)
# define UNI_WIN64
# define UNI_WIN
#elif defined (macintosh) || (defined(__APPLE__) && defined(__MACH__))
# define UNI_MACOS
#else
# error Code requires port to host OS!
#endif

//////////////////////////////////////////////////////////////////////////
// This section attempts to auto detect the compiler being used

#if defined (__BORLANDC__)
# define UNI_BORLAND
#elif defined (_MSC_VER)
# define UNI_VISUAL_CPP
#elif defined (__INTEL_COMPILER)
# define UNI_INTEL
#elif defined (UNI_SOLARIS)
# if defined (__SUNPRO_CC) && (__SUNPRO_CC >=0x500)
#   define UNI_SUNCC5
# elif defined (__SUNPRO_CC) && (__SUNPRO_CC <0x500)
#   define UNI_SUNCC
# elif defined (_EDG_RUNTIME_USES_NAMESPACES)
#   define UNI_SOLARIS_KAICC
# elif defined (__GNUG__)
#   define UNI_GNUG
# else
#   error Code requires port to current development environment
# endif
#elif defined (__GNUG__) || defined (__BEOS__) || defined (__linux__) || defined (__CYGWIN__)
# define UNI_GNUG
#else
# error Code requires port to current development environment
#endif

#ifdef SMART_DEBUG
# define UNICOMM_DEBUG
#endif // SMART_DEBUG

#ifdef SMART_TOOLSET_NAME
# define UNICOMM_TOOLSET_NAME SMART_TOOLSET_NAME
#endif // SMART_TOOLSET_NAME

#ifdef SMART_CONFIG_NAME
# define UNICOMM_CONFIG_NAME SMART_CONFIG_NAME
#endif // SMART_CONFIG_NAME

#ifdef UNICOMM_SSL
# define UNICOMM_SSL_CONFIG_NAME "ssl-"
#else 
# define UNICOMM_SSL_CONFIG_NAME ""
#endif // UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// Microsoft Visual C++ dll macro definition

#if (defined (UNI_VISUAL_CPP) || defined (UNI_INTEL)) && defined (UNICOMM_DYN_LINK)
# ifdef UNICOMM_SOURCE
#   define UNICOMM_DECL __declspec(dllexport)
# else // UNICOMM_SOURCE
#   define UNICOMM_DECL __declspec(dllimport)
# endif // UNICOMM_SOURCE
#endif // #if defined (UNI_VISUAL_CPP) && defined (UNICOMM_DYN_LINK)

#ifndef UNICOMM_DECL
# define UNICOMM_DECL
#endif // UNICOMM_DECL

//////////////////////////////////////////////////////////////////////////
// Using complex xml engine
// Define UNICOMM_USE_COMPLEX_XML to use xml

#if !defined (UNICOMM_DYN_LINK) && !defined (UNICOMM_USE_COMPLEX_XML) && defined (UNICOMM_SOURCE)
# define UNICOMM_USE_COMPLEX_XML 
#endif // UNICOMM_DYN_LINK

#ifdef UNICOMM_DEBUG_VERBOSE 
# include <smart/debug_out.hpp> 
# define UNICOMM_DEBUG_OUT(expr) SMART_MT_DEBUG_OUT(expr)
# define UNICOMM_IFDEF_DEBUG(expr) SMART_IFDEF_DEBUG(expr)
#else // UNICOMM_DEBUG_VERBOSE 
# define UNICOMM_DEBUG_OUT(expr)
# define UNICOMM_IFDEF_DEBUG(expr)
#endif // UNICOMM_DEBUG_VERBOSE

#ifdef UNI_VISUAL_CPP
# pragma warning (disable : 4503) // warning C4503: 'std::_Tree<_Traits>::insert' : decorated name length exceeded, name was truncated
#endif // UNI_VISUAL_CPP

#if defined (UNICOMM_FORK_SUPPORT) && !defined (UNI_UNIX)
# error "Current platform seems doesn't support fork operation"
#endif // UNICOMM_FORK_SUPPORT

#define UNICOMM_VERSION "2_17"

#endif // UNI_AUTO_SENSE_HPP_


