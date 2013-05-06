///////////////////////////////////////////////////////////////////////////////
// watchdog.hpp
//
// Copyright (c) 2010 Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file watchdog.hpp Watchdog timer. */

#ifndef SMART_WATCHDOG_HPP_
#define SMART_WATCHDOG_HPP_

#define SMART_SHORT_LIB_NAME "watchdog"
#include <smart/config/auto_link.hpp>
#undef SMART_SHORT_LIB_NAME

//#include <boost/date_time/posix_time/ptime.hpp>
#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4512) // assignment operator could not be generated
# pragma warning (disable : 4244) // conversion from 'int' to 'unsigned short', possible loss of data
# pragma warning (disable : 4267) // conversion from 'size_t' to 'unsigned int', possible loss of data
#endif // _MSC_VER
#include <boost/thread.hpp>
#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // _MSC_VER
#include <boost/thread/condition.hpp>

namespace smart
{

class watchdog
{
public:
  watchdog(const boost::posix_time::time_duration& timeout);
  ~watchdog(void);
  void kick(void);

private:
  void loop(void);

private:
  boost::posix_time::time_duration _timeout;
  bool _working;
  boost::thread _thread;
  boost::condition _condition;
};

} // namespace smart

#endif // #ifndef SMART_WATCHDOG_HPP_