///////////////////////////////////////////////////////////////////////////////
// http_aux.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Auxiliary entities.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_AUX_HPP_
#define UNI_HTTP_AUX_HPP_

/** @file http_aux.hpp Unicomm based HTTP implementation sample auxiliary entities. */

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

#ifdef UNICOMM_FORK_SUPPORT

bool is_child_process(void);
bool is_parent_process(void);
void pid(int v);
int pid(void);

int check_fork(int fork_result);

#endif // UNICOMM_FORK_SUPPORT

/** Synchronizes an access to the std::cout. */
void out_str(const std::string& s);

} // namespace uni_http

#endif // UNI_HTTP_EXCEPT_HPP_
