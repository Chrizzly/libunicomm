///////////////////////////////////////////////////////////////////////////////
// http_server.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http server class definition. It's only defined to setup sockets.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_SERVER_HPP_
#define UNI_HTTP_SERVER_HPP_

/** @file http_server.hpp Http server. */

#include <unicomm/server.hpp>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** HTTP Server. */
class server : public unicomm::server
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an unicomm server object.
   *
   *  By default listen address is any.
   *  
   *  @param config Server configuration object.
   *  @note The copy of config is held.
   */
  explicit server(const unicomm::config& config):
    unicomm::server(config)
  {
    // empty
  }

  /** Constructs a unicomm server object.
   *  
   *  @param config Server object configuration. 
   *  @param listen_endpoint ip-address and port to be listened to. 
   *  @note The copy of config is held.
   */
  server(const unicomm::config& config,
         const boost::asio::ip::tcp::endpoint &listen_endpoint):
    unicomm::server(config, listen_endpoint)
  {
    // empty
  }

  /** Stops processing and destroys an object. */
  ~server(void)
  {
    try
    {
      if (started())
      {
        stop();
      }
    } catch (...)
    {
      BOOST_ASSERT(!" - Ah ah! An unhandled exception on destructor");
    }
  }

public:
  using unicomm::server::stop_accept;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  //////////////////////////////////////////////////////////////////////////
  // By overriding unicomm::server::before_start() virtual function you are
  // able to setup the acceptor entity.

  //void before_start(boost::asio::ip::tcp::acceptor& acceptor)
  //{
  //  acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  //}

  //////////////////////////////////////////////////////////////////////////
  // By overriding unicomm::server::after_accept() virtual function you are
  // able to setup the accepted socket.

  void after_accept(unicomm::tcp_socket_type& socket);
};

} // namespace uni_http

#endif // UNI_HTTP_SERVER_HPP_
