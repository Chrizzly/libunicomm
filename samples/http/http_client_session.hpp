///////////////////////////////////////////////////////////////////////////////
// http_client_session.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Client's session definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_CLIENT_SESSION_HPP_
#define UNI_HTTP_CLIENT_SESSION_HPP_

/** @file http_client_session.hpp Client's session class defintion. */

#include "http_session_base.hpp"

#include <boost/bind.hpp>

/** @namespace echo sample namespace. */
namespace uni_http
{

/** Client session parameters. */
struct client_session_params
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit client_session_params(
      const unicomm::connected_signal_type::slot_type& connected_handler):
    _connected_handler(connected_handler)
  {
    // empty
  }

  //--------------------------------------------------------------------
  const unicomm::connected_signal_type::slot_type& connected_handler(void) const 
    { return _connected_handler; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  unicomm::connected_signal_type::slot_type _connected_handler;
};

/** Client site session object. */
class client_session : public session_base<client_session, client_session_params>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Base type declaration. */
  typedef uni_http::session_base<client_session, client_session_params> base_type;

public:
  /** Creates client session. */
  explicit client_session(const unicomm::connected_params& params,
                          const session_params_type& session_params):
    base_type(session_params),
    _in_buffer(&params.in_buffer())
  {
    add_connected_handler(session_params.connected_handler());
    add_disconnected_handler(boost::bind(&client_session::disconnected_handler_1, this, _1));
    set_message_timeout_handler(boost::bind(&client_session::message_timeout_handler_1, this, _1));
  }

public:
  /** Whether the session is on server side. */
  bool is_server(void) const { return false; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  //////////////////////////////////////////////////////////////////////////
  // assigned handlers

  void message_timeout_handler_1(const unicomm::message_timeout_params& params);
  void disconnected_handler_1(const unicomm::disconnected_params& params);

  //////////////////////////////////////////////////////////////////////////
  // virtual functions

  /** Message arrived callback type. */
  void message_arrived_handler(unicomm::message_arrived_params& params);

private:
  unicomm::comm_buffer* _in_buffer;
};

/** Connect failure handler. */
void connect_error_handler(const unicomm::connect_error_params& params);

} // namespace uni_http

#endif // UNI_HTTP_CLIENT_SESSION_HPP_
