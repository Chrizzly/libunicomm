///////////////////////////////////////////////////////////////////////////////
// bin_message_decoder.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol binary message decoder. 
// Accepts incoming data and creates user defined messages.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_BIN_MESSAGE_DECODER_HPP_
#define UNI_BIN_MESSAGE_DECODER_HPP_

/** @file bin_message_decoder.hpp Binary message decoder. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/message_decoder_base.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class session_base;

/** Unicomm binary message decoder class. 
 *
 *  This stuff is optional.
 *
 *  @see unicomm::message_decoder_base.
 */
class bin_message_decoder : public message_decoder_base
{
//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Should find message and return first and last bounds. 
   *
   *  @param buffer Income raw data buffer.
   *  @param session User session object representing the connection.
   *  @return Pair of iterators where first is the begin of the message and 
   *    second is the end (designates to past the last element) of the message.
   */
  virtual iter_pair_type find_raw_message(comm_buffer::buffer_type& buffer, 
    session_base& session);

  /** Should perform any decode operations if there are.
   *
   *  @param raw_message Message raw data.
   *  @param session User session object representing the connection.
   *  @return Decoded message string.
   *  @throw unicomm::message_decoder_error if an error is encountered while parsing.
   */
  virtual std::string& decode_raw_message(std::string& raw_message, 
    session_base& session);

  /** Should return message name (identifier) extracted from raw data. 
   *  
   *  @param raw_message Message raw data.
   *  @param session User session object representing the connection.
   *  @return Message name (identifier).
   *  @throw unicomm::message_decoder_error if an error is encountered while parsing.
   */
  virtual std::string get_message_name(const std::string& raw_message, 
    session_base& session);
};

/** Creates binary message decoder. 
 *
 *  @return Smart pointer to newly created unicomm::message_decoder_base.
 */
inline message_decoder_base::pointer_type create_binary_decoder(void)
{
  return message_decoder_base::pointer_type(new bin_message_decoder());
}

} // namespace unicomm

#endif // UNI_BIN_MESSAGE_DECODER_HPP_
