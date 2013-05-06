///////////////////////////////////////////////////////////////////////////////
// session_base.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unicomm session base class definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#include <unicomm/session_base.hpp>

//////////////////////////////////////////////////////////////////////////
// session base

/** Actually calls virtual message arrived handler. */
void unicomm::session_base::signal_message_arrived(message_arrived_params& params)
{
  message_arrived_handler(params);
}

/** Calls message timeout handler. */
void unicomm::session_base::signal_message_timeout(const message_timeout_params& params)
{
  message_timeout_handler(params);
}

/** Calls client connected handler. */
void unicomm::session_base::signal_connected(const connected_params& params)
{
  connected_handler(params);
}

/** Calls client disconnected handler. */
void unicomm::session_base::signal_disconnected(const disconnected_params& params)
{
  disconnected_handler(params);
}

/** Calls error handler. */
void unicomm::session_base::signal_error(const error_params& params)
{
  error_handler(params);
}

/** Calls message sent handler. */
void unicomm::session_base::signal_message_sent(const message_sent_params& params)
{
  message_sent_handler(params);
}

/** Calls after processed handler. */
void unicomm::session_base::signal_after_processed(const after_processed_params& params)
{
  after_processed_handler(params);
}



