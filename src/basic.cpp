///////////////////////////////////////////////////////////////////////////////
// basic.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm basic declarations and definitions.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#include <unicomm/basic.hpp>

//////////////////////////////////////////////////////////////////////////
// basic

#ifdef UNI_WIN
# ifdef min
#   undef min
# endif // min
# ifdef max
#   undef max
# endif // max
#endif // UNI_WIN

/** Minimum available message priority. */
size_t unicomm::min_priority(void) { return undefined_priority() + 1; } 

/** Maximum available message priority. */
size_t unicomm::max_priority(void) 
{ 
  return std::numeric_limits<size_t>::max(); 
} 

/** Undefined priority value. */
size_t unicomm::undefined_priority(void) 
{ 
  return std::numeric_limits<size_t>::min(); 
}
