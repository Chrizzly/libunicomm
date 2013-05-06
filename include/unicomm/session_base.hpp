///////////////////////////////////////////////////////////////////////////////
// session_base.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm session base class definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_SESSION_BASE_HPP_
#define UNI_SESSION_BASE_HPP_

/** @file session_base.hpp Session base class definition. */

#include <unicomm/message_base.hpp>
#include <unicomm/basic.hpp>
#include <unicomm/handler_params.hpp>

#include <boost/shared_ptr.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** @brief Session interface which should be implemented 
 *    by user's code to receive event notifications. 
 *
 *  This is base class for user's objects. It receives different 
 *  notifications about the communication process. 
 *  This is the place where the business logic should live.
 *
 *  @note Throwing an exception from any allowed handler interrupts current 
 *    process iteration for this session object. An attempt to call left
 *    handlers will take place on next process iteration.
 *  
 *  @see @ref sessions_section, unicomm::extended_session, 
 *    unicomm::basic_session.
 */
class UNICOMM_DECL session_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Smart pointer to a session. */
  typedef boost::shared_ptr<session_base> pointer_type;

  /** Session factory type. */
  typedef boost::function<pointer_type (const connected_params&)> factory_type;

public:
  /** Empty virtual destructor. */
  virtual ~session_base(void) { /* empty */ }

public:
  /** Returns an indication that the session is on server side. 
   *
   *  @return By default always return true, override it if necessary. 
   *  @note To distinct the side in runtime should be overridden by 
   *    only descendant client side entity. It's assumed by default 
   *    the session regards to server side. This flag can be useful
   *    when implementing message decoder.
   *  
   *  @see message_decoder_base::perform_decode();
   */
  virtual bool is_server(void) const { return true; }

public:
  /** Actually calls virtual message arrived handler. 
   *
   *  @param params Message arrived handler parameters.
   */
  void signal_message_arrived(message_arrived_params& params);
  
  /** Calls message timeout handler. 
   *
   *  @param params Message timeout handler parameters.
   */
  void signal_message_timeout(const message_timeout_params& params);

  /** Calls client connected handler. 
   *
   *  @param params Connected handler parameters.
   */
  void signal_connected(const connected_params& params);

  /** Calls client disconnected handler. 
   *
   *  @param params Disconnected handler parameters.
   */
  void signal_disconnected(const disconnected_params& params);

  /** Calls error handler. 
   *
   *  @param params Error handler parameters.
   */
  void signal_error(const error_params& params);

  /** Calls message sent handler. 
   *
   *  @param params Message sent handler parameters.
   */
  void signal_message_sent(const message_sent_params& params);

  /** Calls after processed handler. 
   *
   *  @param params After processed handler parameters.
   */
  void signal_after_processed(const after_processed_params& params);

public:
  // fixme: resolve via friend declarations

  /** Maps handler to the given message identifier. 
   *
   *  If you are not intended to use handlers mapped to certain message and 
   *  called when this message is sent just leave it empty.
   * 
   *  @see unicomm::extended_session.
   */
  virtual void reg_messsage_sent(const messageid_type& /*mid*/, 
    const message_sent_handler_type& /*handler*/) { /* empty */ }

//////////////////////////////////////////////////////////////////////////
// protected interface
protected:
  //////////////////////////////////////////////////////////////////////////
  // event handlers

  /** Message arrived handler. 
   *
   *  Override this to get the notification when the message is arrived.
   *
   *  @param params Message arrived handler parameters.
   *  @note This handler allows to throw any exception types. But exception
   *    not derived from std::exception causes debug will assert.
   *    To perform disconnect on this session throw unicomm::disconnected_error 
   *    from the handler or call params.comm().disconnect().
   *
   *  @see @ref sessions_section, unicomm::extended_session, 
   *    unicomm::disconnected_error, unicomm::communicator::disconnect().
   */
  virtual void message_arrived_handler(message_arrived_params&) { /* empty */ } // = 0;

  /** Connected handler. 
   *
   *  There are two ways to track the connection establishing. The first
   *  is to use native C++ session object's constructor. The second is to 
   *  override connected handler. When the connection either succeeded 
   *  or connection is accepted as incoming, at first, the session's 
   *  constructor is called. Connected handler is called immediately after 
   *  the constructor exits.
   *  
   *  It is possible to throw any type exceptions from the session's
   *  constructor, but throwing not derived from std::exception causes debug 
   *  asserts. The exception from constructor will brakes the connection 
   *  immediately. 
   *
   *  @param param Connected handler parameters.
   *  @note This handler allows to throw any exception types. But exception
   *    not derived from std::exception causes debug will assert.
   *    To perform disconnect on this session throw unicomm::disconnected_error 
   *    from the handler or call params.comm().disconnect().
   * 
   *  @see @ref sessions_section, unicomm::extended_session, 
   *    unicomm::disconnected_error, unicomm::communicator::disconnect().
   */
  virtual void connected_handler(const connected_params&) { /* empty */ } // = 0;

  /** Disconnected handler. 
   *
   *  There are two ways to track the connection braking. The first
   *  is to use native C++ session object's destructor (no parameters). 
   *  The second is to override disconnected handler.
   *
   *  @param param Disconnected handler parameters.
   *  @note It doesn't matter connection is closed by other side or 
   *    disconnect is made on this side, this handler will be called anyway.
   *    <br/>This handler does <strong>not</strong> allow to throw exceptions. 
   *    Exceptions thrown from this handler causes debug will assert. 
   *    Release does nothing.
   *
   *  @see @ref sessions_section, unicomm::extended_session.
   */
  virtual void disconnected_handler(const disconnected_params&) { /* empty */ } // = 0;

  /** Message timeout handler. 
   *
   *  If message reply is not got within a timeout this handler is invoked.
   *
   *  @param param Message timeout handler parameters.
   *  @note This handler allows to throw any exception types. But exception
   *    not derived from std::exception causes debug will assert.
   *    To perform disconnect on this session throw unicomm::disconnected_error 
   *    from the handler or call params.comm().disconnect().
   *
   *  @see @ref sessions_section, unicomm::extended_session, 
   *    unicomm::disconnected_error, unicomm::communicator::disconnect().
   */
  virtual void message_timeout_handler(const message_timeout_params&) 
    { /* empty */ } // = 0;

  /** Error handler. 
   *
   *  All the none unicomm exceptions thrown from any handler allows 
   *  the exception throwing leads error handler to be called. 
   *
   *  @param param Error handler parameters.
   *  @note This handler does <strong>not</strong> allow to throw exceptions. 
   *    Exceptions thrown from this handler causes debug will assert. 
   *    Release does nothing. The handler is called from catch block. 
   *    To obtain the exception type rethrow the exception inside try-catch. 
   *    To disconnect the session use params.comm().disconnect().
   *
   *  @see @ref sessions_section, unicomm::extended_session, 
   *    unicomm::communicator::disconnect().
   */
  virtual void error_handler(const error_params&) { /* empty */ } // = 0;

  /** Message is actually sent handler. 
   *
   *  When one of the send() functions is called the message is just put into 
   *  outgoing message queue and sent as soon as possible by the 
   *  dispatcher's thread. To be really sure that message is sent 
   *  override this function. 
   *
   *  @param param Message sent handler parameters.
   *  @note This handler allows to throw any exception types. But exception
   *    not derived from std::exception causes debug will assert.
   *    To perform disconnect on this session throw unicomm::disconnected_error 
   *    from the handler or call params.comm().disconnect().
   *
   *  @see @ref sessions_section, disconnected_error, communicator::disconnect(),
   *    send_one(), send_all(), dispacher::send_one(), dispacher::send_all().
   */
  virtual void message_sent_handler(const message_sent_params&) { /* empty */ } // = 0;

  /** After processed handler. 
   *
   *  This is the last handler to be invoked on current process iteration.
   *  
   *  @param param After process handler parameters.
   *  @note This handler allows to throw any exception types. But exception
   *    not derived from std::exception causes debug will assert.
   *    To perform disconnect on this session throw unicomm::disconnected_error 
   *    from the handler or call params.comm().disconnect().
   *
   *  @see @ref sessions_section, extended_session, 
   *    disconnected_error, communicator::disconnect().
   */
  virtual void after_processed_handler(const after_processed_params&) 
    { /* empty */ } // = 0;
};

// fixme: make unregistered message arrived handler?

} // namespace unicomm

#endif // UNI_SESSION_BASE_HPP_
