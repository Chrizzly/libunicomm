///////////////////////////////////////////////////////////////////////////////
// http_config.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// This is just an example shows how to use unicomm to define 
// different type of protocols based on tcp/ip. 
// There are only few messages defined not all of HTTP.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_CONFIG_HPP_
#define UNI_HTTP_CONFIG_HPP_

/** @file http_config.hpp Unicomm configuration. */

#include <unicomm/config.hpp>
#include <unicomm/client.hpp>

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** Returns a reference to the unicomm server configuration object. */
unicomm::config& server_config(void);

/** Returns a reference to the unicomm client configuration object. */
unicomm::config& client_config(void);

} // namespace uni_http

#endif // UNI_HTTP_CONFIG_HPP_
