///////////////////////////////////////////////////////////////////////////////
// echo.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Simple echo server & client example based on unicomm engine.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.
 
#include "echo.hpp"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/assign.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <iostream>
#include <stdexcept>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::stringstream;
using std::runtime_error;
using std::flush;

using unicomm::message_arrived_params;

using boost::recursive_mutex;
using boost::this_thread::sleep;
using boost::posix_time::milliseconds;
using boost::asio::ip::tcp;

//////////////////////////////////////////////////////////////////////////
// aux
/** Synchronize an access to the std::cout. */
void uni_echo::out_str(const std::string& s)
{
  static recursive_mutex m;

  recursive_mutex::scoped_lock lock(m);

  cout << "\r\n" << s << flush;
}

/** Client connect error callback. */
void uni_echo::connect_error_handler(const unicomm::connect_error_params& params)
{
  const tcp::endpoint& remote_endpoint = params.remote_endpoint();
  const boost::system::error_code& err = params.error_code();

  stringstream ss;

  ss << "client> connect to [" << remote_endpoint << "] failed with error [" 
    << err << "; " << err.message().c_str() << "]";
  out_str(ss.str());

  // fixme: add session parameters to be available through config
  if (params.client().connections_count() == 0)
  {
    sleep(milliseconds(500));
    params.client().connect();
  }
}

//////////////////////////////////////////////////////////////////////////
// configuration

/** Returns a reference to the echo configuration object. */
unicomm::config& uni_echo::config(void)
{
  using namespace unicomm;

  using boost::assign::list_of;

  static unicomm::config conf
    (
      unicomm::config()
        .tcp_port(55555)
        .timeouts_enabled(true)
        .use_unique_message_id(true)
        .message_info(echo_request_message::static_name(), true, 
          1000, list_of(echo_reply_message::static_name()))
        .message_info(echo_reply_message::static_name())
        .message_factory
          (
            message_base::factory_type()
              (uni_echo::echo_request_message::static_name(), 
                &unicomm::create<uni_echo::echo_request_message>)
              (uni_echo::echo_reply_message::static_name(), 
                &unicomm::create<uni_echo::echo_reply_message>)
          )

#ifdef UNICOMM_SSL

        .ssl_client_verity_fn("../ssl/ca.pem")
        .ssl_server_key_password("test")
        .ssl_server_cert_chain_fn("../ssl/server.pem")
        .ssl_server_key_fn("../ssl/server.pem")
        .ssl_server_dh_fn("../ssl/dh512.pem")

#endif // UNICOMM_SSL
    );

  return set_binary_message_format(conf);
}
