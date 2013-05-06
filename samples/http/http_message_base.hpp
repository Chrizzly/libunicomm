///////////////////////////////////////////////////////////////////////////////
// http_message_base.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http message base class.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_MESSAGE_BASE_HPP_
#define UNI_HTTP_MESSAGE_BASE_HPP_

/** @file http_message_base.hpp Base http message. */

#include "http_basic.hpp"

#include <unicomm/message_base.hpp>

#include <boost/lexical_cast.hpp>

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** Parses http header and returns multimap<name, value> collection. */
header::header_collection_type parse_header(const std::string& header);

/** Http message base class. */
struct message_base : public unicomm::message_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit message_base(const std::string& data): 
    _data(data) 
  { 
    // empty
  }

public:
  message_base& add_header(const std::string& name, const std::string& value);
  message_base& remove_header(const std::string& name);
  const std::string& get_header(const std::string& name) const;
  void clear_headers(void);
  std::string headers_str(void) const;

  const std::string& data(void) const { return _data; }
  void data(const std::string& s) { _data = s; }

  /** Serializes message to a std::string. */
  std::string serialize(void) const;

  /** Restores message from a std::string. */
  void unserialize(const std::string &message);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Serializes http first string. */
  virtual std::string serialize_first_str(void) const = 0;

  /** Unserializes http first string. */
  virtual void unserialize_first_str(const std::string &message) = 0;

private:
  header::header_collection_type _headers;
  std::string _data;
};

/** Message pointer type. */
typedef message_base::pointer_type message_pointer_type;

} // namespace uni_http

#endif // UNI_HTTP_MESSAGE_BASE_HPP_
