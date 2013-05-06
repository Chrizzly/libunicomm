///////////////////////////////////////////////////////////////////////////////
// message_decoder_base.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol message decoder. Accepts incoming data and 
// creates user defined messages.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#include <unicomm/message_decoder_base.hpp>
#include <unicomm/except.hpp>

#include <boost/assert.hpp>
#include <boost/regex.hpp>

using std::string;
using std::make_pair;

using boost::regex;
using boost::cmatch;
using boost::regex_match;

//////////////////////////////////////////////////////////////////////////
// message decoder base
unicomm::message_base::pointer_type 
unicomm::message_decoder_base::perform_decode(comm_buffer& buffer, 
                                              session_base& session)
{
  unicomm::message_base::pointer_type message;
  // find message bounds
  comm_buffer::buffer_lock lock(buffer);
  const iter_pair_type bounds = find_raw_message(lock.buffer(), session);

  if (bounds.first != bounds.second)
  {
    BOOST_ASSERT(bounds.first < bounds.second && "Invalid iterators range");

    string m_str(bounds.first, bounds.second);
    lock.buffer().erase(bounds.first, bounds.second);
    lock.unlock();

    const string m_name = 
      get_message_name(decode_raw_message(m_str, session), session);

    message = create_message(m_name, session);

    BOOST_ASSERT(m_name == message->name() && 
      "It seems invalid type mapped to requested name");

    if (m_name != message->name())
    {
      throw message_decoder_error("Invalid message received [wanted: " + 
        message->name() + ", got: " + m_name + "]");
    }

    // fixme: what if decoder encounters error? clear income buffer? or what?
    // e.g. if message::unserialize throw...
    message->unserialize(m_str);
  }

  return message;
}

//-----------------------------------------------------------------------------
unicomm::message_base::pointer_type 
unicomm::message_decoder_base::create_message(const string& name, 
                                              session_base& /*session*/)
{
  return _factory.create(name);
}

