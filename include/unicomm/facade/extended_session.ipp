///////////////////////////////////////////////////////////////////////////////
// extended_session.ipp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm extended session. Allows to map different event handlers via 
// boost::signals and boost::function. Just an optional facade.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifndef UNI_EXTENDED_SESSION_HPP_
# error 'extended_session.ipp' is not for direct inclusion, please use 'extended_session.hpp'
#endif // UNI_EXTENDED_SESSION_HPP_

#include <unicomm/detail/helper_detail.hpp>

#include <smart/utils.hpp>
#include <smart/scoped_sentinel.hpp>

#include <boost/bind.hpp>

#include <utility>

using std::string;
using std::make_pair;

using boost::mutex;

using smart::scoped_sentinel;

//////////////////////////////////////////////////////////////////////////
// extended session

//////////////////////////////////////////////////////////////////////////
// handlers setters
template <typename DerivedT, typename SessionParamsT, typename BaseT>
boost::signals2::connection unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  add_connected_handler(const connected_signal_type::slot_type& handler)
{
  return _connected_signal.connect(handler);
}

////------------------------------------------------------------------------
//void unicomm::extended_session::remove_connected_handler(const connected_signal_type::slot_type& handler)
//{
//  //_connected_signal.disconnect(handler);
//}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
boost::signals2::connection unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  add_disconnected_handler(const disconnected_signal_type::slot_type& handler)
{
  return _disconnected_signal.connect(handler);
}

////------------------------------------------------------------------------
//void unicomm::extended_session::remove_disconnected_handler(const disconnected_signal_type::slot_type& /*handler*/)
//{
////  _disconnected_signal.disconnect(handler);
//}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  set_any_message_arrived_handler(const message_arrived_handler_type& handler)
{
  smart::set(_any_mes_arrived_handler, handler, _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  clear_any_message_arrived_handler(void)
{
  smart::set(_any_mes_arrived_handler, message_arrived_handler_type(0), _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  set_message_arrived_handler(const string& name, 
                              const message_arrived_handler_type& handler)
{
  if (handler)
  {
    mutex::scoped_lock lock(_mutex);

    _mes_arrived_handlers[name] = handler;
  } else
  {
    // if f.empty() - removes callback
    clear_message_arrived_handler(name);
  }
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  clear_message_arrived_handler(const std::string& name)
{
  mutex::scoped_lock lock(_mutex);

  _mes_arrived_handlers.erase(name);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  set_message_sent_handler(const message_sent_handler_type& handler)
{
  smart::set(_def_message_sent_handler, handler, _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  clear_message_sent_handler(void)
{
  smart::set(_def_message_sent_handler, message_sent_handler_type(0), _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  set_message_timeout_handler(const message_timeout_handler_type& handler)
{
  smart::set(_def_message_timeout_handler, handler, _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  clear_message_timeout_handler(void)
{
  smart::set(_def_message_timeout_handler, message_timeout_handler_type(0), _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  set_error_handler(const error_handler_type& handler)
{
  smart::set(_err_handler, handler, _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  clear_error_handler(void)
{
  smart::set(_err_handler, error_handler_type(0), _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  set_after_processed_handler(const after_processed_handler_type& handler)
{
  smart::set(_after_processed_handler, handler, _mutex);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  clear_after_processed_handler(void)
{
  smart::set(_after_processed_handler, after_processed_handler_type(0), _mutex);
}

//////////////////////////////////////////////////////////////////////////
// private stuff: virtual handlers
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  connected_handler(const connected_params& params)
{
  call_connected(params);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  disconnected_handler(const disconnected_params& params)
{
  call_disconnected(params);
}

//------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  message_arrived_handler(message_arrived_params& params)
{
  call_message_arrived(params);
}

/** Message is actually sent handler. */
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  message_sent_handler(const message_sent_params& params)
{
  call_message_sent(params);
}

/** Message timeout handler. */
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  message_timeout_handler(const message_timeout_params& params)
{
  call_message_timeout(params);
}

/** Error handler. */
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  error_handler(const error_params& params)
{
  call_error(params);
}

/** After processed handler. */
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  after_processed_handler(const after_processed_params& params)
{
  call_after_processed(params);
}

//////////////////////////////////////////////////////////////////////////
// private stuff: handlers thread safe getters
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::message_arrived_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  find_message_arrived_handler(const string& name) const
{
  mutex::scoped_lock lock(_mutex);

  message_arrived_handler_type handler = 0;
  // look for handler for this communicator
  message_arrived_handlers_collection_type::const_iterator handler_cit = 
    _mes_arrived_handlers.find(name);
  if (handler_cit != _mes_arrived_handlers.end())
  {
    handler = handler_cit->second;
  } 

  return handler;
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::message_sent_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  find_message_sent_handler(const messageid_type& mid)
{
  message_sent_handler_type handler = 0;

  mutex::scoped_lock lock(_mutex);
  // look for registered handler
  message_sent_handlers_collection_type::const_iterator cit = 
    _sent_handlers.find(mid);

  if (cit != _sent_handlers.end())
  {
    handler = cit->second;
  }

  return handler;
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::message_arrived_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  get_any_message_arrived_handler(void) const
{
  return smart::get(_any_mes_arrived_handler, _mutex);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::message_sent_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  get_message_sent_handler(void) const
{
  return smart::get(_def_message_sent_handler, _mutex);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::message_timeout_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  get_message_timeout_handler(void)
{
  return smart::get(_def_message_timeout_handler, _mutex);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::error_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  get_error_handler(void) const
{
  return smart::get(_err_handler, _mutex);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
const unicomm::after_processed_handler_type 
unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  get_after_processed_handler(void) const
{
  return smart::get(_after_processed_handler, _mutex);
}

//////////////////////////////////////////////////////////////////////////
// handler callers
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_message_arrived(message_arrived_params& params)
{
  const string& name                               = 
    get_name(params.in_message());

  const message_arrived_handler_type any_handler   = 
    get_any_message_arrived_handler();

  const message_arrived_handler_type exact_handler = 
    find_message_arrived_handler(name);

  detail::call_handler(exact_handler, params);
  detail::call_handler(any_handler, params);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_connected(const connected_params& params)
{
  detail::execute_signal(_connected_signal, params);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_disconnected(const disconnected_params& params)
{
  detail::execute_signal(_disconnected_signal, params);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_message_sent(const message_sent_params& params)
{
  const messageid_type mid = params.message_id();
  scoped_sentinel sentry(
    boost::bind(&extended_session::unreg_message_sent, this, mid));

  // at first, call handler mapped to the message id 
  detail::call_handler(find_message_sent_handler(mid), params);
  // and then call default handler
  detail::call_handler(get_message_sent_handler(), params);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_message_timeout(const message_timeout_params& params)
{
  detail::call_handler(get_message_timeout_handler(), params);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_error(const error_params& params)
{
  detail::call_handler(get_error_handler(), params);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  call_after_processed(const after_processed_params& params)
{
  detail::call_handler(get_after_processed_handler(), params);
}

//////////////////////////////////////////////////////////////////////////
// misc aux
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  reg_messsage_sent(const messageid_type& mid, 
                    const message_sent_handler_type& handler)
{
  if (handler)
  {
    mutex::scoped_lock lock(_mutex); 

    if (is_message_sent_registered(mid))
    {
      _sent_handlers[mid] = handler;
    } else
    {
      _sent_handlers.insert(make_pair(mid, handler));
    }
  }
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
void unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  unreg_message_sent(const messageid_type& mid)
{
  mutex::scoped_lock lock(_mutex); _sent_handlers.erase(mid);
}

//-----------------------------------------------------------------------------
template <typename DerivedT, typename SessionParamsT, typename BaseT>
bool unicomm::extended_session<DerivedT, SessionParamsT, BaseT>::
  is_message_sent_registered(messageid_type mid) const
{
  //mutex::scoped_lock lock(_mutex);

  return _sent_handlers.find(mid) != _sent_handlers.end();
}

