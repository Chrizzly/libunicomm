///////////////////////////////////////////////////////////////////////////////
// bin_message_decoder.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol binary message decoder. 
// Accepts incoming data and creates user defined messages.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#include <unicomm/facade/bin_message_decoder.hpp>
#include <unicomm/except.hpp>
#include <unicomm/basic.hpp>
#include <detail/basic_detail.hpp>

#include <smart/utils.hpp>

#include <boost/bind.hpp>

#include <functional>

using std::string;
using std::not_equal_to;
using std::less;

//////////////////////////////////////////////////////////////////////////
// auxiliary
namespace 
{

string& binary_decode(string& s)
{
  for (string::iterator it = s.begin(); it != s.end(); ++it)
  {
    if (unicomm::detail::is_bin_escape_sym(*it))
    {
      if ((it = s.erase(it)) == s.end())
      {
        throw unicomm::message_decoder_error(
          "Can't decode message [Message is invalid]");
      }

      *it = unicomm::detail::bin_process_one(*it);
    } 
  }

  return s;
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// binary message decoder
unicomm::message_decoder_base::iter_pair_type 
unicomm::bin_message_decoder::find_raw_message(comm_buffer::buffer_type& buffer, 
                                               session_base& /*session*/)
{
  message_decoder_base::iter_pair_type bounds = make_pair(buffer.end(), buffer.end());

  const string::size_type pos = buffer.find(detail::bin_message_end());
  if (pos != string::npos)
  {
    // found something
    bounds.first  = buffer.begin();
    bounds.second = bounds.first + pos + sizeof(detail::bin_message_end());
  }

  return bounds;
}

//-----------------------------------------------------------------------------
string& unicomm::bin_message_decoder::decode_raw_message(string& raw_message, 
                                                         session_base& /*session*/)
{
  // remove marker
  raw_message.erase(raw_message.end() - sizeof(detail::bin_message_end()), 
    raw_message.end());

  // remove 'escape symbols'
  return binary_decode(raw_message);
}

//-----------------------------------------------------------------------------
string unicomm::bin_message_decoder::get_message_name(const string& raw_message, 
                                                      session_base& /*session*/)
{
  smart::throw_if<message_decoder_error>(boost::bind(less<size_t>(), 
    raw_message.size(), detail::bin_header_min_len()), 
    "Invalid message format [Incomplete header received]");

  // version
  smart::throw_if<message_decoder_error>(boost::bind(not_equal_to<size_t>(), 
    raw_message[0], detail::bin_version()), 
    "Invalid message format [Illegal version]");

  // header length
  const size_t head_len = raw_message[1];

  smart::throw_if<message_decoder_error>(boost::bind(less<size_t>(), 
    raw_message.size(), head_len), 
    "Invalid message format [Incomplete header received]");

  // flags
  const string::value_type flags = raw_message[2];

  const size_t tmp = detail::is_id_exists(flags) * sizeof(messageid_type) + 
    detail::is_rid_exists(flags) * sizeof(messageid_type);

  const size_t name_len = head_len - detail::bin_header_min_len() - tmp;

  return raw_message.substr(detail::bin_header_min_len(), name_len);
}

