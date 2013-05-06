///////////////////////////////////////////////////////////////////////////////
// timres.cpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file timers.cpp Time measuring stuff. */

#include <smart/timers.hpp>

//#ifdef _MSC_VER
//  #pragma warning (push)
//  #pragma warning (disable : 4511) // copy constructor could not be generated
//  #pragma warning (disable : 4512) // assignment operator could not be generated
//  #pragma warning (disable : 4127) // conditional expression is constant
//  #pragma warning (disable : 4702) // unreachable code
//#endif
//
//#include <boost/date_time/posix_time/posix_time.hpp>
//
//#ifdef _MSC_VER
//  #pragma warning (pop)
//#endif

////////////////////////////////////////////////////////////////////////////////
// timeout

//------------------------------------------------------------------------------
bool smart::timeout::elapsed(void) const 
{ 
  const boost::posix_time::ptime finish = finish_time();
  const boost::posix_time::ptime now    = boost::posix_time::microsec_clock::local_time();

  //return _tout == uni_config::infinite_timeout? false: boost::posix_time::microsec_clock::local_time() > finishtime(); 
  return _tout == infinite_timeout() ? false : now > finish; 
}

//------------------------------------------------------------------------------
const boost::posix_time::time_duration& smart::timeout::infinite_timeout(void)
{
  static const boost::posix_time::time_duration res(boost::posix_time::pos_infin);
  return res;
}
