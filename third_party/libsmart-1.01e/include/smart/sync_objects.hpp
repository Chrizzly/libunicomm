///////////////////////////////////////////////////////////////////////////////
// sync_objects.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/// @file sync_objects.hpp Different syncronization objects declarations and definitions.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SYNC_OBJECTS_HPP_
#define SYNC_OBJECTS_HPP_

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) // 'initializing' : conversion from 'size_t' to 'const int', possible loss of data
#endif // SMART_VISUAL_CPP

#ifdef SMART_WIN
# ifdef min
#   undef min
# endif // min
# ifdef max
#   undef max
# endif // max
#endif // SMART_WIN

#ifdef SMART_VISUAL_CPP
# pragma warning (pop) 
#endif // SMART_VISUAL_CPP 

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4244) // warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#endif // SMART_VISUAL_CPP

#include <boost/thread/recursive_mutex.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (pop) 
#endif // SMART_VISUAL_CPP 

#include <string>
#include <queue>

/// @namespace smart 
/// Library root namespace.
namespace smart
{

/// Synchronized queue. Provides thread safe access to the queue.
template <typename T, typename QueueT = std::queue<T> >
class sync_queue
{
public:
  /// Stored value type.
  typedef typename QueueT::value_type value_type;
  /// Container type used to store elements.
  typedef typename QueueT::container_type container_type;
  /// Size type.
  typedef typename QueueT::size_type size_type;
  /// Reference type to a value.
  typedef typename QueueT::reference reference;
  /// Const reference type to a value.
  typedef typename QueueT::const_reference const_reference;

public:
  /// Removes element from queue front.
  void pop(void) { boost::recursive_mutex::scoped_lock lock(_guard); _queue.pop(); }
  /// Pushes element into queue end.
  /// @param elem Element to be pushed to queue.
  void push(const value_type& elem) { boost::recursive_mutex::scoped_lock lock(_guard); _queue.push(elem); }
  /// Whether queue empty.
  bool empty(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.empty(); }
  /// Returns elements count stored by the queue.
  size_type size(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.size(); }
  /// Returns a reference to a first object in the queue.
  reference front(void) { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.front(); }
  /// Returns a const reference to a first object in the queue.
  const_reference front(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.front(); }
  /// Returns a reference to a last object in the queue.
  reference back(void) { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.back(); }
  /// Returns a const reference to a last object in the queue.
  const_reference back(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.back(); }
  /// Returns a first object copy.
  value_type front_copy(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.front(); }
  /// Returns a last object copy.
  value_type back_copy(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.back(); }

private:
  QueueT _queue;
  mutable boost::recursive_mutex _guard;
};

/// Partial specialization for priority_queue
template <typename T>
class sync_queue<T, std::priority_queue<T> >
{
public:
  /// Stored value type.
  typedef typename std::priority_queue<T>::value_type value_type;
  /// Container type used to store elements.
  typedef typename std::priority_queue<T>::container_type container_type;
  /// Size type.
  typedef typename std::priority_queue<T>::size_type size_type;
  /// Reference type to a value.
  typedef typename std::priority_queue<T>::reference reference;
  /// Const reference type to a value.
  typedef typename std::priority_queue<T>::const_reference const_reference;

public:
  /// Removes element from queue front.
  void pop(void) { boost::recursive_mutex::scoped_lock lock(_guard); _queue.pop(); }
  /// Pushes element into queue end.
  /// @param elem Element to be pushed to queue.
  void push(const value_type& elem) { boost::recursive_mutex::scoped_lock lock(_guard); _queue.push(elem); }
  /// Whether queue empty.
  bool empty(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.empty(); }
  /// Returns elements count stored by the queue.
  size_type size(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.size(); }
  /// Returns a const reference to a first object in the queue.
  const_reference front(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.top(); }
  /// Returns a first object copy.
  value_type front_copy(void) const { boost::recursive_mutex::scoped_lock lock(_guard); return _queue.top(); }

private:
  std::priority_queue<T> _queue;
  mutable boost::recursive_mutex _guard;
};

} // namespace smart

#endif // SYNC_OBJECTS_HPP_
