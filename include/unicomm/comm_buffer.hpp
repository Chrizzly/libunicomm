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

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_COMM_BUFFER_HPP_
#define UNI_COMM_BUFFER_HPP_

/** @file comm_buffer.hpp Communicator buffer definition. */

#include <unicomm/config/auto_link.hpp>

#include <boost/thread/mutex.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Communicator buffer provides thread safe operations on buffer. */
class UNICOMM_DECL comm_buffer
{
public:
  /** Actual buffer type that provides specific operations. */
  typedef std::string buffer_type;

public:
  /** Provides thread safe access to the internal buffer. */
  struct buffer_lock
  {
    /** Creates buffer lock object. 
     *
     *  @param buffer Buffer to guard.
     */
    explicit buffer_lock(comm_buffer& buffer): 
      _buf(&buffer),
      _lock(buffer.mutex())
    { 
      // empty
    }

    /** Returns a reference to the internal buffer object. 
     *
     *  Thread safe.
     *  
     *  @return Returns a reference to the internal buffer object.
     */
    buffer_type& buffer(void) { return _buf->inner_buffer(); }

    /** Unlocks the buffer. */
    void unlock(void) { _lock.unlock(); }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
  private:
    comm_buffer* _buf;
    boost::mutex::scoped_lock _lock;
  };

public:
  /** Appends tail to the buffer in thread safe manner.
   *
   *  @param tail Data to be appended.
   *  @return *this.
   */
  comm_buffer& append(const buffer_type& tail);

  /** Swaps the buffer with the object of the internal type. 
   *
   *  @param other The object to swap with. 
   *
   *  @return *this.
   */
  comm_buffer& swap(buffer_type& other);

  /** Clears the buffer. 
   *
   *  @return *this.
   */
  comm_buffer& clear(void);

  /** Whether buffer is empty or not.
   *
   *  @return True if empty and false otherwise.
   */
  bool empty(void) const;

  /** Returns a copy of the data. 
   *
   *  @return The internal buffer copy.
   */
  buffer_type data(void) const;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  buffer_type& inner_buffer(void) { return _buffer; }
  buffer_type& inner_buffer(void) const { return _buffer; }
  boost::mutex& mutex(void) { return _buf_mutex; }
  boost::mutex& mutex(void) const { return _buf_mutex; }

private:
  mutable boost::mutex _buf_mutex;
  mutable buffer_type _buffer;
};

} // namespace unicomm

#endif // UNI_COMM_BUFFER_HPP_

