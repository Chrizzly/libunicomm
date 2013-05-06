///////////////////////////////////////////////////////////////////////////////
// helper.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol different helper entities.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HELPER_HPP_
#define UNI_HELPER_HPP_

/** @file helper.hpp Helper routines, entities definitions and declarations. */

#include <unicomm/config/auto_link.hpp>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Host resolving helper. */
class UNICOMM_DECL host_resolver
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Constructs an object. */
  host_resolver(void);

  /** Resolves the given query. 
   *
   *  @param query Host name or ip-address to be resolved by DNS service.
   *  @throw boost::system::system_error is thrown on failure. 
   *    Throws the same as thrown by 
   *    boost::asio::ip::tcp::resolver::resolve.
   *
   *  @return Iterator to first resolved endpoint.
   */
  boost::asio::ip::tcp::resolver::iterator resolve(
    const boost::asio::ip::tcp::resolver::query& query);

  /** Returns iterator to the first resolved endpoint. 
   *
   *  @return Iterator to the first resolved endpoint.
   */
  boost::asio::ip::tcp::resolver::iterator begin(void) const 
    { return _begin; }

  /** Returns the end iterator. 
   *
   *  @return End iterator.
   */
  static boost::asio::ip::tcp::resolver::iterator end(void) 
    { return boost::asio::ip::tcp::resolver::iterator(); }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
  typedef boost::shared_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;

private:
  io_service_ptr _io_service;
  resolver_ptr _resolver;
  boost::asio::ip::tcp::resolver::iterator _begin;
};

/** Resolves specified host name or address.
 *
 *  @param query Host name or ip-address to be resolved by DNS service.
 *  @return Returns host resolver object provides an interface to resolved 
 *    endpoints collection.
 *
 *  @throw If resolving is failed throws 
 *    the same as boost::asio::ip::tcp::resolver::resolve.
 */
UNICOMM_DECL host_resolver 
resolve_host(const boost::asio::ip::tcp::resolver::query& query);

/** Serializes given integer type into binary collection. 
 *
 *  @tparam IntegerT Integer type the object is serialized of.
 *  @tparam CollectionT Collection type which holds the serialized data.
 *  @param data Target raw data collection that will hold the serialized value.
 *  @param value The integer type value.
 *  @param value_sz Size of the raw data. By default sizeof(value) is taken.
 *  @note lsb goes first - little endian.
 */
template <typename IntegerT, typename CollectionT>
CollectionT& int_to_vec(CollectionT& data, IntegerT value, 
                        size_t value_sz = sizeof(IntegerT))
{
  typedef typename CollectionT::value_type dest_value_type;

  for (size_t i = 0; value_sz; --value_sz, ++i)
  {
    data.push_back(static_cast<dest_value_type>(value >> (i * 8) & 0xff));
  }

  return data;
}

/** Unserialize given integer type from a raw data sequence. 
 *
 *  @tparam IntegerT Integer type the object is unserialized of.
 *  @tparam IterT Iterator to the collection holds raw data to be parsed.
 *  @param first First element of raw data representing the value.
 *  @param last Last element of raw data representing the value. 
 *    Actually should point to the element next the last element.
 *
 *  @param value The integer type value object that will hold parsed value.
 *  @note lsb goes first - little endian.
 */
template <typename IntegerT, typename IterT>
IntegerT& int_from_vec(IterT first, IterT last, IntegerT &value)
{
  IntegerT v = 0;
  
  for (size_t i = 0; first != last; ++first, ++i)
  {
    v |= (static_cast<IntegerT>(*first) & 0xff) << (i * 8);
  }

  return value = v;
}

} // namespace unicomm

#endif // UNI_HELPER_HPP_
