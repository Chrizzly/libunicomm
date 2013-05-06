///////////////////////////////////////////////////////////////////////////////
// xml_message.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol complex xml message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_XML_MESSAGE_HPP_
#define UNI_XML_MESSAGE_HPP_

/** @file xml_message.hpp Xml message definition based on 
 *    @e smart::data::complex. 
 */

#include <unicomm/config/auto_link.hpp>

#ifdef UNICOMM_USE_COMPLEX_XML

#include <unicomm/message_base.hpp>
#include <unicomm/basic.hpp>

#include <smart/data/complex.hpp>

#include <boost/shared_ptr.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Message which is serialized into complex xml. 
 *
 *  This stuff is optional. If you are not satisfied with 
 *  xml_message implement your own derived from unicomm::message_base. 
 */
class UNICOMM_DECL xml_message : public message_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Format identifier used by the message. 
   *
   *  @return Message format name this class is regarded to.
   *  @see message_base::message_format(), setup_message_format(),
   *    config::message_decoder(), config::message_encoder().
   */
  static const std::string& message_format(void) { return xml_message_format(); }

public:
  /** Constructs a message object with given priority. 
   *
   *  @param p Message priority. This value is used when message is popped out 
   *    from the message outgoing queue. Undefined priority value causes that when
   *    the message is pushed into the queue it will be assigned default priority
   *    value specified by the @ref config_section "configuration".
   *    If you want this instance of the message to have distinct priority, 
   *    pass it to the constructor.
   *
   *  @see undefined_priority(), min_priority(), 
   *    max_priority(), config::message_priority().
   */
  explicit xml_message(size_t p = undefined_priority());

public:
  /** Returns message instance identifier. 
   *
   *  @return Message instance identifier.
   */
  messageid_type id(void) const;

  /** Sets message instance identifier. 
   *
   *  @param mid Message instance identifier. If message identifiers are used, 
   *    unicomm takes care about identifiers. If identifiers are not used 
   *    by the engine you are able to specify the identifier in order to 
   *    distinct the message on sent event.
   *  
   *  @note You are also able to set this identifier manually for the message 
   *    when identifiers are enabled and used by the engine, but in this case 
   *    the uniqueness of assigned identifier is on you.
   */
  xml_message& id(messageid_type mid); 

  /** Returns message instance reply id. 
   *
   *  @return Message reply id. If message is reply and identifiers are 
   *    used by the engine this value is an id of the request message that 
   *    this message is reply to.
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
  xml_message& priority(size_t p);

  /** Returns message priority. 
   *
   *  @return Message priority.
   *  @see unicomm::min_priority(), unicomm::max_priority().
   */
  size_t priority(void) const;

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:
  /** Returns a reference to a data holder. 
   *
   *  @return Reference to internal @e smart::data::complex
   *    object that holds data.
   */
  smart::data::complex& complex(void) const;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  struct impl;

  typedef boost::shared_ptr<impl> impl_ptr;

private:
  impl_ptr _impl;
};

} // namespace unicomm

#endif // UNICOMM_USE_COMPLEX_XML

#endif // UNI_XML_MESSAGE_HPP_
