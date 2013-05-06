///////////////////////////////////////////////////////////////////////////////
// code_page.hpp
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

#ifndef SMART_CODE_PAGE_HPP_
#define SMART_CODE_PAGE_HPP_

#define SMART_SHORT_LIB_NAME "code_page"
#include <smart/config/auto_link.hpp>
#undef SMART_SHORT_LIB_NAME

#include <string>

namespace smart
{

namespace code_page
{

std::string ansi_to_oem(const char* st);

} // namespace code_page

} // namespace smart

#endif // SMART_CODE_PAGE_HPP_
