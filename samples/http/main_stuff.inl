///////////////////////////////////////////////////////////////////////////////
// main_stuff.inl
//
// unicomm - Unified Communication protocol C++ library.
//
// Simple HTTP server & client example based on unicomm engine.
// Main stuff.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.

namespace
{

namespace po = boost::program_options;

#if defined (UNI_WIN)
typedef _TCHAR char_type;
#else
typedef char char_type;
#endif // UNI_WIN

//////////////////////////////////////////////////////////////////////////
// data
uni_http::request _request;

void print_hello(void)
{
#ifdef UNICOMM_SSL
  cout << "\r\nUnicomm based HTTPS server & client sample";
#else
  cout << "\r\nUnicomm based HTTP server & client sample";
#endif // UNICOMM_SSL
}

//------------------------------------------------------------------------
void stop(unicomm::dispatcher& d)
{
  if (d.started())
  {
    d.stop();
  }
}

//------------------------------------------------------------------------
template <typename FunctionT>
void start_task(const FunctionT& f, unicomm::dispatcher& d, size_t n = 1)
{
  while (n--)
  {
    boost::thread(f, boost::ref(d));
  }
}

//////////////////////////////////////////////////////////////////////////
// client

void start(unicomm::client& c, size_t threads_num = 1);

//------------------------------------------------------------------------
void client_send_request(unicomm::client& c, const string& url)
{
  const string pattern = "((https?)://([-\\w\\.]+)+(:\\d+)?(/([\\w/_\\.]*(\\?\\S+)?)?)?)";

  const regex ex(pattern, boost::regex::mod_s);

  cmatch matches;
  if (!regex_match(url.c_str(), matches, ex))
  {
    throw runtime_error("Target url is invalid");
  }

  //smart::throw_if<runtime_error>(boost::bind(equal_to<bool>(), regex_match(url.c_str(), matches, ex), false),
  //  "Target url is invalid");
  smart::throw_if<runtime_error>(boost::bind(equal_to<bool>(), matches[2].matched, false),
    "Invalid protocol type specified only http or https allowed");
  smart::throw_if<runtime_error>(boost::bind(equal_to<bool>(), matches[3].matched, false),
    "Host name unrecognized");

  const string protocol = matches[2];
  const string host     = matches[3];
  const string port     = matches[4].matched? string(matches[4]):
    lexical_cast<string>(uni_http::default_port());
  const string uri      = string(matches[5]).empty()? string("/"): 
    string(matches[5]);

#ifdef UNICOMM_SSL
  const string used_proto = "https";
  const char* err_message = "HTTP is not accessible in current build. "
                            "To exclude SSL remove UNICOMM_SSL macro.";
#else
  const string used_proto = "http";
  const char* err_message = "HTTPS is not accessible in current build. "
                            "To make it to be available define UNICOMM_SSL macro.";
#endif // UNICOMM_SSL

  smart::throw_if<runtime_error>(boost::bind(
    not_equal_to<string>(), used_proto, protocol), err_message);

  const tcp::resolver::query q(host, port);
  unicomm::host_resolver resolver = unicomm::resolve_host(q);

  _request.name(uni_http::request::get());
  _request.uri(uri);

  _request.clear_headers();

  _request
    .add_header(uni_http::header::user_agent(), "Unicomm based HTTP 1.0 client")
    .add_header(uni_http::header::accept(), uni_http::mime_type::text_html())
    .add_header(uni_http::header::host(), host)
    .add_header(uni_http::header::accept_language(), "ru")
    .add_header(uni_http::header::accept_encoding(), "")
  ;

  c.connect(tcp::endpoint(*resolver.begin()));

  start(c);
}

//////////////////////////////////////////////////////////////////////////
// client handlers
//
// Those show how to add handlers instead of overriding
// unicomm::session_base virtual members. You are enabled to map
// arbitrary number of handlers to connected and disconnected events.
// Other events provide only one extra handler to be mapped.
// If you need a sequence of handlers to be mapped to other events
// use boost signals to implement this.

void client_connected_handler(const unicomm::connected_params& params)
{
  params.comm().send(_request);
}

//////////////////////////////////////////////////////////////////////////
// client

unicomm::client& client(void)
{
  // initialize client's configuration
  static const unicomm::config& client_config = uni_http::client_config()
    .session_factory(boost::bind(&uni_http::client_session::create, _1,
      uni_http::client_session_params(&client_connected_handler)));

  // create client and assign a connection error handler
  // connection error handler is an exception to the rules
  // this handler is not included to the session due to session object
  // is not created to the moment when connection error occurs.
  // this handler is only called if client::connect() invoked and connect failed,
  // so in such a case there is no session object exists

  // create client
  static unicomm::client client(client_config, &uni_http::connect_error_handler);

  return client;
}

//------------------------------------------------------------------------
void client_task(unicomm::dispatcher& c)
{
  c.run();
}

//------------------------------------------------------------------------
void start(unicomm::client& c, size_t threads_num)
{
  if (!c.started())
  {
    c.reset();
  }

  //c.connect();
  start_task(&client_task, c, threads_num);
  uni_http::out_str("client> started");
}

//////////////////////////////////////////////////////////////////////////
// server

///////////////////////////////////////////////////////////////////////////
// Performing fork only from within parent process listening the port for
// incoming connections. Uncomment second part of the statement
// if you wish to fork process only if current connections number exceeds the
// limit. If concurrent connections number is lesser than ten they will be
// served by parent process itself, but if the limit exceeded
// process will fork and give all the connections to the child process.
// Please note connections count may take any value greater than or equal one.
// While the statement is commented the limit is equal to one.
// The statement implicit statement in this case looks like
// params.comm().owner().connections_count() > 0 due to
// connected handler invoked only if one or more incoming connections
// accepted on current iteration.
//
// To strictly have one served connection per process implement
// fork on unicomm::server::after_accept(). On how it could be done please
// take a look at uni_http::server::after_accept().
// Define HTTP_FORK_ON_ACCEPT to make this code to be compiled.
// Do not forget to clear unicomm/out/stuff before start building
// with new define.

void actual_server_connected_handler(const unicomm::connected_params& params)
{
#if defined (UNICOMM_FORK_SUPPORT) && !defined (HTTP_FORK_ON_ACCEPT)

  try
  {
    if (uni_http::is_parent_process() /*&& params.comm().owner().connections_count() > 9*/)
    {
      UNICOMM_DEBUG_OUT("[http]: Performing fork...")

      params.comm().fork_prepare();

      uni_http::pid(uni_http::check_fork(fork()));

      if (uni_http::is_child_process())
      {
        // child process
        UNICOMM_DEBUG_OUT("[http]: Child: fork sweeping; pid = " << uni_http::pid())
        params.comm().fork_child();
        UNICOMM_DEBUG_OUT("[http]: Child: io service ready")

        static_cast<uni_http::server&>(params.comm().owner()).stop_accept();
      } else
      {
        // parent process
        UNICOMM_IFDEF_DEBUG(static size_t childs_count = 0;)
        UNICOMM_DEBUG_OUT("[http]: Parent: fork sweeping; childs produced: [" 
          << ++childs_count << "]")
        params.comm().fork_parent();
        UNICOMM_DEBUG_OUT("[http]: Parent: io service ready")
      }
    }
  }
  catch (const uni_http::fork_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[http]: Fork error [" << e.what() << "]")
  }
  catch (const boost::system::system_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[http]: Fatal error, can't continue [" << e.what() << "]")
    exit(EXIT_FAILURE);
  }
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[http]: An UNEXPECTED std::exception [" << e.what() << "]")

    BOOST_ASSERT(!"An UNEXPECTED std::exception");
  }
  catch (...)
  {
    UNICOMM_DEBUG_OUT("[http]: An UNEXPECTED UNKNOWN exception")

    BOOST_ASSERT(!"An UNEXPECTED UNKNOWN exception");
  }

#elif defined (UNI_VISUAL_CPP)

  params; // avoid unreference parameter warning

#endif // UNICOMM_FORK_SUPPORT
}

//------------------------------------------------------------------------
void actual_server_disconnected_handler(const unicomm::disconnected_params& params)
{
#if defined (UNICOMM_FORK_SUPPORT)

  if (uni_http::is_child_process() && (params.comm().owner().connections_count() == 1))
  {
    UNICOMM_DEBUG_OUT("[http]: Child: exit")
    exit(EXIT_SUCCESS);
  }

#elif defined (UNI_VISUAL_CPP)

  params; // avoid unreference parameter warning

#endif // UNICOMM_FORK_SUPPORT
}

//------------------------------------------------------------------------
void server_connected_handler(const unicomm::connected_params& params)
{
  UNICOMM_DEBUG_OUT("[http]: CONNECTED; Connections count = " 
    << params.comm().owner().connections_count())

  actual_server_connected_handler(params);
}

//------------------------------------------------------------------------
void server_disconnected_handler(const unicomm::disconnected_params& params)
{
  UNICOMM_DEBUG_OUT("[http]: DISCONNECTED; Connections count = " 
    << params.comm().owner().connections_count())

  actual_server_disconnected_handler(params);
}


//////////////////////////////////////////////////////////////////////////
// server definition
uni_http::server& server(void)
{
  // setup extra parameters before use configuration
  static const unicomm::config& server_config = uni_http::server_config().session_factory
    (
      boost::bind
        (
          &uni_http::server_session::create, _1,
          uni_http::server_session_params
            (
              map_list_of
                ("localhost", uni_http::host_config(
                  "localhost", "../../../../http/www/localhost"))
                ("bora-bora", uni_http::host_config(
                  "bora-bora", "../../../../http/www/bora-bora")),
              &server_connected_handler,
              &server_disconnected_handler
            )
        )
    )
  ;

  // create server
  static uni_http::server server(server_config);

  return server;
}

//------------------------------------------------------------------------
po::variables_map handle_command_line(int argc, char_type* argv[])
{
  po::options_description desc("Options");

  desc.add_options()
    ("help", "Show this screen")
    ("host", po::value<string>()->default_value("0.0.0.0"),
      "Ip-address or host name to be listened by the server")
    ("port", po::value<string>()->default_value(
      lexical_cast<string>(uni_http::default_port())),
      "TCP port to be listened by the server")
    ("url", po::value<string>(), "Run as client and connect to the url. "
      "If omited runs as server")
    ("threads", po::value<size_t>()->default_value(1), 
      "Threads to run server or client task [1, 10]")

#if defined (UNI_LINUX)
    (
      "daemon", "Start as daemon. Only matters under Linux like systems "
      "and if server option specified"
    )
#endif // UNI_LINUX
    // fixme: add daemon & fork support
    ;

  po::variables_map vm;

  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || vm.count("h"))
    {
      cout << "\nUnicomm based HTTP sample.\n\n" << desc << "\n";

      exit(EXIT_SUCCESS);
    }
  } catch (...)
  {
    cout << "\nUnicomm based HTTP sample.\n\n" << desc << "\n";

    exit(EXIT_SUCCESS);
  }

  return vm;
}

//------------------------------------------------------------------------
void server_task(unicomm::dispatcher& s)
{
  s.run();
}

//------------------------------------------------------------------------
void start(unicomm::server& s, size_t threads_num = 1)
{
  if (!s.started())
  {
    s.reset();
  }

  s.accept();
  start_task(&server_task, s, threads_num);

  stringstream ss;
  ss << s.config().endpoint();
  uni_http::out_str("Listening on " + ss.str() + " for " + uni_http::version());
}

//////////////////////////////////////////////////////////////////////////
// This stuff used to make process run as daemon under Unix systems
#ifdef UNI_UNIX

void signal_handler(int sig)
{
  switch(sig)
  {
    //case SIGUSR1:
    //  UNICOMM_DEBUG_OUT("[http]: SIGUSR1 is arrived")
    //  start(server());
    //break;

    //case SIGHUP:
    //  UNICOMM_DEBUG_OUT("[http]: SIGHUP is arrived")
    //break;

    case SIGTERM:
      {
        UNICOMM_DEBUG_OUT("[http]: SIGTERM is arrived")

        int res = EXIT_FAILURE;
        try
        {
          UNICOMM_DEBUG_OUT("[http]: Stopping services...")

          stop(server());
          res = EXIT_SUCCESS;

          UNICOMM_DEBUG_OUT("[http]: Services are successfully stopped")
        }
        catch (const std::exception& e)
        {
          UNICOMM_DEBUG_OUT("[http]: An exception has occurred while stopping [" 
            << e.what() << "]")
        }
        catch (...)
        {
          UNICOMM_DEBUG_OUT("[http]: An unknwon exception has occurred while stopping")
        }

        UNICOMM_DEBUG_OUT("[http]: Finita")

        exit(res);
      }
    break;
  }
}

//------------------------------------------------------------------------
bool is_daemon(void)
{
  return getppid() == 1;
}

//------------------------------------------------------------------------
int check_error(int i, const string& err_message)
{
  if (i < 0)
  {
    throw runtime_error(err_message);
  }

  return i;
}

//------------------------------------------------------------------------
int dup_fd(int old_fd)
{
  return check_error(dup(old_fd), "Couldn't dup");
}

//------------------------------------------------------------------------
void setup_linux_signals(void)
{
  // register signal handlers
#ifndef UNICOMM_FORK_SUPPORT

  signal(SIGCHLD, SIG_IGN); // ignore child

#endif // UNICOMM_FORK_SUPPORT

  signal(SIGTSTP, SIG_IGN); // ignore tty signals
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGHUP, &signal_handler); // catch hangup signal
  signal(SIGTERM, &signal_handler); // catch term signal
  signal(SIGUSR1, &signal_handler); // catch usr1 signal
}

//-------------------------------------------------------------------------
typedef std::pair<bool, int> daemonize_result_type;

//-------------------------------------------------------------------------
bool is_daemonize_error(const daemonize_result_type& res)
{
  return res.second == 0;
}

//------------------------------------------------------------------------
bool is_child_process(const daemonize_result_type& res)
{
  return res.first;
}

//------------------------------------------------------------------------
bool is_parent_process(const daemonize_result_type& res)
{
  return !is_child_process(res);
}

//------------------------------------------------------------------------
daemonize_result_type daemonize(void)
{
  // create child process
  int i = fork();
  // check fork error
  check_error(i, "Couldn't fork");
  // parent returns
  if (i > 0)
  {
    UNICOMM_DEBUG_OUT("[http]: Parent: fork succeeded")
    return daemonize_result_type(false, i);
  }
  // child (daemon) continues
  UNICOMM_DEBUG_OUT("[http]: Child: fork succeeded")
  // obtain a new process group
  check_error(setsid(), "Couldn't setsid");

  for (int i = getdtablesize(); i >= 0; --i)
  {
    // close all descriptors
    close(i);
  }

  // handle standart I/O
  check_error(open("/dev/null", O_RDWR), "Couldn't open [/dev/null]");

  dup_fd(i); dup_fd(i);
  // set newly created file permissions
  umask(027);

  //const string running_dir = "/tmp";
  // change running directory
  //check_error(chdir(running_dir.c_str()), 
  //  "Couldn't change process current directory to [" + running_dir + "]");

  //////////////////////////////////////////////////////////////////////////
  // lock file - commented
  UNICOMM_DEBUG_OUT("[http]: Child: start locking file...")
  const string lock_file   = "/tmp/uni_http.lock";

  i = check_error(open(lock_file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0640), 
    "Couldn't open lock file [" + lock_file + "]");
  UNICOMM_DEBUG_OUT("[http]: Child: lock file is opened successfully [" 
    << lock_file << "]")
  // try to lock
  check_error(lockf(i, F_TLOCK, 0), "Couldn't set lock to the file [" + 
    lock_file + "]");
  UNICOMM_DEBUG_OUT("[http]: Child: lock file is successfully captured [" 
    << lock_file << "]")
  // first instance continues
  stringstream ss;
  const int child_pid = getpid();
  ss << child_pid;
  const string s = ss.str();
  // write pid to lockfile
  check_error(write(i, s.c_str(), s.size()), "Couldn't write lock file [" + 
    lock_file + "]");

  // register signal handlers
  setup_linux_signals();

  return daemonize_result_type(true, child_pid);
}

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

//-------------------------------------------------------------------------
daemonize_result_type become_daemon(void)
{
  daemonize_result_type res(false, 0);
	// is already a daemon
	if (!is_daemon())
	{
		try
		{
			UNICOMM_DEBUG_OUT("[http]: Trying to become a daemon...")
			res = daemonize();
		}
		catch (const std::exception& e)
		{
      cout << "[http]: An std::exception occurred [" << e.what() << "]";

			UNICOMM_DEBUG_OUT("[http]: An error [" << e.what() << "]")
		}
		catch (...)
		{
      cout << "[http]: An UNKNOWN exception occurred; pid = " << res.second;

			UNICOMM_DEBUG_OUT("[http]: Unknown error at startup; pid = " << res.second)
		}
	} else
	{
    cout << "[http]: Already a daemon";

		UNICOMM_DEBUG_OUT("[http]: Already a daemon")
	}

	return res;
}

#endif // defined (UNI_UNIX)


} // unnamed namespace
