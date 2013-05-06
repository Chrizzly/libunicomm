///////////////////////////////////////////////////////////////////////////////
// bin_message_encoder.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol binary message encoder.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_BIN_MESSAGE_ENCODER_HPP_
#define UNI_BIN_MESSAGE_ENCODER_HPP_

/** @file bin_message_encoder.hpp Binary message encoder. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/message_encoder_base.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class session_base;

/** Unicomm binary message encoder class. 
 *
 *  This stuff is optional.
 *
 *  @see unicomm::message_encoder_base.
 */
class bin_message_encoder : public message_encoder_base
{
//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Should perform necessary encoding operations. 
   *
   *  @param buffer Serialized message data.
   *  @param session User session object representing the connection.
   *  @return Should return reference to the buffer.
   */
  virtual out_buffer_type& encode_raw_message(out_buffer_type& buffer, 
    session_base& session);
};

/** Creates binary message encoder object. 
 *
 *  @return Smart pointer to newly created unicomm::message_encoder_base.
 */
inline message_encoder_base::pointer_type create_binary_encoder(void)
{
  return message_encoder_base::pointer_type(new bin_message_encoder());
}

} // namespace unicomm

#endif // UNI_BIN_MESSAGE_ENCODER_HPP_
