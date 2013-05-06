///////////////////////////////////////////////////////////////////////////////
// message_base.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol message base class definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_MESSAGE_BASE_HPP_
#define UNI_MESSAGE_BASE_HPP_

/** @file message_base.hpp Message base class definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/basic.hpp>

#include <smart/factory.hpp>

#include <boost/shared_ptr.hpp>

#include <string>

#include <cstddef>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Unicomm message base class.
 *
 *  Inherit this class or use already defined.
 *
 *  @see bin_message, xml_message.
 */
class message_base 
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Message smart pointer type. */
  typedef boost::shared_ptr<message_base> pointer_type;

  /** Messages factory type. */
  typedef smart::factory<message_base> factory_type;

  /** Message format identifier implemented by this class.
   *
   *  Message format is just a string (identifier) which tells the engine 
   *  to use predefined mechanisms or user implemented. Can have any value. 
   *  This is optional feature and only have effect with 
   *  unicomm::setup_message_format() and unicomm::load_from_complex_xml().
   *  
   *  @return Message format name this class is regarded to.
   *  @see setup_message_format(), load_from_complex_xml(),
   *    config::message_decoder(), config::message_encoder().
   */
  static const std::string& message_format(void) 
    { static const std::string s = "custom"; return s; }

public:
  /** Destroys an object. */
  virtual ~message_base(void) { /* empty */ }

public:
  /** Returns message instance identifier. 
   *
   *  If message identifiers are used, 
   *  unicomm takes care about identifiers. If identifiers are not 
   *  used by the engine you are able to specify the identifier 
   *  in order to distinct the message on sent event.
   *
   *  @return Message instance identifier. 
   *    By default returns undefined_messageid().
   *
   *  @see undefined_messageid(), config::use_unique_message_id().
   */
  virtual messageid_type id(void) const { return undefined_messageid(); }

  /** Sets message instance identifier. 
   *
   *  @param mid Message instance identifier. If message identifiers are used, 
   *    unicomm takes care about identifiers. If identifiers are not 
   *    used by the engine you are able to specify the identifier 
   *    in order to distinct the message on sent event.
   *  
   *  @return *this.
   *
   *  @note You are also able to set this identifier manually for the message when 
   *    identifiers are enabled and used by the engine, but in this case 
   *    the uniqueness of assigned identifier is on you.
   *
   *  @see undefined_messageid(), config::use_unique_message_id().
   */
  virtual message_base& id(messageid_type /*mid*/) { return *this; } 

  /** Returns message instance reply id. 
   *
   *  @return Message reply id. If message is reply and identifiers are used 
   *    by the engine this value is an id of the request message what this 
   *    message is reply to. By default returns unicomm::undefined_messageid().
   *
   *  @see unicomm::undefined_messageid().
   */
  virtual messageid_type rid(void) const { return undefined_messageid(); }

  /** Sets message instance reply id. 
   *
   *  @param mid Reply message instance identifier.
   */
  virtual void rid(messageid_type) { ; }

  /** Returns message name. 
   *
   *  Message name is message type identifier. In general this should be
   *  overridden to return a reference to specific message name string.
   * 
   *  @return Returns message name.
   */
  virtual const std::string& name(void) const 
    { static const std::string s; return s; }

  /** Whether id exists. 
   *
   *  It's assumed that value undefined_messageid() means unidentified instance.
   *
   *  @return True if message_base::id() returns value different from
   *    undefined_messageid() otherwise returns false.
   * 
   *  @see undefined_messageid().
   */
  virtual bool id_exists(void) const { return id() != undefined_messageid(); }

  /** Whether rid exists. 
   *
   *  Existence of reply id means that the message is reply to a 
   *  message with message_base::id() equal to this 
   *  message_base::rid() value.
   *
   *  @return True if message_base::rid() differs from 
   *    undefined_messageid().
   *
   *  @see undefined_messageid().
   */
  virtual bool rid_exists(void) const { return rid() != undefined_messageid(); }

  /** Serializes message to a std::string. 
   *
   *  @return Standard string containing serialized message data.
   *  @see unicomm::message_base::unserialize().
   */
  virtual std::string serialize(void) const = 0;

  /** Restores message from a std::string. 
   *
   *  @param message Raw data message string.
   *  @see unicomm::message_base::serialize().
   */
  virtual void unserialize(const std::string &message) = 0;

  /** Sets message priority. 
   *
   *  @param p Message priority.
   *  @return *this.
   *  @see unicomm::min_priority(), unicomm::max_priority(),
   *    unicomm::undefined_priority().
   */
  virtual message_base& priority(size_t /*p*/) { return *this; }

  /** Returns message priority. 
   *
   *  @return Message priority.
   *  @see unicomm::min_priority(), unicomm::max_priority(),
   *    unicomm::undefined_priority().
   */
  virtual size_t priority(void) const { return undefined_priority(); }
};

/** Reads predefined id property. 
 *
 *  @return Message instance identifier contained by the specified message.
 *  @see unicomm::message_base::id().
 */
inline messageid_type get_id(const message_base& m)
{
  return m.id();
}

/** Assigns id to a message object. 
 *
 *  @param m Message the identifier to be set to.
 *  @param id Message instance identifier value.
 *  @return Reference to m.
 *  @see unicomm::message_base::id().
 */
inline message_base& set_id(message_base& m, messageid_type id)
{
  return m.id(id);
}

/** Reads reply id property. 
 *
 *  @param m Message instance to get reply id of.
 *  @return Reply id property contained by the specified message.
 *  @see unicomm::message_base::rid().
 */
inline messageid_type get_rid(const message_base& m)
{
  return m.rid();
}

/** Sets reply id property to the message object. 
 *
 *  @param m Message instance to set the reply id.
 *  @param rid Reply id to be assigned to the message.
 *  @see unicomm::message_base::rid().
 */
inline void set_rid(message_base& m, messageid_type rid)
{
  m.rid(rid); 
}

/** Reads name property. 
 *
 *  @param m Message instance.
 *  @return name property contained by the specified message.
 *  @see unicomm::message_base::name().
 */
inline const std::string& get_name(const message_base& m)
{
  return m.name();
}

/** Whether id property exists. 
 *
 *  @param m Message to test.
 *  @return True if identifier exists otherwise return false.
 *  @see unicomm::message_base::id_exists().
 */
inline bool id_exists(const message_base& m)
{
  return m.id_exists();
}

/** Whether rid property exists. 
 * 
 *  @param m Message to test.
 *  @return True if identifier exists otherwise return false.
 *  @see unicomm::message_base::rid_exists().
 */
inline bool rid_exists(const message_base& m)
{
  return m.rid_exists();
}

/** Whether message is reply to an another message. 
 *
 *  @param m Message to be tested.
 *  @return True if message is reply to another message.
 */
inline bool is_reply(const message_base& m)
{
  return rid_exists(m);
}

/** Returns message priority. 
 *
 *  @param m Message to read the priority of.
 *  @return Message priority.
 *  @see unicomm::message_base::priority().
 */
inline size_t get_priority(const message_base& m)
{
  return m.priority();
}

/** Sets message priority. 
 *
 *  @param m Message to assign the priority to.
 *  @param priority Priority value being assigned.
 *  @return *this.
 *  @see unicomm::message_base::priority().
 */
inline message_base& set_priority(message_base& m, size_t priority)
{
  return m.priority(priority);
}

} // namespace unicomm

#endif // UNI_MESSAGE_BASE_HPP_
