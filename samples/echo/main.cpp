///////////////////////////////////////////////////////////////////////////////
// main.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Simple echo server & client example based on unicomm engine.
// Defines the entry point for the console application.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#include "echo.hpp"

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio/ip/tcp.hpp>

#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable : 4267)  // warning C4267: 'argument' : conversion from 'size_t' to 'unsigned int', possible loss of data
# pragma warning (disable : 4512)  // warning C4512: 'boost::program_options::options_description' : assignment operator could not be generated
#endif // _MSC_VER

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#ifdef _MSC_VER
# pragma warning (pop)
#endif // _MSC_VER

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <locale>
#include <iterator>

#include <cstdio>

#if defined (UNI_WIN)
# include <tchar.h>
# include <conio.h>
#elif defined (UNI_LINUX)
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
using std::flush;
using std::ostream;
using std::istream;
using std::ifstream;
using std::runtime_error;
using std::getline;
using std::copy;
using std::locale;
using std::back_inserter;

using boost::lexical_cast;
using boost::asio::ip::tcp;

using unicomm::strings_type;

namespace 
{

namespace po = boost::program_options;

#if defined (UNI_WIN)
typedef _TCHAR char_type;
#else
typedef char char_type;
#endif // UNI_WIN

po::variables_map handle_command_line(int argc, char_type* argv[]);
void server_loop(const unicomm::config& config, size_t threads_num = 1);
void client_loop(const unicomm::config& config, size_t threads_num = 1);
strings_type request_list(const po::variables_map& vm);

} // unnamed namespace

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
    po::variables_map vm  = handle_command_line(argc, argv);

    const string& host    = vm["host"].as<string>();
    const string& port    = vm["port"].as<string>();
    size_t threads        = vm["threads"].as<size_t>();

    if (threads < 1) { threads = 1; }
    if (threads > 10) { threads = 10; }

    const tcp::resolver::query q(host, lexical_cast<string>(port));
    unicomm::host_resolver resolver = unicomm::resolve_host(q);

    if (const bool is_server = vm.count("client") == 0)
    {
      const unicomm::config& config = uni_echo::config()
        .session_factory(&uni_echo::server_session_type::create)
        .endpoint(*resolver.begin());

      server_loop(config, threads);
    } else
    {
      const bool is_infinite   = vm["req-number"].as<size_t>() == 0;
      const size_t clients_num = vm["client"].as<size_t>();

      const unicomm::config& config = uni_echo::config()
        .session_factory(boost::bind(&uni_echo::client_session_type::create, _1, 
          uni_echo::client_session_params(request_list(vm), is_infinite, clients_num)))
        .endpoint(*resolver.begin());

      client_loop(config, threads);
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

// aux
namespace 
{

//-----------------------------------------------------------------------------
string now(void) 
{
  const time_t now = time(0);
#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4996)  // warning C4996: 'asctime': This function or variable may be unsafe. Consider using asctime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif // UNI_VISUAL_CPP

  string s = asctime(gmtime(&now));
  BOOST_ASSERT(!s.empty() && " - Date time string could not be empty");

  // remove asctime new line character if there is
  if (s.at(s.size() - 1) < ' ') { s.resize(s.size() - 1); }

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP
  return s;
}

//------------------------------------------------------------------------
//typedef std::ctype<char> ctype;
//
//struct tokens : ctype
//{
//  tokens(void): ctype(get_table()) { /* empty */ }
//
//  static const std::ctype_base::mask* get_table(void)
//  {
//    static const ctype::mask *basic = ctype::classic_table();
//
//    static ctype::mask rc[ctype::table_size];
//    //std::memcpy(rc, basic, ctype::table_size * sizeof(ctype::mask));
//    copy(basic, basic + ctype::table_size, rc);
//    
//    rc[' '] = 0;
//
//    return &rc[0];
//  }
//};

//------------------------------------------------------------------------
strings_type request_list(const po::variables_map& vm)
{
  size_t req_num         = vm["req-number"].as<size_t>();
  const bool is_infinite = req_num == 0;

  strings_type result;

  if (vm.count("request"))
  {
    string request = vm["request"].as<string>();

    strings_type tmp(is_infinite? 1: req_num, request);
    result.swap(tmp);
  } else if (vm.count("request-list"))
  {
    const string& req_list_fn = vm["request-list"].as<string>();

    ifstream inf(req_list_fn.c_str());
    inf.exceptions(ifstream::badbit);

    //inf.imbue(locale(locale(), new tokens()));

    //std::istream_iterator<string> eos;
    //std::istream_iterator<string> it(inf);

    //copy(it, eos, back_inserter(result));  

    for (string s; getline(inf, s) && (is_infinite? true: req_num--); )
    {
      result.push_back(s);
    }
  } else
  {
    strings_type tmp(is_infinite? 1: req_num, now());
    result.swap(tmp);
  }

  return result;
}

//------------------------------------------------------------------------
template <typename FunctionT>
void start_task(const FunctionT& f, unicomm::dispatcher& d, size_t n = 1)
{
  while (n--)
  {
    boost::thread t(f, boost::ref(d));
  }
}

//////////////////////////////////////////////////////////////////////////
// client
void client_task(unicomm::dispatcher& c)
{
  c.run();
}

//------------------------------------------------------------------------
void client_start(unicomm::client& c, size_t threads_num = 1)
{
  if (!c.started())
  {
    c.reset();
  }

  c.connect();
  start_task(&client_task, c, threads_num);
  uni_echo::out_str("client: started");
}

//------------------------------------------------------------------------
void client_stop(unicomm::client& c)
{
  if (c.started())
  {
    c.stop();
    uni_echo::out_str("client: stopped");
  } else
  {
    uni_echo::out_str("client: i'm not started yet");
  }
}

//////////////////////////////////////////////////////////////////////////
// server
void server_task(unicomm::dispatcher& s)
{
  s.run();
}

//------------------------------------------------------------------------
void server_start(unicomm::server& s, size_t threads_num = 1)
{
  if (!s.started())
  {
    s.reset();
  }

  s.accept();
  start_task(&server_task, s, threads_num);
  uni_echo::out_str("server: started");
}


//------------------------------------------------------------------------
void server_stop(unicomm::server& s)
{
  if (s.started())
  {
    s.stop();
    uni_echo::out_str("server: stopped");
  } else
  {
    uni_echo::out_str("server: I'm not started yet");
  }
}

#ifdef UNI_LINUX

/*
 *  *getch*()  --  a blocking single character input from stdin
 *
 *  Returns a character, or -1 if an input error occurs.
 *
 *
 *  Conditionals allow compiling with or without echoing of
 *  the input characters, and with or without flushing pre-existing
 *  existing  buffered input before blocking.
 *
 */
int getch(void) 
{
  char                  ch;
  int                   error;
  static struct termios Otty, Ntty;

  fflush(stdout);
  tcgetattr(0, &Otty);
  Ntty = Otty;

  Ntty.c_iflag  =  0;           /* input mode           */
  Ntty.c_oflag  =  0;           /* output mode          */
  Ntty.c_lflag &= ~ICANON;  /* line settings        */

#if 1
  /* disable echoing the char as it is typed */
  Ntty.c_lflag &= ~ECHO;    /* disable echo         */
#else
  /* enable echoing the char as it is typed */
  Ntty.c_lflag |=  ECHO;        /* enable echo          */
#endif

  Ntty.c_cc[VMIN]  = CMIN;      /* minimum time to wait */
  Ntty.c_cc[VTIME] = CTIME;     /* minimum chars to wait for */

#if 1
/*
 * use this to flush the input buffer before blocking for new input
 */
#define FLAG TCSAFLUSH
#else
/*
 * use this to return a char from the current input buffer, or block if
 * no input is waiting.
 */
#define FLAG TCSANOW

#endif

 if (0 == (error = tcsetattr(0, FLAG, &Ntty))) {
    error  = read(0, &ch, 1 );                /* get char from stdin */
    error += tcsetattr(0, FLAG, &Otty);   /* restore old settings */
  }

  return (error == 1 ? (int) ch : -1 );

}

//-------------------------------------------------------------------------
inline int _getch(void)
{
  return getch();
}

#endif // UNI_LINUX

//-----------------------------------------------------------------------------
po::variables_map handle_command_line(int argc, char_type* argv[])
{
  po::options_description desc("Options");

  desc.add_options()
    ("help,h", "Show this screen")
    ("host", po::value<string>()->default_value("0.0.0.0"), 
      "Ip-address or host name to be listened by server or connected to by client")
    ("port", po::value<string>()->default_value("55555"), 
      "TCP port to be listened by the server or connected to by client")
    ("client", po::value<size_t>()->implicit_value(1), 
      "Run as client, by default run as server. Setup the number of clients to produce.")
    ("request", po::value<string>()->implicit_value(""),
      "Request data, by default current date time string is used")
    ("req-number", po::value<size_t>()->default_value(1), 
      "Number of requests to send. If zero client will produce requests infinitely")
    (
      "request-list", po::value<string>()->implicit_value("requests"), 
      "If specified, requests will be taken from given text file. "
      "This file is just strings list (each line is request). "
      "Implicit value is 'requests'."
    )
    ("threads", po::value<size_t>()->default_value(1), 
      "Threads to run server or client task [1, 10]")
    ;

  po::variables_map vm;

  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || vm.count("h"))
    {
      cout << "\nUnicomm based echo server & client sample." 
        << " Sends and receives messages based on unicomm predefined format.\n\n" 
        << desc << "\n";

      exit(EXIT_SUCCESS);
    }
  } catch (...)
  {
    cout << "\nUnicomm based echo server & client sample." 
      << " Sends and receives messages based on unicomm predefined format.\n\n" 
      << desc << "\n";

    exit(EXIT_SUCCESS);
  }

  return vm;
}

//-----------------------------------------------------------------------------
void server_loop(const unicomm::config& config, size_t threads_num)
{
  unicomm::server server(config);

  server_start(server, threads_num);

  cout << "\nq for exit" << endl; 
  for (char c = 0; c != 'q' && c != 'Q'; c = char(_getch()))
  {
    ;
  }
}

//-----------------------------------------------------------------------------
void client_loop(const unicomm::config& config, size_t threads_num)
{
  unicomm::client client(config, &uni_echo::connect_error_handler);

  client_start(client, threads_num);

  cout << "\nq for exit" << endl; 
  for (char c = 0; c != 'q' && c != 'Q'; c = char(_getch()))
  {
    ;
  }
}

} // unnamed namespace
