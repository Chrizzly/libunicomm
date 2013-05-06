///////////////////////////////////////////////////////////////////////////////
// message_encoder_base.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol message encoder base class definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_MESSAGE_ENCODER_BASE_HPP_
#define UNI_MESSAGE_ENCODER_BASE_HPP_

/** @file message_encoder_base.hpp Message encoder base class definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/basic.hpp>
#include <unicomm/message_base.hpp>

#include <boost/shared_ptr.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward 
class session_base;

/** Unicomm message encoder class. 
 *
 *  Performs message serialization.
 *
 *  @see bin_message_encoder, xml_message_encoder.
 */
class UNICOMM_DECL message_encoder_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Message encoder base smart pointer type. */
  typedef boost::shared_ptr<message_encoder_base> pointer_type;

public:
  /** Destroys an object. */
   virtual ~message_encoder_base(void) { /* empty */ }

public:
  /** Algorithm implementation. 
   *
   *  @param message Message to be encoded (serialized).
   *  @param session User session object representing the connection.
   *  @return Raw buffer contained encoded message to be 
   *    written to the channel.
   */
  virtual const out_buffer_type perform_encode(const message_base& message, 
    session_base& session);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Should perform any encoding operations. 
   *
   *  @param buffer Serialized message data.
   *  @param session User session object representing the connection.
   *  @return Should return reference to the buffer.
   */
  virtual out_buffer_type& encode_raw_message(out_buffer_type& buffer, 
    session_base& /*session*/) { return buffer; } // = 0;
};

} // namespace unicomm

#endif // UNI_MESSAGE_ENCODER_BASE_HPP_
