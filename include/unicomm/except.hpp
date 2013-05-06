///////////////////////////////////////////////////////////////////////////////
// except.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol exception classes definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_EXCEPT_HPP_
#define UNI_EXCEPT_HPP_

/** @file except.hpp Exception classes definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/comm.hpp>

#include <boost/system/system_error.hpp>

#include <string>
#include <stdexcept>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{
// fixme: Rename to except.hpp
//////////////////////////////////////////////////////////////////////////
// Communication errors

/** Disconnected error exception. 
 *
 *  Can be thrown from any allowed place (session event handlers) 
 *  to break the connection.
 *
 *  @see @ref events_subsection.
 */
class disconnected_error : public boost::system::system_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param err Disconnected reason.
   */
  explicit disconnected_error(const boost::system::error_code& err):
    boost::system::system_error(err)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit disconnected_error(const std::string& what):
    boost::system::system_error(boost::system::error_code(), what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param err Disconnected reason.
   *  @param what Extra error description.
   */
  explicit disconnected_error(const boost::system::error_code& err, 
                              const std::string& what):
    boost::system::system_error(err, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param err Disconnected reason.
   *  @param what Extra error description c-string.
   */
  explicit disconnected_error(const boost::system::error_code& err,
                              const char* what):
    boost::system::system_error(err, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   */
  explicit disconnected_error(int ev, const boost::system::error_category& ecat):
    boost::system::system_error(ev, ecat)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   *  @param what Extra error description.
   */
  explicit disconnected_error(int ev, const boost::system::error_category& ecat, 
                              const std::string& what):
    boost::system::system_error(ev, ecat, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   *  @param what Extra error description c-string.
   */
  explicit disconnected_error(int ev, const boost::system::error_category& ecat, 
                              const char* what):
    boost::system::system_error(ev, ecat, what)
  {
    // empty
  }
};

/** Communication error exception. */
class communication_error : public boost::system::system_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param err Error code information.
   */
  explicit communication_error(const boost::system::error_code& err):
    boost::system::system_error(err)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param err Error code information.
   *  @param what Extra error description.
   */
  explicit communication_error(const boost::system::error_code& err, 
                               const std::string& what):
    boost::system::system_error(err, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param err Error code information.
   *  @param what Extra error description c-string.
   */
  explicit communication_error(const boost::system::error_code& err, 
                               const char* what):
    boost::system::system_error(err, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   */
  explicit communication_error(int ev, const boost::system::error_category& ecat):
    boost::system::system_error(ev, ecat)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   *  @param what Extra error description.
   */
  explicit communication_error(int ev, const boost::system::error_category& ecat, 
                               const std::string& what):
    boost::system::system_error(ev, ecat, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   *  @param what Extra error description c-string.
   */
  explicit communication_error(int ev, const boost::system::error_category& ecat, 
                               const char* what):
    boost::system::system_error(ev, ecat, what)
  {
    // empty
  }
};

#ifdef UNICOMM_SSL

/** Handshake error exception. */
class handshake_error : public boost::system::system_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param err Error code information.
   */
  explicit handshake_error(const boost::system::error_code& err):
    boost::system::system_error(err)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param err Error code information.
   *  @param what Extra error description.
   */
  explicit handshake_error(const boost::system::error_code& err, 
                           const std::string& what):
    boost::system::system_error(err, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param err Error code information.
   *  @param what Extra error description c-string.
   */
  explicit handshake_error(const boost::system::error_code& err, 
                           const char* what):
    boost::system::system_error(err, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   */
  explicit handshake_error(int ev, const boost::system::error_category& ecat):
    boost::system::system_error(ev, ecat)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   *  @param what Extra error description.
   */
  explicit handshake_error(int ev, const boost::system::error_category& ecat, 
                           const std::string& what):
    boost::system::system_error(ev, ecat, what)
  {
    // empty
  }

  /** Constructs an object.
   *
   *  @param ev Error code value.
   *  @param ecat Error code category.
   *  @param what Extra error description c-string.
   */
  explicit handshake_error(int ev, const boost::system::error_category& ecat, 
                           const char* what):
    boost::system::system_error(ev, ecat, what)
  {
    // empty
  }
};

#endif // UNICOMM_SSL

/** Represents unicomm decoder error. 
 *
 *  Message decoder can throw this exception if an error encountered 
 *  while parsing incoming data. You are also able to use the exception 
 *  type you need. But note in the case of using non-standard exceptions 
 *  (not derived from std::exception) debug build will assert.
 *
 *  @see unicomm::message_decoder_base, unicomm::bin_message_decoder,
 *    unicomm::xml_message_decoder.
 */
class message_decoder_error : public std::runtime_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit message_decoder_error(const std::string& what): 
    std::runtime_error(what) { /* empty */ }
};

/** Disallowed reply received error. 
 *
 *  Thrown by unicomm if received reply is not allowed by the configuration.  
 *
 *  @see unicomm::is_allowed_reply(), unicomm::config::answers().
 */
class disallowed_reply_error : public std::runtime_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit disallowed_reply_error(const std::string& what): 
    std::runtime_error(what) { /* empty */ }
};

/** Invalid session factory error. 
 *
 *  If session factory method is not properly setup this exception is thrown.
 */
class invalid_session_factory : public std::logic_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit invalid_session_factory(const std::string& what): 
    std::logic_error(what) { /* empty */ }
};

/** Designates to user object creation failure. 
 *
 *  Any exception thrown by user session object constructor is 
 *  translated into this class.
 */
class session_creation_error : public std::runtime_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit session_creation_error(const std::string& what): 
    std::runtime_error(what) { /* empty */ }
};

/** Configuration error. 
 *
 *  @note Currently not used by unicomm.
 */
class config_error : public std::runtime_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit config_error(const std::string& what): 
    std::runtime_error(what) { /* empty */ }
};

/** Session not found error. 
 *
 *  If there is no session with specified identifier is found this
 *  exception is thrown. 
 */
class session_not_found : public std::runtime_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit session_not_found(const std::string& what): 
    std::runtime_error(what) { /* empty */ }
};

/** Invalid session error. 
 *
 *  If there the session is in invalid state this exception is thrown
 *  if session accessor is called.
 *
 *  @see unicomm::communicator::is_session_valid(), 
 *    unicomm::communicator::session().
 */
class invalid_session : public std::runtime_error
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param what Error description.
   */
  explicit invalid_session(const std::string& what): 
    std::runtime_error(what) { /* empty */ }
};

} // namespace unicomm

#endif // UNI_EXCEPT_HPP_
