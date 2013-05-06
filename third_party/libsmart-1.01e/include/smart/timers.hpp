///////////////////////////////////////////////////////////////////////////////
// timres.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file timers.hpp Time measuring stuff. */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_TIMERS_HPP_
#define SMART_TIMERS_HPP_

#define SMART_SHORT_LIB_NAME "timers"
#include <smart/config/auto_link.hpp>
#undef SMART_SHORT_LIB_NAME

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4244) //  warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#endif // SMART_VISUAL_CPP

#include <boost/date_time/posix_time/posix_time_types.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

/** @namespace smart 
 *  Library root namespace.
 */
namespace smart
{

/** Simple timeout class. */
struct timeout
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Infinite timeout value. */
  static const boost::posix_time::time_duration& infinite_timeout(void);

public:
  /** Constructs timeout object. */
  explicit timeout(const boost::posix_time::time_duration& tout = infinite_timeout(), 
    const boost::posix_time::ptime& start = boost::posix_time::microsec_clock::local_time()): 
    _start(start),
    _tout(tout)
  {
    // empty
  }

public:
  /** Returns timeout duration. */
  const boost::posix_time::time_duration& time_out(void) const { return _tout; }
  /** Returns start time. */
  const boost::posix_time::ptime& start_time(void) const { return _start; }
  /** Returns finish time. */
  boost::posix_time::ptime finish_time(void) const { return _start + _tout; }
  /** Whether timeout elapsed. */
  bool elapsed(void) const;
  /** Sets timeout duration. */
  void time_out(const boost::posix_time::time_duration& tout) { _tout = tout; }
  /** Sets start time. */
  void start_time(const boost::posix_time::ptime& t) { _start = t; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  boost::posix_time::ptime _start;
  boost::posix_time::time_duration _tout;
};

/** Resets timeout start to now. */
inline void reset(timeout& tout) { tout.start_time(boost::posix_time::microsec_clock::local_time()); }

} // namespace smart

#endif // SMART_TIMERS_HPP_

