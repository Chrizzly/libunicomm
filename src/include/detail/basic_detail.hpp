///////////////////////////////////////////////////////////////////////////////
// basic_detail.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm basic detail declarations and definitions.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_FACADE_BASIC_DETAIL_HPP_
#define UNI_FACADE_BASIC_DETAIL_HPP_

/** @file basic_detail.hpp Unicomm basic detail types and constants declarations. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/message_encoder_base.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <string>

#include <cstddef>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** @namespace detail Unicomm library implementation details. */
namespace detail
{

/** XML message packet end marker. */
inline const std::string& xml_message_end(void) 
{ 
  static const std::string s = "\r\n\r\n"; return s; 
}

/** Returns binary message packet end marker. */
inline std::string::value_type bin_message_end(void) { return '\xff'; }

/** Returns binary escape symbol. */
inline std::string::value_type bin_escape_sym(void) { return '\xfe'; }

/** Whether character is escape symbol. */
inline bool is_bin_escape_sym(std::string::value_type sym) 
{ 
  return sym == bin_escape_sym(); 
}

/** Whether character is message end symbol. */
inline bool is_bin_end_sym(std::string::value_type sym) 
{ 
  return sym == bin_message_end(); 
}

/** Decode/encode one symbol. */
inline std::string::value_type bin_process_one(std::string::value_type sym) 
{ 
  return sym ^ 0x40; 
}

/** Current decode engine version. */
inline std::string::value_type bin_version(void) { return 0; }

/** Minimum binary message header length. */
inline size_t bin_header_min_len(void) { return 3; }

/** Binary message header flags. */
enum bin_header_flag
{
  FLAG_ID_FIELD   = 0x01,
  FLAG_RID_FIELD  = 0x02,

  FLAG_DUMMY      = 0xff
};

/** Gets flag. */
inline bool get_flag(std::string::value_type flag_set, bin_header_flag flag) 
{ 
  return (flag_set & flag) != 0; 
}

/** Whether id field present in header. */
inline bool is_id_exists(std::string::value_type flags) 
{ 
  return get_flag(flags, FLAG_ID_FIELD); 
}

/** Whether rid field present in header. */
inline bool is_rid_exists(std::string::value_type flags) 
{ 
  return get_flag(flags, FLAG_RID_FIELD); 
}

/** Sets flag. */
inline std::string::value_type set_flag(std::string::value_type flag_set, 
                                        bin_header_flag flag, bool value) 
{ 
  return static_cast<std::string::value_type>(
    value? flag_set | flag: flag_set & ~flag); 
}

/** Sets id flag. */
inline std::string::value_type set_id_exists(std::string::value_type flag_set, 
                                             bool value) 
{ 
  return set_flag(flag_set, FLAG_ID_FIELD, value);
}

/** Sets rid flag. */
inline std::string::value_type set_rid_exists(std::string::value_type flag_set, 
                                              bool value) 
{ 
  return set_flag(flag_set, FLAG_RID_FIELD, value);
}

/** Background thread sleep timeout if nothing happens. */
inline size_t default_sleep_timeout(void) { return 50; }

/** Incoming message time quantum in milliseconds. */
inline size_t default_incoming_quantum(void) { return 100; }

/** Outgoing message time quantum in milliseconds. */
inline size_t default_outgoing_quantum(void) { return 100; }

/** Default tcp port value. */
inline unsigned short default_tcp_port(void) { return 0; }

/** Default tcp backlog value. 
 *
 *  This value designates to use current OS default value.
 */
inline int default_tcp_backlog(void) { return 0; }

/** Whether tcp backlog is default. */
inline bool use_default_tcp_backlog(int backlog) 
{ 
  return backlog == default_tcp_backlog(); 
}

/** Default message encoder. */
inline message_encoder_base::pointer_type default_message_encoder(void) 
{ 
  return unicomm::message_encoder_base::pointer_type(
    new unicomm::message_encoder_base);
}

} // namespace detail

} // namespace unicomm

#endif // UNI_FACADE_BASIC_DETAIL_HPP_
