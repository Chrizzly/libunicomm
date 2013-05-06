///////////////////////////////////////////////////////////////////////////////
// helper_detail.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Unified Communication protocol different helper entities.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_HELPER_DETAIL_HPP_
#define UNI_HELPER_DETAIL_HPP_

/** @file helper_detail.hpp Helper routines and entities. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/basic.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** @namespace detail Unicomm library implementation details. */
namespace detail
{

/** Checks path existence.
 *
 *  @throw Throws std::runtime_error if path doesn't exist.
 */
void check_path(const std::string &s);

/** @brief Adds slash to given string tail. Returns reference to a string 
 *    that is passed as parameter. 
 */
std::string& normalize_path(std::string &s);

/** Adds slash to a copy of given string and returns created copy. */
std::string normalize_path(const std::string &s);

// removed due to can't bind and take address of overridden functions (ambiguous)
/** Scans given string for '\x' tokens and converts them to represented char. */
//std::string& process_hex_str(std::string& s);

/** Scans given string for '\x' tokens and converts them to represented char. */
std::string process_hex_str(const std::string& s);

/** Calls handler. */
template <typename CallbackT, typename ArgT>
void execute_signal(const CallbackT& handler, ArgT& arg)
{
  if (!handler.empty())
  {
    handler(arg);
  }
}

/** Calls handler. */
template <typename CallbackT, typename ArgT>
void execute_signal(const CallbackT& handler, const ArgT& arg)
{
  if (!handler.empty())
  {
    handler(arg);
  }
}

/** Calls handler. */
template <typename CallbackT, typename ArgT>
void execute_signal(const CallbackT* handler, ArgT& arg)
{
  if (handler && !handler->empty())
  {
    (*handler)(arg);
  }
}

/** Calls handler. */
template <typename CallbackT, typename Arg1T, typename Arg2T>
void execute_signal(const CallbackT& handler, Arg1T& arg1, Arg2T& arg2)
{
  if (!handler.empty())
  {
    handler(arg1, arg2);
  }
}

/** Calls handler. */
template <typename CallbackT, typename Arg1T, typename Arg2T>
void execute_signal(const CallbackT* handler, Arg1T& arg1, Arg2T& arg2)
{
  if (handler && !handler->empty())
  {
    (*handler)(arg1, arg2);
  }
}

/** Calls handler. */
template <typename HandlerT>
void call_handler(const HandlerT& handler)
{
  if (handler)
  {
    handler();
  }
}

/** Calls handler. */
template <typename HandlerT, typename ArgT>
void call_handler(const HandlerT& handler, ArgT& arg)
{
  if (handler)
  {
    handler(arg);
  }
}

/** Calls handler. */
template <typename HandlerT, typename ArgT>
void call_handler(const HandlerT& handler, const ArgT& arg)
{
  if (handler)
  {
    handler(arg);
  }
}

/** Calls handler. */
template <typename HandlerT, typename Arg1T, typename Arg2T>
void call_handler(const HandlerT& handler, Arg1T& arg1, Arg2T& arg2)
{
  if (handler)
  {
    handler(arg1, arg2);
  }
}

} // namespace detail

} // namespace unicomm

#endif // UNI_HELPER_DETAIL_HPP_
