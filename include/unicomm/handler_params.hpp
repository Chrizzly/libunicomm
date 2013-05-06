///////////////////////////////////////////////////////////////////////////////
// handler_params.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Handler parameter types definitions.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HANDLER_PARAMS_HPP_
#define UNI_HANDLER_PARAMS_HPP_

/** @file handler_params.hpp Handler parameter types definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/comm_buffer.hpp>
#include <unicomm/handler_types.hpp>
#include <unicomm/basic.hpp>

#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class communicator;

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4512) // warning C4512: 'unicomm::params_base' : assignment operator could not be generated
#endif // UNI_VISUAL_CPP

/** Parameters base.
 *
 *  This is base class for any handler parameters except
 *  connect_error_params and after_all_processed_params.
 *  Except mentioned class any parameters have a reference
 *  to the communicator object that generated the event.
 */
class params_base
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Constructs a unicomm::params_base object.
   *
   *  @param comm Reference to communicator that generated the event.
   */
  explicit params_base(const communicator& comm): _comm(comm) { /* empty */ }

  /** Destroys an object. */
  virtual ~params_base(void) { /* empty */ }

public:
  /** Returns an object responsible for communication.
   *
   *  @return Communicator object that generated the event.
   */
  const communicator& comm(void) const { return _comm; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  const communicator& _comm;
};

/** Connected handler parameters.
 *
 *  @see unicomm::connected_signal_type.
 */
class connected_params : public params_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param comm Reference to communicator that generated the event.
   *  @param in_buffer Communicator incoming raw buffer. This parameter
   *    can be stored on connect and used later when it's necessary,
   *    e.g. clear buffer for some reason or something else.
   */
  connected_params(const communicator& comm, comm_buffer& in_buffer):
    params_base(comm),
    _in_buffer(&in_buffer)
  {
    // empty
  }

public:
  /** Returns a reference to internal communicator incoming raw buffer.
   *
   *  @return Reference to communicator incoming raw buffer. This parameter
   *    can be stored on connect and used later when it's necessary,
   *    e.g. clear buffer for some reason or something else.
   */
  comm_buffer& in_buffer(void) const { return *_in_buffer; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  comm_buffer* _in_buffer;
};

/** After processed handler parameters type.
 *
 *  @see unicomm::after_processed_handler_type.
 */
typedef params_base after_processed_params;

namespace detail
{

/** Auxiliary params holder which holds one argument. */
template <typename ArgT>
class params_arg_1 : public params_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param comm Reference to communicator that generated the event.
   *  @param arg An argument to store.
   */
  params_arg_1(const communicator& comm, const ArgT& arg):
    params_base(comm),
    _arg(arg)
  {
    // empty
  }

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:
  /** Returns parameters argument.
   *
   *  @return Stored argument.
   */
  const ArgT& argument(void) const { return _arg; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  ArgT _arg;
};

/** Auxiliary params holder which holds two arguments. */
template <typename Arg1T, typename Arg2T>
class params_arg_2 : public params_base
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Constructs an object.
   *
   *  @param comm Reference to communicator that generated the event.
   *  @param arg1 A first argument to store.
   *  @param arg2 A second argument to store.
   */
  params_arg_2(const communicator& comm, const Arg1T& arg1, const Arg2T& arg2):
    params_base(comm),
    _arg1(arg1),
    _arg2(arg2)
  {
    // empty
  }

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:
  /** Returns first argument.
   *
   *  @return First argument.
   */
  const Arg1T& argument1(void) const { return _arg1; }

  /** Returns second argument.
   *
   *  @return Second argument.
   */
  const Arg2T& argument2(void) const { return _arg2; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  Arg1T _arg1;
  Arg2T _arg2;
};

} // namespace detail

/** Disconnected handler parameters.
 *
 *  @see unicomm::disconnected_signal_type.
 */
class disconnected_params :
  public detail::params_arg_2<boost::system::error_code, std::string>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param comm Reference to communicator that generated the event.
   *  @param code Error code that the information about the error is accessible through.
   *  @param what Additional error description.
   */
  disconnected_params(const communicator& comm,
                      const boost::system::error_code& code,
                      const std::string& what):
    detail::params_arg_2<boost::system::error_code, std::string>(comm, code, what)
  {
    // empty
  }

public:
  /** Returns a disconnected reason.
   *
   *  @return Disconnected reason.
   */
  const boost::system::error_code& error_code(void) const { return argument1(); }

  /** Returns a disconnected reason string.
   *
   *  @return Additional disconnected reason string.
   */
  const std::string& what(void) const { return argument2(); }
};

/** Message sent handler parameters.
 *
 *  @see unicomm::message_sent_handler_type.
 */
class message_sent_params : public detail::params_arg_1<messageid_type>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param comm Reference to communicator that generated the event.
   *  @param mid Sent message's identifier.
   */
  message_sent_params(const communicator& comm, messageid_type mid):
    detail::params_arg_1<messageid_type>(comm, mid)
  {
    // empty
  }

public:
  /** Returns a sent message's identifier.
   *
   *  @return Sent message's identifier.
   */
  messageid_type message_id(void) const { return argument(); }
};

/** Message timeout handler parameters.
 *
 *  @see unicomm::message_timeout_handler_type.
 */
typedef message_sent_params message_timeout_params;

/** Error handler parameters.
 *
 *  @see unicomm::error_handler_type.
 */
class error_params : public detail::params_arg_1<std::string>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object.
   *
   *  @param comm Reference to communicator that generated the event.
   *  @param what Error description string.
   */
  error_params(const communicator& comm, const std::string& what):
    detail::params_arg_1<std::string>(comm, what)
  {
    // empty
  }

public:
  /** Returns an error description.
   *
   *  @return An error description string.
   */
  const std::string& description(void) const { return argument(); }
};

/** Message arrived handler parameters class.
 *
 *  @see unicomm::message_arrived_handler_type.
 */
class message_arrived_params : public params_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Creates message arrived parameters object.
   *
   *  @param comm Communicator object which message is arrived to.
   *  @param inm Arrived message.
   *  @param out_mid Outgoing message identifier would be assigned to
   *    outgoing reply message if reply is specified by handler.
   *    It's the same as returned by any send function.
   *    This value can be stored and father used
   *    in sent and timeout handlers to identify the message which handler
   *    is called for.
   */
  message_arrived_params(const communicator& comm,
                         const message_base& inm, messageid_type out_mid):
    params_base(comm), _inm(inm), _outmesid(out_mid) { /* empty */ }

public:
  /** Returns reference to the arrived message.
   *
   *  @return A reference to the arrived message.
   */
  const message_base& in_message(void) const { return _inm; }

  /** Returns pointer to outgoing reply.
   *
   *  @return Smart pointer to reply message to be sent.
   *  @note The reply is only sent if the out_message() is not null.
   */
  const message_base::pointer_type& out_message(void) const { return _outm; }

  /** Returns message sent handler object.
   *
   *  It's called when outgoing reply message is actually sent.
   *
   *  @return Message sent handler.
   */
  const message_sent_handler_type& get_message_sent_handler(void) const
    { return _mes_sent_handler; }

  /** Sets message sent handler for this outgoing message reply instance.
   *
   *  @param handler Message sent handler to be mapped to the reply message.
   *  @note This handler is only set for this outgoing message reply
   *    instance and called when the message is sent.
   */
  void set_message_sent_handler(const message_sent_handler_type& handler)
    { _mes_sent_handler = handler; }

  /** Outgoing reply message identifier if reply is specified by the handler.
   *
   *  @return Outgoing message identifier would be assigned to outgoing reply
   *    message if reply is specified by handler. It's the same as returned
   *    by any send function. This value can be stored and father used
   *    in sent and timeout handlers to identify the message which handler
   *    is called for.
   *
   *  @see unicomm::dispatcher::send_one(), unicomm::dispatcher::send_all(),
   *    unicomm::send_one(), unicomm::send_all().
   */
  unicomm::messageid_type out_message_id(void) const { return _outmesid; }

  /** Sets outgoing message reply.
   *
   *  @param m Smart pointer to the reply message to be sent when
   *    message arrived handler finishes invoking.
   */
  void out_message(const message_base::pointer_type& m) { _outm = m; }

//////////////////////////////////////////////////////////////////////////
// message arrived params private stuff
private:
  const message_base& _inm;
  message_base::pointer_type _outm;
  unicomm::messageid_type _outmesid;
  message_sent_handler_type _mes_sent_handler;
};

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

class client;

/** Connection failure parameters class.
 *
 *  This stuff regards only to unicomm::client. It is passed to connection
 *  error handler after client::connect() has been called
 *  in case of connection establishing failure.
 *
 *  @see unicomm::connect_error_signal_type.
 */
class connect_error_params
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Creates an object.
   *
   *  @param client A reference to the unicomm::client object produced the event.
   *  @param remote_endpoint Remote end point that connection was attempted
   *    and finished with error.
   *
   *  @param error_code A reason of failure.
   */
  connect_error_params(unicomm::client& client,
                       const boost::asio::ip::tcp::endpoint& remote_endpoint,
                       const boost::system::error_code& error_code):
    _client(&client),
    _remote_ep(remote_endpoint),
    _err_code(error_code)
  {
    // empty
  }

  /** Returns a reference to the client object produced the event.
   *
   *  @return A reference to the client object produced the event.
   */
  unicomm::client& client(void) const { return *_client; }

  /** Returns remote end point that connection was attempted.
   *
   *  @return Remote end point which was tried to connect to.
   */
  const boost::asio::ip::tcp::endpoint& remote_endpoint(void) const
    { return _remote_ep; }

  /** Returns an error code of the connection attempt.
   *
   *  @return Error code which the attempt was finished.
   */
  const boost::system::error_code& error_code(void) const { return _err_code; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  unicomm::client* _client;
  boost::asio::ip::tcp::endpoint _remote_ep;
  boost::system::error_code _err_code;
};

// forward
class dispatcher;

/** After all processed parameters class.
 *
 *  Passed to the after all processed handler specified by user's code.
 *
 *  @see unicomm::after_all_processed_handler_type.
 */
class after_all_processed_params
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Creates an object.
   *
   *  @param manager A reference to the dispatcher object produced
   *    the event.
   */
  explicit after_all_processed_params(dispatcher& manager):
    _comm_man(&manager)
  {
    // empty
  }

  /** Returns a reference to the dispatcher object produced the event.
   *
   *  @return A reference to the dispatcher object produced the event.
   */
  dispatcher& manager(void) const { return *_comm_man; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  dispatcher* _comm_man;
};

} // namespace unicomm

#endif // UNI_HANDLER_PARAMS_HPP_
