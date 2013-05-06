///////////////////////////////////////////////////////////////////////////////
// complex_except.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_EXCEPT_HPP_
#define SMART_COMPLEX_EXCEPT_HPP_

/** @file except.hpp Unicomm exception classes declaration. */

#include <smart/data/complex_auto_link.hpp>

#include <string>
#include <stdexcept>

/** @namespace unicomm Unicomm++ Library root namespace. */
namespace smart
{

/** @namespace data Different entities which help to organize and store different data. */
namespace data
{

//////////////////////////////////////////////////////////////////////////
// Errors definition

/** Invalid property error. */
class invalid_property_error : public std::runtime_error
{
public:
  explicit invalid_property_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** Any serializer error. */
class serializer_error : public std::runtime_error
{
public:
  explicit serializer_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** Serializer not initialized error. */
class serializer_not_initialized_error : public serializer_error
{
public:
  explicit serializer_not_initialized_error(const std::string& what): serializer_error(what) { /* empty */ }
};

//////////////////////////////////////////////////////////////////////////
// Parser errors

/** Represents unicomm basic parser error. */
class parser_error : public std::runtime_error
{
public:
  explicit parser_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_EXCEPT_HPP_
