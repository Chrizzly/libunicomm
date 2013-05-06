///////////////////////////////////////////////////////////////////////////////
// xml_message_decoder.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication Protocol complex xml message decoder. 
// Accepts incoming data and creates user defined messages.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#include <unicomm/facade/xml_message_decoder.hpp>

#ifdef UNICOMM_USE_COMPLEX_XML

#include <unicomm/except.hpp>
#include <detail/basic_detail.hpp>

#include <smart/data/complex_serializer.hpp>
#include <smart/utils.hpp>

#include <boost/regex.hpp>

using std::string;
using std::make_pair;

using boost::regex;
using boost::cmatch;
using boost::regex_match;

//////////////////////////////////////////////////////////////////////////
// xml message decoder
unicomm::message_decoder_base::iter_pair_type 
unicomm::xml_message_decoder::find_raw_message(comm_buffer::buffer_type& buffer, 
                                               session_base& /*session*/)
{
  message_decoder_base::iter_pair_type bounds = 
    make_pair(buffer.end(), buffer.end());

  const string::size_type pos = buffer.find(detail::xml_message_end());
  if (pos != string::npos)
  {
    // found something
    bounds.first  = buffer.begin();
    bounds.second = bounds.first + pos + detail::xml_message_end().size();
  }

  return bounds;
}

//-----------------------------------------------------------------------------
string& unicomm::xml_message_decoder::decode_raw_message(string& raw_message, 
                                                         session_base& /*session*/)
{
  raw_message.erase(raw_message.end() - detail::xml_message_end().size(), 
    raw_message.end());

  return raw_message;
}

//-----------------------------------------------------------------------------
string unicomm::xml_message_decoder::get_message_name(const string& raw_message, 
                                                      session_base& /*session*/)
{
  // fixme: to think better about regex, this doesn't allow attributes 
  // except name, but they should have been
  // don't realize why
  const string pattern = ".*<\\s*" + smart::data::message_tag() + 
    "[.]*(\\s+name\\s*=\\s*\"([^\"]*)\")?[.]*>.*";

  const regex ex(pattern, boost::regex::mod_s);

  cmatch matches;
  if (!regex_match(raw_message.c_str(), matches, ex))
  {
    throw message_decoder_error(
      "The xml message format is invalid, can't find message name");
  }

  return string(matches[2].first, matches[2].second);
}

#endif // UNICOMM_USE_COMPLEX_XML
