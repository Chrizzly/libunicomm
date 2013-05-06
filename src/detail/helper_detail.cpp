///////////////////////////////////////////////////////////////////////////////
// helper_detail.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol different helper entities.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#include <unicomm/detail/helper_detail.hpp>

#include <smart/utils.hpp>

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <stdexcept>
#include <functional>
#include <locale>
#include <sstream>
#include <iomanip>
#include <limits>

#ifdef max
# undef max
#endif 

#ifdef min
# undef min
#endif 

using std::runtime_error;
using std::out_of_range;
using std::string;
using std::isxdigit;
using std::equal_to;
using std::greater;
using std::stringstream;
using std::numeric_limits;

using boost::filesystem::exists;
using boost::filesystem::path;

//-----------------------------------------------------------------------------
void unicomm::detail::check_path(const string &s)
{
  if (!exists(path(s)))
  {
    throw runtime_error("File or directory doesn't exist [" + s + "]");
  }
}

//-----------------------------------------------------------------------------
string& unicomm::detail::normalize_path(string &s)
{
  if (!s.empty())
  {
    const string::value_type c = *(s.end() - 1);
    if (c != '\\' && c != '/')
    {
      s += '/';
    }
  }

  return s;
}

//-----------------------------------------------------------------------------
string unicomm::detail::normalize_path(const string &s)
{
  string ss = s;

  return normalize_path(ss);
}

//-----------------------------------------------------------------------------
// declaration for compiler
namespace unicomm
{

namespace detail 
{

string& process_hex_str(string& s);

} // namespace unicomm

} // namespace detail

// fixme: use boost regex to implement this routine
string& unicomm::detail::process_hex_str(string& s)
{
  static const char* token = "\\x";

  for (size_t pos = s.find(token); pos != string::npos; 
    pos = s.find(token))
  {
    size_t end = pos += 2;
    while (end < s.size() && isxdigit(s[end]))
    {
      ++end;
    }

    smart::throw_if<runtime_error>(boost::bind(equal_to<size_t>(), end, pos), 
      "Hexadecimal number should have at least one digit");

    BOOST_ASSERT(end > pos && " - End index should be greater start");

    stringstream ss(s.substr(pos, end - pos));
    size_t char_code = 0;
    ss >> std::hex >> char_code;

    smart::throw_if<out_of_range>(boost::bind(greater<size_t>(), char_code, 
      numeric_limits<unsigned char>::max()), "Number too big for char code");

    pos -= 2;
    s.replace(pos, end - pos, 1, static_cast<char>(char_code));
  }

  return s;
}

//-----------------------------------------------------------------------------
string unicomm::detail::process_hex_str(const std::string& s)
{
  string ss = s;

  return process_hex_str(ss);
}
