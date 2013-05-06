///////////////////////////////////////////////////////////////////////////////
// xercesc_string.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
  #pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_XERCESC_STRING_HPP_
#define SMART_COMPLEX_XERCESC_STRING_HPP_

#include <xercesc/util/XercesDefs.hpp>

#include <string>

/** @namespace unicomm Unicomm++ Library root namespace. */
namespace smart
{

namespace data
{

namespace detail
{


/** String type that can be used with xercesc interfaces. */
typedef std::basic_string<XMLCh> xml_string_type;

/** Converts xercesc xml string to std::string. */
inline std::string xml_to_std_string(const xml_string_type& s)
{
  return std::string(s.begin(), s.end());
}

/** Converts std::string to xercesc xml string. */
inline xml_string_type std_to_xml_string(const std::string& s)
{
  return xml_string_type(s.begin(), s.end());
}

/** Converts std::wstring to xercesc xml string. */
inline xml_string_type std_to_xml_string(const std::wstring& s)
{
  return xml_string_type(s.begin(), s.end());
}

/** Compares xml string with given c string. */
inline bool operator==(const xml_string_type& l, const char* r)
{
  return l == std_to_xml_string(std::string(r));
}

/** Compares xml string with given c string. */
inline bool operator!=(const xml_string_type& l, const char* r)
{
  return !(l == r);
}

/** Compares xml string with given c multibyte string. */
inline bool operator==(const xml_string_type& l, const wchar_t* r)
{
  return l == std_to_xml_string(std::wstring(r));
}

/** Compares xml string with given c multibyte string. */
inline bool operator!=(const xml_string_type& l, const wchar_t* r)
{
  return !(l == r);
}

} // namespace detail

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_XERCESC_STRING_HPP_
