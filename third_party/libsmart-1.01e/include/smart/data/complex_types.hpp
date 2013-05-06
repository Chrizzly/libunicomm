///////////////////////////////////////////////////////////////////////////////
// complex_types.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_TYPES_HPP_
#define SMART_COMPLEX_TYPES_HPP_

/** @file complex.hpp Types and constants declarations to be used to store data. */

#include <string>
#include <vector>
#include <map>

#include <cstddef>

/** @namespace smart Library root namespace. */
namespace smart
{

namespace data
{

/** Array type.
 *
 *  Used to represent unicomm type arrays.
 *
 *  @tparam T Element type to be stored by the array.
 */
template <typename T>
class array
{
//////////////////////////////////////////////////////////////////////////
// array interface
public:
  /** Intrinsic container type. */
  typedef std::vector<T> container_type;
  /** Item value type. */
  typedef typename container_type::value_type value_type;
  /** Item reference type. */
  typedef typename container_type::reference reference;
  /** Item const reference type. */
  typedef typename container_type::const_reference const_reference;
  /** Item pointer type. */
  typedef typename container_type::pointer pointer;
  /** Item const pointer type. */
  typedef typename container_type::const_pointer const_pointer;
  /** Size type. */
  typedef typename container_type::size_type size_type;

public:
  /** Constructs an array object from sequence designated by first and last iterators.
   *
   *  @tparam IterT type.
   *  @param first indicates begining of the gived sequence.
   *  @param last designates past-the-end element of the sequence.
   *  @throw doesn't throw if contained element type constructor doesn't throw.
   */
  template <typename IterT>
  array(IterT first, IterT last):
    _array(first, last)
  {
    // empty
  }

  /** Constructs an empty array object. */
  array(void) { /*empty*/ }

public:
  /** Returns a reference to an item with specified index.
   *
   *  @param i Element index which reference should be obtained for.
   *  @exception Throw std::out_of_range the same as vector::at.
   */
  reference at(size_type i) { return _array.at(i); }
  /** Returns a const reference to an item with specified index.
   *
   *  @param i Element index which reference should be obtained for.
   *  @exception Throw std::out_of_range the same as vector::at.
   */
  const_reference at(size_type i) const { return _array.at(i); }

  /** Add element to the array tail.
   *
   *  @param elem Element to be stored by the array.
   *  @throw May throw std::bad_alloc.
   */
  void add(const value_type& elem) { _array.push_back(elem); }

  /** Removes element at specified position.
   *
   *  @param i Element index to be deleted.
   *  @exception Throws the same as vector::at - std::out_of_range.
   */
  void remove_at(size_type i) { _array.at(i); _array.erase(_array.begin() + i); }

  /** Clears array. */
  void clear(void) { _array.clear(); }

  /** Returns elements count contained by array. */
  size_type size(void) const { return _array.size(); }

  /** Returns const reference to the internal container object. */
  const container_type& inner_array(void) const { return _array; }

//////////////////////////////////////////////////////////////////////////
// Private declarations
private:
  container_type _array;
};

// forward
template <typename T> struct property;
template <typename IterT> class property_iterator;
template <typename IterT> class property_const_iterator;
class complex;

/** @namespace unicomm::types Unicomm types declaration.
 *  Objects of those types serialized and deserialized into xml format by
 *  unicomm++ serializer.
 */
namespace types
{

//////////////////////////////////////////////////////////////////////////
// basic types

/** Signed platform dependent integer type. */
typedef int int_type;
/** Unsigned platform dependent integer type. */
typedef unsigned int uint_type;
/** Floating point with double precision type. */
typedef double double_type;
/** String type. */
typedef std::string string_type;
/** complex type object.
 *  Those objects can contain other type objects, arrays and nested complex objects.
 */
typedef complex complex_type;

//////////////////////////////////////////////////////////////////////////
// array types

template <typename T>
struct rebind
{
  typedef array<T> array_type;
  //typedef std::vector<U> array_type;
};

/** Signed integers array type. */
typedef rebind<int_type>::array_type int_array_type;
/** Unsigned integers array type. */
typedef rebind<uint_type>::array_type uint_array_type;
/** Doubles array type. */
typedef rebind<double_type>::array_type double_array_type;
/** Strings array type. */
typedef rebind<string_type>::array_type string_array_type;
/** complex objects array type. */
typedef rebind<complex_type>::array_type complex_array_type;

//////////////////////////////////////////////////////////////////////////
// property types

/** Signed integer property type. */
typedef property<types::int_type> int_property_type;
/** Unsigned integer property type. */
typedef property<types::uint_type> uint_property_type;
/** Float with double precision property type. */
typedef property<types::double_type> double_property_type;
/** String property type. */
typedef property<types::string_type> string_property_type;
/** Complex property type. */
typedef property<types::complex_type> complex_property_type;

//////////////////////////////////////////////////////////////////////////
// array property types

/** Signed integers array property type. */
typedef property<types::int_array_type> int_array_property_type;
/** Unsigned integers array property type. */
typedef property<types::uint_array_type> uint_array_property_type;
/** Double precision floats array property type. */
typedef property<types::double_array_type> double_array_property_type;
/** Strings array property type. */
typedef property<types::string_array_type> string_array_property_type;
/** Complexes array property type. */
typedef property<types::complex_array_type> complex_array_property_type;

//////////////////////////////////////////////////////////////////////////
// property collection types

/** Signed integer type properties collection. */
typedef std::map<std::string, int_property_type> int_collection_type;
/** Unsigned integer type properties collection. */
typedef std::map<std::string, uint_property_type> uint_collection_type;
/** Double type properties collection. */
typedef std::map<std::string, double_property_type> double_collection_type;
/** String type properties collection. */
typedef std::map<std::string, string_property_type> string_collection_type;
/** Complex type properties collection. */
typedef std::map<std::string, complex_property_type> complex_collection_type;

//////////////////////////////////////////////////////////////////////////
// arrays property collection types

/** Signed integer array type properties collection. */
typedef std::map<std::string, int_array_property_type> int_array_collection_type;
/** Unsigned integer array type properties collection. */
typedef std::map<std::string, uint_array_property_type> uint_array_collection_type;
/** Double array type properties collection. */
typedef std::map<std::string, double_array_property_type> double_array_collection_type;
/** String array type properties collection. */
typedef std::map<std::string, string_array_property_type> string_array_collection_type;
/** Complex array type properties collection. */
typedef std::map<std::string, complex_array_property_type> complex_array_collection_type;

//////////////////////////////////////////////////////////////////////////
// type names

//extern const std::wstring intTypeName;
//extern const std::wstring uintTypeName;
//extern const std::wstring doubleTypeName;
//extern const std::wstring stringTypeName;
//extern const std::wstring complexTypeName;

/** Signed integer type symbolic name. */
const std::string& int_type_name(void);
/** Unsigned integer type symbolic name. */
const std::string& uint_type_name(void);
/** Double type symbolic name. */
const std::string& double_type_name(void);
/** String type symbolic name. */
const std::string& string_type_name(void);
/** Complex type symbolic name. */
const std::string& complex_type_name(void);

//////////////////////////////////////////////////////////////////////////
// type traits

/** Unicomm type traits class template. */
template <typename T> struct traits { /* empty */ };

/** Integer type traits. */
template <> struct traits<int_type>
{
  /** Property type. */
  typedef int_property_type property_type;
  /** Property collection type. */
  typedef int_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return int_type_name(); }
};

/** Unsigned integer type traits. */
template <> struct traits<uint_type>
{
  /** Property type. */
  typedef uint_property_type property_type;
  /** Property collection type. */
  typedef uint_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return uint_type_name(); }
};

/** Double type traits. */
template <> struct traits<double_type>
{
  /** Property type. */
  typedef double_property_type property_type;
  /** Property collection type. */
  typedef double_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return double_type_name(); }
};

/** const char* type traits.
 *  To allow it to be treated as string.
 */
template <> struct traits<const char*>
{
  /** Property type. */
  typedef string_property_type property_type;
  /** Property collection type. */
  typedef string_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return string_type_name(); }
};

/** String type traits. */
template <> struct traits<string_type>
{
  /** Property type. */
  typedef string_property_type property_type;
  /** Property collection type. */
  typedef string_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return string_type_name(); }
};

/** complex type traits. */
template <> struct traits<complex_type>
{
  /** Property type. */
  typedef complex_property_type property_type;
  /** Property collection type. */
  typedef complex_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return complex_type_name(); }
};

/** Signed integer array type traits. */
template <> struct traits<int_array_type>
{
  /** Property type. */
  typedef int_array_property_type property_type;
  /** Property collection type. */
  typedef int_array_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return int_type_name(); }
};

/** Unsigned integer array type traits. */
template <> struct traits<uint_array_type>
{
  /** Property type. */
  typedef uint_array_property_type property_type;
  /** Property collection type. */
  typedef uint_array_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return uint_type_name(); }
};

/** Double array type traits. */
template <> struct traits<double_array_type>
{
  /** Property type. */
  typedef double_array_property_type property_type;
  /** Property collection type. */
  typedef double_array_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return double_type_name(); }
};

/** String array type traits. */
template <> struct traits<string_array_type>
{
  /** Property type. */
  typedef string_array_property_type property_type;
  /** Property collection type. */
  typedef string_array_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return string_type_name(); }
};

/** Complex array type traits. */
template <> struct traits<complex_array_type>
{
  /** Property type. */
  typedef complex_array_property_type property_type;
  /** Property collection type. */
  typedef complex_array_collection_type property_collection_type;
  /** Property iterator type. */
  typedef property_iterator<property_collection_type::iterator> property_iterator_type;
  /** Property constant iterator type. */
  typedef property_const_iterator<property_collection_type::const_iterator> property_const_iterator_type;
  /** Type name - rtti. */
  static const std::string& name(void) { return complex_type_name(); }
};

} // namespace types

/** Property class template.
 *
 *  Provides named property concept.
 *
 *  @tparam T Property value type.
 */
template <typename T>
struct property
{
//////////////////////////////////////////////////////////////////////////
// property interface
public:
  /** Stored value type. */
  typedef T value_type;

public:
  /** By default construct property object with empty string name and default stored value.
   *
   *  @param name Property name.
   *  @param value Property value.
   */
  explicit property(const std::string &name = "", const value_type& value = value_type()): _name(name), _value(value) { /*empty*/ }

public:
  /** Returns property name. */
  const std::string& name(void) const { return _name; }

  /** Returns property value. */
  const value_type& value(void) const { return _value; }

  /** Sets property value. */
  void value(const value_type& v) { _value = v; }

  /** Returns stored value type symbolic name - rtti. */
  static const std::string& type_name(void) { return types::traits<value_type>::name(); }

//////////////////////////////////////////////////////////////////////////
// Private declarations
private:
  std::string _name;
  value_type _value;
};

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_TYPES_HPP_
