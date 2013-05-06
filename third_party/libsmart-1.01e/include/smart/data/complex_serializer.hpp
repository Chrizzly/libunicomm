///////////////////////////////////////////////////////////////////////////////
// umessage_serializer.hpp
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

#ifndef SMART_COMPLEX_SERIALIZER_HPP_
#define SMART_COMPLEX_SERIALIZER_HPP_

/** @file umessage.hpp Unicomm message declarations. */

#include <smart/data/complex_auto_link.hpp>
#include <smart/data/complex.hpp>

#include <string>
#include <iostream>

#include <cstddef>

/** @namespace unicomm Unicomm++ Library root namespace. */
namespace smart
{

/** @namespace data Different entities which help to organize and store different data. */
namespace data
{

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4505)  // warning C4505: 'xxx' : unreferenced local function has been removed
#endif // SMART_VISUAL_CPP

/** Message xml tag name. */
const std::string& message_tag(void);
/** String xml tag name. */
const std::string& string_tag(void);
/** Int xml tag name. */
const std::string& int_tag(void);
/** Uint xml tag name. */
const std::string& uint_tag(void);
/** Double xml tag name. */
const std::string& double_tag(void);
/** Complex xml tag name. */
const std::string& complex_tag(void);
/** Property xml tag name. */
const std::string& property_tag(void);
/** Array xml tag name. */
const std::string& array_tag(void);
/** Array item xml tag name. */
const std::string& array_item_tag(void);
/** Name xml attribute name. */
const std::string& name_attr(void);
/** Type xml attribute name. */
const std::string& type_attr(void);
/** Array flag xml attribute name. */
const std::string& array_attr(void);

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

/** Basic type for complex serializer. */
class complex_serializer_base
{
public:
  virtual ~complex_serializer_base(void) { /* empty */ }

public:
  virtual std::string serialize(const complex&) = 0;
  virtual void unserialize(complex&, const std::string&) = 0;
};

/** Complex default serializer. */
class complex_default_serializer : public complex_serializer_base
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  explicit complex_default_serializer(bool pretty_format = false, bool canonical_xml = false):
    _pretty_format(pretty_format),
    _canonical_xml(canonical_xml)
  {
    // empty
  }

public:
  std::string serialize(const complex& c);
  void unserialize(complex& c, const std::string& complex_str);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  bool _pretty_format;
  bool _canonical_xml;
};

/** Complex old serializer. */
class complex_old_serializer : public complex_serializer_base
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  explicit complex_old_serializer(bool pretty_format = false, bool canonical_xml = false):
    _pretty_format(pretty_format),
    _canonical_xml(canonical_xml)
  {
    // empty
  }

public:
  std::string serialize(const complex& c);
  void unserialize(complex& c, const std::string& complex_str);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  bool _pretty_format;
  bool _canonical_xml;
};

/** Serializes complex to a string using given serializer. */
std::string serialize(const complex& c, complex_serializer_base& serializer);

/** Unserializes complex from a string using given serializer. */
smart::data::complex& unserialize(smart::data::complex& c, const std::string& complex_str, complex_serializer_base& serializer);

/** Serializes complex to a string using default serializer. */
std::string default_serialize(const smart::data::complex& c);

/** Unserializes complex from a string using given serializer. */
smart::data::complex& default_unserialize(smart::data::complex& c, const std::string& complex_str);

/** Serializes complex to a std::ostream.
 *
 *  Just does os << default_serializer(c).
 */
inline std::ostream& serialize(std::ostream& os, const complex& c) { os << default_serialize(c); return os; }

/** Loads message from a file. */
smart::data::complex& from_file(complex_serializer_base& serializer, smart::data::complex& c, const std::string& fn);
/** Saves message to a files. */
const smart::data::complex& to_file(complex_serializer_base& serializer, const smart::data::complex& c, const std::string& fn, bool create_dirs = true);
/** Loads message from a string. */
smart::data::complex& from_string(complex_serializer_base& serializer, smart::data::complex& c, const std::string &s);
/** Stores message to a string. */
std::string to_string(complex_serializer_base& serializer, const smart::data::complex& c);

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_SERIALIZER_HPP_
