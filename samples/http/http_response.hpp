///////////////////////////////////////////////////////////////////////////////
// http_response.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http response message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_RESPONSE_HPP_
#define UNI_HTTP_RESPONSE_HPP_

/** @file http_response.hpp Http response. */

#include "http_message_base.hpp"
#include "http_basic.hpp"

#include <boost/shared_ptr.hpp>

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** HTTP Reply message. */
struct response : public message_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Message smart pointer type. */
  typedef boost::shared_ptr<response> pointer_type;

public:
  explicit response(const std::string& status_code = status_code::ok(),
                    const std::string& data = ""): 
    message_base(data),
    _status_code(status_code)
  { 
    // empty
  }

public:
  const std::string& name(void) const 
    { static const std::string s = ""; return s; }

public:
  const std::string& status_code(void) const { return _status_code; }

  void status_code(const std::string& s) { _status_code = s; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Serializes HTTP first string. */
  /*virtual*/ std::string serialize_first_str(void) const;

  /** Unserializes HTTP first string. */
  /*virtual*/ void unserialize_first_str(const std::string &s);

private:
  std::string _status_code;
};

} // namespace uni_http

#endif // UNI_HTTP_RESPONSE_HPP_
