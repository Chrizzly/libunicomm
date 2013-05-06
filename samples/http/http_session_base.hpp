///////////////////////////////////////////////////////////////////////////////
// http_session_base.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Session base definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_SESSION_BASE_HPP_
#define UNI_HTTP_SESSION_BASE_HPP_

/** @file http.hpp Http session base class. */

#include "http_aux.hpp"

#include <unicomm/facade/extended_session.hpp>
#include <unicomm/comm.hpp>

#include <string>
#include <sstream>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

inline const string& session_name(const unicomm::session_base& session)
{
  static const std::string server = "server";
  static const std::string client = "client";

  return session.is_server()? server: client;
}

// In this example uni_http::session_base is only prints an information
// to the std::cout.
template <typename DerivedT, typename SessionParamsT>
class session_base : public unicomm::extended_session<DerivedT, SessionParamsT>
{
  typedef unicomm::extended_session<DerivedT, SessionParamsT> base_type;

//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit session_base(const SessionParamsT& session_params):
    base_type(session_params)
  {
    // NOTE:
    // If it's necessary to close connection when it is just 
    // established throw any exception from session constructor. 
    // Connection will be closed and session won't be created. Throwing 
    // disconnect_error from connection_handler causes session 
    // disconnects too. Usual way to disconnect a session is to call 
    // communicator::disconnect() from any handler defined by the session.
    // It would be like (inside one of the session handlers scope): 
    // params.comm().disconnect();
  }

private:
  //////////////////////////////////////////////////////////////////////////
  // virtual functions

  // Override handlers in base class to out some information.

  /** Client connected callback type. */
  void connected_handler(const unicomm::connected_params& params)
  {
    std::stringstream ss;

    ss << name() << ": [" << params.comm().id() << "]: connected; local endpoint: "
      << params.comm().local_endpoint() << ", remote endpoint: "
      << params.comm().remote_endpoint();
    out_str(ss.str());

    // dispatch further
    base_type::connected_handler(params);
  }

  /** Client disconnected callback type. */
  void disconnected_handler(const unicomm::disconnected_params& params)
  {
    std::stringstream ss;

    ss << name() << ": [" << params.comm().id() << "]: disconnected; "
      << params.error_code() << "; " << params.what();
    out_str(ss.str());

    // dispatch further
    base_type::disconnected_handler(params);
  }

  /** Message timeout callback function type. */
  void message_timeout_handler(const unicomm::message_timeout_params& params)
  {
    std::stringstream ss;

    ss << name() << ": [" << params.comm().id()
      << "]: other side didn't reply within timeout, message id: "
      << params.message_id();
    out_str(ss.str());

    // dispatch further
    base_type::message_timeout_handler(params);
  }

  /** Error callback type. */
  void error_handler(const unicomm::error_params& params)
  {
    std::stringstream ss;

    ss << name() << ": [" << params.comm().id()
      << "]: an error occurred, error: " << params.description();
    out_str(ss.str());

    // dispatch further
    base_type::error_handler(params);
  }

protected:
  const string& name(void) const { return session_name(*this); }
};

} // namespace uni_http

#endif // UNI_HTTP_SESSION_BASE_HPP_
