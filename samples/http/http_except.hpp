///////////////////////////////////////////////////////////////////////////////
// http_except.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Exceptions definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_EXCEPT_HPP_
#define UNI_HTTP_EXCEPT_HPP_

/** @file http_except.hpp Unicomm based HTTP implementation sample. */

#include <stdexcept>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** HTTP 400 bad request error. 
 *
 *  Thrown if invalid syntax encountered.
 */
class bad_request_error : public std::runtime_error
{
public:
  explicit bad_request_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** HTTP 422 unprocessible entity. 
 *
 *  Thrown if could not process request with such parameters (invariant error).
 */
class unprocessable_entity_error : public std::runtime_error
{
public:
  explicit unprocessable_entity_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** HTTP 404 not found. 
 *
 *  Thrown if could not find requested resource.
 */
class not_found_error : public std::runtime_error
{
public:
  explicit not_found_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** HTTP 411 length required. 
 *
 *  Thrown by the server if request doesn't contain length.
 */
class length_required_error : public std::runtime_error
{
public:
  explicit length_required_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** HTTP 500 internal server error. 
 *
 *  Thrown if an unexpected or unknown error occurs.
 */
class internal_server_error : public std::runtime_error
{
public:
  explicit internal_server_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

#ifdef UNICOMM_FORK_SUPPORT

/** Fork error. */
class fork_error : public std::runtime_error
{
public:
  explicit fork_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

#endif // UNICOMM_FORK_SUPPORT

} // namespace uni_http

#endif // UNI_HTTP_EXCEPT_HPP_
