///////////////////////////////////////////////////////////////////////////////
// raw_data.cpp
//
// Copyright (c) 2009 Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

#include <smart/raw_data.hpp>

#include <boost/array.hpp>

#include <sstream>

//------------------------------------------------------------------------------
using std::string;

namespace
{

//------------------------------------------------------------------------------
template <typename destT> destT& to_hex_string_impl(destT& dest, const string& src)
{
  destT res;
  res.reserve(src.size() * 2);

  typename destT::value_type c;
  for (string::size_type i = 0; i < src.size(); ++i)
  {
    c = (src[i] & 0xF0) >> 4;
    c = c + ((c < 10) ? '0' : 'a' - 10); // to disable warning C4244: '+=' : conversion from 'int' to 'char', possible loss of data

    res.push_back(c);

    c = src[i] & 0x0F;
    c = c + ((c < 10) ? '0' : 'a' - 10); // to disable warning C4244: '+=' : conversion from 'int' to 'char', possible loss of data
    res.push_back(c);
  }

  dest.swap(res);
  return dest;
}

//------------------------------------------------------------------------------
template <typename T> void assign_hex_string_impl(T& dest, const string& src)
{
  T res;

  if (!(src.size() % 2))
  {
    res.reserve(src.size() / 2);
  }
  else
  {
    throw std::invalid_argument("The hexadecimal string must have an even number of characters");
  }

  unsigned char c;
  for (string::size_type i = 0; i < src.size(); i = i + 2)
  {
    const char& c0 = src[i];
    if (c0 >= '0' && c0 <= '9')
    {
      c = (c0 - '0') << 4;
    }
    else if (c0 >= 'a' && c0 <= 'f')
    {
      c = (c0 - 'a' + 10) << 4;
    }
    else if (c0 >= 'A' && c0 <= 'F')
    {
      c = (c0 - 'A' + 10) << 4;
    }
    else
    {
      throw std::invalid_argument(string("Invalid hexadecimal character [") + c0 + "]");
    }

    const char& c1 = src[i + 1];
    if (c1 >= '0' && c1 <= '9')
    {
      c |= (c1 - '0');
    }
    else if (c1 >= 'a' && c1 <= 'f')
    {
      c |= (c1 - 'a' + 10);
    }
    else if (c1 >= 'A' && c1 <= 'F')
    {
      c |= (c1 - 'A' + 10);
    }
    else
    {
      throw std::invalid_argument(string("Invalid hexadecimal character [") + c1 + "]");
    }

    res.push_back(c);
  }

  dest.swap(res);
}

} // anonymous namespace

namespace smart
{

//------------------------------------------------------------------------------
raw_data_ptr from_stream(std::istream& is, std::streamsize sz)
{
  static const std::streamsize defbufsz = 1024;

  if (sz)
  {
    raw_data_ptr res(new raw_data(sz));
    is.read(&*res->begin(), sz);
    return res;
  }
  else
  {
    // так, как внизу закомментированно, очень медленно :(
    // смотрим: http://easy-coding.blogspot.com/2009/02/stl.html
    // data.insert(data.begin(), istreambuf_iterator<char>(is), istreambuf_iterator<char>());
    // после такого чтения good(1), eof(0), fail(0), bad(0)

    raw_data_ptr res(new raw_data());
    boost::array<char, defbufsz> buf;
    while (is)
    { 
      is.read(&*buf.begin(), static_cast<std::streamsize>(buf.size()));
      res->insert(res->end(), buf.begin(), buf.begin() + is.gcount());
    }
    // после такого чтения good(0), eof(1), fail(1), bad(0)
    return res;
  }
}

//------------------------------------------------------------------------------
string to_hex_string(const std::string& src)
{
  string dest;
  return to_hex_string_impl(dest, src);
}

//------------------------------------------------------------------------------
string from_hex_string(const string& src)
{
  string dest;
  assign_hex_string_impl(dest, src);
  return dest;
}

//------------------------------------------------------------------------------
void assign_hex_string(raw_data& dest, const string& src)
{
  assign_hex_string_impl(dest, src);
}

//------------------------------------------------------------------------------
void assign_hex_string(string& dest, const string& src)
{
  assign_hex_string_impl(dest, src);
}

//------------------------------------------------------------------------------
void write(const void* buf, size_t count, raw_data& data, raw_data::size_type pos)
{
  const char* char_buf = reinterpret_cast<const char*>(buf);
  const raw_data::size_type sz = data.size();
  if (pos == -1)
  {
    pos = sz;
  }
  else if (pos >= sz)
  {
    std::stringstream tmp;
    tmp << "Invalid write position [size = " << sz << ", pos = " << pos << "]";
    throw std::runtime_error(tmp.str());
  }
  
  size_t replace_count = 0;
  for (; replace_count < count && pos < sz; ++replace_count, ++pos)
  {
    data[pos] = char_buf[replace_count];
  }

  const size_t append_count = count - replace_count;
  if (append_count)
  {
    data.reserve(sz + append_count);
    data.insert(data.end(), char_buf + replace_count, char_buf + count);
  }
}

//------------------------------------------------------------------------------
void write(const void* buf, size_t count, raw_data_ptr& data, raw_data::size_type pos)
{
  write(buf, count, *data, pos);
}

} // namespace smart
