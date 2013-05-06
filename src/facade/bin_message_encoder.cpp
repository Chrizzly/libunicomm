///////////////////////////////////////////////////////////////////////////////
// bin_message_encoder.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol binary message encoder.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#include <unicomm/facade/bin_message_encoder.hpp>
#include <detail/basic_detail.hpp>

#include <string>

using std::string;

//////////////////////////////////////////////////////////////////////////
// auxiliary
namespace 
{

string& binary_encode(string& s)
{
  for (string::iterator it = s.begin(); it != s.end(); ++it)
  {
    if (unicomm::detail::is_bin_escape_sym(*it) || 
      unicomm::detail::is_bin_end_sym(*it))
    {
      *it = unicomm::detail::bin_process_one(*it);
      it  = s.insert(it, unicomm::detail::bin_escape_sym());
      ++it; // now points to just processed byte
    }
  }

  return s;
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// bin message encoder
unicomm::out_buffer_type& 
unicomm::bin_message_encoder::encode_raw_message(out_buffer_type& buffer, 
                                                 session_base& /*session*/)
{
  return binary_encode(buffer) += detail::bin_message_end();
}

