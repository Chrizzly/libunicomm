///////////////////////////////////////////////////////////////////////////////
// message_decoder_base.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol message decoder. Accepts incoming data and 
// creates user defined messages.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_MESSAGE_DECODER_BASE_HPP_
#define UNI_MESSAGE_DECODER_BASE_HPP_

/** @file message_decoder_base.hpp Message decoder base class. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/comm_buffer.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/basic.hpp>

#include <boost/shared_ptr.hpp>

#include <string>
#include <utility>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class session_base;

/** Unicomm message decoder base class. 
 *
 *  Accepts incoming data and creates user defined messages.
 *  This is base class for message decoding logic. It's necessary to implement
 *  virtual interface functions.
 *  
 *  @see unicomm::bin_message_decoder, unicomm::xml_message_decoder.
 */
class UNICOMM_DECL message_decoder_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Message decoder base smart pointer type. */
  typedef boost::shared_ptr<message_decoder_base> pointer_type;

public:
  /** Destroys an instance of the class. */
  virtual ~message_decoder_base(void) { /* empty */ }

public:
  /** Sets factory object used to create message.  
   *
   *  @param factory Message factory object.
   */
  void factory(const message_base::factory_type& factory) { _factory = factory; }

  /** Returns factory object. 
   *
   *  @return Message factory object.
   */
  const message_base::factory_type& factory(void) const { return _factory; }

public:
  /** Algorithm implementation. 
   *
   *  The function implements a template using private virtual functions. 
   *  If the implemented template does not suits your requirements
   *  override this and implement all the decoding rules by yourself.
   *
   *  @param buffer Raw data arrived from channel.
   *  @param session Session object that represents the connection.
   *
   *  @return Message base pointer. Should return null if there is no message
   *    found in the buffer and pointer to the received message instead.
   *
   *  @note May throw any derived from std::exception if error occurs. 
   *    Not std::exception throwing causes debug will assert.
   */
  virtual message_base::pointer_type perform_decode(comm_buffer& buffer, 
    session_base& session);

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:
  /** Pair of income buffer iterators. */
  typedef std::pair<comm_buffer::buffer_type::iterator, 
    comm_buffer::buffer_type::iterator> iter_pair_type;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Finds message's bounds. 
   *
   *  @param buffer Raw data arrived from channel.
   *  @param session User session object representing the connection.
   *  @return Pair of iterators that designates to the begin and the end of the 
   *    decoded message data. 
   *
   *  @note If there is no message found both iterators should
   *    have the same value, e.g. points to the buffer.end().
   *
   *  @note May throw any derived from std::exception if error occurs. 
   *    Not std::exception throwing causes debug will assert.
   */
  virtual iter_pair_type find_raw_message(comm_buffer::buffer_type& /*buffer*/, 
                                          session_base& /*session*/) // = 0;
  { 
    return std::make_pair(iter_pair_type::first_type(), 
      iter_pair_type::second_type()); 
  } 

  /** Decode message data. 
   *
   *  This is used to decode message. Remove markers and dereference 
   *  escape symbols for example in case of binary encoding.
   *
   *  @param raw_message Incoming raw data.
   *  @param session User session object representing the connection.
   *  @return Should return a reference to raw_message.
   *  @note May throw any derived from std::exception if error occurs. 
   *    Not std::exception throwing causes debug will assert.
   */
  virtual std::string& decode_raw_message(std::string& raw_message, 
    session_base& /*session*/) { return raw_message; } // = 0;

  /** Returns message name (identifier) extracted from incoming raw data. 
   *
   *  @param raw_message Incoming decoded raw data. 
   *    This is returned result from 
   *    unicomm::message_decoder_base::decode_raw_message().
   * 
   *  @param session User session object representing the connection.
   *  @return Message identifier found in raw_message.
   *
   *  @note If message couldn't be recognized 
   *    throw unicomm::message_decoder_error or
   *    any derived from std::exception. 
   *    It's also possible to throw type not derived 
   *    from std::exception, but debug will assert.
   */
  virtual std::string get_message_name(const std::string& /*raw_message*/, 
    session_base& /*session*/) { return std::string(); } // = 0;

  /** Creates a message using given name (identifier). 
   *
   *  By default, this creates user's message using factory 
   *  specified by configuration.
   *
   *  @param name Name (identifier) of the message type to be created.
   *  @param session User session object representing the connection.
   *  @note May throw any derived from std::exception if error occurs. 
   *    Not std::exception throwing causes debug will assert.
   *  
   *  @throw The same as smart::factory::create():
   *    smart::invalid_type_error if given identifier 
   *    is not found by the factory.
   *
   *  @see unicomm::config::message_factory().
   */
  virtual message_base::pointer_type create_message(const std::string& name, 
    session_base& session);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  // at this moment there is no need to synchronize an access to the factory
  // client & server provides const interface to the configuration and it 
  // can't be changed once created
  message_base::factory_type _factory;
};

} // namespace unicomm

#endif // UNI_MESSAGE_DECODER_BASE_HPP_
