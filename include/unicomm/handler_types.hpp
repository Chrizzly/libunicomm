///////////////////////////////////////////////////////////////////////////////
// handler_types.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Handler types definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HANDLER_TYPES_HPP_
#define UNI_HANDLER_TYPES_HPP_

/** @file handler_types.hpp Handler types definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/basic.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4512) // warning C4512: 'boost::function_output_iterator<UnaryFunction>::output_proxy' : assignment operator could not be generated
# pragma warning (disable : 4244) // warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
# pragma warning (disable : 4100) // warning C4100: 'is_group' : unreferenced formal parameter
#endif // UNI_VISUAL_CPP

#include <boost/system/error_code.hpp>
#include <boost/signals2.hpp>
#include <boost/function.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <boost/asio/ip/tcp.hpp>

#include <map>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class params_base;
class connected_params;
typedef params_base after_processed_params;
class disconnected_params;
class message_sent_params;
typedef message_sent_params message_timeout_params;
class error_params;
class message_arrived_params;
class connect_error_params;
class after_all_processed_params;

/** Connection successfully established handler type. 
 *
 *  This is the session "constructor". 
 *
 *  @note It is possible to throw derived from std::exception. If thrown object
 *    is not derived from std::exception debug will assert, release does nothing.
 * 
 *  @see unicomm::connected_params, @ref sessions_section.
 */
typedef boost::signals2::signal<void (const connected_params&)> 
  connected_signal_type;

/** Disconnected handler type. 
 *  
 *  This is the session "destructor". Second parameter means the disconnect reason.
 * 
 *  @note If handler throws debug build will assert, release does nothing.
 *
 *  @see unicomm::disconnected_params, @ref sessions_section.
 */
typedef boost::signals2::signal<void (const disconnected_params&)> 
  disconnected_signal_type;

/** Message arrived handler type. 
 *
 *  @note It is possible to throw classes derived from std::exception.
 *    Exceptions not derived from std::exception causes debug will assert.
 * 
 *  @see unicomm::message_arrived_params, @ref sessions_section.
 */
typedef boost::function<void (message_arrived_params& params)> 
  message_arrived_handler_type;

/** Message handlers collection type. 
 *
 *  @see unicomm::message_arrived_handler_type.
 */
typedef std::map<std::string, message_arrived_handler_type> 
  message_arrived_handlers_collection_type;

/** Message is actually sent handler type. 
 *
 *  When unicomm::dispatcher::send_one() or unicomm::dispatcher::send_all() 
 *  is called the message is put into outgoing message queue and 
 *  sent as soon as possible by the dispatcher thread.
 *  So, to be really sure that message is sent override (unicomm::session_base) 
 *  this function or set the handler (unicomm::extended_session). 
 *
 *  @note It is possible to throw classes derived from std::exception.
 *    Exception causes sent handlers' call loop to be interrupted. An attempt 
 *    to call left handlers (uncalled) will be issued at the next 
 *    process iteration.
 *
 *  @see unicomm::message_sent_params, @ref sessions_section.
 */
typedef boost::function<void (const message_sent_params&)> 
  message_sent_handler_type;

/** Message timeout handler function type. 
 *
 *  @see unicomm::message_timeout_params, @ref sessions_section.
 */
typedef boost::function<void (const message_timeout_params&)> 
  message_timeout_handler_type;
 
/** Error handler type.
 *
 *  @see unicomm::error_handler_type, @ref sessions_section.
 */
typedef boost::function<void (const error_params&)> error_handler_type;

/** After processed handler type. 
 *
 *  @see unicomm::after_processed_params, @ref sessions_section.
 */
typedef boost::function<void (const after_processed_params&)> 
  after_processed_handler_type;

/** After all processed handler type. 
 *
 *  This handler is called at the end of each unicomm::dispatcher::process()
 *  iteration. If nothing to do the handler is called approximately within
 *  unicomm::config::dispatcher_idle_tout() period.
 *
 *  @note Exception thrown from this handler causes assert in debug. 
 *    Release does nothing.
 *
 *  @see @ref sessions_section, unicomm::config::dispatcher_idle_tout().
 */
typedef boost::function<void (const after_all_processed_params&)> 
  after_all_processed_handler_type;

/** Message sent handlers collection type. 
 *
 *  @see unicomm::message_sent_handler_type.
 */
typedef std::map<messageid_type, message_sent_handler_type> 
  message_sent_handlers_collection_type;

/** Message timeout handlers collection type. 
 *
 *  @see unicomm::message_timeout_handler_type.
 */
typedef std::map<messageid_type, message_timeout_handler_type> 
  message_timeout_handlers_collection_type;

/** Connection failure handler type. 
 *
 *  @see unicomm::connect_error_params, @ref sessions_section.
 */
typedef boost::signals2::signal<void (const connect_error_params&)> 
  connect_error_signal_type;

} // namespace unicomm

#endif // UNI_HANDLER_TYPES_HPP_
