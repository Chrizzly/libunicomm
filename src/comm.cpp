///////////////////////////////////////////////////////////////////////////////
// comm.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Communication service. This represents transport layer of unicomm.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#include <unicomm/comm.hpp>
#include <unicomm/dispatcher.hpp>
#include <unicomm/basic.hpp>
#include <unicomm/config.hpp>
#include <unicomm/except.hpp>

#include <smart/scoped_sentinel.hpp>
#include <smart/debug_out.hpp>
#include <smart/timers.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4702)  // warning C4702: unreachable code
#endif // UNI_VISUAL_CPP

#include <boost/lexical_cast.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <stdexcept>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <sstream>

#ifdef max
# undef max
#endif

#ifdef min
# undef min
#endif

using std::runtime_error;
using std::string;
using std::vector;
using std::make_pair;
using std::find;
using std::hex;
using std::dec;
using std::stringstream;
using std::min;
using std::for_each;
using std::pair;

using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::lexical_cast;
using boost::recursive_mutex;
using boost::asio::buffers_begin;
using boost::system::error_code;
using boost::posix_time::milliseconds;

using smart::generic_scoped_sentinel;
using smart::scoped_sentinel;

//////////////////////////////////////////////////////////////////////////
// aux
namespace
{

//------------------------------------------------------------------------
inline bool use_unique_message_id(const unicomm::config& conf)
{
  return conf.use_unique_message_id();
}

//------------------------------------------------------------------------
inline const unicomm::session_base::factory_type& 
check_session_factory(const unicomm::session_base::factory_type& factory)
{
  if (!factory)
  {
    throw unicomm::invalid_session_factory(
      "Specified session factory method is invalid");
  }

  return factory;
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// communicator

namespace unicomm
{

/** Compares prepared messages. */
inline bool operator<(const communicator::prepeared_message& l, 
                      const communicator::prepeared_message& r)
{
  return l.priority() < r.priority();
}

/** Compares prepared messages. */
inline bool operator>(const communicator::prepeared_message& l, 
                      const communicator::prepeared_message& r)
{
  return l.priority() > r.priority();
}

} // namespace unicomm

#ifdef UNICOMM_SSL

unicomm::communicator::communicator(dispatcher& owner, 
                                    io_service& ioservice, 
                                    boost::asio::ssl::context& context, 
                                    const unicomm::config& config):
  _socket(ioservice, context),

#else

unicomm::communicator::communicator(dispatcher& owner, 
                                    io_service& ioservice, 
                                    const unicomm::config& config):
  _socket(ioservice),

#endif // UNICOMM_SSL
  _id(owner.new_commid()),
  _mesid(undefined_messageid()),
  _connected(false),
  _just_connected(false),
  _config(&config),
  _owner(owner),
  _in_buffer_updated(false),

#ifdef UNICOMM_FORK_SUPPORT
  _is_notify_upper(true),
#endif // UNICOMM_FORK_SUPPORT

  _internal_mid(0)
{
  // empty
}

//-----------------------------------------------------------------------------
unicomm::communicator::~communicator(void)
{
  try
  {
    disconnect();
  } catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Disconnecting or closing "
      << "socket cause an exception is risen")

    BOOST_ASSERT(!"Disconnecting or closing socket cause an exception is risen");
  }
}

//-----------------------------------------------------------------------------
unicomm::tcp_socket_type& unicomm::communicator::get_tcp_socket(void)
{
#ifdef UNICOMM_SSL
  return _socket.lowest_layer();
#else
  return _socket;
#endif
}

//-----------------------------------------------------------------------------
const unicomm::tcp_socket_type& unicomm::communicator::get_tcp_socket(void) const
{
#ifdef UNICOMM_SSL
  return _socket.lowest_layer();
#else
  return _socket;
#endif
}

//-----------------------------------------------------------------------------
unicomm::tcp_socket_type& unicomm::communicator::socket(void)
{
  return get_tcp_socket();
}

//-----------------------------------------------------------------------------
const unicomm::tcp_socket_type& unicomm::communicator::socket(void) const
{
  return get_tcp_socket();
}

#ifdef UNICOMM_SSL

//-----------------------------------------------------------------------------
unicomm::ssl_socket_type& unicomm::communicator::ssl_socket(void)
{
  return _socket;
}

//-----------------------------------------------------------------------------
const unicomm::ssl_socket_type& unicomm::communicator::ssl_socket(void) const
{
  return _socket;
}

#endif // UNICOMM_SSL


//-----------------------------------------------------------------------------
void unicomm::communicator::close_socket(void)
{
  try
  {
    if (socket().is_open())
    {
      connected(false);
      boost::system::error_code err;
      socket().close(err);

      UNICOMM_DEBUG_OUT("[unicomm::communicator]: SOCKET CLOSED; comm ID = " 
        << dec << id() << ", error_code: " << err << "; " << err.message())
    }
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Close socket has risen "
      << "an unexpected exception; comm ID = " << dec << id())

    BOOST_ASSERT(!"Close socket has risen an unexpected exception");
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::reg_message_timeout(messageid_type mid, 
                                                const string& name) const
{
  // register outgoing message
  const unicomm::config& conf = config();

  const smart::timeout tout(conf.message_timeout_used(name)? 
    milliseconds(conf.message_timeout(name)): smart::timeout::infinite_timeout());

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: REGISTER MESSAGE TIMEOUT; comm ID = " 
    << dec << id() << "; message ID = " << mid << "; timeout = " 
    << conf.message_timeout(name) << " ms; message NAME = " << name)

  if (is_timeout_registered(mid))
  {
    // overwrite existing timeout if same message identifier got
    _mes_timeouts[mid] = message_timeout_info(name, tout);
  } else
  {
    // create new one if doesn't exist
    _mes_timeouts.insert(make_pair(mid, message_timeout_info(name, tout)));
  }
}

//-----------------------------------------------------------------------------
unicomm::messageid_type unicomm::communicator::new_mid(void) const
{
  if (++_mesid == undefined_messageid())
  {
    ++_mesid;
  }

  return _mesid;
}

//-----------------------------------------------------------------------------
bool unicomm::communicator::is_timeout_registered(messageid_type id) const
{
  return _mes_timeouts.find(id) != _mes_timeouts.end();
}

/** Creates session object using specified by constructor factory method. */
unicomm::session_base& unicomm::communicator::create_user_session(void)
{
  if (!is_session_valid())
  {
    try
    {
      mutex_type::scoped_lock lock(_session_mutex);

      _user_session = check_session_factory(config().session_factory())
        (connected_params(*this, _in_buffer));
    }
    catch (const std::exception& e)
    {
      UNICOMM_DEBUG_OUT("[unicomm::communicator]: User session constructor "
        << "has risen an exception; comm ID = " << dec << id() 
        << "; what [" << e.what() << "]")

      throw session_creation_error(string("Can't create session object: [") + 
        e.what() + "]");
    }
    catch (...)
    {
      UNICOMM_DEBUG_OUT("[unicomm::communicator]: User session constructor "
        << "has risen an UNKNOWN exception; comm ID = " 
        << dec << id() << "; what [Unknown error]")

      BOOST_ASSERT(!"An UNKNOWN exception thrown by user session constructor");

      throw session_creation_error("Can't create session object: [Unknown error]");
    }
  }

  return user_session();
}

//-----------------------------------------------------------------------------
unicomm::session_base& unicomm::communicator::user_session(void) const
{
  BOOST_ASSERT(is_session_valid() && 
    " - An attempt to access invalid session object");

  mutex_type::scoped_lock lock(_session_mutex);

  return *_user_session;
}

//////////////////////////////////////////////////////////////////////////
// outgoing buffers support
unicomm::messageid_type 
unicomm::communicator::out_buffers_insert(const prepeared_message& m)
{
  const messageid_type int_mid = new_internal_mid();

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: NEW OUT BUFFER; comm ID = " 
    << dec << id() << "; internal ID = " << int_mid << "; message ID = " 
    << m.id() << "; message NAME = " << m.name())

  std::pair<out_buffers_map_type::const_iterator, bool> result = 
    _out_buffers.insert(make_pair(int_mid, m));

  BOOST_ASSERT(result.second && " - Message with such id already exists in buffer");

  return int_mid;
}

//-----------------------------------------------------------------------------
void unicomm::communicator::out_buffers_erase(messageid_type int_mid)
{
  SMART_IFDEF_DEBUG(const bool ok = )
    _out_buffers.erase(int_mid)
  SMART_IFDEF_DEBUG(!= 0);

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: OUT BUFFER ERASED; comm ID = " 
    << dec << id() << "; internal ID = " << int_mid << "; ok = " << ok)

  BOOST_ASSERT(ok && " - Message id has not been found in output messages "
    "buffer, but should have been found");
}

//-----------------------------------------------------------------------------
const unicomm::communicator::prepeared_message& 
unicomm::communicator::get_out_buffers_item(messageid_type int_mid) const
{
  out_buffers_map_type::const_iterator cit = _out_buffers.find(int_mid);

  BOOST_ASSERT(cit != _out_buffers.end() && 
    " - Message is not found in output comm buffer");

  if (cit == _out_buffers.end())
  {
    throw runtime_error("Could not find message id");
  }

  return cit->second;
}

//-----------------------------------------------------------------------------
bool unicomm::communicator::is_out_buffers_empty(void) const
{
  return _out_buffers.empty();
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::communicator::mt_out_buffers_insert(const prepeared_message& m)
{
  out_buffer_mutex_type::scoped_lock lock(_out_buf_mutex);

  return out_buffers_insert(m);
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_out_buffers_erase(messageid_type int_mid)
{
  out_buffer_mutex_type::scoped_lock lock(_out_buf_mutex);

  out_buffers_erase(int_mid);
}

//-----------------------------------------------------------------------------
const unicomm::communicator::prepeared_message& 
unicomm::communicator::mt_get_out_buffers_item(messageid_type int_mid) const
{
  out_buffer_mutex_type::scoped_lock lock(_out_buf_mutex);

  return get_out_buffers_item(int_mid);
}

//-----------------------------------------------------------------------------
bool unicomm::communicator::mt_is_out_buffers_empty(void) const
{
  out_buffer_mutex_type::scoped_lock lock(_out_buf_mutex);

  return is_out_buffers_empty();
}

//////////////////////////////////////////////////////////////////////////
// misc handlers
void unicomm::communicator::handle_error(const boost::system::error_code& error, 
                                         const std::string& what)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Error handler invoked; comm ID = " 
    << id() << " [" << error << ", " << error.message() << "]")

  throw communication_error(error, what);
}

//-----------------------------------------------------------------------------
void unicomm::communicator::handle_disconnected(
  const boost::system::error_code& reason, 
  const std::string& what)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Disconnected handler invoked; comm ID = " 
    << id() << " [" << reason << ", " << reason.message() << "]")

  throw disconnected_error(reason, what);
}

//-----------------------------------------------------------------------------
//void unicomm::communicator::mt_handle_ch_error(void)
//{
//  bool is_error = false;
//
//  { // error lock
//    mutex_type::scoped_lock lock(_read_err_mutex);
//    is_error = _read_error || _write_error;
//  }
//
//  if (is_error && mt_is_out_buffers_empty())
//  {
//    const boost::system::error_code error = _read_error? _read_error: _write_error;
//    _read_error.clear();
//    _write_error.clear();
//
//    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Channel error; comm ID = " 
//      << id() << " [" << error << ", " << error.message() << "]")
//
//    const string what = "Channel error";
//
//    switch (error.value())
//    {
//      // errors list that cause disconnected handler to be invoked
//      case boost::asio::error::eof:
//      case boost::asio::error::connection_aborted:
//      case boost::asio::error::connection_reset:
//      case boost::asio::error::not_socket:
//      case boost::asio::error::shut_down:
//      case boost::asio::error::operation_aborted:
//      case boost::asio::error::bad_descriptor:
//        handle_disconnected(error, what);
//      break;
//
//      default:
//        UNICOMM_DEBUG_OUT("[unicomm::communicator]: UNEXPECTED Channel error; comm ID = " 
//          << id() << " [" << error << ", " << error.message() << "]")
//
//        handle_disconnected(error, what);
//      break;
//    }
//  }
//}

void unicomm::communicator::mt_handle_ch_error(void)
{
  boost::system::error_code error;

  {
    mutex_type::scoped_lock lock(_read_err_mutex);

    error = _read_error? _read_error: _write_error;
    _read_error.clear();
    _write_error.clear();
  }

  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Channel error; comm ID = " 
      << id() << " [" << error << ", " << error.message() << "]")

    const string what = "Channel error";

    switch (error.value())
    {
      // errors list that cause disconnected handler to be invoked
      case boost::asio::error::eof:
      case boost::asio::error::connection_aborted:
      case boost::asio::error::connection_reset:
      case boost::asio::error::not_socket:
      case boost::asio::error::shut_down:
      case boost::asio::error::operation_aborted:
      case boost::asio::error::bad_descriptor:
        handle_disconnected(error, what);
      break;

      default:
        UNICOMM_DEBUG_OUT("[unicomm::communicator]: UNEXPECTED Channel error; comm ID = " 
          << id() << " [" << error << ", " << error.message() << "]")

        handle_disconnected(error, what);
      break;
    }
  }
}

//-----------------------------------------------------------------------------
inline void unicomm::communicator::kick_dispatcher(void)
{
  owner().kick_dispatcher();
}

//-----------------------------------------------------------------------------

#ifdef UNICOMM_SSL

void unicomm::communicator::handle_handshake_error(void)
{
  if (_handshake_error)
  {
    const boost::system::error_code tmp_err = _handshake_error;
    _handshake_error.clear();

    disconnect();

    throw handshake_error(tmp_err, "Handshake error occurred");
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_handle_handshake_error(void)
{
  mutex_type::scoped_lock lock(_hshake_err_mutex);

  handle_handshake_error();
}

#endif // UNICOMM_SSL

//-----------------------------------------------------------------------------
bool unicomm::communicator::connected(void) const
{
  // fixme: how to find out whether socket connected, not using flags?
  // it seems it's impossible with sockets.
  return socket().is_open() && _connected;
}

//-----------------------------------------------------------------------------
void unicomm::communicator::connected(bool c)
{
  _connected = c;
}

//-----------------------------------------------------------------------------
void unicomm::communicator::disconnect(void)
{
  try
  {
    if (connected())
    {
      connected(false);
      boost::system::error_code err;
      socket().shutdown(tcp::socket::shutdown_both, err);

      UNICOMM_DEBUG_OUT("[unicomm::communicator]: SOCKET SHUTDOWN; comm ID = " 
        << dec << id() << ", error_code: " << err << "; " << err.message())
    }

    close_socket();
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Disconnect has risen an "
      << "unexpected exception; comm ID = " << dec << id())

    BOOST_ASSERT(!"Disconnect has risen an unexpected exception;");
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::disconnect(void) const
{
  owner().disconnect_one(id());
}

//-----------------------------------------------------------------------------
unicomm::commid_type unicomm::communicator::id(void) const
{
  return _id;
}

//-----------------------------------------------------------------------------
tcp::endpoint unicomm::communicator::remote_endpoint(void) const
{
  boost::system::error_code err;

  return socket().remote_endpoint(err);
}

//-----------------------------------------------------------------------------
tcp::endpoint unicomm::communicator::local_endpoint(void) const
{
  boost::system::error_code err;

  return socket().local_endpoint(err);
}

//-----------------------------------------------------------------------------
bool unicomm::communicator::is_session_valid(void) const
{
  mutex_type::scoped_lock lock(_session_mutex);

  return _user_session != 0;
}

//-----------------------------------------------------------------------------
unicomm::session_base& unicomm::communicator::session(void) const
{
  if (!is_session_valid())
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Session object is invalid, " 
      << "comm ID = " << id())

    throw invalid_session("The user session object is invalid, comm ID [" + 
      lexical_cast<string>(id()) + "]");
  }

  return user_session();
}

//-----------------------------------------------------------------------------
const unicomm::config& unicomm::communicator::config(void) const
{
  return *_config;
}

//-----------------------------------------------------------------------------
unicomm::dispatcher& unicomm::communicator::owner(void) const
{
  return _owner;
}

#ifdef UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// ssl
void unicomm::communicator::asio_handshake_handler(
  const boost::system::error_code& error)
{
  communicator::handle_handshake(error);
}

#else // UNICOMM_SSL

//-----------------------------------------------------------------------------
void unicomm::communicator::asio_success_connect_handler(void)
{
  just_connected(true);
}

#endif // UNICOMM_SSL

#ifdef UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
void unicomm::communicator::fork_parent(void) const
{
  owner().fork_parent();
}

//-----------------------------------------------------------------------------
void unicomm::communicator::fork_parent(void)
{
  is_notify_upper(false);
  close_socket();
}

//-----------------------------------------------------------------------------
void unicomm::communicator::fork_prepare(void) const
{
  owner().fork_prepare();
}

//-----------------------------------------------------------------------------
void unicomm::communicator::fork_child(void) const
{
  owner().fork_child();
}

#endif // UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
void unicomm::communicator::unreg_message_timeout(messageid_type id)
{
  messages_timeouts_map_type::iterator it = _mes_timeouts.find(id);

  if (it != _mes_timeouts.end())
  {
    unreg_message_timeout(it);
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::unreg_message_timeout(
  messages_timeouts_map_type::iterator it)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: UNREGISTER MESSAGE TIMEOUT; comm ID = " 
    << dec << id() << "; message ID = " << it->first << "; timeout = " 
    << it->second.tout().time_out().total_milliseconds() 
    << " ms; message NAME = " << it->second.name())

  _mes_timeouts.erase(it);
}

//-----------------------------------------------------------------------------
const unicomm::communicator::message_timeout_info& 
unicomm::communicator::get_message_timeout(messageid_type mid) const
{
  messages_timeouts_map_type::const_iterator cit = _mes_timeouts.find(mid);

  if (cit == _mes_timeouts.end())
  {
    throw runtime_error("Requested message is not found");
  }

  return cit->second;
}

//////////////////////////////////////////////////////////////////////////
// sent messages
void unicomm::communicator::reg_sent_message(messageid_type int_mid)
{
  try
  {
    const prepeared_message& m = get_out_buffers_item(int_mid);

    UNICOMM_DEBUG_OUT("[unicomm::communicator]: REGISTER SENT MESSAGE; comm ID = " 
      << dec << id() << "; internal ID = " << int_mid << "; message ID = " 
      << m.id() << "; message NAME = " << m.name())

    _sent_messages.push_back(sent_message_info(int_mid, m.id(), m.name()));
  }
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Unexpected std::exception [what: " 
      << e.what() << "]")

    BOOST_ASSERT(!"[unicomm::communicator]: Unexpected std::exception");
  }
}

//-----------------------------------------------------------------------------
inline unicomm::communicator::sent_messages_vector_type::iterator 
unicomm::communicator::unreg_sent_message(sent_messages_vector_type::iterator it)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: UNREGISTER SENT MESSAGE; comm ID = " 
    << dec << id() << "; internal ID = " << it->int_id() << "; message ID = " 
    << it->id() << "; message NAME = " << it->name())

  return _sent_messages.erase(it);
}

//-----------------------------------------------------------------------------
inline bool unicomm::communicator::is_sent_messages_empty(void) const
{
  return _sent_messages.empty();
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_reg_sent_message(messageid_type int_mid)
{
  try
  {
    const prepeared_message& m = mt_get_out_buffers_item(int_mid);

    UNICOMM_DEBUG_OUT("[unicomm::communicator]: REGISTER SENT MESSAGE; comm ID = " 
      << dec << id() << "; internal ID = " << int_mid << "; message ID = " 
      << m.id() << "; message NAME = " << m.name())

    mutex_type::scoped_lock lock(_sent_mes_mutex);
    _sent_messages.push_back(sent_message_info(int_mid, m.id(), m.name()));
  }
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Unexpected std::exception [what: " 
      << e.what() << "]")

    BOOST_ASSERT(!"[unicomm::communicator]: Unexpected std::exception");
  }
}

//-----------------------------------------------------------------------------
inline unicomm::communicator::sent_messages_vector_type::iterator 
unicomm::communicator::mt_unreg_sent_message(sent_messages_vector_type::iterator it)
{
  mutex_type::scoped_lock lock(_sent_mes_mutex);

  return unreg_sent_message(it);
}

//////////////////////////////////////////////////////////////////////////
// core - processors
void unicomm::communicator::mt_process_arrived(void)
{
  //UNICOMM_DEBUG_OUT("[unicomm::communicator]: DATA ARRIVED; comm ID = " 
  //  << dec << id() << "; buffer = " << _in_buffer)

  if (mt_is_in_buffer_updated() && is_session_valid())
  {
    for (const smart::timeout tout(milliseconds(config().incoming_quantum()));
      mt_is_in_buffer_updated() && !tout.elapsed(); )
    {
      if (message_base::pointer_type m = mt_perform_decode())
      {
        process_incoming(*m);
      } else
      {
        mt_is_in_buffer_updated(false);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::process_incoming(const message_base& m)
{
  is_reply(m)? process_reply(m): process_request(m);
}

//-----------------------------------------------------------------------------
void unicomm::communicator::process_timeouts(void)
{
  // fixme: limit loop by timeout also
  for (messages_timeouts_map_type::iterator it = _mes_timeouts.begin();
    it != _mes_timeouts.end(); /*++it*/)
  {
    if (it->second.tout().elapsed())
    {
      const messageid_type mid = it->first;

      UNICOMM_DEBUG_OUT("[unicomm::communicator]: MESSAGE TIMEOUT ELAPSED; comm ID = " 
        << dec << id() << "; message ID = " << mid << "; message NAME = " 
        << it->second.name())

      unreg_message_timeout(it++);

      // call timeout handler
      call_message_timeout(mid);
    } else
    {
      ++it;
    }
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_process_errors(void)
{

#ifdef UNICOMM_SSL

  mt_handle_handshake_error();

#endif // UNICOMM_SSL

  // handle any channel error
  mt_handle_ch_error();
}

////-----------------------------------------------------------------------------
//void unicomm::communicator::process_sent(void)
//{
//  BOOST_ASSERT(!is_sent_messages_empty() && 
//    " - Sent messages collection could not be empty");
//
//  for (sent_messages_vector_type::iterator it = _sent_messages.begin();
//    it != _sent_messages.end(); /*++it*/)
//  {
//    const messageid_type mid = it->id();
//    const string& name       = it->name();
//
//    // register outgoing messages id and its timeout
//    const unicomm::config& conf = config();
//    if (conf.timeouts_enabled() && conf.need_reply(name))
//    {
//      reg_message_timeout(mid, name);
//    }
//
//    UNICOMM_DEBUG_OUT("[unicomm::communicator]: MESSAGE IS SENT; comm ID = " 
//      << dec << id() << "; internal ID = " << it->int_id()
//      << "; message ID = " << mid << "; message NAME = " << name)
//
//    // remove entry
//    it = unreg_sent_message(it);
//
//    // notify upper level
//    call_message_sent(mid);
//  }
//}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_process_sent(void)
{
  mutex_type::scoped_lock lock(_sent_mes_mutex);

  BOOST_ASSERT(!is_sent_messages_empty() && 
    " - Sent messages collection could not be empty");

  // fixme: limit loop by timeout also
  for (sent_messages_vector_type::iterator it = _sent_messages.begin();
    it != _sent_messages.end(); /*++it*/)
  {
    const messageid_type mid = it->id();
    const string& name       = it->name();

    // register outgoing messages id and its timeout
    const unicomm::config& conf = config();
    if (conf.timeouts_enabled() && conf.need_reply(name))
    {
      reg_message_timeout(mid, name);
    }

    UNICOMM_DEBUG_OUT("[unicomm::communicator]: MESSAGE IS SENT; comm ID = " 
      << dec << id() << "; internal ID = " << it->int_id()
      << "; message ID = " << mid << "; message NAME = " << name)

    // remove entry
    it = unreg_sent_message(it);//_sent_messages.erase(it);

    // notify upper level
    call_message_sent(mid);
  }
}

//-----------------------------------------------------------------------------
#ifdef UNICOMM_DEBUG
void unicomm::communicator::process_received(const message_base& m, bool request)
#else // UNICOMM_DEBUG
void unicomm::communicator::process_received(const message_base& m)
#endif // UNICOMM_DEBUG
{
  const unicomm::config& conf = config();
  //const string& name          = get_name(m);
  //const bool need_reply       = conf.need_reply(name);

  message_arrived_params params = call_message_arrived(m,
    /*need_reply && */use_unique_message_id(conf)? new_mid(): undefined_messageid());

  if (message_base::pointer_type outm = params.out_message())
  {
    // mark message as reply and push it out
    BOOST_ASSERT((!use_unique_message_id(conf) || params.out_message_id() !=
      undefined_messageid()) && " - Message id could not be undefined here, a bug?");

    if (use_unique_message_id(conf) && !id_exists(*outm))
    {
      set_id(*outm, params.out_message_id());
    }

    set_rid(*outm, get_id(m));

    SMART_IFDEF_DEBUG
    (
      if (request)
      {
        UNICOMM_DEBUG_OUT(
          "[unicomm::communicator]: REPLY IS BEING PUSHED TO QUEUE; comm ID = "
          << dec << id() << "; message ID = " << get_id(m) << " ; message RID = "
          << get_rid(*outm) << "; message NAME = " << get_name(*outm))
      } else
      {
        UNICOMM_DEBUG_OUT(
          "[unicomm::communicator]: REPLY TO REPLY IS BEING PUSHED TO QUEUE; comm ID = "
          << dec << id() << "; message ID = " << get_id(m) << " ; message RID = "
          << get_rid(*outm) << "; message NAME = " << get_name(*outm))
      }
    )

    send(*outm, params.get_message_sent_handler());
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::process_request(const message_base& m)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: REQUEST MESSAGE RECEIVED; comm ID = " 
    << dec << id() << "; message ID = " << get_id(m) 
    << "; message NAME = " << get_name(m))

#ifdef UNICOMM_DEBUG
  process_received(m, true);
#else // UNICOMM_DEBUG
  process_received(m);
#endif // UNICOMM_DEBUG
}

//-----------------------------------------------------------------------------
void unicomm::communicator::process_reply(const message_base& m)
{
  const messageid_type rep_mid = get_rid(m);

  if (is_timeout_registered(rep_mid))
  {
    UNICOMM_DEBUG_OUT(
      "[unicomm::communicator]: REGISTERED REPLY MESSAGE RECEIVED; comm ID = "
      << dec << id() << "; message ID = " << get_id(m) << "; message RID = "
      << get_rid(m) << "; message NAME = " << get_name(m))

    const string& rep_name      = get_name(m);
    const unicomm::config& conf = config();
    const string& req_name      = get_message_timeout(rep_mid).name();

    if (!is_allowed_reply(conf, req_name, rep_name))
    {
      stringstream ss;
      ss << "Received reply is not allowed; message id [" << get_id(m)
        << "]; message rid [" << rep_mid << "]; request name [" << req_name
        << "]; reply name [" << rep_name << "]";

      UNICOMM_DEBUG_OUT(
        "[unicomm::communicator]: DISALLOWED REPLY MESSAGE RECEIVED; comm ID = "
        << dec << id() << "; " << ss.str())

      // received reply is not allowed by the protocol settings
      throw disallowed_reply_error(ss.str());
    }

    unreg_message_timeout(rep_mid);

    // call users handler
#ifdef UNICOMM_DEBUG
    process_received(m, false);
#else // UNICOMM_DEBUG
    process_received(m);
#endif // UNICOMM_DEBUG
  } else
  {
    // throw received message out
    UNICOMM_DEBUG_OUT(
      "[unicomm::communicator]: UNREGISTERED REPLY MESSAGE RECEIVED; comm ID = "
      << dec << id() << "; message ID = " << get_id(m) << "; message RID = "
      << get_rid(m) << "; message NAME = " << get_name(m))
    // fixme: add handler to this case?
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::process_connect(void)
{
  handle_connected_success();
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_process(void)
{
//#ifdef UNICOMM_DEBUG
//
//  struct _
//  {
//    static string session_name(const unicomm::communicator& comm)
//    {
//      return (comm.is_session_valid()? 
//        (comm.user_session().is_server()? "[server]": "[client]"): 
//        "[session is invalid]");
//    }
//  };
//
//#endif // UNICOMM_DEBUG

  //UNICOMM_DEBUG_OUT("[unicomm::communicator]: Process ENTER; comm ID = "
  //  << dec << id() << _::session_name(*this))

  this_mutex_type::scoped_try_lock lock(_this_mutex);

  // process if locked
  if (lock.owns_lock())
  {
    //UNICOMM_DEBUG_OUT("[unicomm::communicator]: Process OBJECT LOCKED; comm ID = "
    //  << dec << id() << _::session_name(*this))
    // process connected
    if (just_connected())
    {
      just_connected(false);
      process_connect();
    }

    // the order of sent processing matters
    // request data from one of previous iterations is sent
    if (mt_is_write_completed_successfully())
    {
      mt_process_sent();
    }

    // try to process arrived data if there is something to process
    if (mt_is_arrived())
    {
      mt_process_arrived();
    }

    // try to start writing if necessary - if there is something to write
    if (is_ready_to_write())
    {
      mt_start_write();
    }

    // consider timeouts
    if (config().timeouts_enabled())
    {
      process_timeouts();
    }

    // process errors
    mt_process_errors();

    // call after processed handler
    call_after_processed();

    //UNICOMM_DEBUG_OUT("[unicomm::communicator]: Process ABOUT TO UNLOCK OBJECT; comm ID = "
    //  << dec << id() << _::session_name(*this))
  }

  //UNICOMM_DEBUG_OUT("[unicomm::communicator]: Process EXIT; comm ID = "
  //  << dec << id() << _::session_name(*this))
}

//-----------------------------------------------------------------------------
unicomm::messageid_type unicomm::communicator::send(const message_base &message)
{
  return prepare_to_write(message);
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::communicator::send(const message_base &message) const
{
  return owner().send_one(id(), message);
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::communicator::send(const message_base &message, 
                            const message_sent_handler_type& handler)
{
  const unicomm::messageid_type mid = send(message);

  if (handler)
  {
    session().reg_messsage_sent(mid, handler);
  }

  return mid;
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::communicator::send(const message_base &message, 
                            const message_sent_handler_type& handler) const
{
  return owner().send_one(id(), message, handler);
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::communicator::prepare_to_write(const message_base &message)
{
  // fixme: const_cast, how to avoid?
  message_base& m = const_cast<message_base&>(message);

  // set message id
  messageid_type mid = get_id(m);//undefined_messageid();

  const unicomm::config& conf = config();
  // adjust message id and priority only if timeouts enabled
  if (use_unique_message_id(conf) && !id_exists(m))
  {
    set_id(m, mid = new_mid());
  }

  // set message priority accordingly to protocol settings
  // if it wasn't set before
  if (conf.use_default_message_priority() && 
    is_undefined_priority(get_priority(m)))
  {
    set_priority(m, conf.message_priority(get_name(m)));
  }

  //const out_buffer_type& out_buffer = 
  //  conf.message_encoder().perform_encode(m, session());
  const out_buffer_type out_buffer = 
    conf.message_encoder().perform_encode(m, session());

  push_prepeared_message(
    prepeared_message(mid, get_name(m), get_priority(m), out_buffer));

  UNICOMM_DEBUG_OUT(
    "[unicomm::communicator]: MESSAGE IS PUSHED TO QUEUE; comm ID = " 
    << dec << id() << "; message ID = " << mid << "; message RID = " 
    << (rid_exists(m)? lexical_cast<string>(get_rid(m)): string("<none>"))
    << "; message NAME = " << get_name(m))

  return mid;
}

//////////////////////////////////////////////////////////////////////////
// boost asio manipulators
//void unicomm::communicator::start_read(void)
//{
//  //////////////////////////////////////////////////////////////////////////
//  // don't realize how to make it work as i want
//  // leave it in peace...
//  // fixme: to think again and try to realize
//
//  //streambuf_ptr sb(new streambuf_ptr::value_type);
//
//  //boost::asio::async_read_until(_socket, *sb, messageend,
//  //  boost::bind(&communicator::asio_read_handler, this,
//  //    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, sb));
//
//  //boost::asio::async_read_until(_socket, boost::asio::buffer(_local_buffer), messageend,
//  //  boost::bind(&communicator::asio_read_handler, this,
//  //    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
//
//  _socket.async_read_some(boost::asio::buffer(_local_buffer.c_array(), _local_buffer.static_size),
//    boost::bind(&communicator::asio_read_handler, shared_from_this(),
//      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
//
//}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_start_read(void)
{
  local_buffer_ptr_type buf_ptr(new local_buffer_type());
  //buf_ptr->assign(0);

  _socket.async_read_some(boost::asio::buffer(buf_ptr->c_array(), 
    local_buffer_type::static_size), 
    boost::bind(&communicator::mt_asio_read_handler, shared_from_this(),
      boost::asio::placeholders::error, 
      boost::asio::placeholders::bytes_transferred, buf_ptr));
}

//-----------------------------------------------------------------------------
bool unicomm::communicator::is_prepeared_message_queue_empty(void) const
{
  return _prepeared_m_queue.empty();
}

//-----------------------------------------------------------------------------
bool unicomm::communicator::is_ready_to_write(void) const
{
  return !is_prepeared_message_queue_empty();
}

//-----------------------------------------------------------------------------
void unicomm::communicator::push_prepeared_message(const prepeared_message& m)
{
  _prepeared_m_queue.push(m);
}

//-----------------------------------------------------------------------------
unicomm::communicator::prepeared_message 
unicomm::communicator::pop_prepeared_message(void)
{
  const prepeared_message m = _prepeared_m_queue.front_copy();
  // immediately remove the message from the queue
  _prepeared_m_queue.pop();

  return m;
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_start_write(void)
{
  for (const smart::timeout tout(milliseconds(config().outgoing_quantum()));
    is_ready_to_write() && !tout.elapsed(); )
  {
    // start writing
    const prepeared_message m = pop_prepeared_message();

    BOOST_ASSERT((!use_unique_message_id(config()) || 
      m.id() != undefined_messageid()) && 
      " - Message identifier is undefined. Shouldn't have been.");
    BOOST_ASSERT((!config().use_default_message_priority() || 
      m.priority() != undefined_priority()) && 
      " - Message priority is undefined. Shouldn't have been.");

    // put message into outgoing buffers
    const messageid_type int_mid = mt_out_buffers_insert(m);
    const string& s              = mt_get_out_buffers_item(int_mid).out_buffer();

    BOOST_ASSERT(!s.empty() && " - Output buffer can't be empty");
    // start asio async write
    boost::asio::async_write(_socket, boost::asio::buffer(s),
      boost::bind(&communicator::mt_asio_write_handler, shared_from_this(), 
      boost::asio::placeholders::error, int_mid));
  }
}

//////////////////////////////////////////////////////////////////////////
// boost asio handlers
void unicomm::communicator::mt_asio_read_handler(
  const boost::system::error_code& error, 
  size_t n, const local_buffer_ptr_type& buf_ptr)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Async read completed invoked; comm ID = " 
    << std::dec << id())

  // read error mutex
  mutex_type::scoped_lock lock(_read_err_mutex);

  _read_error = error;
  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Read channel error; comm ID = " 
      << std::dec << id() << "; [" << error << "; " << error.message() << "]")
  } else
  {
    // in buffer mutex
    //in_buffer_mutex_type::scoped_lock lock(_in_buf_mutex);

    // at first, start next read operation anyway
    generic_scoped_sentinel sentry(boost::bind(&communicator::mt_start_read, this));
    // get data
    BOOST_ASSERT(n <= buf_ptr->size() && " - Buffer overrun detected");

    _in_buffer.append(string(buf_ptr->begin(), 
      buf_ptr->begin() + std::min(n, buf_ptr->size())));
    mt_is_in_buffer_updated(true);
  }

  // tell to process either error or incoming data
  kick_dispatcher();

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Async read completed finished; " 
    << "comm ID = " << std::dec << id())
}

//-----------------------------------------------------------------------------
void unicomm::communicator::mt_asio_write_handler(
  const boost::system::error_code& error, 
  messageid_type int_mid)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Async write completed invoked; " 
    << "comm ID = " << std::dec << id())

  // write error mutex
  mutex_type::scoped_lock lock(_write_err_mutex);

  _write_error = error;
  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: Write channel error; comm ID = " 
      << std::dec << id() << "; [" << error << "; " << error.message() << "]")
    // tell to process
    kick_dispatcher();
  } else
  {
    // push message id into messages sent collection on success
    mt_reg_sent_message(int_mid);
  }

  // anyway erase out buffer, doesn't throw
  mt_out_buffers_erase(int_mid);

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Async write completed finished; "
    << "comm ID = " << std::dec << id())
}

//-----------------------------------------------------------------------------
void unicomm::communicator::handle_connected_success(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Connected handler invoked; "
    << "comm ID = " << std::dec << id() << "; remote ep = " << remote_endpoint())
  // start read socket anyway but after every operation is finished
  scoped_sentinel sentry(boost::bind(&communicator::mt_start_read, this)); // no throw
  // state is connected now
  connected(true); // no throw

#ifdef UNICOMM_FORK_SUPPORT
  if (is_notify_upper())
  {
#endif // UNICOMM_FORK_SUPPORT

    // at first create user session object
    create_user_session();
    // notify upper level
    call_connected();

#ifdef UNICOMM_FORK_SUPPORT
  }
#endif // UNICOMM_FORK_SUPPORT

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: Successfully connected; comm ID = " 
    << std::dec << id() << "; remote ep = " << remote_endpoint())
}

#ifdef UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// ssl
void unicomm::communicator::handle_handshake(const boost::system::error_code& error)
{
  UNICOMM_DEBUG_OUT("[unicomm::communicator]: SSL Handshake handler invoked; comm ID = " 
    << std::dec << id() << "; [" << error << "; " << error.message() << "]")

  _handshake_error = error;
  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::communicator]: SSL Handshake failed; comm ID = " 
      << std::dec << id() << "; [" << error << "; " << error.message() << "]")
  } else
  {
    just_connected(true);
  }

  UNICOMM_DEBUG_OUT("[unicomm::communicator]: SSL Handshake handler finished; comm ID = " 
    << std::dec << id() << "; [" << error << "; " << error.message() << "]")
}

#endif // UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// handlers callers
void unicomm::communicator::call_message_timeout(messageid_type mid) const
{
  user_session().signal_message_timeout(message_timeout_params(*this, mid));
}

//-----------------------------------------------------------------------------
void unicomm::communicator::call_message_sent(messageid_type mid) const
{
  user_session().signal_message_sent(message_sent_params(*this, mid));
}

//-----------------------------------------------------------------------------
void unicomm::communicator::call_after_processed(void) const
{
  if (is_session_valid())
  {
    user_session().signal_after_processed(after_processed_params(*this));
  }
}

//-----------------------------------------------------------------------------
void unicomm::communicator::call_connected(void) const
{
  user_session().signal_connected(connected_params(*this, _in_buffer));
}

//-----------------------------------------------------------------------------
unicomm::message_arrived_params 
unicomm::communicator::call_message_arrived(const message_base& m, 
                                            messageid_type out_mes_id) const
{
  message_arrived_params params(*this, m, out_mes_id);

  user_session().signal_message_arrived(params);

  return params;
}

//////////////////////////////////////////////////////////////////////////
// misc
bool unicomm::communicator::mt_is_in_buffer_empty(void) const
{
  return _in_buffer.empty();
}

//-----------------------------------------------------------------------------
unicomm::message_base::pointer_type unicomm::communicator::mt_perform_decode(void)
{
  return config().message_decoder().perform_decode(_in_buffer, session());
}

//-----------------------------------------------------------------------------
inline bool unicomm::communicator::mt_is_sent_messages_empty(void) const
{
  mutex_type::scoped_lock lock(_sent_mes_mutex);

  return is_sent_messages_empty();
}
