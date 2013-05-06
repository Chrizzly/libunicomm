///////////////////////////////////////////////////////////////////////////////
// http_request.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http request message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_REQUEST_HPP_
#define UNI_HTTP_REQUEST_HPP_

/** @file http.hpp Http request. */

#include "http_message_base.hpp"
#include "http_basic.hpp"

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** HTTP Request message. */
struct request : public message_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Message smart pointer type. */
  typedef boost::shared_ptr<request> pointer_type;

public:
  static const std::string& get(void) 
    { static const std::string s  = "GET"; return s; }
  static const std::string& post(void) 
    { static const std::string s = "POST"; return s; }
  static const std::string& head(void) 
    { static const std::string s = "HEAD"; return s; }

public:
  explicit request(const std::string& request_name = "", 
                   const std::string& uri = "", 
                   const std::string& data = ""): 
    message_base(data),
    _name(request_name),
    _uri(uri)
  { 
    // empty
  }

public:
  const std::string& name(void) const { return _name; }
  const std::string& uri(void) const { return _uri; }

  void name(const std::string& s) { _name = s; }
  void uri(const std::string& s) { _uri = s; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Serializes HTTP first string. */
  /*virtual*/ std::string serialize_first_str(void) const;

  /** Unserializes HTTP first string. */
  /*virtual*/ void unserialize_first_str(const std::string &s);

private:
  std::string _name;
  std::string _uri;
};

/** Whether request is get request. */
inline bool is_get(const request& r) { return r.name() == request::get(); }

/** Whether request is post request. */
inline bool is_post(const request& r) { return r.name() == request::post(); }

/** Whether request is head request. */
inline bool is_head(const request& r) { return r.name() == request::head(); }

} // namespace uni_http

#endif // UNI_HTTP_REQUEST_HPP_
