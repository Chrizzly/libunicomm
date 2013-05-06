///////////////////////////////////////////////////////////////////////////////
// http_config.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// This is just an example shows how to use unicomm to define 
// different type of protocols based on tcp/ip. 
// There are only few messages defined not all of HTTP.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_config.hpp"
#include "http_response.hpp"
#include "http_request.hpp"
#include "http_message_encoder.hpp"
#include "http_message_decoder.hpp"

#include <unicomm/config.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>

#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::flush;

namespace 
{

/** Common config that holds same data. */
unicomm::config& common_config(void)
{
  using namespace unicomm;

  static unicomm::config conf
    (
      unicomm::config()
        .tcp_port(uni_http::default_port())
        .timeouts_enabled(true)
        .message_encoder(uni_http::message_encoder::create())
        .message_decoder(uni_http::message_decoder::create())
    );

  return conf;
}

} // unnamed namespace

/** Returns a reference to the echo configuration object. */
unicomm::config& uni_http::server_config(void)
{
  using namespace unicomm;

  static unicomm::config conf
    (
      common_config()
        .dispatcher_idle_tout(10)
        .message_factory
          (
            message_base::factory_type(&unicomm::create<uni_http::request>)
          )

#ifdef UNICOMM_SSL

        .ssl_server_key_password("test")
        .ssl_server_cert_chain_fn("../../../../http/ssl/server.pem")
        .ssl_server_key_fn("../../../../http/ssl/server.pem")
        .ssl_server_dh_fn("../../../../http/ssl/dh512.pem")

#endif // UNICOMM_SSL
    );

  return conf;
}

//////////////////////////////////////////////////////////////////////////
// client definition

/** Returns a reference to the unicomm client configuration object. */
unicomm::config& uni_http::client_config(void)
{
  using namespace unicomm;

  static unicomm::config conf
    (
      common_config()
        .message_info(message_info(uni_http::request::get(), true, 10000))
        .message_factory
          (
            message_base::factory_type(&unicomm::create<uni_http::response>)
          )

#ifdef UNICOMM_SSL

        .ssl_client_verity_fn("../../../../http/ssl/ca.pem")

#endif // UNICOMM_SSL
    );

  return conf;
}
