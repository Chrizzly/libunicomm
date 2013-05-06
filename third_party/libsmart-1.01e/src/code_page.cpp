///////////////////////////////////////////////////////////////////////////////
// code_page.cpp
//
// Copyright (c) 2009 Igor Zimenkov.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

#include <smart/code_page.hpp>

//------------------------------------------------------------------------------
std::string smart::code_page::ansi_to_oem(const char* st)
{
  std::string res;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(st); 
  while (*p)
  {
    res.push_back((*p >= 192) ? ((*p <= 239) ? *p - 64 : *p - 16) : *p);
    p++;
  }
  return res;
}
