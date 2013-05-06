///////////////////////////////////////////////////////////////////////////////
// server.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol server defintion. 
// Adds connection accepting functionality.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_SERVER_HPP_
#define UNI_SERVER_HPP_

/** @file server.hpp Server definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/dispatcher.hpp>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Unicomm server.
 *
 *  Accepts incoming connection requests and manages established connections.
 *
 *  @see dispatcher, client.
 */
class UNICOMM_DECL server : public dispatcher
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs server object.
   *
   *  By default listen address is any.
   *  
   *  @param config Server configuration object.
   *
   *  @note The copy of config is held.
   */
  explicit server(const unicomm::config& config);

  /** Constructs server object.
   *  
   *  @param config Server object configuration. 
   *  @param listen_endpoint ip-address and port to be listened to.
   *    Overrides configuration values.
   *
   *  @note The copy of config is held.
   */
  server(const unicomm::config& config, 
    const boost::asio::ip::tcp::endpoint &listen_endpoint);

  /** Stops dispatcher and destroys an object. */
  ~server(void);

public:
  // fixme: Call this from within connected (accepted) handler to 
  // continue accepting connections.

  /** Tells the underlying logic to perform one accept operation on the socket. 
   *
   *  Should be called once to start accepting incoming connections.
   * 
   *  @see connected_signal_type, session_base::connected_handler().
   */
  void accept(void);

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:
  // fixme: Synchronize access to the acceptor

  /** Returns whether acceptor is valid.
   *
   *  @return True if acceptor is valid and unicomm::dispatcher::acceptor() 
   *    can be safely called otherwise false.
   *
   *  @note @b IMPORTANT! Should not be used in this revision.
   */
  bool is_acceptor(void) const;

  /** @brief Returns a reference to the entity responsible for the 
   *    accepting incoming connections.
   *
   *  @return A reference to a tcp::acceptor.
   *  @note @b IMPORTANT! Should not be used in this revision.
   */ 
  boost::asio::ip::tcp::acceptor& acceptor(void);

  /** Actually creates listening socket.
   *
   *  It only effects if unicomm::server::is_acceptor() returns false.
   *
   *  @note @b IMPORTANT! Should not be used in this revision.
   */
  void start_accept(void);

  /** Actually destroys listening socket.
   *
   *  @note Call to unicomm::server::acceptor() after this leads to undefined 
   *    behavior. Doesn't stop processing of the existent connections. 
   *    Can be used to stop accepting incoming connections due to some reason. 
   *    To start accepting it's necessary to call unicomm::server::start_accept().
   *
   *  @note @b IMPORTANT! Should not be used in this revision.
   */
  void stop_accept(void);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  //////////////////////////////////////////////////////////////////////////
  // virtual stuff 

  /** Called by unicomm before working thread actually started. 
   *
   *  This allows you to apply necessary tcp options to the listening
   *  socket before start accepting clients and also perform necessary
   *  initialization.
   *
   *  @param acceptor tcp acceptor (listening socket) to be setup by derived
   *    implementation.
   *
   *  @note Actually called within unicomm::dispatcher::start().
   */
  //virtual void before_start(boost::asio::ip::tcp::acceptor& /*acceptor*/) 
  //  { /* empty */ }

  /** It's called when the successful accept occurs. 
   *
   *  Can be used to setup socket.
   *
   *  @param socket Socket of the accepted connection.
   *  @note It should be <strong>no throw</strong>. 
   *    Debug asserts on throw. Release does nothing.
   */
  virtual void after_accept(tcp_socket_type& /*socket*/) { /* empty */ }

private:
  //////////////////////////////////////////////////////////////////////////
  // virtual inherited stuff 
  //void before_start(void);
  void on_reset(void);
  void on_stop(void);

private:
  //////////////////////////////////////////////////////////////////////////
  // thread safe interface to callbacks
  after_all_processed_handler_type after_all_processed_handler(void) const;

  void after_all_processed_handler(const after_all_processed_handler_type& fn);

private:
  //////////////////////////////////////////////////////////////////////////
  // misc
  //bool is_acceptor(void) const;
  void constructor(void);
  void initialize(void);
  void finalize(void);
  void just_start_accept(void);
  void create_listening_socket(void);
  void create_acceptor(void);
  void open_acceptor(void);
  void close_acceptor(void);
  void destroy_acceptor(void);
  bool is_acceptor_opened(void) /*const*/ { return acceptor().is_open(); }
  //
  void call_after_accept(tcp_socket_type& socket);

private:
  //////////////////////////////////////////////////////////////////////////
  // boost asio handlers
  void asio_accept_handler(const comm_ptr& client, 
    const boost::system::error_code& error);

#ifdef UNICOMM_SSL

  void asio_handshake_handler(const comm_ptr& client, 
    const boost::system::error_code& error);

#endif // UNICOMM_SSL

private:
  typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr_type;

private:
  // listening stuff
  acceptor_ptr_type _acceptor;
};

} // namespace unicomm

#endif // UNI_SERVER_HPP_
