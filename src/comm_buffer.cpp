///////////////////////////////////////////////////////////////////////////////
// comm_buffer.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Comunicator buffer.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2013, (c) Dmitry Timoshenko.

#include <unicomm/comm_buffer.hpp>

using boost::mutex;

//////////////////////////////////////////////////////////////////////////
// comm_buffer
unicomm::comm_buffer& unicomm::comm_buffer::append(const buffer_type& tail)
{
  mutex::scoped_lock lock(mutex()); 
  
  inner_buffer().append(tail);

  return *this;
}

//------------------------------------------------------------------------
unicomm::comm_buffer& unicomm::comm_buffer::swap(buffer_type& other)
{
  mutex::scoped_lock lock(mutex()); 

  inner_buffer().swap(other);

  return *this;
}

//------------------------------------------------------------------------
unicomm::comm_buffer& unicomm::comm_buffer::clear(void)
{
  mutex::scoped_lock lock(mutex()); 

  inner_buffer().clear();

  return *this;
}

//------------------------------------------------------------------------
bool unicomm::comm_buffer::empty(void) const
{
  mutex::scoped_lock lock(mutex()); 

  return inner_buffer().empty();
}

//------------------------------------------------------------------------
unicomm::comm_buffer::buffer_type unicomm::comm_buffer::data(void) const
{ 
  mutex::scoped_lock lock(mutex()); 

  return inner_buffer(); 
}