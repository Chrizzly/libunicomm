///////////////////////////////////////////////////////////////////////////////
// client_comm.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Client's communication service.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_CLIENT_COMM_HPP_
#define UNI_CLIENT_COMM_HPP_

/** @file client_comm.hpp Client's communication service definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/comm.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class config;

/** Connection service that represents client side. 
 *
 *  Unicomm transport layer.
 */
class UNICOMM_DECL client_communicator : public communicator
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Communicator smart pointer type. */
  typedef boost::shared_ptr<client_communicator> pointer_type;

public:

#ifdef UNICOMM_SSL

  /** Constructs a communicator object.
   *
   *  @param owner Reference to the communicator object's owner.
   *  @param ioservice Asio io service object.
   *  @param context Asio ssl context object.
   *  @param config Reference to a client's configuration object.
   */
  client_communicator(dispatcher& owner, boost::asio::io_service &ioservice, 
    boost::asio::ssl::context& context, const unicomm::config& config);

#else // UNICOMM_SSL

  /** Constructs a communicator object. 
   *
   *  @param owner Reference to the communicator object's owner.
   *  @param ioservice Asio io service object.
   *  @param config Reference to a client's configuration object.
   */
  client_communicator(dispatcher& owner, boost::asio::io_service &ioservice, 
    const unicomm::config& config);

#endif // UNICOMM_SSL

  /** Closes connection and destroys an object. */
  ~client_communicator(void);

public:

#ifdef UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Used by owner object to handle connect. 
   *
   *  @param handler Boost asio handshake handler.
   */
  void asio_success_connect_handler(const asio_handshake_handler_type& handler);

#endif // UNICOMM_SSL

};

} // namespace unicomm

#endif // UNI_CLIENT_COMM_HPP_
