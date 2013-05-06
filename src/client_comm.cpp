///////////////////////////////////////////////////////////////////////////////
// client_comm.cpp
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

#include <unicomm/client_comm.hpp>

using boost::asio::io_service;

//////////////////////////////////////////////////////////////////////////
// client_communicator 
#ifdef UNICOMM_SSL

unicomm::client_communicator::client_communicator(dispatcher& owner, 
    io_service& ioservice, boost::asio::ssl::context& context, 
    const unicomm::config& config):
  communicator(owner, ioservice, context, config)

#else

unicomm::client_communicator::client_communicator(dispatcher& owner, 
    io_service& ioservice, 
    const unicomm::config& config):
  communicator(owner, ioservice, config)

#endif // UNICOMM_SSL

{
  // empty
}

//-----------------------------------------------------------------------------
unicomm::client_communicator::~client_communicator(void) 
{ 
  // empty
}

#ifdef UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// client ssl
void unicomm::client_communicator::asio_success_connect_handler(
  const asio_handshake_handler_type& handler) 
{
  ssl_socket().async_handshake(boost::asio::ssl::stream_base::client, handler);
}

#endif // UNICOMM_SSL
