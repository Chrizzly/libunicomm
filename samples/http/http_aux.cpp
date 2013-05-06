///////////////////////////////////////////////////////////////////////////////
// http_aux.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Auxiliary entities.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_aux.hpp"

#include <boost/thread/mutex.hpp>

#include <iostream>

using boost::mutex;

using std::cout;
using std::flush;
using std::string;

//////////////////////////////////////////////////////////////////////////
// aux
/** Synchronize an access to the std::cout. */
void uni_http::out_str(const string& s)
{
  //static recursive_mutex m;
  static mutex m;

  //recursive_mutex::scoped_lock lock(m);
  mutex::scoped_lock lock(m);

  cout << "\r\n" << s << flush;
}

#ifdef UNICOMM_FORK_SUPPORT

#include "http_except.hpp"

#include <unicomm/config/auto_sence.hpp>

//////////////////////////////////////////////////////////////////////////
// aux

namespace 
{

// global data
int _pid = 0;

} // unnamed namespace

int uni_http::check_fork(int fork_result)
{
  // check fork error
  if (fork_result < 0)
  {
    throw fork_error("Couldn't fork");
  }

  if (fork_result > 0)
  {
    // parent
    UNICOMM_DEBUG_OUT("[http]: Parent: fork succeeded; child pid = " << fork_result)
    return 0;
  } 

  // child (daemon) continues
  UNICOMM_DEBUG_OUT("[http]: Child: fork succeeded")

  return getpid();
}

//------------------------------------------------------------------------
bool uni_http::is_child_process(void)
{
  return pid() != 0;
}

//------------------------------------------------------------------------
bool uni_http::is_parent_process(void)
{
  return !is_child_process();
}

//------------------------------------------------------------------------
void uni_http::pid(int v)
{
  _pid = v;
}

//------------------------------------------------------------------------
int uni_http::pid(void)
{
  return _pid;
}

#endif // UNICOMM_FORK_SUPPORT

