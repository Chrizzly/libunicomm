///////////////////////////////////////////////////////////////////////////////
// server_comm.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Server's communication service definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_SERVER_COMM_HPP_
#define UNI_SERVER_COMM_HPP_

/** @file server_comm.hpp Server's communication service definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/comm.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class config;

/** Server's communication service. 
 *
 *  Unicomm transport layer.
 */
class UNICOMM_DECL server_communicator : public communicator
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Server's communicator smart pointer type. */
  typedef boost::shared_ptr<server_communicator> pointer_type;

public:

#ifdef UNICOMM_SSL

  /** Constructs an object. 
   *
   *  @param owner Reference to the communicator object's owner.
   *  @param ioservice Boost asio io service object.
   *  @param context Boost asio ssl context object.
   *  @param config Configuration object.
   */
  server_communicator(dispatcher& owner, boost::asio::io_service &ioservice, 
    boost::asio::ssl::context& context, const unicomm::config& config);

#else // UNICOMM_SSL

  /** Constructs an object. 
   *
   *  @param owner Reference to the communicator object's owner.
   *  @param ioservice Boost asio io service object.
   *  @param config Configuration object.
   */
  server_communicator(dispatcher& owner, boost::asio::io_service &ioservice, 
    const unicomm::config& config);

#endif // UNICOMM_SSL

  /** Close connection and destroys an object. */
  ~server_communicator(void);

public:

#ifdef UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Used by owner to handle connection success. 
   *
   *  @param handler Boost asio handshake handler.
   */
  void asio_success_connect_handler(const asio_handshake_handler_type& handler);

#endif // UNICOMM_SSL

};

} // namespace unicomm

#endif // UNI_SERVER_COMM_HPP_
