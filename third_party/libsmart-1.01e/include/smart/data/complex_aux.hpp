///////////////////////////////////////////////////////////////////////////////
// complex_uaux.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_AUX_HPP_
#define SMART_COMPLEX_AUX_HPP_

/** @file complex_uaux.hpp Complex auxiliary convenient utilities declaration. */

#include <smart/data/complex_auto_link.hpp>
#include <smart/data/complex.hpp>

#include <string>

/** @namespace smart smart++ Library root namespace. */
namespace smart
{

/** @namespace data Different entities. */
namespace data
{

/** Returns property value by specified name.
 *
 *  @tparam T Data type.
 *  @param c Complex data object.
 *  @param name Property name to be read.
 *  @exception Throws smart::data::invalid_property_error if requested property doesn't exist.
 */
template <typename T>
inline const T& get_property(const smart::data::complex& c, const std::string &name)
{
  return c.get_property<T>(name);
}

/** Sets property with given name to specified value.
 *
 *  If create is true it will create a property with specified name, type and value
 *  if such property doesn't exist. If property exists, just assigns a value.
 *
 *  @tparam T Data type.
 *  @param name Property name whose value should be set.
 *  @param value Property value.
 *  @param create Flag indicates to create property if it doesn't exist.
 *  @throw If property with given name doesn't exist and create is false
 *         throws smart::data::invalid_property_error.
 */
template <typename T>
inline void set_property(smart::data::complex& c, const std::string &name, const T& value, bool create = true)
{
  c.set_property<T>(name, value, create);
}

/** Tests whether property with the specified name and type contained by complex object.
 *
 *  @tparam T property type to be checked.
 *  @param c complex object probably containing property.
 *  @param propname property name to be tested.
 *  @return true if property exists, otherwise false.
 */
template <typename T>
inline bool prop_exists(const smart::data::complex& c, const std::string& prop_name)
{
  return c.prop_exists<T>(prop_name);
}

/** Tries to read specified property.
 *  If there is no such property defvalue is returned as the result.
 *
 *  @tparam T property type to be read.
 *  @param c complex object to read property from.
 *  @param propname property name to be read.
 *  @param defvalue this value returned if property doesn't exist.
 *  @return either read property value or default value if property doesn't exist.
 */
template <typename T>
inline T read_default(const smart::data::complex& c, const std::string& prop_name, const T& def_value = T())
{
  return prop_exists<T>(c, prop_name)? get_property<T>(c, prop_name): def_value;
}

/** Reads name property.
 *
 *  @return name property contained by the specified complex object.
 *  @throw Throws the same as thrown by complex::getproperty<T>
 */
inline const std::string& get_name(const smart::data::complex& c)
{
  return get_property<smart::data::types::string_type>(c, name_property());
}

/** Reads name property.
 *
 *  @return name property contained by the specified complex object.
 *          If there is no name property empty string is returned.
 */
inline std::string get_safe_name(const smart::data::complex& c)
{
  return read_default<smart::data::types::string_type>(c, name_property());
}

/** Sets name property to complex object. */
inline void set_name(smart::data::complex& c, const std::string& value)
{
  return set_property(c, name_property(), value);
}

/** Whether timeout enabled. */
//inline bool timeouts(void) { return unicomm::uni_config::instance().timeouts(); }

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_AUX_HPP_
