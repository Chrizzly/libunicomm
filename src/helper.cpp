///////////////////////////////////////////////////////////////////////////////
// helper.cpp
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

#include <unicomm/helper.hpp>

#include <boost/assert.hpp>

#ifdef max
# undef max
#endif 

#ifdef min
# undef min
#endif 

using std::string;

using boost::asio::ip::tcp;

//////////////////////////////////////////////////////////////////////////
// host resolver
unicomm::host_resolver::host_resolver(void):
  _io_service(new boost::asio::io_service),
  _resolver(new tcp::resolver(*_io_service)),
  _begin(end())
{
  // empty
}

//-----------------------------------------------------------------------------
tcp::resolver::iterator 
unicomm::host_resolver::resolve(const tcp::resolver::query& query)
{
  _begin = _resolver->resolve(query);

  BOOST_ASSERT(_begin != end() && " - A bug in asio resolver?");

  return _begin;
}

//-----------------------------------------------------------------------------
unicomm::host_resolver unicomm::resolve_host(const tcp::resolver::query& query)
{
  unicomm::host_resolver resolver;

  resolver.resolve(query);

  return resolver;
}

