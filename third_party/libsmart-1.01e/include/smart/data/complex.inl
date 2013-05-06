///////////////////////////////////////////////////////////////////////////////
// complex.inl
//
// smart++ - Multipurpose C++ Library.
//
// complex.hpp inline declarations.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#ifndef SMART_COMPLEX_HPP_
  #error "This file could not be used outside complex.hpp"
#endif // SMART_COMPLEX_HPP_

//////////////////////////////////////////////////////////////////////////
// simple types
template <>
inline types::traits<types::int_type>::property_collection_type& complex::get_collection<types::int_type>(void) const
{
  return _ints;
}

template <>
inline types::traits<types::uint_type>::property_collection_type& complex::get_collection<types::uint_type>(void) const
{
  return _uints;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::double_type>::property_collection_type& complex::get_collection<types::double_type>(void) const
{
  return _doubles;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<const char*>::property_collection_type& complex::get_collection<const char*>(void) const
{
  return _strings;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::string_type>::property_collection_type& complex::get_collection<types::string_type>(void) const
{
  return _strings;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::complex_type>::property_collection_type& complex::get_collection<types::complex_type>(void) const
{
  return _complexes;
}

//////////////////////////////////////////////////////////////////////////
// arrays
template <>
inline types::traits<types::int_array_type>::property_collection_type& complex::get_collection<types::int_array_type>(void) const
{
  return _ints_arrays;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::uint_array_type>::property_collection_type& complex::get_collection<types::uint_array_type>(void) const
{
  return _uints_arrays;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::double_array_type>::property_collection_type& complex::get_collection<types::double_array_type>(void) const
{
  return _doubles_arrays;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::string_array_type>::property_collection_type& complex::get_collection<types::string_array_type>(void) const
{
  return _strings_arrays;
}

//---------------------------------------------------------------------------
template <>
inline types::traits<types::complex_array_type>::property_collection_type& complex::get_collection<types::complex_array_type>(void) const
{
  return _complexes_arrays;
}
