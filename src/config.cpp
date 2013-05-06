///////////////////////////////////////////////////////////////////////////////
// config.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Protocol configuration storage.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#include <unicomm/config.hpp>
#include <unicomm/except.hpp>
#include <unicomm/facade/bin_message_decoder.hpp>
#include <unicomm/facade/bin_message_encoder.hpp>
#include <detail/basic_detail.hpp>

#include <smart/debug_out.hpp>

#include <boost/filesystem.hpp>
#include <boost/assert.hpp>

#include <stdexcept>
#include <algorithm>
#include <iomanip>

using std::string;
using std::make_pair;
using std::map;
using std::runtime_error;
using std::for_each;
using std::endl;

using boost::asio::ip::address;
using boost::asio::ip::tcp;
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_directory;

//////////////////////////////////////////////////////////////////////////
// unicomm configuration 
// non-member public interface

bool unicomm::is_allowed_reply(const config& conf, 
                               const string& request_name, 
                               const string& reply_name)
{
  const strings_type& allowed_answers = conf.message_answers(request_name);

  return allowed_answers.empty()? true: 
    std::find(allowed_answers.begin(), allowed_answers.end(), reply_name) != 
      allowed_answers.end();
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::set_binary_message_format(config& conf)
{
  conf.message_format(binary_message_format());

  message_decoder_base::pointer_type decoder = create_binary_decoder();

  decoder->factory(conf.message_factory());

  conf.message_decoder(decoder);
  conf.message_encoder(create_binary_encoder());

  return conf;
}

//////////////////////////////////////////////////////////////////////////
// interface
unicomm::config::config(const session_base::factory_type& session_factory, 
                        const message_base::factory_type& message_factory):
  _tcp_port(detail::default_tcp_port()),
  _tcp_backlog(detail::default_tcp_backlog()),
  _def_tout(infinite_timeout()),
  _def_priority(undefined_priority()),
  _timeouts_enabled(false),
  _mes_encoder(detail::default_message_encoder()),
  _session_factory(session_factory),
  _message_factory(message_factory),
  _use_unique_message_id(false),
  _use_default_message_priority(false),
  _working_th_sleep_tout(detail::default_sleep_timeout()),
  _incoming_quantum(detail::default_incoming_quantum()),
  _outgoing_quantum(detail::default_outgoing_quantum())
{ 
  // empty
}

//-----------------------------------------------------------------------------
unicomm::message_decoder_base& unicomm::config::message_decoder(void) const
{
  return *_mes_decoder;
}

//-----------------------------------------------------------------------------
unicomm::message_encoder_base& unicomm::config::message_encoder(void) const
{
  return *_mes_encoder;
}

//-----------------------------------------------------------------------------
const unicomm::message_info& unicomm::config::mes_info(const string &mesname) const
{
  messages_map_type::const_iterator cit = _messages.find(mesname);

  if (cit == _messages.end())
  {
    static const unicomm::message_info info("", false, default_timeout(), 
      strings_type(), default_priority()); 

    return info;
  } else
  {
    return cit->second;
  }
}

//-----------------------------------------------------------------------------
bool unicomm::config::message_decoder_exists(void) const
{
  return _mes_decoder != 0;
}

//-----------------------------------------------------------------------------
bool unicomm::config::need_reply(const std::string &mesname) const
{
  return mes_info(mesname).need_reply();
}

//-----------------------------------------------------------------------------
const unicomm::session_base::factory_type& 
unicomm::config::session_factory(void) const
{
  return _session_factory;
}

//////////////////////////////////////////////////////////////////////////
// option setters

unicomm::config& unicomm::config::endpoint(const tcp::endpoint& ep)
{
  _endpoint = ep; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::ip_address(const boost::asio::ip::address& addr) 
{ 
  _endpoint.address(addr); return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::tcp_port(unsigned short port)
{
  _tcp_port = port; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::tcp_backlog(int backlog)
{
  _tcp_backlog = backlog; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::default_priority(size_t priority)
{
  _def_priority = priority; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::default_timeout(size_t tout)
{
  _def_tout = tout; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::timeouts_enabled(bool enabled)
{
  _timeouts_enabled = enabled; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::home_dir(const string& dir) 
{ 
  _homedir = dir; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::message_info(const unicomm::message_info& info)
{
  _messages.insert(make_pair(info.name(), info)); return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::message_info(const std::string &name, 
                     bool need_reply, 
                     size_t tout, 
                     const strings_type& answers, 
                     size_t priority)
{
  return message_info(unicomm::message_info(
    name, need_reply, tout, answers, priority));
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::message_format(const std::string& format)
{
  _message_format = format; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::use_unique_message_id(bool use)
{
  _use_unique_message_id = use; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::use_default_message_priority(bool use)
{
  _use_default_message_priority = use; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& 
unicomm::config::message_factory(const message_base::factory_type& factory)
{
  _message_factory = factory;

  if (message_decoder_exists())
  {
    message_decoder().factory(_message_factory);
  }

  return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& 
unicomm::config::session_factory(const session_base::factory_type& factory)
{
  _session_factory = factory; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::message_decoder(
  const message_decoder_base::pointer_type& decoder)
{
  _mes_decoder = decoder; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::message_encoder(
  const message_encoder_base::pointer_type& encoder)
{
  _mes_encoder = encoder; return *this;
}

#ifdef UNICOMM_SSL

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::ssl_client_verity_fn(const string& fn)
{
  _client_verify_fn = fn; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::ssl_server_key_password(const string& password)
{
  _server_key_password = password; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::ssl_server_cert_chain_fn(const string& fn)
{
  _server_cert_chain_fn = fn; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::ssl_server_key_fn(const string& fn)
{
  _server_key_fn = fn; return *this;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::config::ssl_server_dh_fn(const string& fn)
{
  _server_dh_fn = fn; return *this;
}

#endif // UNICOMM_SSL
