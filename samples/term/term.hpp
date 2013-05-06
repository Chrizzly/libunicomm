///////////////////////////////////////////////////////////////////////////////
// term.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Mostly simple example on how to use unicomm engine to transfer custom data.
// Sends characters from stdin to opposite site.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.


#ifdef _MSC_VER
  #pragma once
#endif // _MSC_VER

#ifndef UNI_TERM_HPP_
#define UNI_TERM_HPP_

/** @file term.hpp Unicomm terminal sample. */

#include <unicomm/unicomm.hpp>

#include <string>

/** @namespace echo sample namespace. */
namespace uni_term
{

//////////////////////////////////////////////////////////////////////////
// Each protocol has messages, client and server. Let's define
// them. Messages are objects that hold data sent and received 
// by client and server. In this example there is only the message.
// Each message class should define name() function returns the unique name.
// Here we use default defined one in the base class due to we only have 
// one message.

/** Message. */
struct message : public unicomm::message_base
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit message(const std::string& s = ""): _data(s) { /*empty*/ }

  const std::string& data(void) const { return _data; }
  void data(const std::string& s) { _data = s; }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /** Serializes message to a std::string. */
  std::string serialize(void) const { return data(); }

  /** Restores message from a std::string. */
  void unserialize(const std::string &message) { data(message); }

private:
  std::string _data;
};

//////////////////////////////////////////////////////////////////////////
// Then define user object that will hold data and implement 
// user's handlers logic. The base class for the user object is 
// unicomm::session_base. The unicomm::basic_session class template 
// provides factory methods to create derived session class.
// If you also wish to have extended event handling use 
// unicomm::extended_session. In this example it is one session type 
// is used by client and server. But if you need you may split the behavior
// by making different classes for server and client respectively.

class session : public unicomm::basic_session<session>
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  explicit session(const unicomm::connected_params& /*params*/) { /* empty */ }

private:
  //////////////////////////////////////////////////////////////////////////
  // virtual functions

  /** Message arrived callback type. */
  void message_arrived_handler(unicomm::message_arrived_params& params);

  /** Client connected callback type. */
  void connected_handler(const unicomm::connected_params& params);

  /** Client disconnected callback type. */
  void disconnected_handler(const unicomm::disconnected_params& params);
};

//////////////////////////////////////////////////////////////////////////
// Message decoder.
// This entity is responsible for recognizing data concerned to one message.

class message_decoder : public unicomm::message_decoder_base
{
  message::pointer_type perform_decode(unicomm::comm_buffer& buffer, 
                                       unicomm::session_base& /*session*/)
  {
    if (buffer.empty())
    {
      // null pointer will end the processing
      return message::pointer_type();
    }

    message::pointer_type m(new message(buffer.data()));
    buffer.clear();

    return m;
  }
};

/** Connect failure handler. */
void connect_error_handler(const unicomm::connect_error_params& params);

/** Returns a reference to the term configuration object. */
unicomm::config& config(void);

//////////////////////////////////////////////////////////////////////////
// aux
/** Synchronize an access to the std::cout. */
void out_line(const std::string& s);

} // namespace echo

#endif // UNI_TERM_HPP_
