///////////////////////////////////////////////////////////////////////////////
// config_loader.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Configuration loaders.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.
 
#include <unicomm/facade/config_loader.hpp>

#ifdef UNICOMM_USE_COMPLEX_XML

#include <unicomm/facade/xml_message_decoder.hpp>
#include <unicomm/facade/xml_message_encoder.hpp>
#include <unicomm/except.hpp>
#include <unicomm/detail/helper_detail.hpp>
#include <detail/basic_detail.hpp>

#include <smart/data/complex.hpp>
#include <smart/data/complex_aux.hpp>
#include <smart/data/complex_serializer.hpp>

#include <algorithm>
#include <iterator>

using std::string;
using std::transform;
using std::back_inserter;

//////////////////////////////////////////////////////////////////////////
// aux
namespace 
{

using smart::data::types::string_array_type;

//-----------------------------------------------------------------------------
unicomm::strings_type 
process_hex_str_array(const string_array_type::container_type& array)
{
  unicomm::strings_type result;

  transform(array.begin(), array.end(), back_inserter(result), 
    &unicomm::detail::process_hex_str);

  return result;
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// complex xml loader
unicomm::config unicomm::load_from_complex_xml(const std::string& file_name)
{
  detail::check_path(file_name);

  // loading actual settings from the configuration file
  smart::data::complex m;

  using namespace smart::data;
  using namespace smart::data::types;

  complex_default_serializer ds;
  from_file(ds, m, file_name);

  config config;
  //////////////////////////////////////////////////////////////////////////
  // protocol common settings
  const complex_type& c = get_property<complex_type>(m, "common");

  config
    .tcp_port(static_cast<unsigned short>(get_property<uint_type>(c, "tcp_port")))
    .tcp_backlog(read_default(c, "tcp_backlog", int_type(detail::default_tcp_backlog())))
    .default_timeout(read_default(c, "default_timeout", uint_type(infinite_timeout())))
    .default_priority(read_default(c, "default_priority", uint_type(undefined_priority())))
    .timeouts_enabled(read_default(c, "timeouts_enabled", int_type(0)) != 0)
    .message_format(read_default(c, "message_format", string_type(binary_message_format())))
    .home_dir(detail::normalize_path(read_default(c, "home_dir", string_type())))
    .dispatcher_idle_tout(read_default(c, "dispatcher_idle_tout", 
      uint_type(detail::default_sleep_timeout())))
    .incoming_quantum(read_default(c, "incoming_quantum", 
      uint_type(detail::default_incoming_quantum())))
    .outgoing_quantum(read_default(c, "outgoing_quantum", 
      uint_type(detail::default_outgoing_quantum())))
    .use_unique_message_id(
      read_default(c, "use_unique_message_id", int_type(0)) != 0)
    .use_default_message_priority(
      read_default(c, "use_default_message_priority", int_type(0)) != 0)
  ;

  setup_message_format(config);

  //////////////////////////////////////////////////////////////////////////
  // messages
  const complex_array_type& tmp = get_property<complex_array_type>(m, "messages");
  const complex_array_type::container_type& messages = tmp.inner_array();

  for (complex_array_type::container_type::const_iterator cit = messages.begin();
    cit != messages.end(); ++cit)
  {
    message_info m_info
      (
        is_binary_message_format(config.message_format())? 
          detail::process_hex_str(get_property<string_type>(*cit, "message_name")): 
          get_property<string_type>(*cit, "message_name"),

        read_default(*cit, "need_reply", int_type(0)) != 0,
        read_default(*cit, "timeout", uint_type(config.default_timeout())),

        is_binary_message_format(config.message_format())?
          process_hex_str_array(read_default(*cit, "answers", 
          string_array_type()).inner_array()):
          read_default(*cit, "answers", string_array_type()).inner_array(),

        read_default(*cit, "priority", uint_type(config.default_priority()))/*,*/
        /*read_default(*cit, "sequence", int_type(0)) != 0*/
      );

    config.message_info(m_info);
  }

#ifdef UNICOMM_SSL

  //////////////////////////////////////////////////////////////////////////
  // ssl config
  if (prop_exists<complex_type>(m, "ssl"))
  {
    const complex_type& c = get_property<complex_type>(m, "ssl");

    config
      .ssl_client_verity_fn(read_default(c, "client_verify_file", string_type()))
      .ssl_server_key_password(read_default(c, "server_key_password", string_type()))
      .ssl_server_cert_chain_fn(read_default(c, "server_cert_chain_file", string_type()))
      .ssl_server_key_fn(read_default(c, "server_key_file", string_type()))
      .ssl_server_dh_fn(read_default(c, "server_dh_file", string_type()))
    ;
  } else
  {
    config
      .ssl_client_verity_fn("")
      .ssl_server_key_password("")
      .ssl_server_cert_chain_fn("")
      .ssl_server_key_fn("")
      .ssl_server_dh_fn("")
    ;
  }

#endif // UNICOMM_SSL

  return config;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::set_xml_message_format(config& conf)
{
  conf.message_format(xml_message_format());

  message_decoder_base::pointer_type decoder = create_xml_decoder();

  decoder->factory(conf.message_factory());

  conf.message_decoder(decoder);
  conf.message_encoder(create_xml_encoder());

  return conf;
}

//-----------------------------------------------------------------------------
unicomm::config& unicomm::setup_message_format(config& conf)
{
  if (is_binary_message_format(conf.message_format()))
  {
    set_binary_message_format(conf);
  } 
  else if (is_xml_message_format(conf.message_format()))
  {
    set_xml_message_format(conf);
  }

  return conf;
}

#endif // UNICOMM_USE_COMPLEX_XML
