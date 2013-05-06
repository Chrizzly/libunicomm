///////////////////////////////////////////////////////////////////////////////
// raw_data.hpp
//
// Copyright (c) 2009 Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_RAW_DATA_HPP_
#define SMART_RAW_DATA_HPP_

#define SMART_SHORT_LIB_NAME "raw_data"
#include <smart/config/auto_link.hpp>
#undef SMART_SHORT_LIB_NAME

#include <vector>
#include <istream>
#include <boost/shared_ptr.hpp>

namespace smart
{

typedef std::vector<char> raw_data;
typedef boost::shared_ptr<raw_data> raw_data_ptr;
 
/**
 *  Creates raw_data from stream.
 *
 *  @param is Reference to the input stream.
 *  @param sz Size of the input stream (optional - for increasing performance).
 *  @return Returns a smart pointer to the desired raw_data.
 */
raw_data_ptr from_stream(std::istream& is, std::streamsize sz = 0);

/**
 *  Convert to the hexadecimal character string.
 *
 *  Converts the source string to the hexadecimal character string object.
 *
 *  @param src Reference to the source string object.
 *  @return Returns the resulting hexadecimal character string.
 */
std::string to_hex_string(const std::string& src);

/**
 *  Convert from the hexadecimal character string.
 *
 *  Converts the source hexadecimal character string to the string object.
 *
 *  @param src Reference to the source hexadecimal character string object.
 *  @return Returns the resulting string.
 */
std::string from_hex_string(const std::string& src);

/**
 *  Assign raw_data content.
 *
 *  Assigns new content to the raw_data object, dropping all the elements
 *  contained in the raw_data before the call and replacing them by those
 *  specified in the string argument.
 *
 *  @param dest Reference to the raw_data object.
 *  @param src The new content in hexadecimal character string form.
 */
void assign_hex_string(raw_data& dest, const std::string& src);

/**
 *  Assign string content.
 *
 *  Assigns new content to the string object, dropping all the elements
 *  contained in the string before the call and replacing them by those
 *  specified in the second string argument.
 *
 *  @param dest Reference to the target string object.
 *  @param src The new content in hexadecimal character string form.
 */
void assign_hex_string(std::string& dest, const std::string& src);

/**
 *  Writes data to the raw_data.
 *
 *  @param buf Pointer to bytes to be written.
 *  @param count Number of bytes to be written.
 *  @param data Reference to the raw_data.
 *  @param pos Specifies the position in the raw_data for the write operation
 *  (optional - the default value indicates the end of the raw_data).
 */
void write(const void* buf, size_t count, raw_data& data, raw_data::size_type pos = -1);

/**
 *  Writes data to the raw_data.
 *
 *  @param buf Pointer to bytes to be written.
 *  @param count Number of bytes to be written.
 *  @param data Reference to the raw_data_ptr.
 *  @param pos Specifies the position in the raw_data for the write operation
 *  (optional - the default value indicates the end of the raw_data).
 */
void write(const void* buf, size_t count, raw_data_ptr& data, raw_data::size_type pos = -1);

} // namespace smart

#endif // SMART_RAW_DATA_HPP_
