///////////////////////////////////////////////////////////////////////////////
// std_priority_queue_adapter.hpp
//
// Copyright (c) 2009 Igor Zimenkov, Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file dispatcher_queue.hpp Dispatcher queue interface adapter. */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_STD_PRIORITY_QUEUE_HPP_
#define SMART_STD_PRIORITY_QUEUE_HPP_

#include <queue>
#include <functional>
#include <vector>

/** @namespace smart Library root namespace. */
namespace smart
{

/// Partial specialization for 
template <typename T, typename CompareT = std::less<T> >
class std_priority_queue_adapter
{
public:
  /// Container type used to store elements.
  typedef std::priority_queue<T, std::vector<T>, CompareT> container_type;
  /// Stored value type.
  typedef typename container_type::value_type value_type;
  /// Size type.
  typedef typename container_type::size_type size_type;
  /// Reference type to a value.
  typedef typename container_type::reference reference;
  /// Const reference type to a value.
  typedef typename container_type::const_reference const_reference;

public:
  /// Removes element from queue front.
  void pop(void) { _queue.pop(); }
  /// Pushes element into queue end.
  /// @param elem Element to be pushed to queue.
  void push(const value_type& elem) { _queue.push(elem); }
  /// Whether queue empty.
  bool empty(void) const { return _queue.empty(); }
  /// Returns elements count stored by the queue.
  size_type size(void) const { return _queue.size(); }
  ///// Returns a reference to a first object in the queue.
  //reference front(void) { return _queue.top(); }
  /// Returns a const reference to a first object in the queue.
  const_reference front(void) const { return _queue.top(); }

private:
  container_type _queue;
};

} // namespace smart

#endif // SMART_STD_PRIORITY_QUEUE_HPP_

