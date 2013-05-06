///////////////////////////////////////////////////////////////////////////////
// uhelper.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_HELPER_HPP_
#define SMART_COMPLEX_HELPER_HPP_

/** @file uhelper.hpp Unicomm helper routines and entities. */

/** @namespace unicomm Unicomm++ Library root namespace. */
namespace smart
{

/** @namespace data Different entities which help to organize and store different data. */
namespace data
{

/** Throws given exception if predicate returns true. */
template <typename T, typename PredT, typename ExcT>
void throwif(const T &what, const PredT &pred, const ExcT &exc)
{
  if (pred(what))
  {
    throw exc;
  }
}

/** Throws given exception if predicate returns true. */
template <typename ExcT, typename T, typename PredT>
const T& throwif(const T &what, const PredT &pred, const char* message)
{
  if (pred(what))
  {
    throw ExcT(message);
  }

  return what;
}

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_HELPER_HPP_
