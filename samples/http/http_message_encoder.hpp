///////////////////////////////////////////////////////////////////////////////
// http_message_encoder.hpp
//
// unicomm++ - Unified Communication Protocol C++ Library.
//
// Http server & client example based on unicomm engine.
// Http message encoder. 
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_MESSAGE_ENCODER_HPP_
#define UNI_HTTP_MESSAGE_ENCODER_HPP_

/** @file http_message_encoder.hpp Http message encoder. */

#include <unicomm/message_encoder_base.hpp>
#include <unicomm/basic.hpp>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** Unicomm http message encoder class. */
class message_encoder : public unicomm::message_encoder_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  static unicomm::message_encoder_base::pointer_type create(void) 
  { 
    return unicomm::message_encoder_base::pointer_type(new message_encoder());
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  unicomm::out_buffer_type& encode_raw_message(unicomm::out_buffer_type& buffer, 
    unicomm::session_base& session);
};

} // namespace uni_http

#endif // UNI_HTTP_MESSAGE_ENCODER_HPP_
