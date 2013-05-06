///////////////////////////////////////////////////////////////////////////////
// complex_serializer_details.cpp
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


#include <smart/data/complex_serializer_details.hpp>

#include <xercesc/util/PlatformUtils.hpp>

#include <boost/assert.hpp>

//////////////////////////////////////////////////////////////////////////
// complex serialization helper class

//-----------------------------------------------------------------------------
smart::data::serialization_helper& smart::data::serialization_helper::instance(void)
{
  static serialization_helper helper;

  return helper;
}

//-----------------------------------------------------------------------------
smart::data::serialization_helper::serialization_helper(void)
{
  // empty
}

//-----------------------------------------------------------------------------
smart::data::serialization_helper::~serialization_helper(void)
{
  try
  {
    uninit();
  }
  catch (...)
  {
    BOOST_ASSERT(!"Unexpected exception");
  }
}

//-----------------------------------------------------------------------------
void smart::data::serialization_helper::uninit(void)
{
  if (initialized())
  {
    xercesc::XMLPlatformUtils::Terminate();
    _memman.reset();
  }
}

//-----------------------------------------------------------------------------
void smart::data::serialization_helper::init(void)
{
  BOOST_ASSERT(!initialized() && " - is alredy initialized");

  if (!initialized())
  {
    // initialize xercesc library
    _memman.reset(new xercesc::MemoryManagerImpl());
    xercesc::XMLPlatformUtils::Initialize(xercesc::XMLUni::fgXercescDefaultLocale, 0, 0, _memman.get(), true);
  }
}

