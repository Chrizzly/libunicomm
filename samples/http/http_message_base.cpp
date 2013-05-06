///////////////////////////////////////////////////////////////////////////////
// http_message_base.cpp
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
 
#include "http_message_base.hpp"
#include "http_except.hpp"

#include <smart/utils.hpp>

#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include <sstream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <utility>

using std::string;
using std::equal_to;
using std::stringstream;
using std::runtime_error;
using std::vector;
using std::make_pair;

//////////////////////////////////////////////////////////////////////////
// aux
namespace 
{

//------------------------------------------------------------------------
inline string& trimr(string& s) 
{
  return s.erase(s.find_last_not_of(' ') + 1);
}

//------------------------------------------------------------------------
inline string& triml(string& s) 
{
  return s.erase(0, s.find_first_not_of(' '));
}

//------------------------------------------------------------------------
inline string& trim(string& s) 
{
  return trimr(triml(s));
}

//------------------------------------------------------------------------
inline string trim(const string& s) 
{
  string tmp = s;
  return trimr(triml(tmp));
}

} // unnamed namespace

/** Parses http header and returns multimap<name, value> collection. */
uni_http::header::header_collection_type uni_http::parse_header(const string& header)
{
  uni_http::header::header_collection_type parsed_header;

  string::size_type i = header.find(uni_http::header::separator());

  smart::throw_if<bad_request_error>(boost::bind(
    equal_to<string::size_type>(), i, string::npos), "Invalid HTTP header format");

  const string first_string    = trim(header.substr(0, i));
  const string header_reminder = header.substr(i + uni_http::header::separator().size());

  // store first string with empty string key
  parsed_header.insert(make_pair("", first_string));
  stringstream ss(header_reminder);

  do 
  {
    buffer_type buf(0x200);
    ss.getline(&*buf.begin(), static_cast<std::streamsize>(buf.size()), ss.widen('\r'));
    ss.get();

    const string s(&*buf.begin()); 
    i = s.find(uni_http::header::name_value_separator());

    smart::throw_if<bad_request_error>(boost::bind(
      equal_to<string::size_type>(), i, string::npos), "Invalid HTTP header format");

    const uni_http::header::header_collection_type::value_type header = 
      make_pair(trim(s.substr(0, i)), 
      trim(s.substr(i + uni_http::header::name_value_separator().size())));

    parsed_header.insert(header);
  } while (ss);

  return parsed_header;
}

//////////////////////////////////////////////////////////////////////////
// http message base 
uni_http::message_base& uni_http::message_base::add_header(const string& name, 
                                                           const string& value)
{
  _headers.insert(make_pair(name, value)); return *this;
}

//------------------------------------------------------------------------
uni_http::message_base& uni_http::message_base::remove_header(const string& name)
{
  _headers.erase(name); return *this;
}

//------------------------------------------------------------------------
const string& uni_http::message_base::get_header(const string& name) const
{
  header::header_collection_type::const_iterator cit = _headers.find(name);

  smart::throw_if<bad_request_error>(boost::bind(
    equal_to<header::header_collection_type::const_iterator>(), cit, _headers.end()), 
    "Header not found");

  return cit->second;
}

//------------------------------------------------------------------------
void uni_http::message_base::clear_headers(void)
{
  _headers.clear();
}

//------------------------------------------------------------------------
string uni_http::message_base::headers_str(void) const
{
  string s;

  for (header::header_collection_type::const_iterator cit = _headers.begin();
    cit != _headers.end(); ++cit)
  {
    s += cit->first + (cit->first.empty()? "": header::name_value_separator() + " ") + 
      cit->second + header::separator();
  }

  return s;
}

//------------------------------------------------------------------------
string uni_http::message_base::serialize(void) const
{ 
  return serialize_first_str() + headers_str() + header::separator() + _data;
}

//------------------------------------------------------------------------
void uni_http::message_base::unserialize(const string& message)
{
  _headers.clear();

  const size_t i = message.find(head_body_separator());

  smart::throw_if<bad_request_error>(boost::bind(
    equal_to<string::size_type>(), i, string::npos), "Invalid HTTP header format");

  _headers = parse_header(message.substr(0, i));

  header::header_collection_type::const_iterator cit = _headers.find("");
  BOOST_ASSERT(cit != _headers.end() && " - First string should exist");

  unserialize_first_str(cit->second);

  _data = message.substr(i + head_body_separator().size());
}

