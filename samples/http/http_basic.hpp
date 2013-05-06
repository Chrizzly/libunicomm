///////////////////////////////////////////////////////////////////////////////
// http_basic.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Declares some http headers, response codes and so on.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2011, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HTTP_BASIC_HPP_
#define UNI_HTTP_BASIC_HPP_

/** @file http_basic.hpp Http basic declarations. */

#include <string>
#include <vector>
#include <map>

/** @namespace uni_http unicomm HTTP example namespace. */
namespace uni_http
{

#ifdef UNICOMM_SSL
/** Default HTTPS port. */
inline unsigned short default_port(void) { return 443; }
#else
/** Default HTTP port. */
inline unsigned short default_port(void) { return 80; }
#endif // UNICOMM_SSL

/** Http version. */
inline const std::string& 
version(void) { static const std::string s = "HTTP/1.0"; return s; }

/** Header/body separator. */
inline const std::string& 
head_body_separator(void) { static const std::string s = "\r\n\r\n"; return s; }

/** Http headers namespace. */
namespace header
{

/** Header set type. */
typedef std::multimap<std::string, std::string> header_collection_type;

//////////////////////////////////////////////////////////////////////////
// name: value separator
inline const std::string& name_value_separator(void) { static const std::string s = ":"; return s; }
inline const std::string& separator(void) { static const std::string s = "\r\n"; return s; }

//////////////////////////////////////////////////////////////////////////
// common headers
inline const std::string& connection(void) { static const std::string s = "Connection"; return s; }
inline const std::string& date(void) { static const std::string s = "Date"; return s; }

//////////////////////////////////////////////////////////////////////////
// request headers
inline const std::string& accept(void) { static const std::string s = "Accept"; return s; }
inline const std::string& host(void) { static const std::string s = "Host"; return s; }
inline const std::string& user_agent(void) { static const std::string s = "User-Agent"; return s; }
inline const std::string& accept_language(void) { static const std::string s = "Accept-Language"; return s; }
inline const std::string& accept_encoding(void) { static const std::string s = "Accept-Encoding"; return s; }

//////////////////////////////////////////////////////////////////////////
// reply headers
inline const std::string& server(void) { static const std::string s = "Server"; return s; }

//////////////////////////////////////////////////////////////////////////
// entity headers
inline const std::string& content_length(void) { static const std::string s = "Content-Length"; return s; }
inline const std::string& content_type(void) { static const std::string s = "Content-Type"; return s; }
inline const std::string& content_language(void) { static const std::string s = "Content-Language"; return s; }
inline const std::string& last_modified(void) { static const std::string s = "Last-Modified"; return s; }

} // namespace header

/** Reply codes namespace. */
namespace status_code
{

//////////////////////////////////////////////////////////////////////////
// success 
inline const std::string& ok(void) { static const std::string s = "200 OK"; return s; }
inline const std::string& created(void) { static const std::string s = "201 Created"; return s; }
inline const std::string& accepted(void) { static const std::string s = "202 Accepted"; return s; }

//////////////////////////////////////////////////////////////////////////
// client error
inline const std::string& bad_request(void) { static const std::string s = "400 Bad Request"; return s; }
inline const std::string& unauthorized(void) { static const std::string s = "401 Unauthorized"; return s; }
inline const std::string& forbidden(void) { static const std::string s = "403 Forbidden"; return s; }
inline const std::string& not_found(void) { static const std::string s = "404 Not Found"; return s; }
inline const std::string& method_not_allowed(void) { static const std::string s = "405 Method Not Allowed"; return s; }
inline const std::string& length_required(void) { static const std::string s = "411 Length Required"; return s; }
inline const std::string& unprocessable_entity(void) { static const std::string s = "422 Unprocessable Entity"; return s; }

//////////////////////////////////////////////////////////////////////////
// server error
inline const std::string& internal_server_error(void) { static const std::string s = "500 Internal Server Error"; return s; }
inline const std::string& not_implemented(void) { static const std::string s = "501 Not Implemented"; return s; }

} // namespace status_code

/** Mime types namespace. */
namespace mime_type
{

//////////////////////////////////////////////////////////////////////////
// application types
inline const std::string& application_zip(void) { static const std::string s = "application/zip"; return s; }
inline const std::string& application_octet_stream(void) { static const std::string s = "application/octet-stream"; return s; }

//////////////////////////////////////////////////////////////////////////
// image types
inline const std::string& image_gif(void) { static const std::string s = "image/gif"; return s; }
inline const std::string& image_jpeg(void) { static const std::string s = "image/jpeg"; return s; }
inline const std::string& image_png(void) { static const std::string s = "image/png"; return s; }

//////////////////////////////////////////////////////////////////////////
// text types
inline const std::string& text_html(void) { static const std::string s = "text/html"; return s; }
inline const std::string& text_plain(void) { static const std::string s = "text/plain"; return s; }
inline const std::string& text_css(void) { static const std::string s = "text/css"; return s; }
inline const std::string& text_xml(void) { static const std::string s = "text/xml"; return s; }

/** Converts given file extension to the mime type. */
const std::string& ext_to_mime(const std::string& ext, const std::string& default_mime = application_octet_stream());

} // namespace mime_type

/** Auxiliary buffer type. */
typedef std::vector<char> buffer_type;

} // namespace uni_http

#endif // UNI_HTTP_BASIC_HPP_
