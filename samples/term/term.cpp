///////////////////////////////////////////////////////////////////////////////
// term.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Mostly simple example on how to use unicomm engine to transfer custom data.
// Sends characters from stdin to opposite site.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.
 
#include "term.hpp"

#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/assign.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

using unicomm::message_arrived_params;
using unicomm::connected_params;
using unicomm::disconnected_params;
using unicomm::connect_error_params;

using boost::mutex;
using boost::asio::ip::tcp;
using boost::system::error_code;

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::stringstream;
using std::runtime_error;
using std::flush;

namespace 
{

void out_str(const std::string& s)
{
  static mutex m;
  mutex::scoped_lock lock(m);

  cout << s << flush;
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// aux
/** Synchronize an access to the std::cout. */
void uni_term::out_line(const std::string& s)
{
  out_str("\r\n" + s);
}

//////////////////////////////////////////////////////////////////////////
// session

// virtual functions

/** Message arrived callback type. */
void uni_term::session::message_arrived_handler(message_arrived_params& params)
{
  const message& inm = static_cast<const message&>(params.in_message());

  out_str(inm.data());
}

/** Client connected callback type. */
void uni_term::session::connected_handler(const connected_params& params)
{
  stringstream ss;

  ss << "> [" << params.comm().id() << "]:\tCONNECTED; local endpoint: " 
    << params.comm().local_endpoint() << ", remote endpoint: " 
    << params.comm().remote_endpoint();
  out_line(ss.str());
}

/** Client disconnected callback type. */
void uni_term::session::disconnected_handler(const disconnected_params& params)
{
  stringstream ss;

  ss << "> [" << params.comm().id() << "]:\tDISCONNECTED; " << params.error_code() 
    << "; " << params.what();
  out_line(ss.str());
}

//////////////////////////////////////////////////////////////////////////
// connect error handler

/** Connect error callback. */
void uni_term::connect_error_handler(const connect_error_params& params)
{
  const tcp::endpoint& remote_endpoint = params.remote_endpoint();
  const error_code& err                = params.error_code();

  stringstream ss;

  ss << "client> connect to [" << remote_endpoint << "] failed with error [" 
    << err << "; " << err.message().c_str() << "]";
  out_line(ss.str());
}

//////////////////////////////////////////////////////////////////////////
// configuration

/** Returns a reference to the term configuration object. */
unicomm::config& uni_term::config(void)
{
  using namespace unicomm;

  using boost::assign::list_of;

  static unicomm::config conf
    (
      unicomm::config()
        .tcp_port(55555)
        .message_decoder(message_decoder::pointer_type(new message_decoder()))
        .session_factory(&uni_term::session::create)

#ifdef UNICOMM_SSL

        .ssl_client_verity_fn("../ssl/ca.pem")
        .ssl_server_key_password("test")
        .ssl_server_cert_chain_fn("../ssl/server.pem")
        .ssl_server_key_fn("../ssl/server.pem")
        .ssl_server_dh_fn("../ssl/dh512.pem")

#endif // UNICOMM_SSL
    );

  return conf;
}

