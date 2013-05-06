///////////////////////////////////////////////////////////////////////////////
// xml_message.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol complex xml message.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2008, (c) Dmitry Timoshenko.

#include <unicomm/facade/xml_message.hpp>

#ifdef UNICOMM_USE_COMPLEX_XML

#include <smart/data/complex.hpp>
#include <smart/data/complex_aux.hpp>
#include <smart/data/complex_serializer.hpp>

using std::string;

using smart::data::get_property;
using smart::data::set_property;
using smart::data::prop_exists;

//////////////////////////////////////////////////////////////////////////
// aux
namespace 
{

/** Request message identifier property name. */
inline const string& id_property(void) { static const string s = "id"; return s; }

/** Reply message identifier property name. */
inline const string& rid_property(void) { static const string s = "rid"; return s; }

/** Reads predefined id property. 
 *
 *  @return id property contained by the specified complex object.
 *  @throw Throws the same as thrown by complex::getproperty<T>
 */
inline unicomm::messageid_type get_id(const smart::data::complex& c)
{
  return get_property<smart::data::types::uint_type>(c, id_property());
}

/** Sets id property to complex object. */
inline void set_id(smart::data::complex& c, unicomm::messageid_type value)
{
  set_property(c, id_property(), smart::data::types::uint_type(value)); 
}

/** Reads reply id property. 
 *
 *  @return reply id property contained by the specified complex object.
 *  @throw Throws the same as thrown by complex::getproperty<T>
 */
inline unicomm::messageid_type get_rid(const smart::data::complex& c)
{
  return get_property<smart::data::types::uint_type>(c, rid_property()); 
}

/** Sets reply id property to complex object. */
inline void set_rid(smart::data::complex& c, unicomm::messageid_type value)
{
  return set_property(c, rid_property(), smart::data::types::uint_type(value)); 
}

/** Whether id property exists in complex object. */
inline bool id_exists(const smart::data::complex& c)
{
  return prop_exists<smart::data::types::uint_type>(c, id_property());
}

/** Whether rid property exists in complex object. */
inline bool rid_exists(const smart::data::complex& c)
{
  return prop_exists<smart::data::types::uint_type>(c, rid_property());
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// xml message impl
struct unicomm::xml_message::impl
{
  impl(size_t p): _priority(p) { /* empty */ }

  //////////////////////////////////////////////////////////////////////////
  // data
  size_t _priority;
  smart::data::complex _complex;
};

//////////////////////////////////////////////////////////////////////////
// xml message

unicomm::xml_message::xml_message(size_t p): 
  _impl(new impl(p))
{
  // empty
}

/** Returns message instance identifier. */
unicomm::messageid_type unicomm::xml_message::id(void) const
{
  return ::id_exists(complex())? ::get_id(complex()): undefined_messageid();
}

//-----------------------------------------------------------------------------
unicomm::xml_message& unicomm::xml_message::id(messageid_type mid)
{
  ::set_id(complex(), mid); return *this;
}

//-----------------------------------------------------------------------------
unicomm::messageid_type unicomm::xml_message::rid(void) const
{
  return ::rid_exists(complex())? ::get_rid(complex()): undefined_messageid();
}

//-----------------------------------------------------------------------------
void unicomm::xml_message::rid(messageid_type mid)
{
  ::set_rid(complex(), mid);
}

//-----------------------------------------------------------------------------
string unicomm::xml_message::serialize(void) const
{
  smart::data::set_name(complex(), name());

  return smart::data::default_serialize(complex());
}

//-----------------------------------------------------------------------------
void unicomm::xml_message::unserialize(const std::string &message)
{
  smart::data::default_unserialize(complex(), message);
}

//-----------------------------------------------------------------------------
unicomm::xml_message& unicomm::xml_message::priority(size_t p) 
{ 
  _impl->_priority = p; 

  return *this;
}

//-----------------------------------------------------------------------------
size_t unicomm::xml_message::priority(void) const 
{ 
  return _impl->_priority; 
}

//-----------------------------------------------------------------------------
smart::data::complex& unicomm::xml_message::complex(void) const
{
  return _impl->_complex;
}

#endif // UNICOMM_USE_COMPLEX_XML
