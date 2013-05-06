///////////////////////////////////////////////////////////////////////////////
// auto_link.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// This header is used to make library linkage automated.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifndef UNI_AUTO_LINK_HPP_
#define UNI_AUTO_LINK_HPP_

/** @file auto_link.hpp Automatic MSVC linkage. 
 *
 *  @note To disable automatic linkage define UNICOMM_NO_AUTO_LINK.
 */

#include <unicomm/config/auto_sence.hpp>

#if defined (UNI_VISUAL_CPP)

// SSL linkage
# ifndef UNICOMM_NO_SSL_AUTO_LINK
#   ifdef UNICOMM_DEBUG 
#     ifdef UNICOMM_SSL 
#       ifdef UNICOMM_LINK_STATIC_SSL
#         pragma comment(lib, "libeay32MTd.lib")
#         pragma comment(lib, "ssleay32MTd.lib")
#       else // UNICOMM_LINK_STATIC_SSL
#         pragma comment(lib, "libeay32.lib")
#         pragma comment(lib, "ssleay32.lib")
#       endif // UNICOMM_LINK_STATIC_SSL
#     endif // UNICOMM_SSL 
#   else // UNICOMM_DEBUG
#     ifdef UNICOMM_SSL
#       ifdef UNICOMM_LINK_STATIC_SSL
#         pragma comment(lib, "libeay32MT.lib")
#         pragma comment(lib, "ssleay32MT.lib")
#       else // UNICOMM_LINK_STATIC_SSL
#         pragma comment(lib, "libeay32.lib")
#         pragma comment(lib, "ssleay32.lib")
#       endif // UNICOMM_LINK_STATIC_SSL
#     endif // UNICOMM_SSL
#   endif // UNICOMM_DEBUG
# endif // UNICOMM_NO_SSL_AUTO_LINK

# if !defined (UNICOMM_NO_AUTO_LINK) && !defined (UNICOMM_SOURCE)
#   ifdef UNICOMM_DYN_LINK
#     define UNICOMM_LIB_SHORT_NAME "unicomm"
#   else // UNICOMM_DYN_LINK
#     define UNICOMM_LIB_SHORT_NAME "libunicomm"
#   endif // UNICOMM_DYN_LINK
#   define UNICOMM_LIB_FULL_NAME UNICOMM_LIB_SHORT_NAME "-" UNICOMM_TOOLSET_NAME "-" \
      UNICOMM_SSL_CONFIG_NAME UNICOMM_CONFIG_NAME UNICOMM_VERSION ".lib"
#   pragma comment(lib, UNICOMM_LIB_FULL_NAME)
# endif // #if !defined (UNICOMM_NO_AUTO_LINK) && !defined (UNICOMM_SOURCE)

#endif // UNI_VISUAL_CPP

#endif // UNI_AUTO_LINK_HPP_

