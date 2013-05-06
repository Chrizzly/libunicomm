///////////////////////////////////////////////////////////////////////////////
// dispatcher.hpp
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

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_DISPATCHER_HPP_
#define UNI_DISPATCHER_HPP_

/** @file dispatcher.hpp Communicator manager definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/basic.hpp>
#include <unicomm/except.hpp>
#include <unicomm/comm_container.hpp>

#include <smart/sync_objects.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/atomic.hpp>

#ifdef UNICOMM_FORK_SUPPORT
# include <boost/asio/signal_set.hpp>
#endif // UNICOMM_FORK_SUPPORT

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4512)  // assignment operator could not be generated
# pragma warning (disable : 4100)  // unreferenced formal parameter
# pragma warning (disable : 4267)  // warning C4267: 'argument' : conversion from 'size_t' to 'DWORD', possible loss of data
#endif // UNI_VISUAL_CPP

#include <boost/asio.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <string>
#include <vector>
#include <map>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

// forward
class config;

/** Unicomm communicator manager class. 
 *
 *  Provides communicators collection management. 
 *  This is base class for unicomm::server and unicomm::client.
 *
 *  @note By default the interface is thread safe. 
 *    It's specified explicitly if it's not.
 *
 *  @see unicomm::client, unicomm::server.
 */
class UNICOMM_DECL dispatcher : private boost::noncopyable
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Smart pointer to a communicator object. */
  typedef comm_pointer_type comm_ptr;

  /** Communicators container type. */
  typedef comm_container comm_container_type;

  /** Communicators collection type. */
  typedef comm_container_type::comm_collection_type comm_collection_type;

  /** Communicator identifiers collection type. */
  typedef comm_container_type::commid_collection_type commid_collection_type;

public:
  /** Constructs an instance of dispatcher.
   *
   *  @param config Configuration object.
   *  @note The copy of config is held.
   */
  explicit dispatcher(const unicomm::config& config);

  /** Constructs an instance of dispatcher.
   *
   *  @param config Configuration object. 
   *  @param endpoint Ip-address and port to be used. Overrides specified 
   *    by the configuration.
   *
   *  @note The copy of config is held.
   */
  dispatcher(const unicomm::config& config, 
    const boost::asio::ip::tcp::endpoint &endpoint);

public:
  /** Main dispatcher's loop. 
   *
   *  Can be called from within different threads. 
   *  Threads execute boost::io_service::run(). If there is a data for unicomm to 
   *  process one of the waited thread is woken up and takes up one of the 
   *  connections (communicators) that having data to process.
   *
   *  Each instance of the unicomm::communicator is only occupied by only one 
   *  thread at a time. It means different connections are processed in 
   *  parallel by different threads but each connection is processed by single 
   *  thread. Connections are picked up by threads accordingly to concurrency rules, 
   *  i.e. race conditions.
   */
  void run(void);

  /** Initializes the dispatcher.
   *
   *  If unicomm::dispatcher::stop() is called unicomm::dispatcher::run() 
   *  will return immediately until unicomm::dispatcher::reset() is called.
   *
   *  @note It's not necessary to call this after dispatcher construction.
   */
  void reset(void);

  /** Stops processing. 
   *
   *  It also breakes all opened connections. After stop returns
   *  there are no connections should left.
   * 
   *  @param wait Designates how long stop caller thread waits until 
   *    all the connections are disconnected.
   *
   *  @return Returns true if threads completed before wait timeout elapses
   *    or threads are NOT started. Returns false, if timeout is elapsed 
   *    and threads are not finished yet.
   *
   *  @note @b IMPORTANT! Do not call this from within session handlers.
   *
   *  @see unicomm::default_wait_on_stop().
   */
  bool stop(const boost::posix_time::time_duration& wait = default_wait_on_stop());

  /** Whether processing started. 
   *
   *  @return True if message processing is started and false instead.
   */
  bool started(void) const;

  /** Disconnects connection specified by the given identifier.
   *
   *  This causes disconnected handler is called.
   * 
   *  @throw If there is no connection found with given id 
   *    throws unicomm::session_not_found.
   *
   *  @note To track connections specify connected and 
   *     disconnected event handlers.
   *
   *  @see unicomm::session_base::connected_handler(), 
   *    unicomm::session_base::disconnected_handler().
   */
  void disconnect_one(commid_type id);

  /** Brakes all currently open connection if there are. */
  void disconnect_all(void);

  /** Sends a message to the remote side of the connection with the given id.
   *  
   *  @param commid Connection identifier which the remote side of 
   *    message should be sent to.
   *
   *  @param message Message to be sent.
   *  @return Returns message identifier assigned to message 
   *    by unicomm engine or by user.
   *    Take a look at unicomm::message_base::id().
   *    Returned message identifier is passed to message sent handler 
   *    and message timeout handler.
   *
   *  @throw unicomm::session_not_found is thrown, if there is no 
   *    such communicator found.
   *
   *  @note Message is pushed to communicator's outgoing queue and dispatcher 
   *    is signaled to send message as soon as possible. 
   *    So, if communicator is disconnected before 
   *    dispatcher process it's outgoing queue the message won't be sent. 
   *    Track such situation using disconnected and 
   *    message sent events.
   *
   *  @see unicomm::session_base::message_sent_handler(), 
   *    unicomm::message_sent_handler_type.
   */
  messageid_type send_one(commid_type commid, const message_base& message);

  /** Sends a message to the remote side of the connection with the given id.
   *  
   *  @param commid Connection identifier which the remote side of 
   *    message should be sent to.
   *
   *  @param message Message to be sent.
   *  @param handler Handler to be called when the message is actually sent.
   *  @return Returns message identifier assigned to message 
   *    by unicomm engine or by user.
   *    Take a look at unicomm::message_base::id().
   *    Returned message identifier is passed to message sent handler 
   *    and message timeout handler.
   *
   *  @throw unicomm::session_not_found is thrown, if there is no 
   *    such connection found.
   *
   *  @note Message is pushed to communicator's outgoing queue and dispatcher 
   *    is signaled to send message as soon as possible. 
   *    So, if communicator is disconnected before 
   *    dispatcher process it's outgoing queue the message won't be sent. 
   *    Track such situation using disconnected and 
   *    message sent events.
   *
   *  @see unicomm::session_base::message_sent_handler(), 
   *    unicomm::message_sent_handler_type.
   */
  messageid_type send_one(commid_type commid, const message_base& message, 
    const message_sent_handler_type& handler);

  /** Sends a message to the remote sides of all currently opened connections. 
   *  
   *  @param message Message to be sent.
   *  @return Returns unicomm::full_messageid_map_type contains connection 
   *    identifier mapped to message identifier to be sent to other side. 
   *    Take a look at unicomm::message_base::id().
   *
   *  @note Message is pushed to communicator's outgoing queue and dispatcher 
   *    is signaled to send message as soon as possible. 
   *    So, if communicator is disconnected before 
   *    dispatcher process it's outgoing queue the message won't be sent. 
   *    Track such situation using disconnected and 
   *    message sent events.
   * 
   *  @see unicomm::message_sent_handler_type, 
   *    unicomm::session_base::message_sent_handler(),
   *    unicomm::full_messageid_map_type.
   */
  full_messageid_map_type send_all(const message_base& message);

  /** Sends a message to the remote sides of all currently opened connections. 
   *  
   *  @param message Message to be sent.
   *  @param handler Handler to be called when the message is actually sent.
   *  @return Returns unicomm::full_messageid_map_type contains connection 
   *    identifier mapped to message identifier to be sent to other side. 
   *    Take a look at unicomm::message_base::id().
   *
   *  @note Message is pushed to communicator's outgoing queue and dispatcher 
   *    is signaled to send message as soon as possible. 
   *    So, if communicator is disconnected before 
   *    dispatcher process it's outgoing queue the message won't be sent. 
   *    Track such situation using disconnected and 
   *    message sent events.
   * 
   *  @see unicomm::message_sent_handler_type, 
   *    unicomm::session_base::message_sent_handler(),
   *    unicomm::full_messageid_map_type.
   */
  full_messageid_map_type send_all(const message_base& message, 
    const message_sent_handler_type& handler);

  /** Returns remote endpoint that the communicator with given id is connected to. 
   *
   *  @param commid Communication object identifier which remote endpoint is 
   *    regarded to.
   *
   *  @throw unicomm::session_not_found is thrown if there is no 
   *    communication object with given commid found.
   *
   *  @return Remote end point of the established connection.
   */
  boost::asio::ip::tcp::endpoint remote_endpoint(commid_type commid) const;

  /** Returns local endpoint that the communicator with given id has. 
   *
   *  @param commid Communication object identifier which local endpoint is 
   *    regarded to.
   *
   *  @throw unicomm::session_not_found is thrown if there is no 
   *    communication object with given commid found.
   *
   *  @return Local end point of the established connection.
   */
  boost::asio::ip::tcp::endpoint local_endpoint(commid_type commid) const;

  /** Returns an identifiers collection of currently connected communicators.
   *  
   *  Identifiers can be used with one of the unicomm::dispatcher::send_one() 
   *  or unicomm::dispatcher::disconnect_one() functions.
   *
   *  @return An identifiers collection of currently connected communicators.
   *  @note Once identifier is got it can be used to manipulate certain 
   *   connection, but note that identifier can become invalid due to 
   *   connection can be broken by e.g. remote side. 
   *   Disconnected handler is called in case of connection breaking.
   */
  commid_collection_type connections(void) const;

  /** Returns current connections count. 
   *
   *  @return Connections count currently served.
   */
  size_t connections_count(void) const;

  // fixme: Add post handler interface

public:
  /** Sets after processed handler. 
   *
   *  This handler is called after all communication objects are processed on 
   *  current iteration.
   * 
   *  Any exception thrown from this handler is caught and assert issued in debug.
   *
   *  @param handler After all communicators processed handler.
   *  @note This will be called anyway even if exception occur in any other handlers
   *    that allowed exception. This is the last called handler on current iteration.
   *
   *  @see unicomm::config::dispatcher_idle_tout().
   */
  void set_after_all_processed_handler(const after_all_processed_handler_type& handler); 

  /** Clears after all processed handler. */
  void clear_after_all_processed_handler(void);

  /** Returns configuration object is held by the dispatcher. 
   *
   *  Reference to the configuration object is passed to each 
   *  newly created communicator.
   *
   *  @return Reference to const configuration object.
   */
  const unicomm::config& config(void) const;

  // fixme: make protected, resolve via friend declaration
  /** Wakes up one of the dispatcher's thread. */
  void kick_dispatcher(void);

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:

#ifdef UNICOMM_FORK_SUPPORT

  //////////////////////////////////////////////////////////////////////////
  // fork support

  /** Prepares for fork.
   *
   *  Performs boost::asio::io_service::notify_fork(fork_prepare). 
   *  Should be called before fork.
   */
  void fork_prepare(void);

  /** Performs boost::asio::io_service::notify_fork(fork_parent). */
  void fork_parent(void);

  /** Performs boost::asio::io_service::notify_fork(fork_child). */
  void fork_child(void);

#endif // UNICOMM_FORK_SUPPORT

protected:
  /** Stops dispatcher and destroys an object. */
  ~dispatcher(void);

protected:
  /// @{

  /** @brief Retrieves smart pointer to the internal communication object 
   *    representing connection. 
   *  
   *  @param commid Communicator identifier.
   *
   *  @throw unicomm::session_not_found is thrown if there is no communicator found.
   *  @return Returns pointer to communicator object representing a connection 
   *    with given identifier.
   *
   *  @note Communicator can be disconnected while having reference to it.
   *
   *  @see unicomm::dispatcher::process_client(), unicomm::dispatcher::on_stop(),
   *    unicomm::dispatcher::extra_process().
   */
  comm_ptr comm(commid_type commid) const { return clients().get(commid); }

  /** Retrieves a copy of the communicators collection. 
   *
   *  @return Communicator objects collection.
   */
  comm_container_type::comm_collection_type communicators(void) const
  { 
    return clients().communicators(); 
  }

  /** Returns a const reference to held endpoint initialized by constructor. 
   *
   *  @return A reference to endpoint stored by communicator manager.
   */
  const boost::asio::ip::tcp::endpoint& endpoint(void) const { return _endpoint; }

  /** Returns a reference to held endpoint initialized by constructor. 
   *
   *  @return A reference to const endpoint stored by communicator manager.
   */
  boost::asio::ip::tcp::endpoint& endpoint(void) { return _endpoint; }

  /** Sets internal endpoint object. 
   *
   *  @param endpoint Endpoint to store.
   */
  void endpoint(const boost::asio::ip::tcp::endpoint& endpoint) 
    { _endpoint = endpoint; }

  /** @brief Returns a reference to boost asio io service object which 
   *    connections are served by. 
   *
   *  @return Reference to boost asio io service object.
   */
  boost::asio::io_service& ioservice(void);

#ifdef UNICOMM_SSL

  /** Returns a reference to boost asio ssl context object. 
   *
   *  @return A reference to boost asio ssl context object.
   */
  boost::asio::ssl::context& ssl_context(void);

#endif // UNICOMM_SSL

protected:
  /** Inserts communicator to the collection. 
   *
   *  Adds a communicator to the processed collection.
   *
   *  @param comm Communicator to be added.
   */
  void insert_comm(const comm_ptr& comm);

  //////////////////////////////////////////////////////////////////////////
  // misc handlers

  /** Handles disconnect error. 
   *
   *  This actually handles disconnect if unicomm::disconnected_error risen.
   *  It calls handlers and remove the communicator object from the collection.
   *
   *  @param comm Communicator to disconnect.
   *  @param reason The instance of boost::system::system_error.
   */
  void handle_disconnected(/*const */communicator& comm, 
    const boost::system::system_error& reason);

  /** Handles any other error. 
   *
   *  Actually calls corresponding handlers.
   *
   *  @param comm Communicator that causes the error.
   *  @param what An encountered error description.
   */
  void handle_error(const communicator& comm, const std::string& what);

  //////////////////////////////////////////////////////////////////////////
  // aux

  /** Creates a communicator of the specified type. 
   *
   *  Creates communicator object derived from unicomm::communicator.
   *  Used by unicomm::dispatcher's derived classes.
   *
   *  @tparam The type of the communicator to be created.
   *  @return Smart pointer to newly created communicator object.
   */
  template <typename T> 
  comm_ptr create_comm(void)
  {
#ifdef UNICOMM_SSL

    return comm_ptr(new T(*this, ioservice(), ssl_context(), config()));

#else 

    return comm_ptr(new T(*this, ioservice(), config()));

#endif // UNICOMM_SSL
  }
  /// @}

//////////////////////////////////////////////////////////////////////////
// private stuff
private:

  //////////////////////////////////////////////////////////////////////////
  // virtual stuff to be implemented by derived classes

  /** Client object processor. */
  virtual void process_client(communicator& comm);

  /** Any initialization should be placed here. 
   *
   *  @note This is called from unicomm::dispatcher::reset() by the
   *    thread that calls unicomm::dispatcher::reset(). 
   */
  virtual void on_reset(void) { /* empty */ }

  /** This is called by internal working thread. 
   *
   *  If throw debug will assert.
   */
  virtual void on_stop(void) { /* empty */ }

  /** Any extra process can be performed here. 
   *
   *  Called by unicomm's working thread.
   *
   *  @note If throws exception debug will assert. Clients processing 
   *    is anyway performed in this case because of this is called before
   *    clients' processing loop.
   */
  virtual void extra_process(void) { /* empty */ }

private:
  //////////////////////////////////////////////////////////////////////////
  // friend declarations
  #ifdef UNICOMM_SSL

  friend communicator::communicator(dispatcher& owner, 
                                    boost::asio::io_service &ioservice, 
                                    boost::asio::ssl::context& context, 
                                    const unicomm::config& config);

#else // UNICOMM_SSL

  friend communicator::communicator(dispatcher& owner, 
                                    boost::asio::io_service &ioservice, 
                                    const unicomm::config& config);

#endif // UNICOMM_SSL

#ifdef UNICOMM_FORK_SUPPORT

  friend void communicator::fork_prepare(void) const;
  friend void communicator::fork_parent(void) const;
  friend void communicator::fork_child(void) const;

#endif // UNICOMM_FORK_SUPPORT

private:
  //////////////////////////////////////////////////////////////////////////
  // thread safe interface to callbacks
  after_all_processed_handler_type after_all_processed_handler(void) const;

  void after_all_processed_handler(const after_all_processed_handler_type& fn);

private:
  //////////////////////////////////////////////////////////////////////////
  // other aux stuff
  //void init(void);
  void initialize(void);
  void finalize(void);
  bool is_clients_empty(void) const;
  void clear_clients(void);
  void create_io_service(void);
  void destroy_io_service(void);
  void disconnect_client(communicator& comm);
  void actually_disconnect_all(void);
  //bool client_exists(commid_type id) const;
  void process_clients(void);
  void set_event_handlers(void);
  void create_service_work(void);
  void destroy_service_work(void);

  /** Returns new unique communicator identifier to this manager. 
   *
   *  @return Unique communicator identifier to this manager instance.
   *  @note Reset on start.
   */
  commid_type new_commid(void);

  boost::asio::deadline_timer& timer(void) const { return *_timer; }
  void create_timer(void);
  void redeem_timer(void);
  void destroy_timer(void);
  void timer_handler(const boost::system::error_code& error);

  /** Returns a clients collection. */
  comm_container_type& clients(void) { return _clients; }
  const comm_container_type& clients(void) const { return _clients; }
  bool remove_client(commid_type id);
  bool client_exists(commid_type id) const;
  //void deffered_remove_client(commid_type id);

  bool is_working(void) const { return _is_working; }
  void is_working(bool b) { _is_working = b; }
  void set_is_working(void) { is_working(true); }
  void clear_is_working(void) { is_working(false); }

  //void inc_kick_count(void);
  void dec_kick_count(void) { --_kick_count; }
  size_t kick_count(void) const { return _kick_count; }
  bool can_kick(void) const 
    { return (_kick_count < run_count()? ++_kick_count, true: false); }

  void inc_run_count(void) { boost::mutex::scoped_lock lock(_run_mutex);  ++_run_count; }
  int dec_run_count(void) 
    { boost::mutex::scoped_lock lock(_run_mutex); return --_run_count; }
  int run_count(void) const { return _run_count; }
  bool is_run_finished(void) const { return run_count() == 0; }

  bool wait_for_run_finished(const boost::posix_time::time_duration& wait);
  void signal_run_finished(void);

#ifdef UNICOMM_FORK_SUPPORT

  void start_signal_wait(void);
  void handle_signal_wait(void);
  void create_signal_wait(void);
  void destroy_signal_wait(void);
  boost::asio::signal_set& signal(void);
  boost::asio::signal_set& signal(void) const;
  void actually_fork_support_all(void);

#endif // UNICOMM_FORK_SUPPORT

  void constructor(void);

#ifdef UNICOMM_SSL

  void create_ssl_context(void);
  void destroy_ssl_context(void);

#endif // UNICOMM_SSL

private:
  //////////////////////////////////////////////////////////////////////////
  // handler callers
  void call_disconnected(const communicator& comm, 
    const boost::system::system_error& reason);
  void call_error(const communicator& comm, const std::string& what);
  void call_after_all_processed(void);

  //////////////////////////////////////////////////////////////////////////
  // virtual derived callers
  void call_on_stop(void);
  void call_extra_process(void);

  //////////////////////////////////////////////////////////////////////////
  // processor
  void process(void);
  void kick_handler(void);

private:
  void actually_stop(const boost::posix_time::time_duration& wait);
  void actually_disconnect_client(commid_type id);

private:
  typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr_type;
  typedef smart::sync_queue<commid_type> disconnect_one_queue_type;
  typedef boost::scoped_ptr<boost::asio::io_service::work> service_work_ptr_type;
  typedef boost::scoped_ptr<boost::asio::deadline_timer> timer_ptr_type;

private:
  // mutex to synchronize an access to resources as handlers and client collection
  mutable boost::mutex _mutex;
  mutable boost::mutex _run_mutex;
  //mutable boost::recursive_mutex _run_mutex;
  //mutable boost::mutex _run_count_mutex;
  boost::condition_variable _run_cond_var;
  //boost::condition_variable_any _run_cond_var;
  // listening stuff
  io_service_ptr_type _ioservice;
  service_work_ptr_type _service_work;
  boost::asio::ip::tcp::endpoint _endpoint;

  // fork support
#ifdef UNICOMM_FORK_SUPPORT

  typedef boost::shared_ptr<boost::asio::signal_set> signal_set_ptr_type;

  signal_set_ptr_type _signal;
  
#endif // UNICOMM_FORK_SUPPORT

  // ssl 
#ifdef UNICOMM_SSL

  typedef boost::shared_ptr<boost::asio::ssl::context> ssl_context_ptr_type;

  ssl_context_ptr_type _ssl_context;

#endif // UNICOMM_SSL

  after_all_processed_handler_type _after_all_processed_handler;
  // clients collection
  //comm_container _clients;
  //mutable comm_collection_type _clients;
  comm_container_type _clients;
  // to gracefully stops the server
  disconnect_one_queue_type _disc_one_queue;
  const unicomm::config _config; 
  boost::posix_time::time_duration _wait_on_stop;
  boost::atomic<commid_type> _new_commid;
  //volatile bool _is_working;
  boost::atomic<bool> _is_working;
  //volatile size_t _run_count;
  boost::atomic_int _run_count;
  timer_ptr_type _timer;
  //mutable volatile int _kick_count;
  mutable boost::atomic_int _kick_count;
};

/** Sends given message to the specified client. 
 * 
 *  @param man Communicator manager object.
 *  @param commid Session identifier message should be sent to.
 *  @param message Message to be sent to the client.
 *  @throw Throws the same as thrown by unicomm::dispatcher::send_one().
 *  @see unicomm::dispatcher::send_one().
 */
inline messageid_type send_one(dispatcher& man, commid_type commid, 
                               const message_base& message) 
{ 
  return man.send_one(commid, message); 
}

/** Sends given message to the specified client. 
 * 
 *  @param man Communicator manager object.
 *  @param commid Session identifier message should be sent to.
 *  @param message Message to be sent to the client.
 *  @param handler Message sent handler to be called when the message is sent.
 *  @throw Throws the same as thrown by unicomm::dispatcher::send_one().
 *  @see unicomm::dispatcher::send_one().
 */
inline messageid_type send_one(dispatcher& man, commid_type commid, 
                               const message_base& message, 
                               const message_sent_handler_type& handler)
{
  return man.send_one(commid, message, handler);
}

/** Sends given message to all currently connected clients. 
 *
 *  @param man Server object responsible for communication.
 *  @param message Message to be sent to all currently connected clients.
 *  @throw Throws the same as thrown by unicomm::dispatcher::send_one().
 *  @see unicomm::dispatcher::send_all().
 */
inline full_messageid_map_type send_all(dispatcher& man, 
                                        const message_base& message) 
{ 
  return man.send_all(message); 
}

/** Sends given message to all currently connected clients. 
 *
 *  @param man Server object responsible for communication.
 *  @param message Message to be sent to all currently connected clients.
 *  @param handler Message sent handler to be called when the message is sent.
 *  @throw Throws the same as thrown by unicomm::dispatcher::send_one().
 *  @see unicomm::dispatcher::send_all().
 */
inline full_messageid_map_type send_all(dispatcher& man, 
                                        const message_base& message, 
                                        const message_sent_handler_type& handler)
{
  return man.send_all(message, handler);
}

} // namespace unicomm

#endif // UNI_DISPATCHER_HPP_
