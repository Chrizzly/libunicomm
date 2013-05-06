///////////////////////////////////////////////////////////////////////////////
// http_message_decoder.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http message decoder. 
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#include "http_message_decoder.hpp"
#include "http_message_base.hpp"
#include "http_basic.hpp"
#include "http_except.hpp"

// NOTE: 
// if you are intended to use session object inside decoder
// methods you need the corresponding header to be included
// because of the unicomm::session_base class is just declared via 
// forward declaration
#include "http_session_base.hpp"

#include <smart/debug_out.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

#include <utility>
#include <limits>

using std::string;
using std::make_pair;
using std::numeric_limits;

using boost::lexical_cast;

//////////////////////////////////////////////////////////////////////////
// http message decoder
unicomm::message_decoder_base::iter_pair_type 
uni_http::message_decoder::find_raw_message(unicomm::comm_buffer::buffer_type& buffer, 
                                            unicomm::session_base& session)
{
  message_decoder_base::iter_pair_type bounds = make_pair(buffer.end(), buffer.end());

  // if size is not changed assume there were no changes, so, nothing to do
  if (_last_buf_size != buffer.size())
  {
    UNICOMM_DEBUG_OUT("[uni_http::message_decoder]: DECODE ENTER; Length = " 
      << _length << "; Buffer size = " << buffer.size())

    _last_buf_size = buffer.size();

    if (_length == 0) // means header is not parsed yet
    {
      const size_t i = buffer.find(head_body_separator());

      if (i != string::npos) 
      {
        header::header_collection_type headers = parse_header(buffer.substr(0, i));

        // get the content length 
        header::header_collection_type::const_iterator cit = headers.find(header::content_length());
        //_length = (cit != headers.end()? lexical_cast<size_t>(cit->second): 0);
        if (cit != headers.end())
        {
          if ((_length = lexical_cast<size_t>(cit->second)) == 0)
          {
            // means message completed, unicomm will buffer.erase(bounds.first, bounds.second)
            _length = i + head_body_separator().size(); // add header size
            bounds  = finish_decode(buffer);
          } else
          {
            _length += i + head_body_separator().size(); // add header size

            // whether message completed?
            if (_length <= buffer.size())
            {
              bounds = finish_decode(buffer);
            }
          }
        } else
        {
          // means undefined length
          UNICOMM_DEBUG_OUT("[uni_http::message_decoder]: DECODE; Content-Length header is absent")

          // Usually, message processing is identical on both client and server sites. But
          // sometimes it's necessary to distinct one from another. There are several
          // way to achieve this. You may put information into the session object and use it,
          // as done here. Also you may define two different classes derived from 
          // unicomm::message_decoder_base (i.e. server_decoder & client_decoder) and
          // implement necessary interface the way you intend.
          // Then you create two different configuration objects for server and client 
          // respectively and pass corresponding decoder objects to those configurations.

          if (session.is_server())
          { 
            if (i + head_body_separator().size() == buffer.size())
            {
              // it seems there is no body contained by the request
              _length = buffer.size();
              bounds  = finish_decode(buffer);
            } else
            {
              BOOST_ASSERT(buffer.size() > i + head_body_separator().size());

              throw length_required_error(header::content_length() + 
                " header not found, but required");
            }
          } else
          {
            // client waits for the reply within timeout and disconnects if timeout occurs
            _length = numeric_limits<size_t>::max();
          }
        }
      } 
    } else if (_length <= buffer.size())
    {
      // whether message completed?
      bounds = finish_decode(buffer);
    }

    UNICOMM_DEBUG_OUT("[uni_http::message_decoder]: DECODE EXIT; Length = " 
      << _length << "; Buffer size = " << buffer.size())
  }

  return bounds;
}

//-----------------------------------------------------------------------------
string& uni_http::message_decoder::decode_raw_message(string& raw_message, 
                                                      unicomm::session_base& /*session*/)
{
  return raw_message;
}

//-----------------------------------------------------------------------------
string uni_http::message_decoder::get_message_name(const string& /*raw_message*/, 
                                                   unicomm::session_base& /*session*/)
{
  return "";
}

//-----------------------------------------------------------------------------
unicomm::message_decoder_base::iter_pair_type 
uni_http::message_decoder::finish_decode(unicomm::in_buffer_type& buffer)
{
  const size_t tmp          = _length;
  _length = _last_buf_size  = 0;

  return make_pair(buffer.begin(), buffer.begin() + tmp);
}

