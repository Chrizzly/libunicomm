///////////////////////////////////////////////////////////////////////////////
// thread_sentinel.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko, Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file thread_sentinel.hpp
 *
 *  Provides thread safity for object the reference to is held. 
 */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef THREAD_SENTINEL_HPP_
#define THREAD_SENTINEL_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>
#include <boost/assert.hpp>

/** @namespace smart smart++ Library root namespace. */
namespace smart
{

/** Thread sentinel class template.
 *
 *  
 *
 *  @tparam CallableStartupT Operation type which is called on constructor.
 *  @tparam CallableCleanupT Operation type which is called on destructor.
 */
template <typename T, typename MutexT>
struct thread_sentinel
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Type that object of is held. */
  typedef T value_type;

  /** Reference type. */
  typedef T& reference;

  /** Const reference type. */
  typedef const T& const_reference;

  /** Pointer type. */
  typedef T* pointer;

  /** Const pointer type. */
  typedef const T* const_pointer;

  /** Mutex type. */
  typedef MutexT mutex_type;

public:
  /** Constructs sentinel object and calls startup operation if it is specified. */
  explicit thread_sentinel(T& guarded_object): 
    _object(&guarded_object),
    _mutex(boost::make_shared<MutexT>()),
    _owner_thread_id(boost::this_thread::get_id())
  { 
    // empty 
  }

  /** Constructs sentinel object and calls startup operation if it is specified. */
  thread_sentinel(const thread_sentinel<T, MutextT>& other): 
    _object(other._object),
    _mutex(other._mutex),
    _owner_thread_id(boost::this_thread::get_id())
  { 
    lock_mutex();
  }

  /** Destroy sentinel object and calls cleanup operation if it is specified. */
  ~thread_sentinel(void) 
  {  
    BOOST_ASSERT(is_owner_thread() && " - Object is being deleted by another thread");

    if (is_owner_thread() && !_mutex.unique())
    {
      unlock_mutex();
    }
  }

public:
  /** Returns a reference to a stored object. */
  reference operator*(void) const { return *_object; }

  /** Returns a const pointer to a stored object. */
  pointer operator->(void) const { return get(); }

  /** Returns a const pointer to a stored object. */
  pointer get(void) const { return _object; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  typedef boost::shared_ptr<MutexT> mutex_ptr;

private:
  // deny assign
  T& operator=(const thread_sentinel<T, MutexT>& other);

private:
  void lock_mutex(void) { _mutex->lock(); }
  void unlock_mutex(void) { _mutex->unlock(); }
  bool is_owner_thread(void) const { return _owner_thread_id == boost::this_thread::get_id(); }

private:
  pointer _object;
  mutex_ptr _mutex;
  boost::thread::id _owner_thread_id;
};

} // namespace smart

#endif // THREAD_SENTINEL_HPP_

