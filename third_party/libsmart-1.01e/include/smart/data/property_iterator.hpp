///////////////////////////////////////////////////////////////////////////////
// property_iterator.hpp
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

#ifndef SMART_PROPERTY_ITERATOR_HPP_
#define SMART_PROPERTY_ITERATOR_HPP_

/** @file complex.hpp Types and constants declarations to be used to store data. */

#include <smart/data/complex_auto_link.hpp>
#include <smart/data/complex_types.hpp>

#include <boost/assert.hpp>

#include <iterator>

/** @namespace smart Library root namespace. */
namespace smart
{

namespace data
{

//////////////////////////////////////////////////////////////////////////
// Property iterator

template <typename IterT>
class property_iterator;

template <typename IterT>
class property_const_iterator;

template <typename IterT>
inline bool operator==(const property_iterator<IterT>& l, const property_iterator<IterT>& r);

/** Property iterator class template.
 *  Allows to iterate through specified data type properties collection.
 */
template <typename IterT>
class property_iterator : public std::iterator<
  typename std::iterator_traits<IterT>::iterator_category,
  typename std::iterator_traits<IterT>::value_type::second_type,
  typename std::iterator_traits<IterT>::difference_type,
  typename std::iterator_traits<IterT>::value_type::second_type*,
  typename std::iterator_traits<IterT>::value_type::second_type&>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Property type. */
  typedef typename std::iterator_traits<IterT>::value_type::second_type property_type;
  typedef property_iterator<IterT> this_type;

public:
  /** Constructs an iterator object. */
  property_iterator(void) { /*empty*/ }

  /** Constructs an iterator object.
   *
   *  @param iter Designates to the sequence element.
   */
  explicit property_iterator(IterT iter): _cur(iter) { /*empty*/ }

  /** Constructs a copy of given iterator object. */
  property_iterator(const this_type& other): _cur(other._cur) { /*empty*/ }

public:
  //typedef const typename std::iterator_traits<IterT>::value_type::second_type& const_reference;
  typedef typename this_type::reference reference;
  typedef typename this_type::pointer pointer;

  /** Dereferences the iterator.
   *
   *  @return The reference to the property object.
   */
  reference operator*(void) const { return current(); }

  /** Dereferences the iterator.
   *
   *  @return Pointer to the object.
   */
  pointer operator->(void) const
  {
    return &**this;
  }

  /** Pre-increment.
   *
   *  @return *this.
   */
  this_type& operator++(void)
  {
    next();
    return *this;
  }

  /** Post increment.
   *
   *  @param iter Object to perform the operator on.
   *  @return a copy of iter before increment applied.
   */
  this_type operator++(int)
  {
    const this_type tmp = *this;
    ++*this;
    return tmp;
  }

  /** Pre-decrement.
   *
   *  @param iter Object to perform the operator on.
   *  @return iter.
   */
  this_type& operator--(void)
  {
    prev();
    return *this;
  }

  /** Post decrement.
   *
   *  @param iter Object to perform the operator on.
   *  @return a copy of iter before decrement applied.
   */
  this_type operator--(int)
  {
    const this_type tmp = *this;
    --*this;
    return tmp;
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  friend bool operator==<>(const property_iterator<IterT>& l,
    const property_iterator<IterT>& r);

// why doesn't it work??
//  typedef typename std::iterator_traits<IterT>::value_type::second_type::value_type intrinsic_value_type;
//  typedef typename types::traits<intrinsic_value_type>::property_collection_type::const_iterator intrinsic_const_iterator_type;

//  friend property_const_iterator<intrinsic_const_iterator_type>::property_const_iterator(const property_iterator<IterT>& it);

  template <typename T> friend class property_const_iterator;

  /** Compares this with the given instance.
   *
   *  @param other Another instance of the same type iterator.
   *  @return If equal returns true, otherwise returns false.
   */
  bool equal_to(const this_type& other) const
  {
    return _cur == other._cur;
  }

  /** Returns a reference to current property.
   *
   *  Doesn't perform any checks.
   */
  reference current(void) const
  {
    return _cur->second;
  }

  /** Navigate to next element in the collection if it is possible. */
  void next(void) { ++_cur; }

  /** Navigate to previous element in the collection if it is possible. */
  void prev(void) { --_cur; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  mutable IterT _cur;
};

/** Compares left with the given right instance.
*
*  @param l Instance of the property iterator.
*  @param r Another instance of the same type iterator.
*  @return If equal returns true, otherwise returns false.
*/
template <typename IterT>
inline bool operator==(const property_iterator<IterT>& l, const property_iterator<IterT>& r)
{
  return l.equal_to(r);
}

/** Compares left with the given right instance.
*
*  @param l Instance of the property iterator.
*  @param r Another instance of the same type iterator.
*  @return If not equal returns true, otherwise returns false.
*/
template <typename IterT>
inline bool operator!=(const property_iterator<IterT>& l, const property_iterator<IterT>& r)
{
  return !(l == r);
}


//////////////////////////////////////////////////////////////////////////
// Property const iterator

template <typename IterT>
inline bool operator==(const property_const_iterator<IterT>& l, const property_const_iterator<IterT>& r);

/** Property const iterator class template.
 *  Allows to iterate through specified data type properties collection.
 */
template <typename IterT>
class property_const_iterator : public std::iterator<
  typename std::iterator_traits<IterT>::iterator_category,
  const typename std::iterator_traits<IterT>::value_type::second_type,
  typename std::iterator_traits<IterT>::difference_type,
  const typename std::iterator_traits<IterT>::value_type::second_type*,
  const typename std::iterator_traits<IterT>::value_type::second_type&>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Property type. */
  typedef typename std::iterator_traits<IterT>::value_type::second_type property_type;
  typedef property_const_iterator<IterT> this_type;

public:
  /** Constructs an iterator object. */
  property_const_iterator(void) { /*empty*/ }

  /** Constructs an iterator object.
   *
   *  @param iter Designates to the sequence element.
   */
  explicit property_const_iterator(IterT iter): _cur(iter) { /*empty*/ }

  /** Constructs a copy of given iterator object. */
  property_const_iterator(const this_type& other): _cur(other._cur) { /*empty*/ }

  /** Converts iterator to const iterator. */
  template <typename T>
  property_const_iterator(const property_iterator<T>& it): _cur(it._cur) { /*empty*/ }

public:
  //typedef const typename std::iterator_traits<IterT>::value_type::second_type& const_reference;
  typedef typename this_type::reference reference;
  typedef typename this_type::pointer pointer;

  /** Dereferences the iterator.
   *
   *  @return The reference to the property object.
   */
  reference operator*(void) const { return current(); }

  /** Dereferences the iterator.
   *
   *  @return Pointer to the object.
   */
  pointer operator->(void) const
  {
    return &**this;
  }

  /** Pre-increment.
   *
   *  @return *this.
   */
  this_type& operator++(void)
  {
    next();
    return *this;
  }

  /** Post increment.
   *
   *  @param iter Object to perform the operator on.
   *  @return a copy of iter before increment applied.
   */
  this_type operator++(int)
  {
    const this_type tmp = *this;
    ++*this;
    return tmp;
  }

  /** Pre-decrement.
   *
   *  @param iter Object to perform the operator on.
   *  @return iter.
   */
  this_type& operator--(void)
  {
    prev();
    return *this;
  }

  /** Post decrement.
   *
   *  @param iter Object to perform the operator on.
   *  @return a copy of iter before decrement applied.
   */
  this_type operator--(int)
  {
    const this_type tmp = *this;
    --*this;
    return tmp;
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4396) // warning C4396: 'smart::data::operator ==' : the inline specifier cannot be used when a friend declaration refers to a specialization of a function template
#endif // SMART_VISUAL_CPP

  friend bool operator==<>(const property_const_iterator<IterT>& l,
    const property_const_iterator<IterT>& r);

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

  /** Compares this with the given instance.
   *
   *  @param other Another instance of the same type iterator.
   *  @return If equal returns true, otherwise returns false.
   */
  bool equal_to(const this_type& other) const
  {
    return _cur == other._cur;
  }

  /** Returns a reference to current property.
   *
   *  Doesn't perform any checks.
   */
  reference current(void) const
  {
    return _cur->second;
  }

  /** Navigate to next element in the collection if it is possible. */
  void next(void) { ++_cur; }

  /** Navigate to previous element in the collection if it is possible. */
  void prev(void) { --_cur; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  mutable IterT _cur;
};

/** Compares left with the given right instance.
 *
 *  @param l Instance of the property iterator.
 *  @param r Another instance of the same type iterator.
 *  @return If equal returns true, otherwise returns false.
 */
template <typename IterT>
inline bool operator==(const property_const_iterator<IterT>& l, const property_const_iterator<IterT>& r)
{
  return l.equal_to(r);
}

/** Compares left with the given right instance.
 *
 *  @param l Instance of the property iterator.
 *  @param r Another instance of the same type iterator.
 *  @return If not equal returns true, otherwise returns false.
 */
template <typename IterT>
inline bool operator!=(const property_const_iterator<IterT>& l, const property_const_iterator<IterT>& r)
{
  return !(l == r);
}

} // namespace data

} // namespace smart

#endif // SMART_PROPERTY_ITERATOR_HPP_
