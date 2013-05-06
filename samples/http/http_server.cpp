///////////////////////////////////////////////////////////////////////////////
// http_server.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Http server class definition.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_server.hpp"
#include "http_except.hpp"
#include "http_aux.hpp"

#include <stdexcept>

#ifdef UNI_UNIX
# include <sys/types.h>
# include <unistd.h>
#endif // UNI_UNIX

//////////////////////////////////////////////////////////////////////////
// http_server

// When using fork support. 
// Do not forget to implement disconnected handler. In this sample
// it's located in main.cpp and has same implementation for both
// HTTP_FORK_ON_ACCEPT defined and not defined variants.

void uni_http::server::after_accept(unicomm::tcp_socket_type& socket) 
{ 
#if defined (UNICOMM_FORK_SUPPORT) && defined (HTTP_FORK_ON_ACCEPT)

# ifdef UNICOMM_SSL
#   error "Fork in couple with ssl can't be properly used in this sapmle within from uni_http::server::after_accept()"
# endif // UNICOMM_SSL

  try
  {
    if (is_parent_process())
    {
      UNICOMM_DEBUG_OUT("[uni_http::server]: Performing fork...")

      fork_prepare();

      pid(check_fork(fork()));

      if (is_child_process()) 
      {
        // child process
        UNICOMM_DEBUG_OUT("[uni_http::server]: Child: fork sweeping; pid = " << pid())
        fork_child();
        UNICOMM_DEBUG_OUT("[uni_http::server]: Child: io service ready")

        stop_accept();
        socket.set_option(boost::asio::ip::tcp::socket::keep_alive(true));
      } else
      {
        // parent process
        UNICOMM_IFDEF_DEBUG(static size_t childs_count = 0;)
        UNICOMM_DEBUG_OUT("[uni_http::server]: Parent: fork sweeping; childs produced: [" 
          << ++childs_count << "]")
        // need no to close socket explicitly, all necessary operations are
        // performed by fork_parent()
        fork_parent();
        UNICOMM_DEBUG_OUT("[uni_http::server]: Parent: io service ready")
      }
    }
  } 
  catch (const fork_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server]: Fork error [" << e.what() << "]")
  }
  catch (const boost::system::system_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server]: Fatal error, can't continue [" 
      << e.what() << "]")
    exit(EXIT_FAILURE);
  }
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server]: An UNEXPECTED std::exception [" 
      << e.what() << "]")

    BOOST_ASSERT(!"An UNEXPECTED std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[uni_http::server]: An UNEXPECTED UNKNOWN exception")

    BOOST_ASSERT(!"An UNEXPECTED UNKNOWN exception");
  }

#else

  socket.set_option(boost::asio::ip::tcp::socket::keep_alive(true));

#endif // #if defined (UNICOMM_FORK_SUPPORT) && defined (HTTP_FORK_ON_ACCEPT)
}

