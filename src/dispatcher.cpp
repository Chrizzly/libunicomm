///////////////////////////////////////////////////////////////////////////////
// dispatcher.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Dispathcer. Multithread supporting communication processor.
// Manages communicator objects. Creates and destroy
// them when it's necessary. Communicator objects are destroyed 
// when disconnect occurs.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.

#include <unicomm/dispatcher.hpp>
#include <unicomm/config.hpp>
#include <unicomm/except.hpp>
#include <unicomm/comm.hpp>
#include <unicomm/detail/helper_detail.hpp>

#include <smart/debug_out.hpp>
#include <smart/scoped_sentinel.hpp>
#include <smart/utils.hpp>
#include <smart/timers.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/assert.hpp>
#include <boost/ref.hpp>
//#include <boost/thread.hpp>

#include <stdexcept>
#include <algorithm>
#include <iostream>

#ifdef UNICOMM_FORK_SUPPORT
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#endif // UNICOMM_FORK_SUPPORT

using std::cerr;
using std::endl;
using std::runtime_error;
using std::for_each;
using std::make_pair;
using std::string;
using std::vector;

using boost::shared_ptr;
using boost::asio::ip::address_v4;
using boost::asio::ip::tcp;
using boost::recursive_mutex;
using boost::mutex;
using boost::posix_time::milliseconds;
using boost::asio::ip::tcp;
using boost::posix_time::time_duration;
using boost::posix_time::seconds;
using boost::asio::deadline_timer;
//using boost::this_thread::get_id;

using smart::generic_scoped_sentinel;
using smart::scoped_sentinel;

//////////////////////////////////////////////////////////////////////////
// auxiliary
namespace 
{

// could not bind overloads (
//void client_erase_by_key(unicomm::dispatcher::comm_container_type& clients, unicomm::commid_type id)
//{
//  SMART_IFDEF_DEBUG(const bool ok = ) clients.erase(id) SMART_IFDEF_DEBUG(!= 0);
//
//  BOOST_ASSERT(ok && " - Client to be erased is not found");
//
//  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Client erased; comm ID = " << id << ", ok = " << ok)
//}

//////////////////////////////////////////////////////////////////////////
// debug aux
//#ifdef UNICOMM_DEBUG
//
//void debug_out_iteration(void)
//{
//  static size_t i = 0;
//  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: ITERATION: " << i++);
//}
//
//#endif // UNICOMM_DEBUG

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// unicomm dispatcher
unicomm::dispatcher::dispatcher(const unicomm::config& config):
  _endpoint(config.endpoint()),
  _config(config),
  //_wait_on_stop(default_wait_on_stop()),
  _new_commid(0),
  _is_working(true),
  _run_count(0),
  _kick_count(0)
{
  constructor();
}

//-----------------------------------------------------------------------------
unicomm::dispatcher::dispatcher(const unicomm::config& config, 
                                const tcp::endpoint &endpoint):
  _endpoint(endpoint),
  _config(config),
  //_wait_on_stop(default_wait_on_stop()),
  _new_commid(0),
  _is_working(true),
  _run_count(0),
  _kick_count(0)
{
  constructor();
}

//-----------------------------------------------------------------------------
unicomm::dispatcher::~dispatcher(void)
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
void unicomm::dispatcher::disconnect_one(commid_type id)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Request to disconnect client id: " << id)

  actually_disconnect_client(id);
  // request to process
  kick_dispatcher();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::disconnect_all(void)
{
  actually_disconnect_all();
  // request to process
  kick_dispatcher();
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::dispatcher::send_one(commid_type commid, const message_base& message) 
{
  const messageid_type mid = comm(commid)->send(message);
  // to make it to be processed as soon as possible by the dispatcher
  kick_dispatcher();

  return mid;
}

//-----------------------------------------------------------------------------
unicomm::messageid_type 
unicomm::dispatcher::send_one(commid_type commid, const message_base& message, 
                              const message_sent_handler_type& handler)
{
  const messageid_type mid = comm(commid)->send(message, handler);
  // to make it to be processed as soon as possible by the dispatcher
  kick_dispatcher();

  return mid;
}

//-----------------------------------------------------------------------------
unicomm::full_messageid_map_type 
unicomm::dispatcher::send_all(const unicomm::message_base& message) 
{
  const full_messageid_map_type mids = clients().send_all(message);
  // to make it to be processed as soon as possible by the dispatcher
  kick_dispatcher();

  return mids;
}

//-----------------------------------------------------------------------------
unicomm::full_messageid_map_type 
unicomm::dispatcher::send_all(const message_base& message, 
                              const message_sent_handler_type& handler)
{
  const full_messageid_map_type mids = clients().send_all(message, handler);
  // to make it to be processed as soon as possible by the dispatcher
  kick_dispatcher();

  return mids;
}


//-----------------------------------------------------------------------------
tcp::endpoint unicomm::dispatcher::remote_endpoint(commid_type commid) const
{
  return comm(commid)->remote_endpoint();
}

//-----------------------------------------------------------------------------
boost::asio::ip::tcp::endpoint 
unicomm::dispatcher::local_endpoint(commid_type commid) const
{
  return comm(commid)->local_endpoint();
}

//-----------------------------------------------------------------------------
unicomm::commid_type unicomm::dispatcher::new_commid(void)
{
  return _new_commid++;
}

//-----------------------------------------------------------------------------
unicomm::dispatcher::commid_collection_type unicomm::dispatcher::connections(void) const
{
  // fixme: line below is not correct, implementation is necessary
  return commid_collection_type();
}

//-----------------------------------------------------------------------------
size_t unicomm::dispatcher::connections_count(void) const
{
  return clients().size();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::set_after_all_processed_handler(
  const after_all_processed_handler_type& handler) 
{ 
  after_all_processed_handler(handler); 
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::clear_after_all_processed_handler(void)
{
  set_after_all_processed_handler(after_all_processed_handler_type(0));
}

//-----------------------------------------------------------------------------
const unicomm::config& unicomm::dispatcher::config(void) const 
{ 
  return _config; 
}

#ifdef UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
void unicomm::dispatcher::fork_prepare(void)
{
  ioservice().notify_fork(boost::asio::io_service::fork_prepare);
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::fork_parent(void)
{
  ioservice().notify_fork(boost::asio::io_service::fork_parent);
  
  actually_fork_support_all();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::fork_child(void)
{
  ioservice().notify_fork(boost::asio::io_service::fork_child);
}

#endif // UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
bool unicomm::dispatcher::started(void) const
{
  return is_working();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::reset(void)
{
  if (!is_working())
  {
    set_is_working();
    // recreate everything
    initialize();
    // any derived class dependent initialization should be placed here
    on_reset();
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::initialize(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Initializing...")
  // at first, create io service
  create_io_service();

#ifdef UNICOMM_SSL
  // create ssl context
  create_ssl_context();

#endif // UNICOMM_SSL

#ifdef UNICOMM_FORK_SUPPORT

  // child process exit signal
  create_signal_wait();
  start_signal_wait();
  
#endif // UNICOMM_FORK_SUPPORT

  // setup timeout
  //timeout(milliseconds(config().dispatcher_idle_tout()));
  // call derived implementation
  create_service_work();
  create_timer();
  // any derived class dependent initialization should be placed here
  //before_start();

  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Initialized")
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::finalize(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Finalizing...")

  clear_clients();
  destroy_timer();

#ifdef UNICOMM_FORK_SUPPORT

  // wait child process signal
  destroy_signal_wait();
  
#endif // UNICOMM_FORK_SUPPORT

#ifdef UNICOMM_SSL

  destroy_ssl_context();

#endif // UNICOMM_SSL

  destroy_io_service();

  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Finalized")
}

//-----------------------------------------------------------------------------
bool unicomm::dispatcher::is_clients_empty(void) const
{
  return clients().empty();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::clear_clients(void)
{
  if (!clients().empty())
  {
    clients().clear();
  }
}

//-----------------------------------------------------------------------------
bool unicomm::dispatcher::stop(const time_duration& wait)
{
  BOOST_ASSERT(started() && " - NOT started");

  bool ok = !started();

  if (is_working())
  {
    // break run loop
    clear_is_working();
    // stop services
    actually_stop(wait);
  }

  return ok;
}

//////////////////////////////////////////////////////////////////////////
// private stuff
//
// event dispatcher's handlers
void unicomm::dispatcher::actually_stop(const time_duration& wait)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Stopping...")

  try
  {
    // any finalization should be placed here (into on_stop() virtual function)
    call_on_stop();
    // disconnect all currently connected clients
    actually_disconnect_all();
    // dispatch asio events to allow waited handlers to be invoked
    for (const smart::timeout tout(wait); 
      !is_clients_empty() && !tout.elapsed(); )
    {
      // wake up threads
      //kick_dispatcher();
      // process also by itself
      //process();
      // let other tasks to be executed
      //boost::this_thread::yield();

      boost::this_thread::sleep(milliseconds(500));
    }

    //BOOST_ASSERT(clients().empty() && 
    //  " - clients collection should be empty after stop is finished");
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Clients left = " << clients().size())

    // tell io service to stop working
    destroy_service_work();
    // stop io_service
    ioservice().stop();
    // destroy services and clear clients
    if (wait_for_run_finished(wait)) { finalize(); }
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Stop event handler has risen "
      << "an std::exception [what: " << e.what() << "]")

    BOOST_ASSERT(!"Stop event handler has risen an std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Stop event handler has risen " 
      << "an UNKNOWN exception")

    BOOST_ASSERT(!"Stop event handler has risen an UNKNOWN exception");
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::actually_disconnect_client(commid_type id)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Request to disconnect client id: " << id)

  disconnect_client(*comm(id));
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::process_clients(void)
{
  vector<commid_type> comms;
  comms.reserve(0x80);

  while (comm_container_type::comm_ptr comm = clients().take_out())
  {
    comms.push_back(comm->id());

    process_client(*comm);
  }

  // move clients back
  for_each(comms.begin(), comms.end(), 
    boost::bind(&comm_container_type::get_back, &clients(), _1));

  // move all clients back
  //clients().put_in_all();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::process_client(communicator& comm)
{
  try
  {
    // fixme: remove comment
    //UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Processing communicator; comm ID = " 
    //  << comm.id() << "; thread ID = " << std::hex << boost::this_thread::get_id())

    comm.mt_process();
    // if there is still unprocessed data process it as soon as possible
    if (comm.mt_is_unprocessed_incoming() || comm.mt_is_unprocessed_outgoing()) 
    { 
      kick_dispatcher();
    }
  } 
  catch (const disconnected_error& e)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Disconnected error exception; comm ID = " 
      << comm.id() << " [what: " << e.what() << ", reason: " << e.code() 
      << "; " << e.code().message() << "]")

    handle_disconnected(comm, e);
  }
  catch (const session_creation_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Session creation error exception; comm ID = " 
      << comm.id() << " [what: " << e.what() << "]")

    comm.disconnect(); // communicator will be destroyed by disconnected handler
  }

#ifdef UNICOMM_SSL

  catch (const handshake_error& e)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Handshake error exception; comm ID = " << comm.id() << " [what: " << e.what() << ", reason: " << e.code() << "; " << e.code().message() << "]")
    
    handle_disconnected(comm, e);
  }

#endif // UNICOMM_SSL

  catch (const communication_error& e)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Communication error exception [what: " << e.what() << ", reason: " << e.code() << "; " << e.code().message() << "]")

    handle_error(comm, e.what());
  }
  catch (const std::exception& e)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Process client has risen an std::exception [" << e.what() << "]")

    handle_error(comm, e.what()); 
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Process client has risen an UNKNOWN exception!")

    BOOST_ASSERT(!"Process client has risen an UNKNOWN exception!");

    handle_error(comm, "Unknown error!"); 
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::create_service_work(void)
{
  _service_work.reset(new service_work_ptr_type::element_type(ioservice()));
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::destroy_service_work(void)
{
  _service_work.reset();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::create_timer(void)
{
  _timer.reset(new deadline_timer(ioservice()));

  redeem_timer();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::redeem_timer(void)
{
  const size_t tout = config().dispatcher_idle_tout();
  if (!is_infinite_timeout(tout))
  {
    timer().expires_from_now(milliseconds(tout));
    timer().async_wait(boost::bind(&dispatcher::timer_handler, this, _1));
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::destroy_timer(void)
{
  _timer.reset();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::timer_handler(const boost::system::error_code& error)
{
  if (error)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Timer error: " 
      << error.message() << "; " << error)
  } else
  {
    kick_dispatcher();
    redeem_timer();
  }
}

//-----------------------------------------------------------------------------
bool unicomm::dispatcher::remove_client(commid_type id)
{
  const bool ok = clients().erase(id);

  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Client erased; comm ID = " 
    << id << ", ok = " << ok)

  return ok;
}

//-----------------------------------------------------------------------------
bool unicomm::dispatcher::client_exists(commid_type id) const
{
  return clients().exists(id);
}

#ifdef UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
void unicomm::dispatcher::start_signal_wait(void)
{
  signal().async_wait(boost::bind(&dispatcher::handle_signal_wait, this));
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::handle_signal_wait(void)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: SIGCHLD occurred")
  
  // Reap completed child processes so that we don't end up with zombies.
  int status = 0;
  while (waitpid(-1, &status, WNOHANG) > 0) 
  {
    // empty
  }

  start_signal_wait();
  
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: SIGCHLD finished")
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::create_signal_wait(void)
{
  _signal.reset(new signal_set_ptr_type::value_type(ioservice(), SIGCHLD));
}
	
//-----------------------------------------------------------------------------
void unicomm::dispatcher::destroy_signal_wait(void)
{
  _signal.reset();
}

//-----------------------------------------------------------------------------
boost::asio::signal_set& unicomm::dispatcher::signal(void)
{
  return *_signal;
}

//-----------------------------------------------------------------------------
boost::asio::signal_set& unicomm::dispatcher::signal(void) const 
{
  return *_signal;
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::actually_fork_support_all(void)
{
  clients().fork_support_all();
}

#endif // UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
void unicomm::dispatcher::constructor(void)
{
  initialize();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::create_io_service(void)
{
  _ioservice.reset(new io_service_ptr_type::element_type);
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::destroy_io_service(void)
{
  _ioservice.reset();
}

#ifdef UNICOMM_SSL

//-----------------------------------------------------------------------------
void unicomm::dispatcher::create_ssl_context(void)
{
  _ssl_context.reset(new ssl_context_ptr_type::element_type(ioservice(), 
    boost::asio::ssl::context::sslv23));
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::destroy_ssl_context(void)
{
  _ssl_context.reset();
}

//-----------------------------------------------------------------------------
boost::asio::ssl::context& unicomm::dispatcher::ssl_context(void)
{
  return *_ssl_context;
}

#endif // UNICOMM_SSL

//-----------------------------------------------------------------------------
boost::asio::io_service& unicomm::dispatcher::ioservice(void)
{
  return *_ioservice;
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::insert_comm(const comm_ptr& comm)
{
  clients().insert(comm);
}

////-----------------------------------------------------------------------------
//void unicomm::dispatcher::inc_kick_count(void) { ++_kick_count; }

//-----------------------------------------------------------------------------
//void unicomm::dispatcher::dec_kick_count(void) { --_kick_count; }

////-----------------------------------------------------------------------------
//size_t unicomm::dispatcher::kick_count(void) const { return _kick_count; }

////-----------------------------------------------------------------------------
//bool unicomm::dispatcher::can_kick(void) const 
//{ 
//  return (_kick_count < int(run_count())? ++_kick_count, true: false);
//}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::kick_dispatcher(void) 
{ 
  try 
  {
    if (can_kick())
    {
      ioservice().post(boost::bind(&dispatcher::kick_handler, this));
    }
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: An UNEXPECTED std::exception: " 
      << e.what() << "!")

    BOOST_ASSERT(!"An UNEXPECTED std::exception!");
  } 
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: An UNKNOWN UNEXPECTED exception!")
    BOOST_ASSERT(!"An UNKNOWN UNEXPECTED exception!");
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::disconnect_client(communicator& comm)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Actually disconnecting client id: " 
    << comm.id())

  comm.disconnect();
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::actually_disconnect_all(void)
{
  clients().disconnect_all();
}

////-----------------------------------------------------------------------------
//bool unicomm::dispatcher::client_exists(commid_type id) const
//{
//  return clients().exists(id);
//}

//////////////////////////////////////////////////////////////////////////
// handler callers
void unicomm::dispatcher::call_disconnected(const communicator& comm, 
                                            const boost::system::system_error& reason)
{
  if (comm.is_session_valid())
  {
    comm.session().signal_disconnected(
      disconnected_params(comm, reason.code(), reason.what()));
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::call_error(const communicator& comm, const std::string& what)
{
  BOOST_ASSERT(comm.is_session_valid() && " - Session should be valid here");

  if (comm.is_session_valid())
  {
    comm.session().signal_error(error_params(comm, what));  
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::call_after_all_processed(void)
{
  try
  {
    detail::call_handler(after_all_processed_handler(), 
      after_all_processed_params(*this));
  } catch (...)
  {
    BOOST_ASSERT(!"Communicator manager's after processed "
      "handler has risen an exception");
  }
}

//////////////////////////////////////////////////////////////////////////
// virtual derived callers
void unicomm::dispatcher::call_on_stop(void)
{
  try
  {
    // any finalization should be placed here
    on_stop();
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Virtual before stop has " 
      << "risen a std::exception [what: " << e.what() << "]")

    BOOST_ASSERT(!"Virtual before stop has risen a std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Virtual before stop has risen "
      << "an UNKNOWN exception")

    BOOST_ASSERT(!"Virtual before stop has risen an UNKNOWN exception");
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::call_extra_process(void)
{
  try
  {
    // any finalization should be placed here
    extra_process();
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Virtual extra process has risen " 
      << "an std::exception [what: " << e.what() << "]")

    BOOST_ASSERT(!"Virtual extra process has risen a std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Virtual extra process has " 
      << "risen an UNKNOWN exception")

    BOOST_ASSERT(!"Virtual extra process has risen an UNKNOWN exception");
  }
}

//////////////////////////////////////////////////////////////////////////
// misc handlers
void unicomm::dispatcher::handle_disconnected(/*const */communicator& comm, 
                                              const boost::system::system_error& reason)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Disconnected handler invoked; comm ID = " 
    << comm.id() << " [what: " << reason.what() << ", reason: " << reason.code() 
    << ", " << reason.code().message() << "]")

  try
  {
    //if (client_exists(comm.id()) && remove_client(comm.id()))
    if (remove_client(comm.id()))
    {
      // if disconnected_error is thrown by users code 
      // it is necessary to close the connection if it's not closed yet.
      comm.disconnect();

      call_disconnected(comm, reason);
    }
  } 
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Handle disconnected has risen a " 
      << "UNEXPECTED std::exception; comm ID = " << comm.id() 
      << "[what: " << e.what() << "]")

    BOOST_ASSERT(!"Handle disconnected has risen an UNEXPECTED std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Handle disconnected rose an " 
      << "UNEXPECTED exception; comm ID = " << comm.id())

    BOOST_ASSERT(!"Handle disconnected rose an UNEXPECTED exception");
  }
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::handle_error(const communicator& comm, 
                                       const std::string& what)
{
  UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: An error has occurred; comm ID = " 
    << std::dec << comm.id() << " [" << what << "]")

  try
  {
    call_error(comm, what);
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: An assertion failure has occurred; " 
      << "comm ID = " << std::dec << comm.id() 
      << " [User's error handler has risen an exception]")

    BOOST_ASSERT(!"User's error handler has risen an exception");
  }
}

//////////////////////////////////////////////////////////////////////////
// thread safe interface to callbacks
unicomm::after_all_processed_handler_type 
unicomm::dispatcher::after_all_processed_handler(void) const
{
  return smart::get(_after_all_processed_handler, _mutex);
}

//-----------------------------------------------------------------------------
void unicomm::dispatcher::after_all_processed_handler(
  const after_all_processed_handler_type& fn)
{
  smart::set(_after_all_processed_handler, fn, _mutex);
}

//////////////////////////////////////////////////////////////////////////
// processor
//void unicomm::dispatcher::run(void)
//{
//  {
//    inc_run_count();
//    scoped_sentinel sentry(boost::bind(&dispatcher::dec_run_count, this));
//
//    while (is_working())
//    {
//      ioservice().run();
//    }
//  }
//
//  if (is_run_finished())
//  {
//    signal_run_finished();
//  }
//}

void unicomm::dispatcher::run(void)
{
  {
    inc_run_count();
    //scoped_sentinel sentry(boost::bind(&dispatcher::dec_run_count, this));

    while (is_working())
    {
      ioservice().run();
    }

    if (dec_run_count() == 0)
    {
      signal_run_finished();
    }
  }

  //if (is_run_finished())
  //{
  //  signal_run_finished();
  //}
}

//------------------------------------------------------------------------
void unicomm::dispatcher::process(void)
{
//#ifdef UNICOMM_DEBUG
//  //debug_out_iteration();
//#endif // UNICOMM_DEBUG
  //UNICOMM_DEBUG_OUT("[unicomm::dispatcher]: Clients currently connected: " << _clients.size());

  // derived extra process
  call_extra_process();
  // process clients
  process_clients();
  // post processed callback
  call_after_all_processed();
}

//------------------------------------------------------------------------
void unicomm::dispatcher::kick_handler(void)
{
  dec_kick_count();

  BOOST_ASSERT(kick_count() >= 0 && " - Kicks count can't be less then 0 (zero)");

  process();
}

//------------------------------------------------------------------------
bool unicomm::dispatcher::wait_for_run_finished(const time_duration& wait)
{
  //boost::recursive_mutex::scoped_lock lock(_run_mutex);
  boost::unique_lock<boost::mutex> lock(_run_mutex);

  return _run_cond_var.timed_wait(lock, wait, 
    boost::bind(&dispatcher::is_run_finished, this));
}

//------------------------------------------------------------------------
void unicomm::dispatcher::signal_run_finished(void)
{
  _run_cond_var.notify_one();
}
