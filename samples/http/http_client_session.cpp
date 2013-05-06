///////////////////////////////////////////////////////////////////////////////
// http_client_session.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Client's session definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_client_session.hpp"
#include "http_response.hpp"
#include "http_aux.hpp"

#include <unicomm/comm.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/handler_params.hpp>

#include <boost/assert.hpp>

#include <sstream>
#include <stdexcept>

using std::stringstream;

//////////////////////////////////////////////////////////////////////////
// CLIENT session
//
// assigned handlers

void uni_http::client_session::message_timeout_handler_1(const unicomm::message_timeout_params& params)
{
  params.comm().disconnect();

  // another facility to disconnect
  //throw unicomm::disconnected_error(boost::system::error_code(), 
  //  "Disconnected due to message timeout");
}

//------------------------------------------------------------------------
void uni_http::client_session::disconnected_handler_1(const unicomm::disconnected_params& /*params*/)
{
  BOOST_ASSERT(_in_buffer->empty() && " - Buffer should already be empty here");
}

//////////////////////////////////////////////////////////////////////////
// virtual functions

void uni_http::client_session::message_arrived_handler(unicomm::message_arrived_params& params)
{
  // use the incoming data somehow
  const response& inm = static_cast<const response&>(params.in_message());
  out_str(inm.headers_str());
  //inm.data();

  params.comm().disconnect();
}

//////////////////////////////////////////////////////////////////////////
// connect error handler

/** Connect error callback. */
void uni_http::connect_error_handler(const unicomm::connect_error_params& params)
{
  const boost::asio::ip::tcp::endpoint& remote_endpoint = params.remote_endpoint();
  const boost::system::error_code& err                  = params.error_code();

  stringstream ss;

  ss << "client: connect to [" << remote_endpoint << "] failed with error [" 
    << err << "; " << err.message().c_str() << "]";
  out_str(ss.str());
}
