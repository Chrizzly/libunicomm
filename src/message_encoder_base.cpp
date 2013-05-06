///////////////////////////////////////////////////////////////////////////////
// message_encoder_base.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol message encoder base class definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#include <unicomm/message_encoder_base.hpp>

using std::string;

//////////////////////////////////////////////////////////////////////////
// message decoder base

const unicomm::out_buffer_type 
unicomm::message_encoder_base::perform_encode(const message_base& message, 
                                              session_base& session)
{
  out_buffer_type out_buf = message.serialize();

  return encode_raw_message(out_buf, session);
}

