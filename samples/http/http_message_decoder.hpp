///////////////////////////////////////////////////////////////////////////////
// http_message_decoder.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http message decoder. 
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_MESSAGE_DECODER_HPP_
#define UNI_HTTP_MESSAGE_DECODER_HPP_

/** @file http_message_decoder.hpp Http message decoder. */

#include "http_basic.hpp"

#include <unicomm/message_decoder_base.hpp>

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** Unicomm http message decoder class. */
class message_decoder : public unicomm::message_decoder_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  static unicomm::message_decoder_base::pointer_type create(void) 
  { 
    return unicomm::message_decoder_base::pointer_type(new message_decoder());
  }

public:
  message_decoder(void): _length(0), _last_buf_size(0) { /* emtpy */ }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  virtual iter_pair_type find_raw_message(unicomm::comm_buffer::buffer_type& buffer, 
    unicomm::session_base& session);
  virtual std::string& decode_raw_message(std::string& raw_message, 
    unicomm::session_base& session);
  virtual std::string get_message_name(const std::string& raw_message, 
    unicomm::session_base& session);

private:
  iter_pair_type finish_decode(unicomm::in_buffer_type& buffer);

private:
  size_t _length;
  size_t _last_buf_size;
};

} // namespace unicomm

#endif // UNI_HTTP_MESSAGE_DECODER_HPP_
