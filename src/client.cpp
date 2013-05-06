///////////////////////////////////////////////////////////////////////////////
// client.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm client class definition. Adds connect initiating functuonality.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#include <unicomm/client.hpp>
#include <unicomm/client_comm.hpp>
#include <unicomm/handler_params.hpp>
#include <unicomm/detail/helper_detail.hpp>

#include <smart/debug_out.hpp>

#include <boost/bind.hpp>

using boost::asio::ip::tcp;

//////////////////////////////////////////////////////////////////////////
// unicomm client
unicomm::client::client(const unicomm::config& config):
  dispatcher(config)
{
  constructor();
}

//-----------------------------------------------------------------------------
unicomm::client::client(const unicomm::config& config, 
                        const tcp::endpoint& remote_endpoint):
  dispatcher(config, remote_endpoint)
{
  constructor();
}

//-----------------------------------------------------------------------------
unicomm::client::client(const unicomm::config& config, 
    const connect_error_signal_type::slot_function_type& error_handler):
  dispatcher(config)
{
  constructor(error_handler);
}

//-----------------------------------------------------------------------------
unicomm::client::client(const unicomm::config& config, 
    const boost::asio::ip::tcp::endpoint &remote_endpoint,
    const connect_error_signal_type::slot_function_type& error_handler):
  dispatcher(config, remote_endpoint)
{
  constructor(error_handler);
}

//-----------------------------------------------------------------------------
unicomm::client::~client(void)
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
void unicomm::client::connect(
  const connect_error_signal_type::slot_function_type& error_handler)
{
  start_connect(error_handler);
}

//-----------------------------------------------------------------------------
void unicomm::client::connect(const tcp::endpoint& remote_endpoint, 
  const connect_error_signal_type::slot_function_type& error_handler)
{
  endpoint(remote_endpoint);

  connect(error_handler);
}

//////////////////////////////////////////////////////////////////////////
// handler setters
boost::signals2::connection unicomm::client::add_connect_error_handler(
  const connect_error_signal_type::slot_type& handler)
{
  return _conn_error_signal.connect(handler);
}

//-----------------------------------------------------------------------------
void unicomm::client::constructor(void)
{
  initialize();
}

//-----------------------------------------------------------------------------
void unicomm::client::constructor(
  const connect_error_signal_type::slot_function_type& error_handler)
{
  add_connect_error_handler(error_handler);

  initialize();
}

//-----------------------------------------------------------------------------
void unicomm::client::start_connect(
  const connect_error_signal_type::slot_function_type& error_handler)
{
  comm_ptr client = create_comm<client_communicator>();

  client->socket().async_connect(endpoint(), 
    boost::bind(&client::asio_connect_handler, this, client, 
    connect_error_info(endpoint(), error_handler), 
    boost::asio::placeholders::error));
}

//-----------------------------------------------------------------------------
bool unicomm::client::is_connect_error(void) const
{
  return !_conn_errors.empty();
}

//////////////////////////////////////////////////////////////////////////
// core - processors
void unicomm::client::extra_process(void)
{
  process_connect_error();
}

//-----------------------------------------------------------------------------
void unicomm::client::process_connect_error(void)
{
  if (is_connect_error())
  {
    while (!_conn_errors.empty())
    {
      const connect_error_collection_type::value_type err_info = 
        _conn_errors.front();
      _conn_errors.erase(_conn_errors.begin());

      call_conn_error(err_info);
    }
  }
}

////-----------------------------------------------------------------------------
//void unicomm::client::before_start(void)
//{
//  _conn_errors.clear();
//
//#ifdef UNICOMM_SSL
//
//  ssl_context().set_verify_mode(boost::asio::ssl::context::verify_peer);
//  ssl_context().load_verify_file(config().home_dir() + config().ssl_client_verify_fn());
//
//#endif // UNICOMM_SSL
//}

//-----------------------------------------------------------------------------
void unicomm::client::on_reset(void)
{
  _conn_errors.clear();

#ifdef UNICOMM_SSL

  ssl_context().set_verify_mode(boost::asio::ssl::context::verify_peer);
  ssl_context().load_verify_file(config().home_dir() + config().ssl_client_verify_fn());

#endif // UNICOMM_SSL
}

//-----------------------------------------------------------------------------
void unicomm::client::initialize(void)
{
  _conn_errors.clear();

#ifdef UNICOMM_SSL

  ssl_context().set_verify_mode(boost::asio::ssl::context::verify_peer);
  ssl_context().load_verify_file(config().home_dir() + config().ssl_client_verify_fn());

#endif // UNICOMM_SSL
}

//////////////////////////////////////////////////////////////////////////
// handlers callers
void unicomm::client::call_conn_error(const connect_error_info& info) /*const*/
{ 
  detail::call_handler(info.error_handler(), 
    connect_error_params(*this, info.endpoint(), info.error_code()));

  detail::execute_signal(_conn_error_signal, 
    connect_error_params(*this, info.endpoint(), info.error_code()));
}

//-----------------------------------------------------------------------------
void unicomm::client::call_after_connect(tcp_socket_type& socket)
{
  try
  {
    after_connect(socket);
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::client]: Virtual after connect has risen an "
      << "std::exception [what: " << e.what() << "]")

    BOOST_ASSERT(!"Virtual after connect has risen a std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::client]: Virtual after connect has risen an " 
      << "UNKNOWN exception")

    BOOST_ASSERT(!"Virtual after connect has risen an UNKNOWN exception");
  }
}

//////////////////////////////////////////////////////////////////////////
// connection error handling: routines
void unicomm::client::reg_conn_error(const connect_error_info& info)
{
  _conn_errors.push_back(info);
}

//////////////////////////////////////////////////////////////////////////
// boost asio handlers
void unicomm::client::asio_connect_handler(const comm_ptr& client, 
                                           connect_error_info& info, 
                                           const boost::system::error_code& error)
{
  UNICOMM_DEBUG_OUT("[unicomm::client]: Asio connect handler invoked; comm ID = " 
    << std::dec << client->id())

    if (error)
    {
      UNICOMM_DEBUG_OUT("[unicomm::client]: Connection failed; comm ID = " 
        << std::dec << client->id() << "; [" << error << "; " 
        << error.message() << "]")

      info.error_code(error);
      reg_conn_error(info);
    } else
    {
      // add client
      insert_comm(client);
      // call virtual
      call_after_connect(client->socket());

#ifdef UNICOMM_SSL

      client->asio_success_connect_handler(bind(
        &client::asio_handshake_handler, this, client, info, 
        boost::asio::placeholders::error)); // -> client->ssl_socket().async_handshake(...)

#else // UNICOMM_SSL

      // make client know that it is just connected
      client->asio_success_connect_handler(); // -> client->just_connected(true);

#endif // UNICOMM_SSL

      UNICOMM_DEBUG_OUT("[unicomm::client]: Client connected; comm ID = " 
        << std::dec << client->id() << "; remote ep = " << client->remote_endpoint())
    }

    // tell to process
    kick_dispatcher();

    UNICOMM_DEBUG_OUT("[unicomm::client]: Asio connect finished; comm ID = " 
      << std::dec << client->id())
}

#ifdef UNICOMM_SSL

//void unicomm::client::asio_connect_handler(const comm_ptr& client, connect_error_info& info, const boost::system::error_code& error)
//{
//  UNICOMM_DEBUG_OUT("[unicomm::client]: Asio connect handler invoked; comm ID = " << std::dec << client->id())
//
//  if (error)
//  {
//    UNICOMM_DEBUG_OUT("[unicomm::client]: Connection failed; comm ID = " << std::dec << client->id() << "; [" << error << "; " << error.message() << "]")
//
//    info.error_code(error);
//
//    reg_conn_error(info);
//  } else
//  {
//
//#ifdef UNICOMM_SSL
//
//    client->asio_success_connect_handler(bind(
//      &client::asio_handshake_handler, this, client, info, boost::asio::placeholders::error)); // -> client->ssl_socket().async_handshake(...)
//
//#else // UNICOMM_SSL
//
//    insert_comm(client);
//    // make client know that it is just connected
//    client->asio_success_connect_handler(); // -> client->just_connected(true);
//
//#endif // UNICOMM_SSL
//
//    // call virtual
//    call_after_connect(client->socket());
//  }
//
//  UNICOMM_DEBUG_OUT("[unicomm::client]: Asio connect finished; comm ID = " << std::dec << client->id())
//}
//
//#ifdef UNICOMM_SSL

//-----------------------------------------------------------------------------
void unicomm::client::asio_handshake_handler(const comm_ptr& client, 
                                             connect_error_info& info, 
                                             const boost::system::error_code& error)
{
  UNICOMM_DEBUG_OUT("[unicomm::client]: SSL Handshake handler invoked; comm ID = " 
    << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")

  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::client]: SSL Handshake error; comm ID = " 
      << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")

    info.error_code(error);

    reg_conn_error(info);
  } else
  {
    //insert_comm(client);
  }

  client->asio_handshake_handler(error); // client->just_connected(true);

  UNICOMM_DEBUG_OUT("[unicomm::client]: SSL Handshake handler finished; comm ID = " 
    << std::dec << client->id() << "; [" << error << ", " << error.message() << "]")
}

#endif // UNICOMM_SSL



