///////////////////////////////////////////////////////////////////////////////
// http_server_session.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Server session object class.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_SERVER_SESSION_HPP_
#define UNI_HTTP_SERVER_SESSION_HPP_

/** @file http.hpp Http server session object class definition. */

#include "http_session_base.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_message_base.hpp"
#include "http_basic.hpp"

#include <smart/timers.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <string>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

/** Host configuration holder. */
struct host_config
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit host_config(const std::string& host_name, const std::string& root_dir):
    _name(host_name),
    _root_dir(root_dir)
  {
    // empty
  }

  const std::string& name(void) const { return _name; }
  const std::string& root_directory(void) const { return _root_dir; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  std::string _name;
  std::string _root_dir;
};

/** Served hosts collection. */
typedef std::map<std::string, host_config> hosts_collection_type;

/** Server session parameters. */
struct server_session_params
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit server_session_params(const hosts_collection_type& served_hosts,
    const unicomm::connected_signal_type::slot_type& connected_handler,
      const unicomm::disconnected_signal_type::slot_type& disconnected_handler):
    _served_hosts(served_hosts),
    _connected_handler(connected_handler),
    _disconnected_handler(disconnected_handler)
  {
    // empty
  }

  //--------------------------------------------------------------------
  const hosts_collection_type& served_hosts(void) const { return _served_hosts; }
  const unicomm::connected_signal_type::slot_type& connected_handler(void) const 
    { return _connected_handler; }
  const unicomm::disconnected_signal_type::slot_type& disconnected_handler(void) const 
    { return _disconnected_handler; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  hosts_collection_type _served_hosts;
  unicomm::connected_signal_type::slot_type _connected_handler;
  unicomm::disconnected_signal_type::slot_type _disconnected_handler;
};

/** Server site session object. */
class server_session : public session_base<server_session, server_session_params>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Base type declaration. */
  typedef uni_http::session_base<server_session, server_session_params> base_type;

public:
  // don't need connected parameters here in this sample
  server_session(const unicomm::connected_params& /*params*/, 
                 const session_params_type& session_params):
    base_type(session_params),
    _served_hosts(session_params.served_hosts()),
    _timeout(boost::posix_time::seconds(30)),
    _track_tout(false)
  {
    add_connected_handler(session_params.connected_handler());
    add_disconnected_handler(session_params.disconnected_handler());
    set_message_sent_handler(boost::bind(&server_session::message_sent_handler_1, this, _1));

    // instead of using connected handler start track timeout here
    track_timeout();
  }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  struct check_resource_result
  {
  //////////////////////////////////////////////////////////////////////////
  // interface
  check_resource_result(hosts_collection_type::const_iterator host_cit,
    boost::filesystem::path target, bool is_dir):
    _host_cit(host_cit),
    _target(target),
    _is_dir(is_dir)
  {
    // empty
  }

  hosts_collection_type::const_iterator requested_host(void) const { return _host_cit; }
  const boost::filesystem::path& requested_target(void) const { return _target; }
  bool is_directory(void) const { return _is_dir; }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
  private:
    hosts_collection_type::const_iterator _host_cit;
    boost::filesystem::path _target;
    bool _is_dir;
  };

private:
  //////////////////////////////////////////////////////////////////////////
  // http server site protocol implementation
  response::pointer_type handle_get(const uni_http::request& inm);
  response::pointer_type handle_get_file(const check_resource_result& r);
  response::pointer_type handle_get_dir(const check_resource_result& r, const std::string& uri);
  response::pointer_type handle_head(const uni_http::request& inm);
  response::pointer_type handle_default(void); // default handler
  //message_pointer_type handle_post(const uni_http::request& inm);
  response::pointer_type process_request(const uni_http::request& inm);
  check_resource_result check_resource(const uni_http::request& inm);

  //////////////////////////////////////////////////////////////////////////
  // incoming connection idle timeout
  void is_track_timeout(bool v) { _track_tout = v; }
  bool is_track_timeout(void) const { return _track_tout; }
  bool is_timeout_elapsed(void) const { return _timeout.elapsed(); }
  /** Tells session to track timeout and disconnect when it elapsed. */
  void track_timeout(void) { is_track_timeout(true); smart::reset(_timeout); }

private:
  //////////////////////////////////////////////////////////////////////////
  // assigned handlers
  void message_sent_handler_1(const unicomm::message_sent_params& params);

  //////////////////////////////////////////////////////////////////////////
  // virtual functions

  /** Message arrived callback type. */
  void message_arrived_handler(unicomm::message_arrived_params& params);

  /** Error handler. */
  void error_handler(const unicomm::error_params& params);

  /** After processed callback type. */
  void after_processed_handler(const unicomm::after_processed_params& params);

private:
  hosts_collection_type _served_hosts;
  smart::timeout _timeout;
  bool _track_tout;
};

} // namespace uni_http

#endif // UNI_HTTP_SERVER_SESSION_HPP_
