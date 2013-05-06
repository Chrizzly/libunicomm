///////////////////////////////////////////////////////////////////////////////
// client.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm client class definition. Allows to initiate connection.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_CLIENT_HPP_
#define UNI_CLIENT_HPP_

/** @file client.hpp Client definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/dispatcher.hpp>
#include <unicomm/session_base.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) //  warning C4267: 'argument' : conversion from 'size_t' to 'DWORD', possible loss of data
#endif // UNI_VISUAL_CPP

#include <boost/asio.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <vector>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Unicomm client.
 *
 *  Initiates connect and if connect is successful adds connection to processing.
 *
 *  @see unicomm::dispatcher, unicomm::server.
 */
class UNICOMM_DECL client : public dispatcher
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Constructs client object.
   *
   *  @param config Client configuration object.
   *  @note The copy of config is held by the client.
   *    TCP endpoint that client will connect to is taken from the configuration.
   */
  explicit client(const unicomm::config& config);

  /** Constructs client object.
   *
   *  @param config Client configuration object. 
   *  @param remote_endpoint Remote host end point to connect to.
   *    Overrides the configuration value.
   *
   *  @note The copy of config is held by the client. 
   */
  client(const unicomm::config& config, 
         const boost::asio::ip::tcp::endpoint &remote_endpoint);

  /** Constructs client object.
   *
   *  @param config Client configuration object. 
   *  @param error_handler Connect error handler. 
   *    It will be called if connect fails.
   *
   *  @note The copy of config is held by the client. 
   *    TCP endpoint that client will connect to is taken from the configuration.
   */
  client(const unicomm::config& config, 
         const connect_error_signal_type::slot_function_type& error_handler);

  /** Constructs client object.
   *
   *  @param config Client configuration object. 
   *  @param remote_endpoint Remote host end point to connect to.
   *    Overrides the configuration value.
   *
   *  @param error_handler Connect error handler. 
   *    It will be called if connect fails.
   *
   *  @note The copy of config is held by the client. 
   */
  client(const unicomm::config& config, 
         const boost::asio::ip::tcp::endpoint &remote_endpoint,
         const connect_error_signal_type::slot_function_type& error_handler);

  /** Stops dispatcher.
   *
   *  Waits for threads to exit unicomm::dispatcher::run() and destroys an object.
   */
  ~client(void);

public:
  /** @brief Request to connect using endpoint which is set on constructor 
   *    or previous connect call. 
   *  
   *  @param error_handler Connect error handler. 
   *    It will be called if connect fails.
   *    There is also possible to set default handler.
   *
   *  @see unicomm::client::add_connect_error_handler().
   */
  void connect(const connect_error_signal_type::slot_function_type& error_handler = 0);

  /** Sets endpoint that client to be connected to and tries to connect to it. 
   *
   *  It means if after disconnect unicomm::client::connect() (with only handler parameter) 
   *  is called, connection will be attempted to last used endpoint.
   * 
   *  @param remote_endpoint Remote host end point to connect to.
   *  @param error_handler Connect error handler. 
   *    It will be called if connect fails.
   *    There is also possible to set default handler.
   *
   *  @see unicomm::client::add_connect_error_handler().
   */
  void connect(const boost::asio::ip::tcp::endpoint& remote_endpoint, 
    const connect_error_signal_type::slot_function_type& error_handler = 0);

  /** Adds connection failure handler.
   *
   *  The sequence of handlers is called when the event occurs. 
   *
   *  @param handler Connection error handler.
   *  @return boost::signals2::connection.
   *  @note To remove handler use returned boost::signals2::connection. 
   *    On how to do this take a look at http://boost.org.
   */
  boost::signals2::connection add_connect_error_handler(
    const connect_error_signal_type::slot_type& handler);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  //////////////////////////////////////////////////////////////////////////
  // virtual stuff

  /** It's called when the successful connect occurs. 
   *
   *  Can be used to setup socket.
   *
   *  @param socket Socket of the connection.
   *  @note It should be <strong>no throw</strong>. 
   *    Debug asserts on throw. Release does nothing.
   */
  virtual void after_connect(tcp_socket_type& /*socket*/) { /* empty */ }

  //////////////////////////////////////////////////////////////////////////
  // connection error handling: types
  struct connect_error_info
  {
  //////////////////////////////////////////////////////////////////////////
  // public interface
  connect_error_info(const boost::asio::ip::tcp::endpoint& ep, 
    const connect_error_signal_type::slot_function_type& handler,
      const boost::system::error_code& ec = boost::system::error_code()):
    _endpoint(ep),
    _error_handler(handler),
    _error_code(ec)
  {
    // empty
  }

  const boost::asio::ip::tcp::endpoint& endpoint(void) const { return _endpoint; }
  const boost::system::error_code& error_code(void) const { return _error_code; }
  const connect_error_signal_type::slot_function_type& error_handler(void) const 
    { return _error_handler; }
  
  void error_code(const boost::system::error_code& ec) { _error_code = ec; }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
  private:
    boost::asio::ip::tcp::endpoint _endpoint;
    connect_error_signal_type::slot_function_type _error_handler;
    boost::system::error_code _error_code;
  };

  typedef std::vector<connect_error_info> connect_error_collection_type;

private:
  //////////////////////////////////////////////////////////////////////////
  // asio handlers
  void asio_connect_handler(const comm_ptr& client, connect_error_info& info, 
    const boost::system::error_code& error);

#ifdef UNICOMM_SSL

  void asio_handshake_handler(const comm_ptr& client, 
    connect_error_info& info, const boost::system::error_code& error);

#endif // UNICOMM_SSL

  //////////////////////////////////////////////////////////////////////////
  // aux
  void constructor(void);
  void constructor(const connect_error_signal_type::slot_function_type& error_handler);
  void initialize(void);
  void start_connect(const connect_error_signal_type::slot_function_type& error_handler);
  bool is_connect_error(void) const;

  //////////////////////////////////////////////////////////////////////////
  // core - processors
  void extra_process(void);
  void process_connect_error(void);

  //////////////////////////////////////////////////////////////////////////
  // virtual stuff
  //void before_start(void);
  void on_reset(void);

  //////////////////////////////////////////////////////////////////////////
  // handler callers
  void call_conn_error(const connect_error_info& info) /*const*/;
  // 
  void call_after_connect(tcp_socket_type& socket);

  //////////////////////////////////////////////////////////////////////////
  // connection error handling: routines
  void reg_conn_error(const connect_error_info& err_info);

private:
  connect_error_collection_type _conn_errors;
  connect_error_signal_type _conn_error_signal;
};

} // namespace unicomm


#endif // UNI_CLIENT_HPP_
