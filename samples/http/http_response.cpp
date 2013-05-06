///////////////////////////////////////////////////////////////////////////////
// http_response.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http response message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_response.hpp"
#include "http_except.hpp"

#include <smart/utils.hpp>

#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include <functional>

using std::string;
using std::equal_to;

//////////////////////////////////////////////////////////////////////////
// http response 
/*virtual*/ string uni_http::response::serialize_first_str(void) const
{
  return version() + " " + status_code() + header::separator();
}

//------------------------------------------------------------------------
/*virtual*/ void uni_http::response::unserialize_first_str(const string &s)
{
  const string::size_type pos = s.find_first_of(' ');

  smart::throw_if<bad_request_error>(boost::bind(equal_to<string::size_type>(), 
    pos, string::npos), "Invalid HTTP response header first string format");

  BOOST_ASSERT(s.size() >= pos + 1 && 
    " - String size can't be lesser then found position");

  // version
  string ver(s.begin(), s.begin() + pos);

  smart::throw_if<bad_request_error>(boost::bind(equal_to<bool>(), ver.empty(), 
    true), "Invalid HTTP response header first string format [version missing]");

  // response code
  _status_code.assign(s.begin() + pos + 1, s.end());

  smart::throw_if<bad_request_error>(boost::bind(equal_to<bool>(), 
    _status_code.empty(), true), "Invalid HTTP response header first string "
      "format [response code missing]");
}

