///////////////////////////////////////////////////////////////////////////////
// config.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm configuration storage.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_CONFIG_HPP_
#define UNI_CONFIG_HPP_

/** @file config.hpp Configuration storage. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/message_decoder_base.hpp>
#include <unicomm/message_encoder_base.hpp>
#include <unicomm/session_base.hpp>
#include <unicomm/basic.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <map>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Message meta information. */
struct message_info
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Creates an object. 
   *
   *  @param name Message name.
   *  @param need_reply Whether reply needed for this message. This affects 
   *    the timeouts processing. If reply is not needed the timeout for the 
   *    send message is just ignored even if the timeouts enabled and 
   *    timeout value is specified for the message.
   *
   *  @param tout Message reply waiting timeout.
   *  @param answers Allowed answers list. Empty list allows everything.
   *  @param priority Default priority value for the message. 
   *    If not explicitly set by the user's code this value is assigned. 
   *    Only if unicomm::config::use_default_message_priority() is set.
   *
   *  @see unicomm::config::timeouts_enabled(), 
   *    unicomm::config::use_default_message_priority().
   */
  explicit message_info(const std::string &name, 
                        bool need_reply = false, 
                        size_t tout = infinite_timeout(), 
                        const strings_type& answers = strings_type(), 
                        size_t priority = undefined_priority()): 
    _name(name), 
    _need_reply(need_reply), 
    _tout(tout),
    _answers(answers),
    _priority(priority)
  { 
    /*empty*/ 
  }

public:
  /** Returns message name. 
   *
   *  @return Message name. 
   *  @see unicomm::message_base::name().
   */
  const std::string& name(void) const { return _name; }

  /** Whether reply needed for the request message. 
   *
   *  Defines whether reply to the message needed. If it is and timeouts enabled
   *  unicomm will wait for the reply within time period specified for 
   *  the request message.
   *
   *  @return True if reply for the message with the 
   *    unicomm::message_info::name() is needed otherwise returns false.
   */
  bool need_reply(void) const { return _need_reply; }

  /** Message reply waiting timeout. 
   *
   *  @return Message timeout in milliseconds. 
   *  @see unicomm::infinite_timeout().
   */
  size_t timeout(void) const { return _tout; } // ms

  /** Allowed answers. 
   *
   *  @return Strings collection with message names
   *    allowed as reply to this message.
   */
  const strings_type& answers(void) const { return _answers; }

  /** Message priority. 
   *
   *  @return Message priority which is set to the message by the unicomm
   *    when the message is about to be sent
   *    if the message has unicomm::undefined_priority().
   */
  size_t priority(void) const { return _priority; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  std::string _name;
  bool _need_reply;
  size_t _tout; // ms
  strings_type _answers;
  size_t _priority;
}; // struct message_info


/** Unicomm configuration storage class.
 *
 *  This configuration contains all necessary settings for unicomm protocol.
 *  Only few of them are required. In general those settings are optional
 *  and take default values.
 * 
 *  @see unicomm::load_from_complex_xml(). 
 */
class UNICOMM_DECL config
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs a configuration object. 
   *
   *  @param session_factory User's session factory method. 
   *  @param message_factory Messages factory.
   */
  explicit config(
    const session_base::factory_type& session_factory = session_base::factory_type(), 
    const message_base::factory_type& message_factory = message_base::factory_type());

public:
  /** Returns protocol home directory.
   *
   *  This prepends file names in configuration 
   *  such as ssl certificates' paths and etc. 
   *
   *  @return Home directory path.
   *  @note Default value is empty string.
   */
  const std::string& home_dir(void) const { return _homedir; }

  /** Returns endpoint to connect to or listen to. 
   *
   *  @return Endpoint to be used by client or server by default.
   *  @note By default address and port have 0 value (zero). 
   *    Applying to address this means any address. 
   *    Concerning port on Unix like systems the system assigns the value. 
   *    On Windows it seems it leads to error. 
   *
   *  @note Required by unicomm::load_from_complex_xml().
   */
  const boost::asio::ip::tcp::endpoint& endpoint(void) const { return _endpoint; }

  /** Returns ip-address to connect to or listen to. 
   *
   *  @return IP-address to be used by client or server by default.
   *  @note By default is 0 (zero) means any.
   *
   *  @note Required by unicomm::load_from_complex_xml().
   */
  boost::asio::ip::address ip_address(void) const { return _endpoint.address(); }

  /** Returns protocol tcp port to communicate through. 
   *
   *  @return Protocol tcp port to be used by default.
   *  @note By default is 0 (zero). On Unix like systems
   *    the system assigns the value. On Windows it leads to error.
   *
   *  @note Required by unicomm::load_from_complex_xml().
   */
  unsigned short tcp_port(void) const { return _tcp_port; }

  /** Returns protocol tcp backlog accepting queue. 
   *
   *  @return Backlog tcp parameter.
   *  @note By default is 0 (zero). If zero the system default is used.
   */
  int tcp_backlog(void) const { return _tcp_backlog; }

  /** Default messages timeout in milliseconds.
   *
   *  In case when configuration loaded from file
   *  this value is used by default for messages if there is no 
   *  timeout specified explicitly for certain message.
   *  This value is overridden by certain message settings.
   *
   *  If the message configuration is setup via unicomm::message_info structure
   *  the timeout value for the message is got from the structure. 
   *  Take a look at unicomm::config::message_timeout().
   *
   *  To disable timeouts processing set unicomm::config::timeouts_enabled() to false.
   *  If you wish unicomm waits for reply infinitely for certain message 
   *  use unicomm::infinite_timeout() (default everywhere in unicomm for timeouts). 
   *
   *  @return Default message timeout to be used if not specified 
   *    by the configuration for certain message.
   * 
   *  @note Default value is unicomm::infinite_timeout().
   *  @see unicomm::infinite_timeout(), unicomm::config::message_timeout(),
   *    unicomm::config::timeouts_enabled(), unicomm::load_from_complex_xml().
   */
  size_t default_timeout(void) const { return _def_tout; }

  /** @brief If there is no priority specified for message explicitly 
   *    this value is used as default. 
   *
   *  In case when configuration loaded from file
   *  this value is used by default for messages if there is no 
   *  priority specified explicitly for certain message.
   *  This value is overridden by certain message settings.
   *
   *  If the message configuration is setup via unicomm::message_info structure
   *  the priority value for the message is got from the structure. 
   *  Take a look at unicomm::config::message_priority().
   *
   *  @return Default message priority to be used if not specified 
   *    by the configuration for certain message.
   * 
   *  @note Default value is unicomm::undefined_priority().
   *  @see unicomm::undefined_priority(), unicomm::config::message_priority(),
   *    unicomm::config::use_default_message_priority(), unicomm::load_from_complex_xml().
   */
  size_t default_priority(void) const { return _def_priority; }

  /** This option sets message format to be used to communicate. 
   *
   *  This only used by unicomm::setup_message_format() and unicomm::load_from_complex_xml() 
   *  functions.
   *
   *  @return Message format identifier string.
   *  @see unicomm::set_binary_message_format(), unicomm::set_xml_message_format(),
   *    unicomm::load_from_complex_xml(), unicomm::setup_message_format().
   */
  const std::string& message_format(void) const { return _message_format; }

public:
  /** Whether message with given name is configured for reply waiting.
   *
   *  If unicomm::config::need_reply() is true
   *  the reply will be waited for unicomm::config::message_timeout() period.
   *
   *  @param mesname Message name.
   *  @return True if reply is needed for the message with given name.
   *    If message name is not found returns false.
   *
   *  @note Ignored when unicomm::config::timeouts_enabled() is false.
   *  @note Default value is false.
   */
  bool need_reply(const std::string &mesname) const;

  /** Timeout value for given message name. 
   *
   *  @param mesname Message name.
   *  @return Message timeout in milliseconds. If timeouts enabled, 
   *    reply will be waited for period specified by this value. If reply is not
   *    got within this period, timeout event handler will be invoked.
   *    If message name is not found returns unicomm::config::default_timeout().
   *
   *  @note Ignored when unicomm::config::timeouts_enabled() is false and
   *    returns unicomm::infinite_timeout().
   *  
   *  @see unicomm::infinite_timeout(), unicomm::config::default_timeout(),
   *    unicomm::config::timeouts_enabled(), unicomm::load_from_complex_xml().
   */
  size_t message_timeout(const std::string &mesname) const 
    { return timeouts_enabled()? mes_info(mesname).timeout(): infinite_timeout(); }

  /** Whether timeout is used for message with the given name. 
   *
   *  @param mesname Message name.
   *  @return True if timeout is used for the message, false otherwise. 
   *    If unicomm::config::timeouts_enabled() is false always returns false,
   *    otherwise depends on timeout value for the certain message.
   *    Infinite timeout value for the message means timeout is not used. 
   *    If message name is not found the result depends on 
   *    unicomm::config::default_timeout() value.
   *  
   *  @note Default value is false.
   */
  bool message_timeout_used(const std::string &mesname) const 
    { return message_timeout(mesname) != infinite_timeout(); }

  /** Allowed answers list. 
   *
   *  Empty list means any reply is allowed. In other way only
   *  specified messages are allowed to be reply. 
   *  If received message is not allowed unicomm 
   *  will interrupt message handling and call error handler.
   *
   *  @param mesname Message name.
   *  @return Strings collection of message names are allowed to be a 
   *    reply to the message with given name. 
   *    If Message name is not found returns empty collection.
   *
   *  @note Default value is empty collection.
   *  @see unicomm::is_allowed_reply().
   */
  const strings_type& message_answers(const std::string& mesname) const 
    { return mes_info(mesname).answers(); } 

  /** Returns message priority. 
   *
   *  When message is about to be pushed into outgoing queue it's tested 
   *  for the priority. If priority has unicomm::undefined_priority() value 
   *  the unicomm::config::message_priority() is used to setup message priority.
   *  Actually, it returns the value specified by unicomm::config::message_info().
   *  If there is no message with given name unicomm::config::default_priority() 
   *  value returned. In case when configuration loaded from file and priority is 
   *  not specified for the certain message 
   *  the unicomm::config::default_priority() is used for the message.
   *
   *  @param mesname Message name.
   *  @return Returns priority for the message with given name. If message is not found
   *    returns unicomm::config::default_priority().
   *
   *  @see unicomm::undefined_priority(), unicomm::config::default_priority(),
   *    unicomm::config::use_default_message_priority(), unicomm::load_from_complex_xml().
   */
  size_t message_priority(const std::string& mesname) const 
    { return mes_info(mesname).priority(); }

  /** Returns session factory object. 
   *
   *  @return User's session objects factory.
   *  @see unicomm::communicator::session(), unicomm::session_base.
   */
  const session_base::factory_type& session_factory(void) const;

  /** Returns message factory to be used to create messages. 
   *
   *  @return A reference to messages factory object.
   */
  const message_base::factory_type& message_factory(void) const 
    { return _message_factory; }

public:
  /** This flag defines timeouts tracking state.
   *
   *  If this is false, timeouts are disabled. This means
   *  unicomm::config::need_reply() and unicomm::config::message_timeout() 
   *  are not used. If unicomm::config::timeouts_enabled() is true then 
   *  timeouts processing  for each message depends on certain 
   *  message's settings.
   *
   *  @return True if timeouts enabled and false instead.
   *  @note By default timeouts are disabled.
   *  @see unicomm::config::need_reply(), unicomm::config::message_timeout(), 
   *    unicomm::config::default_timeout().
   */
  bool timeouts_enabled(void) const { return _timeouts_enabled; }

  /** Indicates whether to use unique message identifier for every message to be sent. 
   *
   *  If this is false unique message identifiers are not used. 
   *  In this case all the message instances use 
   *  unicomm::undefined_messageid() value as identifier.
   *  Message sent handlers are always called with 
   *  unicomm::undefined_messageid() value. 
   *
   *  This option makes unicomm to check whether message identifier is set and 
   *  assign a new one to the message if it's not. How the identifier stored
   *  is fully on unicomm::message_base descendants. 
   *
   *  @return True if unique message identifier should be used, false otherwise.
   *  @note Default value is false.
   *  @see unicomm::undefined_messageid().
   */
  bool use_unique_message_id(void) const { return _use_unique_message_id; }

  /** @brief Indicates whether to use default message priority defined by 
   *  configuration for every message to be sent. 
   *
   *  If this is false priority check doesn't occur, so message priority is left as is.
   *  If this is true and message priority is equal to unicomm::undefined_priority() 
   *  default priority value (unicomm::config::message_priority()) is assigned 
   *  to the message in this case.
   *
   *  @return True if unicomm::config::message_priority() value should be used if
   *    there is no priority set for the message. 
   *    No priority means priority is set to unicomm::undefined_priority() value.
   *
   *  @note Default value is false.
   */
  bool use_default_message_priority(void) const 
    { return _use_default_message_priority; }

  /** Returns unicomm working thread sleep timeout. 
   *
   *  This value specifies how long thread sleeps when there is nothing 
   *  to send and receive.
   *
   *  @return Working thread sleep timeout between process iterations.
   *  @note Zero is infinite timeout, @see unicomm::infinite_timeout().
   */
  size_t dispatcher_idle_tout(void) const { return _working_th_sleep_tout; }

  /** Incoming message processing time quantum per client.
   *
   *  It means if there are incoming data it's only processed for this
   *  period of time and then next client is processed. So, left data for the client
   *  will be processed on next process iteration which will be immediately 
   *  executed after current finishes.
   *
   *  @return Time quantum to process incoming data for the each connection.
   *  @note Default value is 
   *    unicomm::detail::default_incoming_quantum() == 100 milliseconds.
   */
  size_t incoming_quantum(void) const { return _incoming_quantum; }

  /** Outgoing message processing time quantum per client. 
   *
   *  This period of time is taken to send data. If there are still unsent 
   *  data when this period elapsed then they will be sent on next process
   *  iteration.
   *
   *  @return Time quantum to process outgoing data for the each connection.
   *  @note Default value is 100 milliseconds.
   */
  size_t outgoing_quantum(void) const { return _outgoing_quantum; }

public:
  /** Returns message decoder object. 
   *
   *  It's also possible to set custom decoder. 
   *
   *  @return A reference to the class instance derived from 
   *    unicomm::message_decoder_base.
   *
   *  @see unicomm::message_decoder_base,
   *    unicomm::bin_message_decoder, unicomm::xml_message_decoder.
   */
  message_decoder_base& message_decoder(void) const;

  /** Returns message encoder object. 
   *
   *  It's also possible to set custom encoder. 
   *
   *  @return A reference to the class instance derived from 
   *    unicomm::message_encoder_base.
   *
   *  @see unicomm::message_encoder_base,
   *    unicomm::bin_message_encoder, unicomm::xml_message_encoder.
   */
  message_encoder_base& message_encoder(void) const;

#ifdef UNICOMM_SSL

public:
  //////////////////////////////////////////////////////////////////////////
  // ssl config interface

  /** Returns file name contains one or more trusted certification authorities. 
   *
   *  @return File name contains one or more trusted certification authorities.
   */
  const std::string& ssl_client_verify_fn(void) const 
    { return _client_verify_fn; }

  /** Returns password information about an encrypted key in PEM format. 
   *
   *  @return Password information about an encrypted key in PEM format. 
   */
  const std::string& ssl_server_key_password(void) const 
    { return _server_key_password; }

  /** Returns file name contains a certificate chain. 
   *
   *  @return File name contains a certificate chain. 
   */
  const std::string& ssl_server_cert_chain_fn(void) const 
    { return _server_cert_chain_fn; }

  /** Returns the name of the file containing a private key. 
   *
   *  @return The name of the file containing a private key.
   */
  const std::string& ssl_server_key_fn(void) const { return _server_key_fn; }

  /** Returns the name of the file containing the Diffie-Hellman parameters. 
   *
   *  @return The name of the file containing the Diffie-Hellman parameters. 
   */
  const std::string& ssl_server_dh_fn(void) const { return _server_dh_fn; }

#endif // UNICOMM_SSL

public:
  /** Sets the endpoint to be used by the engine. 
   *
   *  @param ep Endpoint to be used by default 
   *    by server or client.
   *
   *  @return *this.
   *  @note To find out more details see the unicomm::config::endpoint() getter.
   */
  config& endpoint(const boost::asio::ip::tcp::endpoint& ep);

  /** Sets the address to be used by the engine. 
   *
   *  @param address Ip-address value to be used by server or client.
   *
   *  @return *this.
   *  @note To find out more details see the unicomm::config::ip_address() getter.
   */
  config& ip_address(const boost::asio::ip::address& address);

  /** Sets a tcp port to be used by the protocol engine. 
   *
   *  @param port TCP port value to be used by default.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::tcp_port() getter.
   */
  config& tcp_port(unsigned short port);

  /** Sets a tcp backlog. 
   *
   *  @param backlog Backlog value to be used.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::tcp_backlog() getter.
   */
  config& tcp_backlog(int backlog);

  /** Sets messages default priority. 
   *
   *  @param priority Default message priority.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::default_priority() getter.
   */
  config& default_priority(size_t priority);

  /** Sets messages default timeout. 
   *
   *  @param tout Default message timeout.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::default_timeout() getter.
   */
  config& default_timeout(size_t tout);

  /** Sets timeouts enabled flag. 
   *
   *  @param enabled Timeouts state flag.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::timeouts_enabled() getter.
   */
  config& timeouts_enabled(bool enabled);

  /** Sets protocol home directory.
   *
   *  @param dir Home directory path.
   *  @return *this.
   *  @note To find out more details see the unicomm::config::home_dir() getter.
   */
  config& home_dir(const std::string& dir);

  /** This option sets certain message meta information. 
   *
   *  Makes the message to be known by the unicomm.
   *
   *  @param info Message information.
   *  @return *this.
   *  @note unicomm::config::message_priority() and etc. use that information 
   *    when it's necessary by the engine.
   */
  config& message_info(const unicomm::message_info& info);

  /** This option sets certain message meta information. 
   *
   *  Makes the message to be known by the unicomm.
   *
   *  @param name Message name.
   *  @param need_reply Whether reply needed for this message. This affects 
   *    the timeouts processing. If reply is not needed the timeout for the 
   *    send message is just ignored even if the timeouts enabled and 
   *    timeout value is specified for the message.
   *
   *  @param tout Message reply waiting timeout.
   *  @param answers Allowed answers list. Empty list allows everything.
   *  @param priority Default priority value for the message. If not 
   *    explicitly set by the user's code this value is assigned. 
   *    Only if unicomm::config::use_default_message_priority() is set.
   *
   *  @return *this.
   *
   *  @see unicomm::config::timeouts_enabled(), 
   *    unicomm::config::use_default_message_priority().
   */
  config& message_info(const std::string &name, 
                       bool need_reply = false, 
                       size_t tout = infinite_timeout(), 
                       const strings_type& answers = strings_type(), 
                       size_t priority = undefined_priority());

  /** This option sets message format to be used to communicate. 
   *
   *  @param format Message format identifier.
   *  @note To find out more details see the 
   *    unicomm::config::message_format() getter.
   *
   *  @return *this.
   */
  config& message_format(const std::string& format);

  /** @brief Indicates whether to use unique message identifier for 
   *    every message to be sent. 
   *
   *  @param use Specifies whether to use unique message identifiers.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::use_unique_message_id() getter.
   */
  config& use_unique_message_id(bool use);

  /** @brief Indicates whether to use default message priority defined by 
   *  the configuration for every message to be sent. 
   *
   *  @param use Specifies whether to default message priority 
   *    if it is not set for the message.
   *
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::use_default_message_priority() getter.
   */
  config& use_default_message_priority(bool use);

  /** Sets unicomm working thread sleep timeout. 
   *
   *  @param tout Working thread sleep timeout in milliseconds.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::dispatcher_idle_tout() getter.
   */
  config& dispatcher_idle_tout(size_t tout) 
    { _working_th_sleep_tout = tout; return *this; }

  /** Incoming message processing time quantum per client. 
   *
   *  @param quantum Time quantum to process incoming messages.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::incoming_quantum() getter.
   */
  config& incoming_quantum(size_t quantum) 
    { _incoming_quantum = quantum; return *this; }

  /** Outgoing message processing time quantum per client. 
   *
   *  @param quantum Time quantum to process outgoing messages.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::outgoing_quantum() getter.
   */
  config& outgoing_quantum(size_t quantum) 
    { _outgoing_quantum = quantum; return *this; }

  /** Sets message factory to be used to create messages. 
   *
   *  @param factory Message factory.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::message_factory() getter.
   */
  config& message_factory(const message_base::factory_type& factory);

  /** Sets session factory to be used to create messages. 
   *
   *  @param factory User's session object factory.
   *  @return *this.
   *  @note To find out more details see the 
   *    unicomm::config::session_factory() getter.
   */
  config& session_factory(const session_base::factory_type& factory);

  /** Sets custom message decoder to be used to decode messages. 
   *
   *  @param decoder Pointer to message decoder object.
   *  @return *this.
   *  @note To find out more details see the unicomm::config::message_decoder().
   */
  config& message_decoder(const message_decoder_base::pointer_type& decoder);

  /** Sets custom message encoder to be used to encode messages. 
   *
   *  @param encoder Pointer to message encoder object.
   *  @return *this.
   *  @note To find out more details see the unicomm::config::message_encoder().
   */
  config& message_encoder(const message_encoder_base::pointer_type& encoder);

#ifdef UNICOMM_SSL

  /** Sets file name contains one or more trusted certification authorities. 
   *
   *  @return *this.
   */
  config& ssl_client_verity_fn(const std::string& fn);

  /** Sets password information about an encrypted key in PEM format. 
   *
   *  @return *this. 
   */
  config& ssl_server_key_password(const std::string& password);
  
  /** Sets file name contains a certificate chain. 
   *
   *  @return *this. 
   */
  config& ssl_server_cert_chain_fn(const std::string& fn);
  
  /** Sets the name of the file containing a private key. 
   *
   *  @return *this.
   */
  config& ssl_server_key_fn(const std::string& fn);
  
  /** Sets the name of the file containing the Diffie-Hellman parameters. 
   *
   *  @return *this.
   */
  config& ssl_server_dh_fn(const std::string& fn);

#endif // UNICOMM_SSL

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  typedef std::map<std::string, unicomm::message_info> messages_map_type;

  const unicomm::message_info& mes_info(const std::string &mesname) const;

  bool message_decoder_exists(void) const;

private:
  std::string _homedir;
  std::string _conffn;
  unsigned short _tcp_port;
  boost::asio::ip::tcp::endpoint _endpoint;
  int _tcp_backlog;
  messages_map_type _messages;
  std::string _file_message_name;
  size_t _def_tout;
  size_t _def_priority;
  bool _timeouts_enabled;
  //bool _ok;
  message_decoder_base::pointer_type _mes_decoder; 
  message_encoder_base::pointer_type _mes_encoder; 
  session_base::factory_type _session_factory;
  message_base::factory_type _message_factory;
  std::string _message_format;
  bool _use_unique_message_id;
  bool _use_default_message_priority;
  size_t _working_th_sleep_tout;
  size_t _incoming_quantum;
  size_t _outgoing_quantum;

#ifdef UNICOMM_SSL

  std::string _client_verify_fn;
  std::string _server_key_password;
  std::string _server_cert_chain_fn;
  std::string _server_key_fn;
  std::string _server_dh_fn;

#endif // UNICOMM_SSL
};

/** Whether given reply name is allowed.
 *
 *  Used to test reply is either allowed by protocol configuration or not.
 *
 *  @param conf Protocol configuration object reference.
 *  @param request_name Request name that reply is received to.
 *  @param reply_name Reply name to test whether it is allowed.
 *  @return True if reply presents in answers or answers is empty, 
 *    otherwise false is returned.
 *
 *  @note Empty answers means that the reply value is allowed, 
 *    i.e. empty reply list allows any reply.
 *
 *  @see unicomm::message_info, unicomm::config::message_answers().
 */
UNICOMM_DECL bool is_allowed_reply(const config& conf, 
                                   const std::string& request_name, 
                                   const std::string& reply_name);

/** Sets up binary message format to be used by the configuration. 
 *
 *  It creates and sets up necessary decoders and encoders.
 *  
 *  @param conf Configuration object to be setup.
 *  @return conf.
 */
UNICOMM_DECL config& set_binary_message_format(config& conf);

} // namespace unicomm

#endif // UNI_CONFIG_HPP_
