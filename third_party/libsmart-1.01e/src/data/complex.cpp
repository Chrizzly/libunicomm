///////////////////////////////////////////////////////////////////////////////
// complex.cpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#include <smart/data/complex.hpp>

using std::string;

namespace sdt = smart::data::types;

//////////////////////////////////////////////////////////////////////////
// Type names
//const std::wstring sdt::intTypeName     = L"int";
//const std::wstring sdt::uintTypeName    = L"uint";
//const std::wstring sdt::doubleTypeName  = L"double";
//const std::wstring sdt::stringTypeName  = L"string";
//const std::wstring sdt::complexTypeName = L"complex";

// define this macros to reduce service information size
#ifndef SMART_COMPLEX_SHORT_XML_TAGS

const string& sdt::int_type_name(void) { static const string s = "int"; return s; }
const string& sdt::uint_type_name(void) { static const string s = "uint"; return s; }
const string& sdt::double_type_name(void) { static const string s = "double"; return s; }
const string& sdt::string_type_name(void) { static const string s = "string"; return s; }
const string& sdt::complex_type_name(void) { static const string s = "complex"; return s; }

#else // SMART_COMPLEX_SHORT_XML_TAGS

const string& sdt::int_type_name(void) { static const string s = "i"; return s; }
const string& sdt::uint_type_name(void) { static const string s = "ui"; return s; }
const string& sdt::double_type_name(void) { static const string s = "d"; return s; }
const string& sdt::string_type_name(void) { static const string s = "s"; return s; }
const string& sdt::complex_type_name(void) { static const string s = "c"; return s; }

#endif // SMART_COMPLEX_SHORT_XML_TAGS

//////////////////////////////////////////////////////////////////////////
// unicomm engine reserved property names

// define this macros to reduce service information size
#ifndef SMART_COMPLEX_SHORT_XML_TAGS

const string& smart::data::name_property(void) { static const string s = "name"; return s; }

#else

const string& smart::data::name_property(void) { static const string s = "n"; return s; }

#endif // SMART_COMPLEX_SHORT_XML_TAGS

