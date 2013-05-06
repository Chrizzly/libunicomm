///////////////////////////////////////////////////////////////////////////////
// http_message_encoder.cpp
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

#include "http_message_encoder.hpp"

//////////////////////////////////////////////////////////////////////////
// http message encoder
unicomm::out_buffer_type& 
uni_http::message_encoder::encode_raw_message(unicomm::out_buffer_type& buffer, 
                                              unicomm::session_base& /*session*/)
{
  return buffer;
}

