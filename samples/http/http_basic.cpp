///////////////////////////////////////////////////////////////////////////////
// http_basic.cpp
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
 
#include "http_basic.hpp"

#include <boost/assign.hpp>

using std::string;
using std::multimap;

using boost::assign::map_list_of;

/** Converts given file extension to the mime type. */
const string& uni_http::mime_type::ext_to_mime(const string& ext, 
                                               const string& default_mime)
{
  typedef multimap<string, string> mime_collection_type;

  static const mime_collection_type mime_types = 
    map_list_of
      (".zip", application_zip())
      (".gif", image_gif())
      (".jpe", image_jpeg())
      (".jpg", image_jpeg())
      (".jpeg", image_jpeg())
      (".png", image_png())
      (".htm", text_html())
      (".html", text_html())
      (".txt", text_plain())
      (".xml", text_xml())
      (".css", text_css())
    ;

  mime_collection_type::const_iterator cit = mime_types.find(ext);

  // by default send reply as binary data
  return cit == mime_types.end()? 
    (default_mime.empty()? ext: default_mime): cit->second;
}
