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

#include <smart/watchdog.hpp>

#ifdef SMART_WIN
# include <Windows.h>
#endif // SMART_WIN

namespace smart
{

//------------------------------------------------------------------------------
watchdog::watchdog(const boost::posix_time::time_duration& timeout):
  _timeout(timeout),
  _working(true)
{
  _thread = boost::thread(&watchdog::loop, this);
}

//------------------------------------------------------------------------------
watchdog::~watchdog(void)
{
  _working = false;
  kick();
  _thread.join();
}

//------------------------------------------------------------------------------
void watchdog::kick(void)
{
  _condition.notify_one();
}

//------------------------------------------------------------------------------
void watchdog::loop(void)
{
  // we do not need common mutex for kick() and loop(), see
  // http://www.boost.org/doc/libs/1_35_0/doc/html/thread/synchronization.html#thread.synchronization.condvar_ref
  boost::mutex mut;
  boost::mutex::scoped_lock lock(mut);
  while (_working)
  {
    if (!_condition.timed_wait(lock, _timeout))
    {
#ifdef SMART_WIN
      TerminateProcess(GetCurrentProcess(), 0);
      //ExitProcess(0);
#elif defined SMART_LINUX
# error fixme: Incompleted yet, sorry
#endif // 
    }
  }
}

} // namespace smart
