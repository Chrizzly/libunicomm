///////////////////////////////////////////////////////////////////////////////
// stable_priority_queue.hpp
//
// Copyright (c) 2011 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/// @file stable_priority_queue.hpp Priority queue which preservs the order 
///   of elements with similar priority.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_STABLE_PRIORITY_QUEUE_HPP_
#define SMART_STABLE_PRIORITY_QUEUE_HPP_

#include <deque>
#include <functional>
#include <algorithm>

/// @namespace smart Library root namespace.
namespace smart
{

/// Priority queue preserving the order of elements with equal priority.
///
/// @tparam T Queue elements type.
/// @tparam ContainerT Container type the queue is implemented by.
/// @tparam ComparatorT Represents the operation elements is compared by.
template<typename T, 
  typename ContainerT = std::deque<T>, 
  typename ComparatorT = std::less<typename ContainerT::value_type> >
class stable_priority_queue
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
  stable_priority_queue(void)
  {
    // empty
  }

  /// Constructs a queue object.
  ///
  /// @param comp Comparator object to be used. A copy is made.
  explicit stable_priority_queue(const ComparatorT& comp): 
    _comp(comp)
  {
    // empty
  }

  /// Constructs a queue object.
  ///
  /// @param comp Comparator object to be used. A copy is made.
  /// @param cont Container 
	stable_priority_queue(const ComparatorT& comp, const container_type& cont): 
    _cont(cont), 
    _comp(comp)
	{	
    std::stable_sort(container().begin(), container().end(), comparator());
	}

  /// Constructs a queue object.
  ///
  /// Creates the queue based on range [first, last). The elements are 
  /// sorted accordingly to comparator specified.
  /// 
  /// @param first Designates first element in the range to copy from.
  /// @param last Designates past the last element to be copied.
	template<typename IteratorT>
  stable_priority_queue(IteratorT first, IteratorT last): 
    _cont(first, last)
	{	
    std::stable_sort(container().begin(), container().end(), comparator());
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
  stable_priority_queue(IteratorT first, IteratorT last, const ComparatorT& comp): 
    _cont(first, last),
    _comp(comp)
	{	
    std::stable_sort(container().begin(), container().end(), comparator());
	}

public:
  /// Removes element from queue front.
  void pop(void) { container().pop_front(); }

  /// Pushes element into queue end.
  ///
  /// @param elem Element to be pushed to queue.
  void push(const value_type& element) 
  {
    typename container_type::iterator it = std::upper_bound(
      container().begin(), container().end(), element, comparator());

    container().insert(it, element);
  }

  /// Whether queue empty.
  /// 
  /// @return Returns true if the queue is empty and false otherwise.
  bool empty(void) const { return container().empty(); }

  /// Returns elements count stored by the queue.
  /// 
  /// @return Returns elements count contained by the queue.
  size_type size(void) const { return container().size(); }

  /// Returns a reference to a first object in the queue.
  /// 
  /// @return Returns a reference to the queue front element.
  reference front(void) { return container().front(); }

  /// Returns a const reference to a first object in the queue.
  /// 
  /// @return Returns a const reference to the queue front element.
  const_reference front(void) const { return container().front(); }

  /// Whether a such element contained by the queue.
  ///
  /// Uses comparator to find element. Has O(logN) complexity.
  /// 
  /// @param element Element to look for.
  /// @return Returns true if element is contained and false otherwise.
  bool contains(const value_type& element) const
  {
    return std::binary_search(container().begin(), container().end(), element, comparator());
  }

  /// Whether a such element contained by the queue.
  ///
  /// Uses pred operation to compare elements.
  /// Has O(N) complexity. Can be used to look up set as unordered 
  /// collection using different search criteria instead of using
  /// criteria the collection is ordered by.
  ///
  /// @tparam PredicateT Comparator object to use while looking up the queue.
  /// @param element Element to look for.
  /// @param pred Predicate used to search the element.
  /// @return Returns true if element is contained and false otherwise.
  template <typename PredicateT>
  bool contains(const value_type& element, const PredicateT& pred) const
  {
    typename container_type::const_iterator cit = std::find_if(
      container().begin(), container().end(), std::bind2nd(pred, element));

    return cit != container().end();
  }

protected:
  /// Returns a reference to underlying container.
  /// 
  /// @return Returns a reference container object used to store elements.
  container_type& container(void) { return _cont; }

  /// Returns a const reference to underlying container.
  /// 
  /// @return Returns a const reference container object used to store elements.
  const container_type& container(void) const { return _cont; }

  /// Returns a reference to comparator object.
  /// 
  /// @return Returns a reference comparator object used to store elements.
  ComparatorT& comparator(void) { return _comp; }

  /// Returns a const reference to comparator object.
  /// 
  /// @return Returns a const reference comparator object used to store elements.
  const ComparatorT& comparator(void) const { return _comp; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  container_type _cont;
  ComparatorT _comp;
};

/// Inserts an element if it's not contained by the queue.
///
/// @tparam T Queue elements type.
/// @tparam ContainerT Container type the queue is implemented by.
/// @tparam ComparatorT Represents the operation elements is compared by.
/// @param queue Queue to look up the element.
/// @param element An element to look for.
template<typename T, typename ContainerT, typename ComparatorT>
bool push_if_unique(stable_priority_queue<T, ContainerT, ComparatorT>& queue, 
                 const typename stable_priority_queue<T, ContainerT, ComparatorT>::value_type& element)
{
  const bool is_unique = !queue.contains(element);

  if (is_unique)
  {
    queue.push(element);
  }

  return is_unique;
}

/// Inserts an element if it's not contained by the queue.
///
/// @tparam T Queue elements type.
/// @tparam ContainerT Container type the queue is implemented by.
/// @tparam ComparatorT Represents the operation elements is compared by.
/// @tparam PredicateT Comparator object type to use while looking up the queue.
/// @param queue Queue to look up the element.
/// @param element An element to look for.
/// @param pred A comparator object used to look for the element.
template<typename T, typename ContainerT, typename ComparatorT, typename PredicateT>
bool push_if_unique(stable_priority_queue<T, ContainerT, ComparatorT>& queue, 
                 const typename stable_priority_queue<T, ContainerT, ComparatorT>::value_type& element, 
                 const PredicateT& pred)
{
  const bool is_unique = !queue.contains(element, pred);

  if (is_unique)
  {
    queue.push(element);
  }

  return is_unique;
}

} // namespace smart

#endif // SMART_STABLE_PRIORITY_QUEUE_HPP_

 