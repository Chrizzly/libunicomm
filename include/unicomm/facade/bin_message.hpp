///////////////////////////////////////////////////////////////////////////////
// bin_message.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm binary message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_BIN_MESSAGE_HPP_
#define UNI_BIN_MESSAGE_HPP_

/** @file bin_message.hpp Binary message definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/basic.hpp>

#include <boost/shared_ptr.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Message which is serialized into binary array. 
 *
 *  This is optional stuff implemented as facade to library core. 
 *  If you are developing own protocol to exchange data in most cases 
 *  it's would be enough. If you are not satisfied with 
 *  unicomm::bin_message implement your 
 *  own derived from it or from unicomm::message_base.
 */
class UNICOMM_DECL bin_message : public message_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Message format identifier implemented by this class.
   *  
   *  @return Message format name this class is regarded to.
   *  @see message_base::message_format(), setup_message_format(),
   *    config::message_decoder(), config::message_encoder().
   */
  static const std::string& message_format(void) { return binary_message_format(); }

public:
  /** Constructs a message object with given priority. 
   *
   *  @param p Message priority. This value is used when message is popped out 
   *    from the message outgoing queue. Undefined priority value causes that when
   *    the message is pushed into the queue it will be assigned default priority
   *    value specified by the @ref config_section "configuration".
   *    If you want this instance of the message to have distinct priority 
   *    pass it to the constructor.
   *
   *  @see undefined_priority(), min_priority(), 
   *    max_priority(), config::message_priority().
   */
  explicit bin_message(size_t p = undefined_priority());

public:
  /** Returns message instance identifier. 
   *
   *  @return Message instance identifier.
   */
  messageid_type id(void) const;

  /** Sets message instance identifier. 
   *
   *  @see unicomm::message_base::id().
   */
  bin_message& id(messageid_type mid); 

  /** Returns message instance reply id. 
   *
   *  @return Message reply id. If message is reply and identifiers are used 
   *    by the engine this value is an id of the request message that this 
   *    message is reply to.
   */
  messageid_type rid(void) const;

  /** Sets message instance reply id. 
   *
   *  @param mid Reply message instance identifier.
   */
  void rid(messageid_type mid);

  /** Serializes message to a std::string. 
   *
   *  @return Serialized string.
   */
  std::string serialize(void) const;

  /** Restores message from a std::string. 
   *
   *  @param message Serialized message string.
   */
  void unserialize(const std::string &message);

public:
  /** Sets message priority. 
   *
   *  @param p Message priority.
   *  @see unicomm::min_priority(), unicomm::max_priority().
   */
  bin_message& priority(size_t p);

  /** Returns message priority. 
   *
   *  @return Message priority.
   *  @see unicomm::min_priority(), unicomm::max_priority().
   */
  size_t priority(void) const;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  virtual void unserialize_tail(const std::string& /*message_tail*/) { /* empty */ }

private:
  struct impl;

  typedef boost::shared_ptr<impl> impl_ptr;

private:
  impl_ptr _impl;
};

} // namespace unicomm

#endif // UNI_BIN_MESSAGE_HPP_
