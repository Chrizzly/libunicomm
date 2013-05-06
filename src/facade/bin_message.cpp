///////////////////////////////////////////////////////////////////////////////
// bin_message.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm binary message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#include <unicomm/facade/bin_message.hpp>
#include <unicomm/helper.hpp>
#include <detail/basic_detail.hpp>

#include <smart/utils.hpp>

#include <boost/bind.hpp>

#include <limits>
#include <functional>
#include <stdexcept>

#ifdef max
# undef max
#endif 

#ifdef min
# undef min
#endif 

using std::string;
using std::numeric_limits;
using std::greater;
using std::not_equal_to;
using std::less;
using std::runtime_error;
using std::range_error;

//////////////////////////////////////////////////////////////////////////
// binary message impl
struct unicomm::bin_message::impl
{
  impl(size_t p): 
    _id(undefined_messageid()),
    _rid(undefined_messageid()),
    _priority(p)
  { 
    // empty  
  }

  //////////////////////////////////////////////////////////////////////////
  // data
  messageid_type _id;
  messageid_type _rid;
  size_t _priority;
};

//////////////////////////////////////////////////////////////////////////
// binary message
unicomm::bin_message::bin_message(size_t p): 
  _impl(new impl(p))
{
  // empty
}

/** Returns message instance identifier. */
unicomm::messageid_type unicomm::bin_message::id(void) const
{
  return _impl->_id;
}

//-----------------------------------------------------------------------------
unicomm::bin_message& unicomm::bin_message::id(messageid_type mid)
{
  _impl->_id = mid; return *this;
}

//-----------------------------------------------------------------------------
unicomm::messageid_type unicomm::bin_message::rid(void) const
{
  return _impl->_rid;
}

//-----------------------------------------------------------------------------
void unicomm::bin_message::rid(messageid_type mid)
{
  _impl->_rid = mid;
}

//-----------------------------------------------------------------------------
string unicomm::bin_message::serialize(void) const
{
  string s;
  // version
  s += detail::bin_version();

  const size_t head_len = detail::bin_header_min_len() + name().size() + 
    id_exists() * sizeof(messageid_type) + rid_exists() * sizeof(messageid_type);

  // length
  s += static_cast<string::value_type>(smart::throw_if<range_error>(
    head_len, boost::bind(greater<size_t>(), _1, numeric_limits<unsigned char>::max()), 
    "Can't serialize [Binary header too big]"));

  // flags
  string::value_type flags  = 0;
  flags                     = detail::set_id_exists(flags, id_exists());
  flags                     = detail::set_rid_exists(flags, rid_exists());
  s += flags;

  // name
  s += name();

  // id
  if (id_exists()) { int_to_vec(s, id()); }

  // rid
  if (rid_exists()) { int_to_vec(s, rid()); }

  return s;
}

//-----------------------------------------------------------------------------
void unicomm::bin_message::unserialize(const std::string &message)
{
  smart::throw_if<runtime_error>(boost::bind(less<size_t>(), message.size(), 
    detail::bin_header_min_len()), 
    "Invalid message format [Incomplete header received]");

  // version
  smart::throw_if<runtime_error>(boost::bind(not_equal_to<size_t>(), message[0], 
    detail::bin_version()), "Invalid message format [Illegal version]");

  // header length
  const size_t head_len = message[1];

  smart::throw_if<runtime_error>(boost::bind(less<size_t>(), message.size(), 
    head_len), "Invalid message format [Incomplete header received]");

  // flags
  const string::value_type flags = message[2];

  // name
  const size_t tmp = detail::is_id_exists(flags) * sizeof(messageid_type) + 
    detail::is_rid_exists(flags) * sizeof(messageid_type);

  const size_t name_len = head_len - detail::bin_header_min_len() - tmp;
  const string got_name = message.substr(detail::bin_header_min_len(), name_len);

  if (got_name != name())
  {
    throw runtime_error("Message identifier is invalid [wanted: " + name() + 
      ", got: " + got_name + "]");
  }

  // id
  if (detail::is_id_exists(flags))
  { 
    string::const_iterator first = message.begin() + 
      detail::bin_header_min_len() + name_len;
    string::const_iterator last  = first + sizeof(messageid_type);

    int_from_vec(first, last, _impl->_id);
  }

  // rid
  if (detail::is_rid_exists(flags))
  { 
    string::const_iterator first = message.begin() + detail::bin_header_min_len() + 
      name_len + detail::is_id_exists(flags) * sizeof(messageid_type);
    string::const_iterator last  = first + sizeof(messageid_type);

    int_from_vec(first, last, _impl->_rid);
  }

  unserialize_tail(string(message.begin() + head_len, message.end()));
}

//-----------------------------------------------------------------------------
unicomm::bin_message& unicomm::bin_message::priority(size_t p) 
{ 
  _impl->_priority = p; 

  return *this;
}

//-----------------------------------------------------------------------------
size_t unicomm::bin_message::priority(void) const 
{ 
  return _impl->_priority; 
}
