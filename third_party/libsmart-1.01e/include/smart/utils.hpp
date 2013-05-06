///////////////////////////////////////////////////////////////////////////////
// utils.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file utils.hpp Different utilities. */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_UTILS_HPP_
#define SMART_UTILS_HPP_

/** @file utils.hpp Different utilities. */

/** @namespace smart Library root namespace. */
namespace smart
{

/** Thread safe getter. */
template <typename ValueT, typename MutexT>
inline ValueT get(const ValueT& v, MutexT& l)
{
  typename MutexT::scoped_lock lock(l); return v;
}

/** Thread safe setter. */
template <typename ValueT, typename MutexT>
inline void set(ValueT& dest, const ValueT& source, MutexT& l)
{
  typename MutexT::scoped_lock lock(l); dest = source;
}

///** Throws given exception if predicate returns true. */
//template <typename T, typename PredT, typename ExcT>
//void throw_if(const T &what, const PredT &pred, const ExcT &exc)
//{
//  if (pred(what))
//  {
//    throw exc;
//  }
//}
//

/** Throws given exception if predicate returns true. 
 * 
 *  This version is made to be used in assignment operations.
 *  Something like this: const size_t i = throw_if(5, ...);
 *
 *  @tparam ExceptionT Exception type to be thrown.
 *  @tparam T Type of result to be tested.
 *  @tparam PredecateT Type of predecate object.
 *  @param what The value to test.
 *  @param pred Predecate object reference.
 *  @param message Message to be passed to exception class constructor.
 *  @return Returns what.
 */
template <typename ExceptionT, typename ArgT, typename PredicateT>
const ArgT& throw_if(const ArgT& arg, const PredicateT& pred, const char* message)
{
  if (pred(arg))
  {
    throw ExceptionT(message);
  }

  return arg;
}

/** Throws given exception if predicate returns true. 
 *
 *  @tparam ExceptionT Exception type to be thrown.
 *  @tparam PredecateT Type of predecate object.
 *  @param pred Predecate object reference.
 *  @param message Message to be passed to exception class constructor.
 */
template <typename ExceptionT, typename PredicateT>
void throw_if(const PredicateT& pred, const char* message)
{
  if (pred())
  {
    throw ExceptionT(message);
  }
}

} // namespace smart

#endif // SMART_UTILS_HPP_
