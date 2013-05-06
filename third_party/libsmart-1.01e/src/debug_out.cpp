///////////////////////////////////////////////////////////////////////////////
// debug_out.cpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

#include <smart/debug_out.hpp>

#ifdef SMART_WIN
# include <windows.h>
#elif defined SMART_LINUX
# include <syslog.h>
#endif

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4511) // copy constructor could not be generated
# pragma warning (disable : 4512) // assignment operator could not be generated
# pragma warning (disable : 4127) // conditional expression is constant
# pragma warning (disable : 4702) // unreachable code
# pragma warning (disable : 4244) // warning C4244: '+=' : conversion from 'int' to 'unsigned short', possible loss of data
#endif // SMART_VISUAL_CPP

#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

#include <boost/assert.hpp>
//#include <boost/thread/mutex.hpp>

#include <fstream> 
#include <string>
#include <stdexcept>

using std::string;
using std::streamsize;
using std::exception;
using std::runtime_error;

using boost::mutex;

//////////////////////////////////////////////////////////////////////////
// aux stuff
namespace
{

//////////////////////////////////////////////////////////////////////////
// debug writer
class debug_writer
{
  ////////////////////////////////////////////////////////////////////////
  // debug writer interface
public:
  debug_writer(void)
  {
    open();
  }

  ~debug_writer(void)
  {
    try
    {
      close();
    } 
    catch (const exception& /*e*/)
    {
      BOOST_ASSERT(!"An std::exception on destructor");
    }
    catch (...)
    {
      BOOST_ASSERT(!"An unknown exception on destructor");
    }
  }

  void write(const string& s)
  {
#ifdef SMART_WIN
    ::OutputDebugStringA(s.c_str());
#elif defined (SMART_LINUX)
    try
    {
      syslog(LOG_DEBUG, "%s", s.c_str());
    } catch (...)
    {
      // don't know what exception is thrown by syslog it's not described
      throw runtime_error("Can not write to system log");
    }
#endif
  }

  ////////////////////////////////////////////////////////////////////////
  // private stuff
private:
  void open(void)
  {
#ifdef SMART_LINUX
    try
    {
      openlog("smart::debug_out", LOG_CONS, LOG_USER);
    } catch (...)
    {
      // don't know what exception is thrown
      throw runtime_error("Can not open system log");
    }
#endif // SMART_LINUX
  }

  void close(void)
  {
#ifdef SMART_LINUX
    try
    {
      closelog();
    } catch (...)
    {
      // don't know what exception is thrown
      throw runtime_error("Can not close system log");
    }
#endif // SMART_LINUX
  }
};


} // unnamed namespace

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4244) // warning C4244: 'argument' : conversion from 'std::streamsize' to 'unsigned int', possible loss of data
#endif // SMART_VISUAL_CPP

//////////////////////////////////////////////////////////////////////////
// debug_sink class. Provides operation which puts data into system 
// debug console
streamsize smart::debug_sink::write(const char* s, streamsize n)
{
  debug_writer().write(string(s, n));

  return n;
}

//-----------------------------------------------------------------------------
smart::debug_sink_stream_type& smart::debug_ostream(void)
{
  static debug_sink sink;
  static debug_sink_stream_type stream(sink);

  return stream;
}

//////////////////////////////////////////////////////////////////////////
// mt_debug_sink class. Provides operation which puts data into system 
// debug console
streamsize smart::mt_debug_sink::write(const char* s, streamsize n)
{
  static boost::mutex _mutex;

  boost::mutex::scoped_lock lock(_mutex);

  debug_writer().write(string(s, n));

  return n;
}

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

//-----------------------------------------------------------------------------
smart::mt_debug_sink_stream_type& smart::mt_debug_ostream(void)
{
  static mt_debug_sink sink;
  static mt_debug_sink_stream_type stream(sink);

  return stream;
}

//void f(void) { SMART_MT_DEBUG_OUT("") }