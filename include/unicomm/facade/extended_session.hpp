///////////////////////////////////////////////////////////////////////////////
// extended_session.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm extended session. Allows to map different event handlers via 
// boost::signals and boost::function. Just an optional facade.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_EXTENDED_SESSION_HPP_
#define UNI_EXTENDED_SESSION_HPP_

/** @file extended_session.hpp Extended session that 
 *    allows to bind handlers to events. 
 *
 *  Allows to map different event handlers via boost::signals 
 *  and boost::function. 
 */

#include <unicomm/basic.hpp>
#include <unicomm/basic_session.hpp>
#include <unicomm/comm.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4512) // warning C4512: 'boost::function_output_iterator<UnaryFunction>::output_proxy' : assignment operator could not be generated
# pragma warning (disable : 4244) // warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
# pragma warning (disable : 4100) // warning C4100: 'is_group' : unreferenced formal parameter
#endif // UNI_VISUAL_CPP

#include <boost/system/error_code.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/mutex.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

#ifdef _MSC_VER
# pragma warning (disable : 4505)  // warning C4505: 'unicomm::extended_session<DerivedT>::connected_handler' : unreferenced local function has been removed
#endif // _MSC_VER

// forward
class communicator;

/** @brief Session class which allows to map callable objects to events. 
 *
 *  @tparam DerivedT Type that inherits extended_session.
 *  @tparam SessionParamsT Parameters type that will be passed to the
 *    session constructor. By default void_session_params is used.
 *
 *  @tparam BaseT Base type for the session. Default is session_base.
 */
template <
  typename DerivedT, 
  typename SessionParamsT = void_session_params, 
  typename BaseT = session_base>
class extended_session : public basic_session<DerivedT, SessionParamsT, BaseT>
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** This type. */
  typedef extended_session<DerivedT, SessionParamsT, BaseT> this_type;

  /** Smart pointer to a session. */
  typedef boost::shared_ptr<this_type> pointer_type;

public:
  /** Default constructor. */
  extended_session(void) { /* empty */ }

  /** Constructor that takes extra session parameters. 
   *
   *  @param extra_params Extra parameters having type SessionParamsT.
   */
  explicit extended_session(const SessionParamsT& extra_params): 
    basic_session<DerivedT, SessionParamsT, BaseT>(extra_params)
  {
    // empty
  }

public:
  /** Adds connected handler.
   *
   *  The sequence of handlers is called when the event occurs. 
   *
   *  @param handler Connected handler.
   *  @return An instance of boost::signals2::connection.
   *  @note To remove handler use returned boost::signals2::connection.
   *    More details on boost signals are available at http://boost.org.
   *
   *  @see session_base::connected_handler().
   */
  boost::signals2::connection 
  add_connected_handler(const connected_signal_type::slot_type& handler);

  /** Adds disconnected handler.
   *
   *  The sequence of handlers is called when the event occurs.
   *
   *  @param handler Disconnected handler.
   *  @return An instance of boost::signals2::connection.
   *  @note To remove handler use returned boost::signals2::connection.
   *    More details on boost signals are available at http://boost.org.
   *
   *  @see session_base::disconnected_handler().
   */
  boost::signals2::connection 
  add_disconnected_handler(const disconnected_signal_type::slot_type& handler);
  // fixme: Add remove connected/disconnected handler?

  /** Sets any message arrived handler. 
   *
   *  This handler is called when message is arrived.
   * 
   *  @param handler Message arrived handler.
   *  @see session_base::message_arrived_handler().
   */
  void set_any_message_arrived_handler(const message_arrived_handler_type& handler);

  /** Clears any message arrived handler. 
   *
   *  @see session_base::message_arrived_handler().
   */
  void clear_any_message_arrived_handler(void);

  /** Maps message arrived handler to given name.
   *
   *  @param name Message name to map handler to.
   *  @param handler Message arrived handler.
   *  @note When message is received mapped handler is called first if there is.
   *    After that any message arrived handler is called.
   *
   *  @see session_base::message_arrived_handler().
   */
  void set_message_arrived_handler(const std::string& name, 
                                   const message_arrived_handler_type& handler);

  /** Clears message arrived handler mapped to the given name. 
   *
   *  @param name Name (identifier) of the message to clear handler mapped to.
   *  @see session_base::message_arrived_handler().
   */
  void clear_message_arrived_handler(const std::string& name);

  /** Sets message sent handler. 
   *
   *  This handler is called after the handler mapped to specific
   *  message id is called.
   *
   *  @param handler Message sent handler.
   *  @note Sent handler can be mapped to certain message by using 
   *    unicomm::dispatcher::send_one() and unicomm::dispatcher::send_all().
   *
   *  @see session_base::message_sent_handler().
   */
  void set_message_sent_handler(const message_sent_handler_type& handler);

  /** Clears message sent handler. 
   *
   *  @see session_base::message_sent_handler().
   */
  void clear_message_sent_handler(void);

  /** Sets message timeout handler. 
   *
   *  If timeouts enabled this handler is called instead handler is ignored.
   *
   *  @param handler Message timeout handler.
   *  @see session_base::message_timeout_handler().
   */
  void set_message_timeout_handler(const message_timeout_handler_type& handler);

  /** Clears message timeout handler. 
   *
   *  @see session_base::message_timeout_handler().
   */
  void clear_message_timeout_handler(void);

  /** Sets error handler. 
   *
   *  If any error occurs in working thread this handler is called. 
   *  The handler is called from catch block, so it possible to 
   *  rethrow and catch the exception to obtain exception type information. 
   *
   *  @param handler Error handler.
   * 
   *  @see session_base::error_handler().
   */
  void set_error_handler(const error_handler_type& handler);

  /** Clears error handler. */
  void clear_error_handler(void);

  /** Sets after processed handler. 
   *
   *  Called when all the operations completed. 
   *  At the end of communicator::process().
   *
   *  @param handler After processed handler.
   *  @see session_base::after_processed_handler().
   */
  void set_after_processed_handler(const after_processed_handler_type& handler);

  /** Clears after processed handler. 
   *
   *  @see session_base::after_processed_handler().
   */
  void clear_after_processed_handler(void);

//////////////////////////////////////////////////////////////////////////
// protected interface
protected:
  //////////////////////////////////////////////////////////////////////////
  // virtual unicomm handlers, lowest level

  /** Connected handler. 
   *
   *  Calls connected signal.
   *
   *  @param params Connected handler parameters.
   *  @see session_base::connected_handler().
   */
  void connected_handler(const connected_params& params);

  /** Disconnected handler. 
   *
   *  @param params Disconnected handler parameters.
   *  @see session_base::disconnected_handler().
   */
  void disconnected_handler(const disconnected_params& params);

  /** Message arrived handler. 
   *
   *  @param params Message arrived handler parameters.
   *  @see session_base::message_arrived_handler().
   */
  void message_arrived_handler(message_arrived_params& params);

  /** Message is actually sent handler. 
   *
   *  @param params Message sent handler parameters.
   *  @see session_base::message_sent_handler().
   */
  void message_sent_handler(const message_sent_params& params);

  /** Message timeout handler. 
   *
   *  @param params Message timeout handler parameters.
   *  @see session_base::message_timeout_handler().
   */
  void message_timeout_handler(const message_timeout_params& params);

  /** Error handler. 
   *
   *  @param params Error handler parameters.
   *  @see session_base::error_handler().
   */
  void error_handler(const error_params& params);

  /** After processed handler. 
   *
   *  @param params After processed handler parameters.
   *  @see session_base::after_processed_handler().
   */
  void after_processed_handler(const after_processed_params& params);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Maps handler to the given message identifier. 
   *
   *  Register message sent handler with given message id. When message is sent
   *  this handler (if not null) called first, then called common message sent handler.
   *
   *  @param mid Message id to map the handler to.
   *  @param handler Message sent handler.
   */
  void reg_messsage_sent(const messageid_type& mid, 
    const message_sent_handler_type& handler);

private:
  //////////////////////////////////////////////////////////////////////////
  // aux
  const message_arrived_handler_type find_message_arrived_handler(
    const std::string& name) const;
  const message_sent_handler_type find_message_sent_handler(
    const messageid_type& mid);
  const message_arrived_handler_type get_any_message_arrived_handler(void) const;
  const message_sent_handler_type get_message_sent_handler(void) const;
  const message_timeout_handler_type get_message_timeout_handler(void);
  const error_handler_type get_error_handler(void) const;
  const after_processed_handler_type get_after_processed_handler(void) const;

  //////////////////////////////////////////////////////////////////////////
  // handler callers
  void call_message_arrived(message_arrived_params& params);
  void call_connected(const connected_params& params);
  void call_disconnected(const disconnected_params& params);
  void call_message_sent(const message_sent_params& params);
  void call_message_timeout(const message_timeout_params& params);
  void call_error(const error_params& params);
  void call_after_processed(const after_processed_params& params);

  //////////////////////////////////////////////////////////////////////////
  // misc
  void unreg_message_sent(const messageid_type& mid);
  bool is_message_sent_registered(messageid_type mid) const;

private:
  //////////////////////////////////////////////////////////////////////////
  // data
  //mutable boost::recursive_mutex _mutex; // 
  mutable boost::mutex _mutex;
  message_arrived_handlers_collection_type _mes_arrived_handlers;
  message_arrived_handler_type _any_mes_arrived_handler;
  message_sent_handlers_collection_type _sent_handlers;
  message_sent_handler_type _def_message_sent_handler;
  //message_timeout_handlers_collection_type _timeout_handlers;
  message_timeout_handler_type _def_message_timeout_handler;
  error_handler_type _err_handler;
  after_processed_handler_type _after_processed_handler;
  connected_signal_type _connected_signal;
  disconnected_signal_type _disconnected_signal;
};

} // namespace unicomm

#include "extended_session.ipp"

#endif // UNI_EXTENDED_SESSION_HPP_
