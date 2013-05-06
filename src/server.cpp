///////////////////////////////////////////////////////////////////////////////
// server.cpp
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

#include <unicomm/server.hpp>
#include <unicomm/server_comm.hpp>
#include <unicomm/except.hpp>
#include <detail/basic_detail.hpp>

#include <smart/debug_out.hpp>
#include <smart/scoped_sentinel.hpp>

#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using boost::asio::socket_base;

using smart::scoped_sentinel;

//////////////////////////////////////////////////////////////////////////
// unicomm server
unicomm::server::server(const unicomm::config& config):
  dispatcher(config)
{
  constructor();
}

//-----------------------------------------------------------------------------
unicomm::server::server(const unicomm::config& config, 
                        const tcp::endpoint &listen_endpoint):
  dispatcher(config, listen_endpoint)
{
  constructor();
}

//-----------------------------------------------------------------------------
unicomm::server::~server(void)
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

//-----------------------------------------------------------------------------
void unicomm::server::accept(void)
{
  just_start_accept();
}

////-----------------------------------------------------------------------------
//void unicomm::server::start(const tcp::endpoint& listen_endpoint)
//{
//  endpoint(listen_endpoint);
//
//  start();
//}

//////////////////////////////////////////////////////////////////////////
// private stuff
//void unicomm::server::before_start(void)
//{
//#ifdef UNICOMM_SSL
//  // setup ssl context
//  ssl_context().set_options(
//    boost::asio::ssl::context::default_workarounds
//      | boost::asio::ssl::context::no_sslv2
//        | boost::asio::ssl::context::single_dh_use);
//
//  const unicomm::config& conf = config();
//
//  ssl_context().set_password_callback(boost::bind(&unicomm::config::ssl_server_key_password, &conf));
//  ssl_context().use_certificate_chain_file(conf.home_dir() + conf.ssl_server_cert_chain_fn());
//  ssl_context().use_private_key_file(conf.home_dir() + conf.ssl_server_key_fn(), boost::asio::ssl::context::pem);
//  ssl_context().use_tmp_dh_file(conf.home_dir() + conf.ssl_server_dh_fn());
//
//#endif // UNICOMM_SSL
//
//  // create listening socket
//  create_listening_socket();
//  // allow to setup acceptor
//  before_start(acceptor());
//  // start
//  just_start_accept();
//}

void unicomm::server::initialize(void)
{
#ifdef UNICOMM_SSL
  // setup ssl context
  ssl_context().set_options(
    boost::asio::ssl::context::default_workarounds
    | boost::asio::ssl::context::no_sslv2
    | boost::asio::ssl::context::single_dh_use);

  const unicomm::config& conf = config();

  ssl_context().set_password_callback(boost::bind(&unicomm::config::ssl_server_key_password, &conf));
  ssl_context().use_certificate_chain_file(conf.home_dir() + conf.ssl_server_cert_chain_fn());
  ssl_context().use_private_key_file(conf.home_dir() + conf.ssl_server_key_fn(), boost::asio::ssl::context::pem);
  ssl_context().use_tmp_dh_file(conf.home_dir() + conf.ssl_server_dh_fn());

#endif // UNICOMM_SSL

  // create listening socket
  //create_listening_socket();
  create_acceptor();
  //// allow to setup acceptor
  //before_start(acceptor());
}

//-----------------------------------------------------------------------------
void unicomm::server::finalize(void)
{
  // stop accepting incoming connections
  destroy_acceptor();
}

//-----------------------------------------------------------------------------
void unicomm::server::on_reset(void)
{
  initialize();
  // fixme: add on_reset as before_start(acceptor());
}

//-----------------------------------------------------------------------------
void unicomm::server::on_stop(void)
{
  finalize();
}

//-----------------------------------------------------------------------------
bool unicomm::server::is_acceptor(void) const
{
  return _acceptor != 0;
}

//-----------------------------------------------------------------------------
void unicomm::server::constructor(void)
{
  initialize();
}

//-----------------------------------------------------------------------------
void unicomm::server::just_start_accept(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::server]: Starting accept; is_acceptor() = " 
    << is_acceptor())
  
  BOOST_ASSERT(is_acceptor() && " - Acceptor should exist");

  open_acceptor();

  comm_ptr client = create_comm<server_communicator>();

  UNICOMM_DEBUG_OUT("[unicomm::server]: New comm created; comm ID = " 
    << std::dec << client->id())

  // fixme: incapsulate async_accept() & async_connect() by communicator
  acceptor().async_accept(client->socket(), 
    boost::bind(&server::asio_accept_handler, this, client,
      boost::asio::placeholders::error));
}

//-----------------------------------------------------------------------------
void unicomm::server::create_listening_socket(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::server]: Creating listening socket; binding to [" 
    << endpoint() << "]")

  create_acceptor();
  open_acceptor();
}

//-----------------------------------------------------------------------------
void unicomm::server::create_acceptor(void)
{
  _acceptor.reset(new acceptor_ptr_type::element_type(ioservice()));
}

//-----------------------------------------------------------------------------
void unicomm::server::open_acceptor(void)
{
  BOOST_ASSERT(is_acceptor() && " - Acceptor should exist");

  if (!is_acceptor_opened())
  {
    UNICOMM_DEBUG_OUT("[unicomm::server]: Opening listening socket; binding to [" 
      << endpoint() << "]")

    acceptor().open(tcp::v4());
    acceptor().bind(endpoint()); // fixme: can acceptor be rebinded?

    const int backlog = detail::use_default_tcp_backlog(config().tcp_backlog())? 
      socket_base::max_connections: config().tcp_backlog();

    acceptor().listen(backlog);
  }
}

//-----------------------------------------------------------------------------
void unicomm::server::close_acceptor(void)
{
  BOOST_ASSERT(is_acceptor() && " - Acceptor should exist");

  if (is_acceptor_opened())
  {
    try
    {
      acceptor().close();
    } catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
    {
      UNICOMM_DEBUG_OUT("[unicomm::server]: Acceptor closing has risen an " 
        << "std::exception [" << e.what() << "]")
    }
  }
}

//-----------------------------------------------------------------------------
void unicomm::server::destroy_acceptor(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::server]: ENTER; Destroying listening socket; " 
    << "is_acceptor() = " << is_acceptor())
  
  if (is_acceptor())
  {
    close_acceptor();

    _acceptor.reset();
  }
  
  UNICOMM_DEBUG_OUT("[unicomm::server]: EXIT; Destroying listening socket; " 
    << "is_acceptor() = " << is_acceptor())
}

//-----------------------------------------------------------------------------
void unicomm::server::call_after_accept(tcp_socket_type& socket)
{
  try
  {
    after_accept(socket);
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::server]: Virtual after accept has risen an " 
      << "std::exception [what: " << e.what() << "]")

    BOOST_ASSERT(!"Virtual after accept has risen a std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::server]: Virtual after accept has risen " 
      << "an UNKNOWN exception")

    BOOST_ASSERT(!"Virtual after accept has risen an UNKNOWN exception");
  }
}

//-----------------------------------------------------------------------------
boost::asio::ip::tcp::acceptor& unicomm::server::acceptor(void)
{
  BOOST_ASSERT(is_acceptor() && " - Acceptor is invalid");

  return *_acceptor;
}

//-----------------------------------------------------------------------------
void unicomm::server::start_accept(void)
{
  if (!is_acceptor())
  {
    create_listening_socket();
  }

  BOOST_ASSERT(is_acceptor() && " - Acceptor should exist");

  just_start_accept();
}

//-----------------------------------------------------------------------------
void unicomm::server::stop_accept(void)
{
  destroy_acceptor();
}

//-----------------------------------------------------------------------------
void unicomm::server::asio_accept_handler(const comm_ptr& client, 
                                          const boost::system::error_code& error)
{
  UNICOMM_DEBUG_OUT("[unicomm::server]: Asio accept handler invoked; comm ID = " 
    << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")

  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::server]: Accept error [" << error << ", " 
      << error.message() << "]")
    // fixme: what to do here? how many times to start accept if errors?
    //just_start_accept();
  } else
  {
    // start new accept anyway
    scoped_sentinel accept_sentry(boost::bind(&server::just_start_accept, this));
    // add client
    insert_comm(client);
    // call virtual
    call_after_accept(client->socket());

#ifdef UNICOMM_SSL

    // perform handshake 
    client->asio_success_connect_handler(bind( // -> client->ssl_socket().async_handshake(...)
      &server::asio_handshake_handler, this, client, boost::asio::placeholders::error)); 

#else // UNICOMM_SSL

    // make client know that it is just connected
    client->asio_success_connect_handler(); // -> client->just_connected(true);
    
#endif // UNICOMM_SSL

    // tell to process
    kick_dispatcher();

    UNICOMM_DEBUG_OUT("[unicomm::server]: Client accepted; comm ID = " 
      << std::dec << client->id() << "; remote ep = " << client->remote_endpoint())
  }

  UNICOMM_DEBUG_OUT("[unicomm::server]: Asio accept handler finished; comm ID = " 
    << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")
}

#ifdef UNICOMM_SSL

//-----------------------------------------------------------------------------
void unicomm::server::asio_handshake_handler(const comm_ptr& client, const boost::system::error_code& error)
{
  UNICOMM_DEBUG_OUT("[unicomm::server]: SSL Handshake handler invoked; comm ID = " 
    << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")

  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::server]: SSL Handshake error; comm ID = " 
      << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")
  } else
  {
    //insert_comm(client); 
  }

  client->asio_handshake_handler(error); // client->just_connected(true);

  UNICOMM_DEBUG_OUT("[unicomm::server]: SSL Handshake handler finished; comm ID = " 
    << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")
}

#endif // UNICOMM_SSL

