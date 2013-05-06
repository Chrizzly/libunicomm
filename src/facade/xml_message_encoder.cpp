///////////////////////////////////////////////////////////////////////////////
// xml_message_encoder.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol xml message encoder.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#include <unicomm/facade/xml_message_encoder.hpp>

#ifdef UNICOMM_USE_COMPLEX_XML

#include <detail/basic_detail.hpp>

using std::string;

//////////////////////////////////////////////////////////////////////////
// xml message encoder
unicomm::out_buffer_type& 
unicomm::xml_message_encoder::encode_raw_message(out_buffer_type& buffer, 
                                                 session_base& /*session*/)
{
  return buffer.append(detail::xml_message_end());
}

#endif // UNICOMM_USE_COMPLEX_XML
