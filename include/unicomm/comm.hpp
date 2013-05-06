///////////////////////////////////////////////////////////////////////////////
// comm.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Communication service. Represents transport layer of unicomm.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_COMM_HPP_
#define UNI_COMM_HPP_

/** @file comm.hpp Communication service definition. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/comm_buffer.hpp>
#include <unicomm/config.hpp>
#include <unicomm/session_base.hpp>
#include <unicomm/basic.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) // warning C4267: 'argument' : conversion from 'size_t' to 'DWORD', possible loss of data
# pragma warning (disable : 4244) // warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#endif // UNI_VISUAL_CPP

#include <smart/sync_objects.hpp>
#include <smart/debug_out.hpp>
#include <smart/stable_priority_queue.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <smart/timers.hpp>

#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) // warning C4267: 'argument' : conversion from 'size_t' to 'DWORD', possible loss of data
# pragma warning (disable : 4100) // unreferenced formal parameter
# pragma warning (disable : 4244) // warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
# pragma warning (disable : 4510) // warning C4510: 'boost::array<T,N>' : default constructor could not be generated
# pragma warning (disable : 4512) // warning C4512: 'boost::array<T,N>' : assignment operator could not be generated
# pragma warning (disable : 4610) // warning C4610: class 'boost::array<T,N>' can never be instantiated - user defined constructor required
# ifdef UNICOMM_SSL
#   pragma warning (disable : 4996) // warning C4996: 'strncat': This function or variable may be unsafe. Consider using strncat_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
# endif // UNICOMM_SSL
#endif // UNI_VISUAL_CPP

#include <boost/array.hpp>
#include <boost/asio.hpp>

#ifdef UNICOMM_SSL
# include <boost/asio/ssl.hpp>
#endif // UNICOMM_SSL

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <map>
#include <vector>
#include <string>
#include <functional>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

#ifdef UNICOMM_SSL

/** SSL Socket type. */
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket_type;
/** Socket type. */
typedef ssl_socket_type socket_type;
/** TCP Socket type. */
typedef socket_type::lowest_layer_type tcp_socket_type;

#else

/** Plain socket type. */
typedef boost::asio::ip::tcp::socket plain_socket_type;
/** Socket type. */
typedef plain_socket_type socket_type;
/** TCP Socket type. */
typedef socket_type tcp_socket_type;

#endif // UNICOMM_SSL

// forward
class dispatcher;

/** TCP communication service.
 *
 *  Implements communication rules. Also provides an interface for 
 *  manipulating the connection from session handlers. For those purposes
 *  const interface is used.
 */
class UNICOMM_DECL communicator : 
  public boost::enable_shared_from_this<communicator>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Communicator smart pointer type. */
  typedef comm_pointer_type comm_ptr;

#ifdef UNICOMM_SSL
  /** Asio handshake handler type. */
  typedef boost::function<void (const boost::system::error_code&)> 
    asio_handshake_handler_type;

#endif //UNICOMM_SSL

public:
#ifdef UNICOMM_SSL

  /** Constructs a communicator object.
   *
   *  @param owner Communicator's owner object.
   *  @param ioservice Asio io service object.
   *  @param context Asio ssl context object.
   *  @param config Configuration to be used by the object.
   */
  communicator(dispatcher& owner, 
               boost::asio::io_service &ioservice, 
               boost::asio::ssl::context& context, 
               const unicomm::config& config);

#else // UNICOMM_SSL

  /** Constructs a communicator object.
   *
   *  @param owner Communicator's owner object.
   *  @param ioservice Asio io service object.
   *  @param config Configuration to be used by the object.
   */
  communicator(dispatcher& owner, 
               boost::asio::io_service &ioservice, 
               const unicomm::config& config);

#endif // UNICOMM_SSL

  /** Closes connection and destroys an object. */
  virtual ~communicator(void);

public:
  /** Returns reference to internal asio socket.
   *
   *  @return A reference to TCP socket contained by the communicator.
   */
  tcp_socket_type& socket(void);

public:
  /** Whether connection is established.
   *
   *  @return Returns true if connected, otherwise returns false.
   */
  bool connected(void) const;

  /** Puts message into outgoing queue.
   *
   *  @param message Message to be sent.
   *  @return Message identifier assigned to a given message by the unicomm.
   *    Can be used within sent handler to identify the sent message.
   *
   *  @throw Throw the same as unicomm::message_base::serialize() implementation.
   *  @note Thread safe.
   *  @see unicomm::session_base::message_sent_handler().
   */
  messageid_type send(const message_base &message);

  /** Sends a message.
   *
   *  @param message Message to be sent.
   *  @return Message identifier assigned to a given message by the unicomm.
   *    Can be used within common sent handler to identify the sent message.
   *
   *  @throw Throw the same as unicomm::message_base::serialize() implementation.
   *  @note Thread safe.
   *  @see unicomm::session_base::message_sent_handler().
   */
  messageid_type send(const message_base &message) const;

  /** Puts message into outgoing queue.
   *
   *  @param message Message to be sent.
   *  @param handler Handler to be called when the message is sent. 
   *    The handler is called once when message is sent.
   *
   *  @return Message identifier assigned to a given message by the unicomm.
   *    Can be used within common sent handler to identify the sent message.
   *
   *  @throw Throw the same as unicomm::message_base::serialize() implementation.
   *  @note Thread safe.
   *  @see unicomm::session_base::message_sent_handler().
   */
  messageid_type send(const message_base &message, 
                      const message_sent_handler_type& handler);

  /** Puts message into outgoing queue.
   *
   *  @param message Message to be sent.
   *  @param handler Handler to be called when the message is sent. 
   *    The handler is called once when message is sent.
   *
   *  @return Message identifier assigned to a given message by the unicomm.
   *    Can be used within common sent handler to identify the sent message.
   *
   *  @throw Throw the same as unicomm::message_base::serialize() implementation.
   *  @note Thread safe.
   *  @see unicomm::session_base::message_sent_handler().
   */
  messageid_type send(const message_base &message, 
                      const message_sent_handler_type& handler) const;

  /** Processes outgoing messages and receives incoming if there are.
   *
   *  @throw Different types derived from std::exception are thrown.
   *    Also throws everything is thrown by user's session handlers.
   *    unicomm::disallowed_reply_error is thrown if reply is not allowed.
   *
   *  @note If user handler throws something not derived from std::exception
   *    debug will assert.
   */
  void mt_process(void);

  /** Brakes the connection.
   *
   *  Performs tcp shutdown(both) and then closes the socket.
   *  Unicomm intrinsic. Should not be used externally.
   *
   *  @note Doesn't throw.
   */
  void disconnect(void);

  /** Brakes the connection.
   *
   *  Should be used by user's session object to brake the connection
   *  associated with the communicator.
   *
   *  @note Doesn't throw.
   */
  void disconnect(void) const;

  /** Returns current object identifier.
   *
   *  @return Communicator object identifier.
   *  @note Doesn't throw.
   */
  commid_type id(void) const;

  /** Generates unique message identifier.
   *
   *  To externally set message identifier new_mid should be used 
   *  to generate it before. This guarantees uniqueness of the identifier.
   *
   *  @return Message identifier unique to this communicator object.
   *  @note Doesn't throw.
   */
  messageid_type new_mid(void) const;

  /** Returns remote endpoint that the communicator is connected to.
   *
   *  If there is no connection returns a default-constructed endpoint object.
   *
   *  @return Remote end point of the established connection.
   *  @note Doesn't throw.
   */
  boost::asio::ip::tcp::endpoint remote_endpoint(void) const;

  /** Returns local endpoint of the established connection.
   *
   *  If there is no connection returns a default-constructed endpoint object.
   *
   *  @return Local end point of the established connection.
   *  @note Doesn't throw.
   */
  boost::asio::ip::tcp::endpoint local_endpoint(void) const;

public:
  /** Whether the user session is valid within this communicator.
   *
   *  @return True if session is valid and false otherwise.
   */
  bool is_session_valid(void) const;

  /** Returns a user's session object.
   *
   *  @return A reference to an object derived from unicomm::session_base.
   *  @throw unicomm::invalid_session exception is thrown if the
   *    session object is invalid.
   *
   *  @note Session object is only valid if communicator is in connected state,
   *    because of session is created on successful connection and destroyed
   *    when disconnect occurs.
   */
  unicomm::session_base& session(void) const;

  /** Returns a configuration object associated with the communicator.
   *
   *  @return Reference to communicator's configuration object.
   *  @note This is the reference to an object specified on communicator's 
   *    owner constructor.
   *
   *  @see unicomm::client::client(), unicomm::server::server(), 
   *    unicomm::dispatcher::dispatcher().
   */
  const unicomm::config& config(void) const;

  /** Returns a reference to the communicator object's owner.
   *
   *  @return A reference to the communicator object's owner.
   */
  dispatcher& owner(void) const;

  /** Whether something income data to be processed.
   *
   *  @return Returns true if there is some income got from channel,
   *    otherwise returns false.
   */
  bool mt_is_unprocessed_incoming(void) const 
    { return !mt_is_in_buffer_empty() && mt_is_in_buffer_updated(); }

  /** Whether something outgoing data to be processed.
   *
   *  @return Returns true if there is some outgoing data in the buffer
   *    to be sent, otherwise returns false.
   */
  bool mt_is_unprocessed_outgoing(void) const 
    { return is_ready_to_write(); }

public:

  // fixme: resolve via friend declarations
#ifdef UNICOMM_SSL

  /** Used by owner (client or server) to handle connect.
   *
   *  Unicomm intrinsic.
   *
   *  @param handler Handshake handler to be called when handshake is completed.
   */
  virtual void asio_success_connect_handler(const asio_handshake_handler_type& handler) = 0;

  /** Used by owner (client or server) to handle handshake.
   *
   *  Unicomm intrinsic.
   *
   *  @param error Error information holder.
   */
  void asio_handshake_handler(const boost::system::error_code& error);

#else // UNICOMM_SSL

  /** Used by owner (client or server) to handle connect.
   *
   *  Unicomm intrinsic.
   */
  void asio_success_connect_handler(void);

#endif // UNICOMM_SSL

#ifdef UNICOMM_FORK_SUPPORT

   /** Performs operations to support fork logic.
    *
    *  Unicomm intrinsic.
    */
   void fork_parent(void) const;

   /** Performs operations to support fork logic.
    *
    *  Unicomm intrinsic.
    */
   void fork_parent(void);

  /** Prepares for fork.
   *
   *  Performs boost::asio::io_service::notify_fork(fork_prepare).
   *  Should be called before fork.
   */
  void fork_prepare(void) const;

  /** Performs boost::asio::io_service::notify_fork(fork_child). */
  void fork_child(void) const;

#endif // UNICOMM_FORK_SUPPORT

//////////////////////////////////////////////////////////////////////////
// protected stuff
protected:
  /// @{
  /** Creates session object using specified factory method by constructor.
   *
   *  @return Reference to unicomm::session_base. Actually points to
   *    user's session object.
   */
  unicomm::session_base& create_user_session(void);

#ifdef UNICOMM_SSL

  //////////////////////////////////////////////////////////////////////////
  // ssl

  /** Returns a reference to ssl socket.
   *
   *  @return A reference to ssl socket contained by unicomm::communicator.
   */
  ssl_socket_type& ssl_socket(void);

  /** Returns reference to const ssl socket.
   *
   *  @return A reference to const ssl socket contained by unicomm::communicator.
   */
  const ssl_socket_type& ssl_socket(void) const;

#endif // UNICOMM_SSL
  /// @}

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  struct prepeared_message
  {
  //////////////////////////////////////////////////////////////////////////
  // interface
  public:
    prepeared_message(messageid_type id = undefined_messageid(),
      const std::string& name = "", size_t priority = undefined_priority(),
        const out_buffer_type& out_buffer = ""):
      _id(id),
      _name(name),
      _priority(priority),
      _out_buffer(out_buffer)
    {
      // empty
    }

  public:
    messageid_type id(void) const { return _id; }
    const std::string& name(void) const { return _name; }
    size_t priority(void) const { return _priority; }
    const out_buffer_type& out_buffer(void) const { return _out_buffer; }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
  private:
    messageid_type _id;
    std::string _name;
    size_t _priority;
    out_buffer_type _out_buffer;
  };

  friend bool operator<(const communicator::prepeared_message& l, 
    const communicator::prepeared_message& r);
  friend bool operator>(const communicator::prepeared_message& l, 
    const communicator::prepeared_message& r);

  //////////////////////////////////////////////////////////////////////////
  // timeout message info
  struct message_timeout_info
  {
  //////////////////////////////////////////////////////////////////////////
  // interface
  public:
    explicit message_timeout_info(const std::string& name = "", 
                                  const smart::timeout& tout = smart::timeout()):
        _name(name), _tout(tout) { /*empty*/ }

  public:
    const std::string& name(void) const { return _name; }
    const smart::timeout& tout(void) const { return _tout; }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
  private:
    std::string _name;
    smart::timeout _tout;
  };

  //////////////////////////////////////////////////////////////////////////
  // sent message info
  struct sent_message_info
  {
  //////////////////////////////////////////////////////////////////////////
  // interface
  public:
    sent_message_info(messageid_type int_mid, 
                      messageid_type mid, 
                      const std::string& name):
      _int_mid(int_mid), _mid(mid), _name(name) { /*empty*/ }

  public:
    messageid_type int_id(void) const { return _int_mid; }
    messageid_type id(void) const { return _mid; }
    const std::string& name(void) const { return _name; }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
  private:
    messageid_type _int_mid;
    messageid_type _mid;
    std::string _name;
  };

private:
  //////////////////////////////////////////////////////////////////////////
  // typedefs

  typedef std::map<messageid_type, message_timeout_info> 
    messages_timeouts_map_type;
  typedef boost::array<char, 0x10000> local_buffer_type;
  typedef boost::shared_ptr<local_buffer_type> local_buffer_ptr_type;

  typedef smart::sync_queue<
    prepeared_message,
    smart::stable_priority_queue<
      prepeared_message,
      std::deque<prepeared_message>, std::greater<prepeared_message> >
  > prepeared_messages_queue_type;

  typedef std::map<messageid_type, prepeared_message> out_buffers_map_type;
  typedef std::vector<sent_message_info> sent_messages_vector_type;
  typedef boost::mutex mutex_type;
  typedef mutex_type this_mutex_type;
  typedef mutex_type in_buffer_mutex_type;
  typedef mutex_type out_buffer_mutex_type;

private:
  //////////////////////////////////////////////////////////////////////////
  // core - processors
  void process_arrived(void);
  void process_incoming(const message_base& m);
  //void process_sent(void);
  void process_timeouts(void);

#ifdef UNICOMM_DEBUG
  void process_received(const message_base& m, bool request);
#else // UNICOMM_DEBUG
  void process_received(const message_base& m);
#endif // UNICOMM_DEBUG

  void process_request(const message_base& m);
  void process_reply(const message_base& m);
  void process_connect(void);

  void mt_process_arrived(void);
  void mt_process_sent(void);
  void mt_process_errors(void);

  //////////////////////////////////////////////////////////////////////////
  // aux
  messageid_type prepare_to_write(const message_base &message);
  inline bool is_prepeared_message_queue_empty(void) const;
  /*inline*/ bool is_ready_to_write(void) const;
  inline void push_prepeared_message(const prepeared_message& m);
  prepeared_message pop_prepeared_message(void);

  message_base::pointer_type mt_perform_decode(void);

  //////////////////////////////////////////////////////////////////////////
  // timeouts support
  void reg_message_timeout(messageid_type mid, const std::string& name) const;
  bool is_timeout_registered(messageid_type id) const;
  void unreg_message_timeout(messageid_type id);
  void unreg_message_timeout(messages_timeouts_map_type::iterator it);
  const message_timeout_info& get_message_timeout(messageid_type mid) const;

  //////////////////////////////////////////////////////////////////////////
  // sent messages
  void reg_sent_message(messageid_type int_mid);
  inline sent_messages_vector_type::iterator unreg_sent_message(
    sent_messages_vector_type::iterator it);
  /*inline*/ bool is_sent_messages_empty(void) const;
  /*inline*/ bool mt_is_sent_messages_empty(void) const;
  void mt_reg_sent_message(messageid_type int_mid);
  inline sent_messages_vector_type::iterator mt_unreg_sent_message(
    sent_messages_vector_type::iterator it);

  void connected(bool c);

  void mt_start_read(void);

  bool mt_is_in_buffer_empty(void) const;
  // means read operation completed and there is data to process
  bool mt_is_arrived(void) const { return !mt_is_in_buffer_empty(); }

  bool is_write_completed_successfully(void) const 
    { return !is_sent_messages_empty(); }
  bool is_undefined_priority(size_t priority) const 
    { return priority == undefined_priority(); }

  bool mt_is_write_completed_successfully(void) const 
    { return !mt_is_sent_messages_empty(); }

  bool just_connected(void) const { return _just_connected; }
  void just_connected(bool jc) { _just_connected = jc; }

  bool mt_is_in_buffer_updated(void) const { return _in_buffer_updated; }
  void mt_is_in_buffer_updated(bool updated) { _in_buffer_updated = updated; }

  tcp_socket_type& get_tcp_socket(void);
  const tcp_socket_type& get_tcp_socket(void) const;
  const tcp_socket_type& socket(void) const;

  inline unicomm::session_base& user_session(void) const;

  dispatcher& owner(void) { return _owner; }

  /** Closes socket.
   *
   *  Just closes the socket unlike unicomm::communicator::disconnect() which
   *  also perform shutdown(both) operation on the socket before close it.
   */
  void close_socket(void);

#ifdef UNICOMM_FORK_SUPPORT

  bool is_notify_upper(void) const { return _is_notify_upper; }
  void is_notify_upper(bool v) { _is_notify_upper = v; }

#endif // UNICOMM_FORK_SUPPORT


  //////////////////////////////////////////////////////////////////////////
  // outgoing buffer support
  messageid_type out_buffers_insert(const prepeared_message& m);
  void out_buffers_erase(messageid_type int_mid);
  const prepeared_message& get_out_buffers_item(messageid_type mid) const;
  bool is_out_buffers_empty(void) const;

  messageid_type mt_out_buffers_insert(const prepeared_message& m);
  void mt_out_buffers_erase(messageid_type int_mid);
  const prepeared_message& mt_get_out_buffers_item(messageid_type mid) const;
  bool mt_is_out_buffers_empty(void) const;

  messageid_type new_internal_mid(void) { return _internal_mid++; }

  void mt_handle_ch_error(void);

#ifdef UNICOMM_SSL

  void handle_handshake_error(void);
  void mt_handle_handshake_error(void);
  void handle_handshake(const boost::system::error_code& error);

#endif // UNICOMM_SSL

  void handle_error(const boost::system::error_code& error, 
    const std::string& what);
  void handle_disconnected(const boost::system::error_code& reason, 
    const std::string& what);

  inline void kick_dispatcher(void);
private:
  //////////////////////////////////////////////////////////////////////////
  // boost asio manipulators
  void mt_start_write(void);

  //////////////////////////////////////////////////////////////////////////
  // boost asio handlers
  void mt_asio_read_handler(const boost::system::error_code& error, size_t n, 
    const local_buffer_ptr_type& buf_ptr);
  void mt_asio_write_handler(const boost::system::error_code& error, 
    messageid_type mid);

  void handle_connected_success(void);

private:
  //////////////////////////////////////////////////////////////////////////
  // handlers callers
  void call_message_timeout(messageid_type mid) const;
  void call_message_sent(messageid_type mid) const;
  void call_after_processed(void) const;
  void call_connected(void) const;
  message_arrived_params call_message_arrived(const message_base& m, 
    messageid_type out_mes_id) const;

private:
  // deny copying
  communicator(const communicator&);
  communicator& operator=(const communicator&);

private:
  //////////////////////////////////////////////////////////////////////////
  // data
  this_mutex_type _this_mutex;
  mutable out_buffer_mutex_type _out_buf_mutex;
  mutex_type _read_err_mutex;
  mutex_type _write_err_mutex;
  mutex_type _hshake_err_mutex;
  // access should be always synchronized (one/multi threaded)
  // due to external access is possible
  mutable mutex_type _session_mutex;
  mutable mutex_type _sent_mes_mutex;

  socket_type _socket;
  commid_type _id;
  mutable comm_buffer _in_buffer;
  local_buffer_type _local_buffer;
  sent_messages_vector_type _sent_messages;
  prepeared_messages_queue_type _prepeared_m_queue;
  out_buffers_map_type _out_buffers;
  //volatile mutable messageid_type _mesid;
  mutable boost::atomic<messageid_type> _mesid;
  mutable messages_timeouts_map_type _mes_timeouts;

  //////////////////////////////////////////////////////////////////////////
  // flags
  boost::system::error_code _read_error;
  boost::system::error_code _write_error;

#ifdef UNICOMM_SSL

  boost::system::error_code _handshake_error;

#endif // UNICOMM_SSL

  //volatile bool _connected; 
  boost::atomic<bool> _connected; 
  //volatile bool _just_connected;
  boost::atomic<bool> _just_connected;
  session_base::pointer_type _user_session;
  const unicomm::config* _config;
  dispatcher& _owner;
  //volatile bool _in_buffer_updated;
  boost::atomic<bool> _in_buffer_updated;

#ifdef UNICOMM_FORK_SUPPORT

  bool _is_notify_upper;

#endif // UNICOMM_FORK_SUPPORT

  messageid_type _internal_mid;
};

} // namespace unicomm

#endif // UNI_COMM_HPP_
