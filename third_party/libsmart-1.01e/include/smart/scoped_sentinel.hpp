///////////////////////////////////////////////////////////////////////////////
// scoped_sentinel.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko, Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file scoped_sentinel.hpp
 *
 *  Scoped sentinel provides simple RAII idiom implementaion. 
 */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SCOPED_SENTINEL_HPP_
#define SCOPED_SENTINEL_HPP_

#include <boost/function.hpp>

/** @namespace smart smart++ Library root namespace. */
namespace smart
{

/** Sentinel class template.
 *
 *  Implements RAII idiom. Allows to define operations executed on constuctor
 *  and destructor. Use @e boost::bind to bind any parameters to 
 *  an operation or use own function objects to define operations you need 
 *  to be executed.
 *
 *  @tparam CallableStartupT Operation type which is called on constructor.
 *  @tparam CallableCleanupT Operation type which is called on destructor.
 */
template <typename CallableStartupT, typename CallableCleanupT>
struct sentinel
{
public:
  /** Startup operation type. 
   *  Operation of this type is called on constructor if the operation is specified.
   */
  typedef CallableStartupT startup_operation_type;
  /** Cleanup operation type. 
   *  Operation of this type is called on destructor if the operation is specified.
   */
  typedef CallableCleanupT cleanup_operation_type;

public:
  /** Constructs sentinel object and calls startup operation if it is specified. */
  explicit sentinel(const cleanup_operation_type& ondestroy, const startup_operation_type& oncreate = 0): _ondestroy(ondestroy) { if (oncreate) { oncreate(); } }
  /** Destroy sentinel object and calls cleanup operation if it is specified. */
  ~sentinel(void) { if (_ondestroy) { _ondestroy(); } }

public:
  /** Returns a const reference to a cleanup operation object. */
  const cleanup_operation_type& cleanup_operation(void) const { return _ondestroy; }
  /** Sets cleanup operation object to be executed on sentinel destructor. */
  void cleanup_operation(const cleanup_operation_type& ondestroy) { _ondestroy = ondestroy; }
  /** Clears cleanup operation object. */
  void clear(void) { _ondestroy = cleanup_operation_type(); }

private:
  cleanup_operation_type _ondestroy;
};

/** Special case startup operation type. */
typedef boost::function<void (void)> generic_startup_operation_type;
/** Special case cleanup operation type. */
typedef boost::function<void (void)> generic_cleanup_operation_type;

typedef sentinel<generic_startup_operation_type, generic_cleanup_operation_type> generic_scoped_sentinel;
/** Short type name synonym. */
typedef generic_scoped_sentinel scoped_sentinel;

} // namespace smart

#endif // SCOPED_SENTINEL_HPP_

