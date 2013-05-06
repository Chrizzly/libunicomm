///////////////////////////////////////////////////////////////////////////////
// complex.hpp
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

#ifndef SMART_COMPLEX_HPP_
#define SMART_COMPLEX_HPP_

/** @file complex.hpp Types and constants declarations to be used to store data. */

#include <smart/data/complex_auto_link.hpp>
#include <smart/data/complex_except.hpp>
#include <smart/data/property_iterator.hpp>
#include <smart/data/complex_types.hpp>

#include <cstddef>

/** @namespace smart Library root namespace. */
namespace smart
{

namespace data
{

/** Complex type object name. */
const std::string& name_property(void);

/** Complex type class as C++ struct. */
class complex
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Iterator type. 
   *
   *  @note use complex::iterator<int_type>::type to declare iterator.
   */
  template <typename T> struct iterator 
  {  
    typedef property_iterator<
      typename types::traits<T>::property_collection_type::iterator> type;
  };

  /** Constant iterator type. 
   *
   *  @note use complex::const_iterator<int_type>::type to declare const iterator.
   */
  template <typename T> struct const_iterator
  {
    typedef property_const_iterator<
      typename types::traits<T>::property_collection_type::const_iterator> type;
  };

public:
  /** Destroys a complex object. */
  virtual ~complex(void) { /* empty */ }

public:
  /** Creates a property iterator of given type refers to the collection begin.
   *
   *  @tparam T Unicomm type to create an iterator to the collection holds
   *  properties of the specified type.
   */
  template <typename T>
  typename types::traits<T>::property_iterator_type begin(void)
  {
    typename types::traits<T>::property_collection_type& coll = get_collection<T>();

    return typename types::traits<T>::property_iterator_type(coll.begin());
  }

  /** Creates a property iterator of given type refers to the collection begin.
   *
   *  @tparam T Unicomm type to create an iterator to the collection holds
   *  properties of the specified type.
   */
  template <typename T>
  typename types::traits<T>::property_const_iterator_type begin(void) const
  {
    const typename types::traits<T>::property_collection_type& coll = get_collection<T>();

    return typename types::traits<T>::property_const_iterator_type(coll.begin());
  }

  /** Creates a property iterator of given type refers to the collection end.
   *
   *  @tparam T Unicomm type to create an iterator to the collection holds
   *  properties of the specified type.
   */
  template <typename T>
  typename types::traits<T>::property_iterator_type end(void)
  {
    typename types::traits<T>::property_collection_type& coll = get_collection<T>();

    return typename types::traits<T>::property_iterator_type(coll.end());
  }

  /** Creates a property iterator of given type refers to the collection end.
   *
   *  @tparam T Unicomm type to create an iterator to the collection holds
   *  properties of the specified type.
   */
  template <typename T>
  typename types::traits<T>::property_const_iterator_type end(void) const
  {
    const typename types::traits<T>::property_collection_type& coll = get_collection<T>();

    return typename types::traits<T>::property_const_iterator_type(coll.end());
  }

  /** Returns property value by specified name.
   *
   *  @tparam T Data type.
   *  @param name Property name.
   *  @exception Throws smart::data::invalid_property_error if requested 
   *    property doesn't exist.
   */
  template <typename T>
  const T& get_property(const std::string &name) const
  {
    return get_prop_value<T>(name, get_collection<T>());
  }

  /** Sets property with given name to specified value.
   *
   *  If create is true it will create a property with specified name, type and value
   *  if such property doesn't exist. If property exists, just assigns a value.
   *
   *  @tparam T Data type.
   *  @param name Property name whose value should be set.
   *  @param value Property value.
   *  @param create Flag indicates to create property if it doesn't exist.
   *  @throw If property with given name doesn't exist and create is false
   *         throws smart::data::invalid_property_error.
   */
  template <typename T>
  void set_property(const std::string &name, const T& value, bool create = true)
  {
    set_prop_value(name, value, get_collection<T>(), create);
  }

  /** Whether property with given name and type exists.
   *
   *  @tparam T Unicomm data type.
   *  @param name Tested property name.
   *  @return True if property exists and false instead.
   */
  template <typename T>
  bool prop_exists(const std::string &name) const
  {
    return propexists<T>(name, get_collection<T>());
  }

  /** Removes specified property.
   *
   *  @tparam T Unicomm data type.
   *  @param name Property's name to be deleted.
   */
  template <typename T>
  void erase_prop(const std::string &name)
  {
    return eraseprop(name, get_collection<T>());
  }

  /** Whether complex object is empty. */
  bool empty(void) const
  {
    return _ints.empty() && _strings.empty() && _uints.empty() && 
      _doubles.empty() && _complexes.empty() && _ints_arrays.empty() && 
      _uints_arrays.empty() && _doubles_arrays.empty() && 
      _strings_arrays.empty() && _complexes_arrays.empty();
  }

  /** Clears complex object.
   *
   *  Post-condition: complex::empty() returns true
   */
  void clear(void)
  {
    _ints.clear();
    _strings.clear();
    _uints.clear();
    _doubles.clear();
    _complexes.clear();
    _ints_arrays.clear();
    _uints_arrays.clear();
    _doubles_arrays.clear();
    _strings_arrays.clear();
    _complexes_arrays.clear();
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  //////////////////////////////////////////////////////////////////////////
  // generic template for collection getter.
  template <typename T>
  typename types::traits<T>::property_collection_type& get_collection(void) const;

  //---------------------------------------------------------------------------
  template <typename T>
  bool propexists(const std::string &name, 
    const typename types::traits<T>::property_collection_type &c) const
  {
    return c.find(name) != c.end();
  }

  //---------------------------------------------------------------------------
  template <typename T>
  void eraseprop(const std::string &name, 
    typename types::traits<T>::property_collection_type &c)
  {
    c.erase(name);
  }

  //---------------------------------------------------------------------------
  template <typename T>
  const T& get_prop_value(const std::string &name, 
    const typename types::traits<T>::property_collection_type &c) const
  {
    typename types::traits<T>::property_collection_type::const_iterator it = c.find(name);

    if (it == c.end())
    {
      throw smart::data::invalid_property_error(
        "Invalid property has been requested [type: " 
          + types::traits<T>::name() + ", name: " + name + "]");
    }

    return it->second.value();
  }

  //---------------------------------------------------------------------------
  template <typename T>
  void set_prop_value(const std::string &name, const T& value, 
    typename types::traits<T>::property_collection_type &c, bool create)
  {
    if (create)
    {
      c[name] = property<T>(name, value);
    } else
    {
      typename types::traits<T>::property_collection_type::iterator it = c.find(name);
      if (it == c.end())
      {
        throw smart::data::invalid_property_error(
          "Invalid property has been requested [type: " + 
            types::traits<T>::name() + ", name: " + name + "]");
      }

      it->second.value(value);
    }
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  mutable types::int_collection_type _ints;
  mutable types::uint_collection_type _uints;
  mutable types::double_collection_type _doubles;
  mutable types::string_collection_type _strings;
  // complex
  mutable types::complex_collection_type _complexes;
  // arrays
  mutable types::int_array_collection_type _ints_arrays;
  mutable types::uint_array_collection_type _uints_arrays;
  mutable types::double_array_collection_type _doubles_arrays;
  mutable types::string_array_collection_type _strings_arrays;
  mutable types::complex_array_collection_type _complexes_arrays;
};

//////////////////////////////////////////////////////////////////////////
// complex methods definitions
#include "complex.inl"

} // namespace data

} // namespace unicomm

#endif // SMART_COMPLEX_HPP_
