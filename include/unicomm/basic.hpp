///////////////////////////////////////////////////////////////////////////////
// basic.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm basic declarations and definitions.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_BASIC_HPP_
#define UNI_BASIC_HPP_

/** @file basic.hpp Basic types and constants declarations. */

#include <unicomm/config/auto_link.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <map>

#include <cstddef>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

//@{
/** Message identifier type. */
typedef unsigned long messageid_type;

/** Communicator identifier type. */
typedef unsigned long commid_type;

/** Full message identifier. */
typedef std::pair<commid_type, messageid_type> full_messageid_type;
//@}

//@{
/** Incoming buffer type. */
typedef std::string in_buffer_type;

/** Outgoing buffer type. */
typedef std::string out_buffer_type;
//@}

/** Auxiliary strings type. */
typedef std::vector<std::string> strings_type;

//@{
/** Minimum available message priority. */
UNICOMM_DECL size_t min_priority(void);

/** Maximum available message priority. */
UNICOMM_DECL size_t max_priority(void);

/** Undefined priority value. */
UNICOMM_DECL size_t undefined_priority(void);
//@}

/** Infinite timeout value. 
 *
 *  The value means if reply to a message is needed it will be waited for infinitely.
 *
 *  @return Infinite timeout value.
 */
inline size_t infinite_timeout(void) { return 0; }

/** Returns whether timeout value means infinite timeout. 
 *
 *  @param tout Timeout value.
 *  @return True if timeout is infinite and false otherwise.
 */
inline bool is_infinite_timeout(size_t tout) 
  { return tout == infinite_timeout(); }

/** Undefined message identifier. 
 *
 *  Means message id doesn't exist i.e. message instance has no identifier.
 */
inline messageid_type undefined_messageid(void) { return 0; }

/** Default creator function, can be used with the factory. */
template <typename T> boost::shared_ptr<T> inline create(void) 
{ 
  return boost::shared_ptr<T>(new T()); 
}

/** Used by unicomm::dispatcher::send_all() to identify messages were sent to each client. 
 *
 *  @see unicomm::dispatcher::send_all().
 */
typedef std::map<commid_type, messageid_type> full_messageid_map_type;

/** Default wait on stop value. 
 *
 *  @return Wait on stop value in seconds. 
 *  @see unicomm::dispatcher::stop.
 */
inline const boost::posix_time::seconds& default_wait_on_stop(void) 
{ 
  static const boost::posix_time::seconds wait(50); return wait; 
}

/** Xml message format identifier value. */
inline const std::string& xml_message_format(void) 
{ 
  static const std::string s = "xml"; return s; 
}

/** Binary message format identifier value. */
inline const std::string& binary_message_format(void) 
{ 
  static const std::string s = "binary"; return s; 
}

/** Whether binary format used. */
inline bool is_binary_message_format(const std::string& format) 
{ 
  return format == binary_message_format(); 
}

/** Whether xml format used. */
inline bool is_xml_message_format(const std::string& format) 
{ 
  return format == xml_message_format(); 
}

/** Whether custom message format used. */
inline bool is_custom_message_format(const std::string& format) 
{ 
  return !is_xml_message_format(format) && !is_binary_message_format(format); 
}

// forward
class communicator;

/** Communicator smart pointer type. */
typedef boost::shared_ptr<communicator> comm_pointer_type;

} // namespace unicomm

#endif // UNI_BASIC_HPP_

