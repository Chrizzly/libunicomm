///////////////////////////////////////////////////////////////////////////////
// main.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Mostly simple example on how to use unicomm engine to transfer custom data.
// Sends characters from stdin to opposite site.
// Defines the entry point for the console application.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.

#include "term.hpp"

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio/ip/tcp.hpp>

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

using boost::lexical_cast;
using boost::asio::ip::tcp;

namespace 
{

namespace po = boost::program_options;

#if defined (UNI_WIN)
typedef _TCHAR char_type;
#else
typedef char char_type;
#endif // UNI_WIN

po::variables_map handle_command_line(int argc, char_type* argv[]);
void server_loop(const unicomm::config& config);
void client_loop(const unicomm::config& config);

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
    po::variables_map vm = handle_command_line(argc, argv);

    const string& host   = vm["host"].as<string>();
    const string& port   = vm["port"].as<string>();
    const bool is_server = vm.count("client") == 0;

    const tcp::resolver::query q(host, lexical_cast<string>(port));
    unicomm::host_resolver resolver = unicomm::resolve_host(q);

    const unicomm::config& config = uni_term::config().endpoint(*resolver.begin());

    is_server? 
      server_loop(config): 
      client_loop(config);
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

//////////////////////////////////////////////////////////////////////////
// main implementation detail

namespace 
{

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
  uni_term::out_line("client: started");
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
  uni_term::out_line("server: started");
}

#ifdef UNI_LINUX
int getch(void);
inline int _getch(void);
#endif // UNI_LINUX

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4127) //  warning C4127: conditional expression is constant
#endif // UNI_VISUAL_CPP

//-----------------------------------------------------------------------------
void server_loop(const unicomm::config& config)
{
  unicomm::server server(config);

  server_start(server, 2);

  while (true) //  warning C4127
  {
    unicomm::send_all(server, uni_term::message(string(1, char(_getch()))));
  }
}

//-----------------------------------------------------------------------------
void client_loop(const unicomm::config& config)
{
  unicomm::client client(config, &uni_term::connect_error_handler);

  client_start(client, 2);

  while (true) //  warning C4127
  {
    unicomm::send_all(client, uni_term::message(string(1, char(_getch()))));
  }
}

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

//-----------------------------------------------------------------------------
po::variables_map handle_command_line(int argc, char_type* argv[])
{
  po::options_description desc("Options");

  desc.add_options()
    ("help", "Show this screen")
    ("host", po::value<string>()->default_value("0.0.0.0"), 
      "Ip-address or host name to be listened by server or connected to by client")
    ("port", po::value<string>()->default_value("55555"), 
      "TCP port to be listened by the server or connected to by client")
    ("client", "Run as client, by default run as server")
    ;

  po::variables_map vm;

  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || vm.count("h"))
    {
      cout << "\nUnicomm based terminal sample." 
        << " Sends input from stdin to opposite site.\n\n" 
        << desc << "\n";

      exit(EXIT_SUCCESS);
    }
  } catch (...)
  {
    cout << "\nUnicomm based terminal sample." 
      << " Sends input from stdin to opposite site.\n\n" 
      << desc << "\n";

    exit(EXIT_SUCCESS);
  }

  return vm;
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

} // unnamed namespace

