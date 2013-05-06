///////////////////////////////////////////////////////////////////////////////
// debug_out.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file debug_out.hpp Debug logger. 
 *  
 *  Provides stream interface and maps output to system debug console.
 *  On linux platrom writes to rsyslog using LOG_DEBUG flag.
 */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_DEBUG_OUT_HPP_
#define SMART_DEBUG_OUT_HPP_

#define SMART_SHORT_LIB_NAME "debug_out"
#include <smart/config/auto_link.hpp>
#undef SMART_SHORT_LIB_NAME

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4511) // copy constructor could not be generated
# pragma warning (disable : 4512) // assignment operator could not be generated
# pragma warning (disable : 4702) // unreachable code
# pragma warning (disable : 4996) 
#endif // SMART_VISUAL_CPP

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

#include <ostream>

namespace smart
{

/** debug_sink class. 
 *
 *  Provides operation which puts data into system debug console.
 *  Under windows uses OutputDebugString to write to debugger console. 
 *  Under linux uses rsyslog services, calls syslog(LOG_DEBUG, "%s", s.c_str());
 * 
 *  @see openlog, syslog. 
 */
class debug_sink : public boost::iostreams::sink 
{
public:
  std::streamsize write(const char* s, std::streamsize n);
};

/** mt_debug_sink class. 
 *
 *  Provides thread safe operation which puts data into system debug console.
 *  Under windows uses OutputDebugString to write to debugger console. 
 *  Under linux uses rsyslog services, calls syslog(LOG_DEBUG, "%s", s.c_str());
 * 
 *  @see openlog, syslog. 
 */
class mt_debug_sink : public boost::iostreams::sink
{
public:
  std::streamsize write(const char* s, std::streamsize n);
};

/** Debug sink stream type. */
typedef boost::iostreams::stream<debug_sink> debug_sink_stream_type;

/** Multi threading debug sink stream type. */
typedef boost::iostreams::stream<mt_debug_sink> mt_debug_sink_stream_type;

/** Debug output stream.
 *
 *  Singleton implementation.
 */
debug_sink_stream_type& debug_ostream(void);

/** Multi threading debug output stream.
 *
 *  Singleton implementation.
 */
mt_debug_sink_stream_type& mt_debug_ostream(void);

namespace detail 
{

//////////////////////////////////////////////////////////////////////////
// stream locker
struct locker
{
  locker(void): _lock(mutex()) { /* empty */ }

private:
  boost::mutex::scoped_lock _lock;

  static boost::mutex& mutex(void) { static boost::mutex m; return m; }
};

template<typename CharT, typename TraitsT>
inline std::basic_ostream<CharT, TraitsT>& 
operator<<(std::basic_ostream<CharT, TraitsT>& os, const smart::detail::locker&)
{
  return os;
}

} // namespace detail

} // namespace smart

#ifdef SMART_DEBUG
/** Debug output macros. */
# define SMART_DEBUG_OUT(expr) { smart::debug_ostream() << \
  std::dec << __FILE__ << "[" << __LINE__ << "]: " << expr << std::endl; }
# define SMART_MT_DEBUG_OUT(expr) { smart::debug_ostream() << smart::detail::locker() \
  << std::dec << __FILE__ << "[" << __LINE__ << "]: (" << std::hex << std::showbase \
  << boost::this_thread::get_id() << ") " << std::dec << expr << std::endl; }
#else
# define SMART_DEBUG_OUT(expr)
# define SMART_MT_DEBUG_OUT(expr)
#endif // SMART_DEBUG

/** Debug output macros.
 *
 *  @deprecated Use SMART_DEBUG_OUT instead.
 */
#define SIMPLE_DEBUG_OUT(expr) SMART_DEBUG_OUT(expr)

/** Checks if debug configuration is used and throws out expression if not. */
#ifdef SMART_DEBUG
# define SMART_IFDEF_DEBUG(expr) expr
#else
# define SMART_IFDEF_DEBUG(expr)
#endif // #ifdef SMART_DEBUG

#endif // SMART_DEBUG_OUT_HPP_
