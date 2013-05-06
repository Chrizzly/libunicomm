///////////////////////////////////////////////////////////////////////////////
// echo.hpp
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

#ifdef _MSC_VER
  #pragma once
#endif // _MSC_VER

#ifndef UNI_ECHO_HPP_
#define UNI_ECHO_HPP_

/** @file echo.hpp Unicomm echo sample. */

#include <unicomm/facade/bin_message.hpp>
#include <unicomm/unicomm.hpp>

#include <boost/bind.hpp>

#include <string>
#include <sstream>
#include <algorithm>

#include <cstddef>

/** @namespace echo sample namespace. */
namespace uni_echo
{

/** Synchronizes an access to the std::cout. */
void out_str(const std::string& s);

//////////////////////////////////////////////////////////////////////////
// Messages

/** Basic message class template. */
template <char Name>
struct basic_message : public unicomm::bin_message
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  // this is only made for convenience
  // strange initialization due to string constructor assumes
  // that '\0' is null-terminator.
  static const std::string& static_name(void)
    { static const std::string s = std::string() += Name; return s; }

public:
  const std::string& name(void) const { return static_name(); }

public:
  explicit basic_message(const std::string& s = ""): _data(s) { /*empty*/ }

  const std::string& data(void) const { return _data; }
  void data(const std::string& s) { _data = s; }

private:
  /** Serializes message to a std::string. */
  std::string serialize(void) const
  {
    return unicomm::bin_message::serialize().append(_data);
  }

  /** Restores message from a std::string. */
  void unserialize_tail(const std::string &tail)
  {
    _data = tail;
  }

private:
  std::string _data;
};

/** Echo request message. */
//typedef basic_message<'\0'> echo_request_message;
typedef basic_message<'1'> echo_request_message;

/** Echo reply message. */
typedef basic_message<'2'> echo_reply_message;

// client session parameters
struct client_session_params
{
  explicit client_session_params(const unicomm::strings_type& requests_list,
                                 bool is_infinite, size_t clients_num):
    _requests_list(requests_list),
    _is_infinite(is_infinite),
    _clients_num(clients_num)
  {
    // empty
  }

  const unicomm::strings_type& requests_list(void) const { return _requests_list; }
  bool is_infinite(void) const { return _is_infinite; }
  size_t clients_number(void) const { return _clients_num; }

private:
  unicomm::strings_type _requests_list;
  bool _is_infinite;
  size_t _clients_num;
};

// server session parameters
typedef unicomm::void_session_params server_session_params;

//////////////////////////////////////////////////////////////////////////
// define class that will hold user data and implement necessary logic.

template <typename SessionParamsT>
class session : public unicomm::extended_session<session<SessionParamsT>, SessionParamsT>
{
typedef unicomm::extended_session<session<SessionParamsT>, SessionParamsT> base_type;

//////////////////////////////////////////////////////////////////////////
// interface

public:
  // called on client's side
  explicit session(const unicomm::connected_params& /*params*/,
                   const SessionParamsT& extra_params):
    base_type(extra_params),
    _timeout(timeout)
  {
    // bind handlers
    set_message_arrived_handler(echo_request_message::static_name(),
      boost::bind(&session::echo_request_arrived_handler, this, _1));

    set_message_arrived_handler(echo_reply_message::static_name(),
      boost::bind(&session::echo_reply_arrived_handler, this, _1));
  }

  // called on server's side
  explicit session(const unicomm::connected_params& /*params*/):
    _timeout(timeout)
  {
    // bind handlers
    set_message_arrived_handler(echo_request_message::static_name(),
      boost::bind(&session::echo_request_arrived_handler, this, _1));

    set_message_arrived_handler(echo_reply_message::static_name(),
      boost::bind(&session::echo_reply_arrived_handler, this, _1));
  }

//////////////////////////////////////////////////////////////////////////
// private stuff

private:
  template <typename ParamsT> bool is_server(void) const;
  bool is_server(void) const { return is_server<SessionParamsT>(); }

  string name(void) const { return is_server()? "server": "client"; }

  //////////////////////////////////////////////////////////////////////////
  // assigned handlers

  // echo request handler
  void echo_request_arrived_handler(unicomm::message_arrived_params& params)
  {
    const echo_request_message& inm =
      static_cast<const echo_request_message&>(params.in_message());

    // to reply just fill the out_message() or use unicomm::send_one()
    typedef unicomm::message_base::pointer_type message_pointer_type;
    // the reply's id will be assigned params.out_message_id() returned value
    params.out_message(message_pointer_type(new echo_reply_message(inm.data())));

    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]: [" << inm.id()
      << "]:\tREQUEST: " << inm.data();
    out_str(ss.str());
  }

  // echo reply handler
  void echo_reply_arrived_handler(unicomm::message_arrived_params& params)
  {
    const echo_reply_message& inm =
      static_cast<const echo_reply_message&>(params.in_message());

    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]: [" << inm.id()
      << "]: [" << inm.rid() << "]: \tREPLY: " << inm.data();
    out_str(ss.str());
  }

  //////////////////////////////////////////////////////////////////////////
  // virtual functions

  // connected handler
  void connected_handler(const unicomm::connected_params& params)
  {
    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]:\tCONNECTED; local endpoint: "
      << params.comm().local_endpoint() << ", remote endpoint: "
      << params.comm().remote_endpoint();
    out_str(ss.str());

    // split the behavior in compile time
    side_connected_handler<SessionParamsT>(params);

    // call base (extended_session) handler if we need
    // assigned connected handlers to be called if there are
    base_type::connected_handler(params);
  }

  template <typename ParamsT>
  void side_connected_handler(const unicomm::connected_params& params);

  //----------------------------------------------------------------------
  // after processed handler
  void after_processed_handler(const unicomm::after_processed_params& params)
  {
    // called approximately within config().sleep_working_thread_tout() ms
    // zero means infinite timeout, if timeout is infinte only called
    // if there is data to be processed
    // default timeout is 50 ms.
    side_after_processed_handler<SessionParamsT>(params);
  }

  template <typename ParamsT>
  void side_after_processed_handler(const unicomm::after_processed_params& params);

  // below stuff just prints the info to the console
  // disconnected handler
  void disconnected_handler(const unicomm::disconnected_params& params)
  {
    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]:\tDISCONNECTED; "
      << params.error_code() << "; " << params.what();
    out_str(ss.str());
  }

  // message timeout handler
  void message_timeout_handler(const unicomm::message_timeout_params& params)
  {
    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]: ["
      << params.message_id() << "]:\tTIMEOUT";
    out_str(ss.str());
  }

  // error handler
  void error_handler(const unicomm::error_params& params)
  {
    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]:\tERROR: "
      << params.description();
    out_str(ss.str());

    // to obtain exception type rethrow the exception

    //try
    //{
    //  throw;
    //} catch (const my_exception& e)
    //{
    //  //...
    //}
  }

  // message is actually sent handler
  void message_sent_handler(const unicomm::message_sent_params& params)
  {
    std::stringstream ss;

    ss << name() << "> [" << params.comm().id() << "]: ["
      << params.message_id() << "]:\tSENT";
    out_str(ss.str());
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  void send_echo_request(const unicomm::communicator& comm, const std::string& data)
  {
    comm.send(echo_request_message(data));
  }

  //----------------------------------------------------------------------
  void send_echo_requests(const unicomm::communicator& comm)
  {
    const unicomm::strings_type& list = base_type::parameters().requests_list();
    std::for_each(list.begin(), list.end(),
      boost::bind(&uni_echo::session<SessionParamsT>::send_echo_request,
      this, boost::ref(comm), _1));
  }

  bool is_time(void) const { return _timeout-- == 0; }
  void reset_timeout(void) { _timeout = timeout; }
  mutable volatile size_t _timeout;

  static const size_t timeout = 30; // 1.5 seconds by default
                                    // in config::dispatcher_idle_tout() intervals
};

/** Client session type. */
typedef session<client_session_params> client_session_type;

/** Server session type. */
typedef session<server_session_params> server_session_type;

// client
template <>
template <>
inline bool client_session_type::is_server<client_session_params>(void) const 
{ 
  return false; 
}

// server
template <>
template <>
inline bool server_session_type::is_server<server_session_params>(void) const 
{ 
  return true; 
}

// client
template <>
template <>
inline void client_session_type::side_connected_handler<client_session_params>(
  const unicomm::connected_params& params)
{
  if (params.comm().owner().connections_count() < parameters().clients_number())
  {
    // if we didn't reach the requested clients number start new connection
    static_cast<unicomm::client&>(params.comm().owner()).connect();
  }

  send_echo_requests(params.comm());
  reset_timeout();
}

// server
template <>
template <>
inline void server_session_type::side_connected_handler<server_session_params>(
  const unicomm::connected_params& /*params*/)
{
  // empty
}

// client
template <>
template <>
inline void client_session_type::side_after_processed_handler<client_session_params>(
  const unicomm::after_processed_params& params)
{
  if (is_time())
  {
    reset_timeout();

    if (parameters().is_infinite())
    {
      send_echo_requests(params.comm());
    }
  }
}

// server
template <>
template <>
inline void server_session_type::side_after_processed_handler<server_session_params>(
  const unicomm::after_processed_params& /*params*/)
{
  // empty
}

/** Connect failure handler. */
void connect_error_handler(const unicomm::connect_error_params& params);

/** Returns a reference to the echo configuration object. */
unicomm::config& config(void);

} // namespace uni_echo

#endif // UNI_ECHO_HPP_
