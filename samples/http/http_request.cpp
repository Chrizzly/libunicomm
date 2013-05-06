///////////////////////////////////////////////////////////////////////////////
// http_request.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http request message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_request.hpp"
#include "http_except.hpp"

#include <sstream>
#include <stdexcept>
#include <iomanip>

using std::stringstream;
using std::string;
using std::skipws;

//////////////////////////////////////////////////////////////////////////
// http request 
/*virtual*/ string uni_http::request::serialize_first_str(void) const
{
  return name() + " " + uri() + " " + version() + header::separator();
}

//------------------------------------------------------------------------
/*virtual*/ void uni_http::request::unserialize_first_str(const string &s)
{
  stringstream ss(s);

  ss.exceptions(stringstream::failbit | stringstream::badbit);

  try
  {
    string ver;
    ss >> skipws >> _name >> _uri >> ver;
  } catch (const std::exception& e)
  {
    throw bad_request_error(string("Invalid HTTP request header first string format [") 
      + e.what() + "]");
  }
}

