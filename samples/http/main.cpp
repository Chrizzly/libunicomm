///////////////////////////////////////////////////////////////////////////////
// main.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Simple HTTP server & client example based on unicomm engine.
// Defines the entry point for the console application.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#include "http_client_session.hpp"
#include "http_server_session.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_server.hpp"
#include "http_aux.hpp"
#include "http_config.hpp"
#include "http_except.hpp"
#include "http_basic.hpp"

#include <unicomm/helper.hpp>

#include <smart/utils.hpp>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/assign.hpp>
#include <boost/regex.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/date_time.hpp>

#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable : 4512)  // warning C4512: 'boost::program_options::options_description' : assignment operator could not be generated
#endif // _MSC_VER

#include <boost/program_options.hpp>

#ifdef _MSC_VER
# pragma warning (pop)
#endif // _MSC_VER

#include <string>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <utility>

#if defined (UNI_WIN)
# include <tchar.h>
# include <conio.h>
#elif defined (UNI_UNIX)
# include <fcntl.h>
# include <signal.h>
# include <stdio.h>
# include <unistd.h>
# include <termios.h>
# include <sys/ioctl.h>
# include <sys/time.h>
# include <sys/types.h>
#endif // UNI_WIN

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::runtime_error;
using std::equal_to;
using std::less;
using std::not_equal_to;
using std::flush;
using std::stringstream;
using std::make_pair;

using boost::regex;
using boost::cmatch;
using boost::regex_match;
using boost::lexical_cast;
using boost::assign::map_list_of;
using boost::asio::ip::tcp;
using boost::this_thread::sleep;
using boost::posix_time::millisec;
using boost::posix_time::seconds;

#include "main_stuff.inl"

//////////////////////////////////////////////////////////////////////////
// main
#if defined (UNI_WIN)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif // UNI_WIN

{
  try
  {
    po::variables_map vm = handle_command_line(argc, argv);

    size_t threads = vm["threads"].as<size_t>();

    if (threads < 1) { threads = 1; }
    if (threads > 10) { threads = 10; }

    if (vm.count("url") == 0)
    {
      const string& host = vm["host"].as<string>();
      const string& port = vm["port"].as<string>();

      //fixme: setter for configuration in dispatcher?
      const boost::asio::ip::tcp::resolver::query q(host, port);
      unicomm::host_resolver resolver = unicomm::resolve_host(q);

      uni_http::server_config().endpoint(*resolver.begin());

#ifdef UNI_UNIX

      if (vm.count("daemon") != 0)
      {
        const daemonize_result_type res = become_daemon();
        if (!is_daemonize_error(res))
        {
          if (is_parent_process(res))
          {
            cout << "\r\n[http]: Daemon successfully started; pid = " << res.second
              << "\r\n[http]: To terminate process use 'sudo kill -s SIGTERM "
              << res.second << "'\r\n";

            UNICOMM_DEBUG_OUT("[http]: Parent: exit")
            exit(EXIT_SUCCESS);
          }

          UNICOMM_DEBUG_OUT("[http]: Child: demon now; pid = " << res.second)
          UNICOMM_DEBUG_OUT("[http]: Child: starting forever loop...")
          // start server in child process
          start(server(), threads);
          // forever loop
          while (true) { sleep(seconds(1)); }
        } else
        {
          cout << "\r\n[http]: An error occurred while trying to become a demon... exit" << endl;
          UNICOMM_DEBUG_OUT("[http]: An error occurred while trying to become a demon... exit")
          exit(EXIT_FAILURE);
        }
      } else

#endif // UNI_UNIX

      {
        start(server(), threads);

        uni_http::out_str("Press a key to quit");
        _getch();

        stop(server());
      }
    } else
    {
      const string& url  = vm["url"].as<string>();

      client_send_request(client(), url);

      do
      {
        sleep(millisec(200));
      } while (client().connections_count());
    }
  }
  catch (const std::exception& e)
  {
    cout << endl << "An error occurred: " << e.what() << endl;

    return EXIT_FAILURE;
  }
  catch (...)
  {
    cout << endl << "An error occurred: Unknown error" << endl;

    BOOST_ASSERT(!"Unknown error");

    return EXIT_FAILURE;
  }

	return EXIT_SUCCESS;
}

