///////////////////////////////////////////////////////////////////////////////
// server_comm.cpp
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

#include <unicomm/server_comm.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_service;

//////////////////////////////////////////////////////////////////////////
// server_communicator 

#ifdef UNICOMM_SSL

unicomm::server_communicator::server_communicator(dispatcher& owner, 
                                                  io_service& ioservice, 
                                                  boost::asio::ssl::context& context, 
                                                  const unicomm::config& config):
  communicator(owner, ioservice, context, config)

#else

unicomm::server_communicator::server_communicator(dispatcher& owner, 
                                                  io_service& ioservice, 
                                                  const unicomm::config& config):
  communicator(owner, ioservice, config)

#endif // UNICOMM_SSL

{
  // empty
}

//-----------------------------------------------------------------------------
unicomm::server_communicator::~server_communicator(void) 
{ 
  // empty
}

#ifdef UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// server ssl
void unicomm::server_communicator::asio_success_connect_handler(
  const asio_handshake_handler_type& handler) 
{
  ssl_socket().async_handshake(boost::asio::ssl::stream_base::server, handler);
}

#endif // UNICOMM_SSL
