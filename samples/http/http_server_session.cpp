///////////////////////////////////////////////////////////////////////////////
// http_server_session.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Http server & client example based on unicomm engine.
// Server session object class.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.
 
#include "http_server_session.hpp"
#include "http_aux.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_basic.hpp"
#include "http_except.hpp"

#include <unicomm/comm.hpp>

#include <smart/debug_out.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4127) // warning C4127: conditional expression is constant
#endif // UNI_VISUAL_CPP

#include <boost/date_time/posix_time/time_formatters.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>

#include <ctime>

using std::stringstream;
using std::ifstream;
using std::string;
using std::setprecision;
using std::setw;
using std::for_each;

using boost::filesystem::exists;
using boost::filesystem::path;
using boost::filesystem::file_size;
using boost::filesystem::is_directory;
using boost::filesystem::directory_iterator;
using boost::filesystem::extension;
using boost::filesystem::last_write_time;
using boost::lexical_cast;
using boost::posix_time::ptime;
using boost::posix_time::from_time_t;
using boost::posix_time::to_simple_string;
using boost::uintmax_t;

//////////////////////////////////////////////////////////////////////////
// aux
namespace 
{

//////////////////////////////////////////////////////////////////////////
// http logic

uni_http::response::pointer_type create_reply(const string& reply_code, 
                                              const string& data = "", 
                                              bool add_content_length = true)
{
  uni_http::response::pointer_type reply(new uni_http::response(reply_code, data));

  reply->add_header(uni_http::header::server(), "Unicomm based HTTP 1.0 server");

  const time_t now = time(0);
#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4996)  // warning C4996: 'asctime': This function or variable may be unsafe. Consider using asctime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif // UNI_VISUAL_CPP

  string s = asctime(gmtime(&now));
  BOOST_ASSERT(!s.empty() && " - Date time string could not be empty");

  // remove asctime new line character if there is
  if (s.at(s.size() - 1) < ' ') { s.resize(s.size() - 1); }

  reply->add_header(uni_http::header::date(), s);

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

  if (!data.empty() && add_content_length)
  {
#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) // warning C4267: 'argument' : conversion from 'size_t' to 'unsigned int', possible loss of data
# pragma warning (disable : 4244) // warning C4244: '+=' : conversion from 'int' to 'unsigned short', possible loss of data
#endif // UNI_VISUAL_CPP

    reply->add_header(uni_http::header::content_length(), 
      lexical_cast<string>(data.size()));

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP
  }

  return reply;
}

//-----------------------------------------------------------------------------
inline string stub_html(const string& text)
{
  return 
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
    "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
    "<head/><body><h1>" + text + "</h1></body></html>";
}

//-----------------------------------------------------------------------------
inline uni_http::response::pointer_type create_service_reply(const std::string& reply_code)
{
  uni_http::response::pointer_type r = create_reply(reply_code, stub_html(reply_code));

  r->add_header(uni_http::header::content_type(), uni_http::mime_type::text_html());

  return r;
}

//------------------------------------------------------------------------
inline uni_http::response::pointer_type create_bad_request_reply(void)
{
  return create_service_reply(uni_http::status_code::bad_request());
}

//------------------------------------------------------------------------
inline uni_http::response::pointer_type create_length_required_reply(void)
{
  return create_service_reply(uni_http::status_code::length_required());
}

//------------------------------------------------------------------------
inline uni_http::response::pointer_type create_internal_server_error_reply(void)
{
  return create_service_reply(uni_http::status_code::internal_server_error());
}

//------------------------------------------------------------------------
inline uni_http::response::pointer_type create_not_implemented_reply(void)
{
  return create_service_reply(uni_http::status_code::not_implemented());
}

//------------------------------------------------------------------------
inline uni_http::response::pointer_type create_not_found_reply(void)
{
  return create_service_reply(uni_http::status_code::not_found());
}

//------------------------------------------------------------------------
inline uni_http::response::pointer_type create_unprocessable_entity_reply(void)
{
  return create_service_reply(uni_http::status_code::unprocessable_entity());
}

//////////////////////////////////////////////////////////////////////////
// directory content listing

string dir_entry_type(const path& entry)
{
  return is_directory(entry)? "DIR": 
    uni_http::mime_type::ext_to_mime(extension(entry), "");
}

//------------------------------------------------------------------------
stringstream& out_file_size(stringstream& ss, const path& entry)
{
  uintmax_t fsz_bytes = file_size(entry);
  const uintmax_t kb  = 1024;
  const uintmax_t mb  = kb * kb; 
  const uintmax_t gb  = mb * kb;
  const uintmax_t tb  = gb * kb;
  double fsz          = double(fsz_bytes);
  string suffix       = "bytes";

  if (fsz_bytes > tb)
  {
    fsz    = fsz_bytes / double(tb);
    suffix = "Tb";
  }
  else if (fsz_bytes > gb)
  {
    fsz    = fsz_bytes / double(gb);
    suffix = "Gb";
  }
  else if (fsz_bytes > mb)
  {
    fsz    = fsz_bytes / double(mb);
    suffix = "Mb";
  }
  else if (fsz_bytes > kb)
  {
    fsz    = fsz_bytes / double(kb);
    suffix = "Kb";
  } 

  ss << setprecision(3) << fsz << suffix;

  return ss;
}

//------------------------------------------------------------------------
inline string boost_path_workaround(const path& p) { return p.string(); }

//------------------------------------------------------------------------
inline const string& boost_path_workaround(const string& s) { return s; }

//------------------------------------------------------------------------
stringstream& dir_listing_line(stringstream& ss, 
                               const directory_iterator::value_type& item, 
                               const path& root_dir_path, 
                               const path& /*sub_dir_path*/)
{
  const path& entry      = item.path();
  const bool is_dir      = is_directory(entry);
  const string file_name = boost_path_workaround(entry.filename()) + (is_dir? "/": "");
  const string href      = entry.string().substr(root_dir_path.string().size());

  ss << "<tr><td><a href=\"" << href << "\">" << file_name << "</a></td><td>" 
     << to_simple_string(ptime(from_time_t(last_write_time(entry)))) << "</td><td class=\"file_size\">";

  if (is_dir) { ss << "-"; } else { out_file_size(ss, entry); }

  ss << "</td><td>" << dir_entry_type(entry) << "</td></tr>";

  return ss;
}

//------------------------------------------------------------------------
stringstream& dir_listing_table(stringstream& ss, const path& dir_path, 
                                const path& root_dir_path, 
                                const path& sub_dir_path)
{
  // name, last modified, size, type
  ss << "<style>td { padding: 1px 7px; }" 
        ".file_size { text-align: right; }" 
        "table { font: 10pt Tahoma; }</style>"
        "<table><tr><th>Name</th><th>Last modified</th><th>Size</th><th>Type</th></tr>"
        "<tr><th colspan=\"4\"><hr/></th></tr>";

  // parent directory link
  ss << "<tr><td><a href=\"" << sub_dir_path.parent_path().string() 
    << "\">.. Up</a></td><td>-</td><td class=\"file_size\">-</td><td>DIR</td></tr>";

  for_each(directory_iterator(dir_path), directory_iterator(), 
    boost::bind(&dir_listing_line, boost::ref(ss), _1, 
    boost::ref(root_dir_path), boost::ref(sub_dir_path)));

  ss << "<tr><th colspan=\"4\"><hr/></th></tr></table><i>Unicomm based HTTP 1.0 server</i>";

  return ss;
}

//------------------------------------------------------------------------
string dir_listing_html(const path& dir_path, const string& root_dir, 
                        const string& sub_dir)
{
  BOOST_ASSERT(is_directory(dir_path) && " - Should be a directory");

  if (!exists(dir_path)) 
  {
    throw uni_http::not_found_error("Requested resource not found [" + 
      dir_path.string() + "]");
  }

  stringstream ss;
  ss << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
     << "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
     << "<html xmlns=\"http://www.w3.org/1999/xhtml\"><body><h1>Index of '" 
     << sub_dir << "'</h1>";

  dir_listing_table(ss, dir_path, path(root_dir), path(sub_dir)) << "</body></html>";

  return ss.str();

}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// SERVER session
//
// assigned handlers

void uni_http::server_session::message_sent_handler_1(
  const unicomm::message_sent_params& params)
{
  params.comm().disconnect();
}

//////////////////////////////////////////////////////////////////////////
// virtual functions

void uni_http::server_session::message_arrived_handler(
  unicomm::message_arrived_params& params)
{
  is_track_timeout(false);

  const request& inm = static_cast<const request&>(params.in_message());
  const response::pointer_type outm = process_request(inm);
  params.out_message(outm);

  stringstream ss;

  ss << name() << ": [" << params.comm().id() << "]: << " 
    << inm.name() << " " << inm.get_header(uni_http::header::host())
    << inm.uri() << " >> " << outm->status_code();

  out_str(ss.str());
}

//------------------------------------------------------------------------
void uni_http::server_session::error_handler(const unicomm::error_params& params)
{
  try
  {
    throw;
  } 
  catch (const bad_request_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: 400 Bad request error [" 
      << e.what() << "]")

    params.comm().send(*create_bad_request_reply());
  }
  catch (const length_required_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: 411 Length required error [" 
      << e.what() << "]")

    params.comm().send(*create_length_required_reply());
  }
  catch (std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: A std::exception [" 
      << e.what() << "]")

    //BOOST_ASSERT(!"An UNEXPECTED std::exception");

    params.comm().send(*create_internal_server_error_reply());
  }
  catch (...)
  {
    BOOST_ASSERT(!"An UNKNOWN UNEXPECTED exception");

    params.comm().send(*create_internal_server_error_reply());
  }
}

//------------------------------------------------------------------------
void uni_http::server_session::after_processed_handler(
  const unicomm::after_processed_params& params)
{
  if (is_track_timeout() && is_timeout_elapsed())
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: Client idle timeout occured")

    is_track_timeout(false);
    params.comm().disconnect();
  }
}

//------------------------------------------------------------------------
uni_http::response::pointer_type 
uni_http::server_session::handle_get(const uni_http::request& inm)
{
  const check_resource_result r = check_resource(inm);

  BOOST_ASSERT(r.requested_host() != _served_hosts.end() && " - Invalid host");

  return r.is_directory()? handle_get_dir(r, inm.uri()): handle_get_file(r);
}

//------------------------------------------------------------------------
uni_http::response::pointer_type 
uni_http::server_session::handle_get_file(const server_session::check_resource_result& r)
{
  const path& file_path   = r.requested_target();
  const string file_name  = r.requested_target().string();

  ifstream file(file_name.c_str(), ifstream::binary);
  if (!file)
  {
    throw internal_server_error("Can't handle GET request, file [" + file_name + "]");
  }

  buffer_type buf(static_cast<buffer_type::size_type>(file_size(file_path)));
  file.read(&*buf.begin(), static_cast<std::streamsize>(buf.size()));

  response::pointer_type rr = create_reply(status_code::ok(), 
    string(buf.begin(), buf.end()));
  const string file_ext  = boost_path_workaround(file_path.extension());

  rr->add_header(header::content_type(), mime_type::ext_to_mime(file_ext));

  return rr;
}

//------------------------------------------------------------------------
uni_http::response::pointer_type 
uni_http::server_session::handle_get_dir(const server_session::check_resource_result& r, 
                                         const string& uri)
{
  const string buf           = dir_listing_html(r.requested_target(), 
    r.requested_host()->second.root_directory(), uri);
  response::pointer_type rr  = create_reply(status_code::ok(), 
    string(buf.begin(), buf.end()));

  rr->add_header(header::content_type(), mime_type::text_html());

  return rr;
}

//------------------------------------------------------------------------
uni_http::response::pointer_type 
uni_http::server_session::handle_head(const uni_http::request& inm)
{
  check_resource(inm);

  return create_reply(status_code::ok());
}

//------------------------------------------------------------------------
uni_http::response::pointer_type uni_http::server_session::handle_default(void)
{
  return create_not_implemented_reply();
}

////------------------------------------------------------------------------
//uni_http::message_pointer_type 
//uni_http::server_session::handle_post(const uni_http::request& inm)
//{
//  return message_pointer_type();
//}

//------------------------------------------------------------------------
uni_http::response::pointer_type 
uni_http::server_session::process_request(const uni_http::request& inm)
{
  response::pointer_type reply;

  try
  {
    if (is_get(inm))
    {
      reply = handle_get(inm);
    } else if (is_head(inm))
    {
      reply = handle_head(inm);
    } else
    {
      reply = handle_default();
    }
  } 
  catch (const unprocessable_entity_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: 422 Unprocessible entity error [" 
      << e.what() << "]")

    reply = create_unprocessable_entity_reply();
  }
  catch (const not_found_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: 404 Not found error [" 
      << e.what() << "]")

    reply = create_not_found_reply();
  }
  catch (const internal_server_error& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: 500 Internal server error [" 
      << e.what() << "]")

    reply = create_internal_server_error_reply();
  }
  catch (const std::exception& UNICOMM_IFDEF_DEBUG(e))
  {
    UNICOMM_DEBUG_OUT("[uni_http::server_session]: An UNEXPECTED std::exception [" 
      << e.what() << "]")

    BOOST_ASSERT(!"An UNEXPECTED std::exception");

    reply = create_internal_server_error_reply();
  }
  catch (...)
  {
    BOOST_ASSERT(!"An UNKNOWN exception!");

    reply = create_internal_server_error_reply();
  }

  BOOST_ASSERT(reply != 0 && " - Reply could not be null");

  return reply;
}

//------------------------------------------------------------------------
uni_http::server_session::check_resource_result 
uni_http::server_session::check_resource(const uni_http::request& inm)
{
  const string& wanted_host = inm.get_header(uni_http::header::host());

  hosts_collection_type::const_iterator cit = _served_hosts.find(wanted_host);
  if (cit == _served_hosts.end())
  {
    throw unprocessable_entity_error("Requested host not found [" + 
      wanted_host + "]");
  }

  const string target_name = cit->second.root_directory() + inm.uri();
  path target_path(target_name);

  if (!exists(target_path))
  {
    throw not_found_error("Requested resource not found [" + target_name + "]");
  }

  bool is_dir = is_directory(target_path);
  if (is_dir)
  {
    path tmp = target_path;

    if (exists(tmp /= string("index.html")))
    {
      target_path = tmp;
      is_dir      = false;
    }
  }

  UNICOMM_DEBUG_OUT("[uni_http::server_session]: Resource requested [" 
    << target_path.string() << "]")

  return check_resource_result(cit, target_path, is_dir);
}
