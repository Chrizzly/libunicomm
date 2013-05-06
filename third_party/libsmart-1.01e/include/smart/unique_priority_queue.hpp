///////////////////////////////////////////////////////////////////////////////
// unique_priority_queue.hpp
//
// Copyright (c) 2011 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/// @file unique_priority_queue.hpp Priority queue provides the uniquness of 
///   stored elements.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_UNIQUE_PRIORITY_QUEUE_HPP_
#define SMART_UNIQUE_PRIORITY_QUEUE_HPP_

#include <smart/stable_priority_queue.hpp>

#include <deque>
#include <functional>
#include <algorithm>

/// @namespace smart Library root namespace.
namespace smart
{

/// Unique priority queue.
///
/// @tparam T Queue elements type.
/// @tparam ContainerT Container type the queue is implemented by.
/// @tparam ComparatorT Represents the operation elements are compared by.
template<typename T, 
typename ContainerT = std::deque<T>, 
typename ComparatorT = std::less<typename ContainerT::value_type> >
class unique_priority_queue
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /// Container type used to store elements.
  typedef ContainerT container_type;

  /// Stored value type.
  typedef typename container_type::value_type value_type;

  /// Size type.
  typedef typename container_type::size_type size_type;

  /// Reference type to a value.
  typedef typename container_type::reference reference;

  /// Const reference type to a value.
  typedef typename container_type::const_reference const_reference;

public:
  /// Constructs a queue object.
  unique_priority_queue(void)
  {
    // empty
  }

  /// Constructs a queue object.
  ///
  /// @param comp Comparator object to be used. A copy is made.
  explicit unique_priority_queue(const ComparatorT& comp): 
    _queue(comp)
  {
    // empty
  }

  /// Constructs a queue object.
  ///
  /// @param comp Comparator object to be used. A copy is made.
  /// @param cont Container 
	unique_priority_queue(const ComparatorT& comp, const container_type& cont): 
    _queue(comp, cont)
	{	
    // empty
	}

  /// Constructs a queue object.
  ///
  /// Creates the queue based on range [first, last). The elements are 
  /// sorted accordingly to comparator specified.
  /// 
  /// @param first Designates first element in the range to copy from.
  /// @param last Designates past the last element to be copied.
	template<typename IteratorT>
  unique_priority_queue(IteratorT first, IteratorT last): 
    _queue(first, last)
	{	
    // empty
	}

  /// Constructs a queue object.
  ///
  /// Creates the queue based on range [first, last). The elements are 
  /// sorted accordingly to comparator specified.
  /// 
  /// @param first Designates first element in the range to copy from.
  /// @param last Designates past the last element to be copied.
  /// @param comp Comparator object to be used. A copy is made.
	template<typename IteratorT>
  unique_priority_queue(IteratorT first, IteratorT last, const ComparatorT& comp): 
    _queue(first, last, comp)
	{	
    // empty
	}

public:
  /// Removes element from queue front.
  void pop(void) { queue().pop(); }

  /// Pushes element into queue end.
  ///
  /// @param elem Element to be pushed to queue.
  bool push(const value_type& element)
  {
    return push_if_unique(queue(), element, is_equal<value_type>());
  }

  /// Whether queue empty.
  /// 
  /// @return Returns true if the queue is empty and false otherwise.
  bool empty(void) const { return queue().empty(); }

  /// Returns elements count stored by the queue.
  /// 
  /// @return Returns elements count contained by the queue.
  size_type size(void) const { return queue().size(); }

  /// Returns a reference to a first object in the queue.
  /// 
  /// @return Returns a reference to the queue front element.
  reference front(void) { return queue().front(); }

  /// Returns a const reference to a first object in the queue.
  /// 
  /// @return Returns a const reference to the queue front element.
  const_reference front(void) const { return queue().front(); }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  typedef stable_priority_queue<T, ContainerT, ComparatorT> impl_queue_type;

private:
  impl_queue_type& queue(void) { return _queue; }
  const impl_queue_type& queue(void) const { return _queue; }

private:
  impl_queue_type _queue;
}; 

} // namespace smart

#endif // SMART_UNIQUE_PRIORITY_QUEUE_HPP_

 