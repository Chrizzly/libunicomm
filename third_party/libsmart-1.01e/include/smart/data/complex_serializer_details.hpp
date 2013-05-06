///////////////////////////////////////////////////////////////////////////////
// complex_serializer_details.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Serialization details.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
  #pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_SERIALIZER_DETAILS_HPP_
#define SMART_COMPLEX_SERIALIZER_DETAILS_HPP_

/** @file complex_serializer_details.hpp Serializer implementation details. */

#include <smart/data/complex_auto_link.hpp>

#include <xercesc/internal/MemoryManagerImpl.hpp>

#include <boost/shared_ptr.hpp>

/** @namespace smart smart++ Library root namespace. */
namespace smart
{

namespace data
{

/** Complex serialization details class. */
class serialization_helper
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Whether serialization engine initialized. */
  bool initialized(void) const { return _memman != 0; }

  /** Initializes serialization engine. */
  void init(void);

  /** Uninitialize serialization engine. */
  void uninit(void);

public:
  /** Returns an instance of the uni_config type. */
  static serialization_helper& instance(void);

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  typedef boost::shared_ptr<xercesc::MemoryManagerImpl> xercesc_mem_man_type;

private:
  serialization_helper(void);
  ~serialization_helper(void);

private:
  xercesc_mem_man_type _memman;
};

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_SERIALIZER_DETAILS_HPP_
