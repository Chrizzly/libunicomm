///////////////////////////////////////////////////////////////////////////////
// config_loader.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Configuration loaders.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_CONFIG_LOADER_HPP_
#define UNI_CONFIG_LOADER_HPP_

/** @file config_loader.hpp Configuration loaders. */

#include <unicomm/config/auto_link.hpp>

#ifdef UNICOMM_USE_COMPLEX_XML

#include <unicomm/config.hpp>

#include <string>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Loads unicomm configuration from smart::data::complex xml format file. 
 *
 *  @param file_name Path to file to be loaded.
 *  @return Instance of unicomm::config.
 *  @throw Derived from std::exception if any error occurs while loading file.
 *    If file doesn't exists or an error encountered while 
 *    reading the file throws std::runtime_error. If any required
 *    settings is not defined by the configuration throws 
 *    smart::data::invalid_property_error derived from std::runtime_error.
 *
 *  @note Usage of this function leads smart::data::complex and 
 *    it's dependencies like Apache Xercesc will be linked to the unicomm.
 */
UNICOMM_DECL config load_from_complex_xml(const std::string& file_name);

/** Sets up xml message format to be used by configuration. 
 *
 *  It creates and sets up necessary decoders and encoders.
 *  
 *  @param conf unicomm::config object to be set up.
 *  @return reference to conf.
 *  @note Usage of this function leads smart::data::complex and 
 *    it's dependencies like Apache Xercesc will be linked to the unicomm.
 */
UNICOMM_DECL config& set_xml_message_format(config& conf);

/** Sets up encoders decoders accordingly message format used by config. 
 *
 *  Tests conf.message_format() and set encoder and decoders accordingly to 
 *  the value. There any string allowed. But 'xml' & 'binary' strings designates 
 *  to create according encoders and decoders. Any other string means custom 
 *  encoder/decoder used.
 *
 *  @note Usage of this function causes smart::data::complex and 
 *    it's dependencies like Apache Xercesc will be linked to the unicomm.
 */
UNICOMM_DECL config& setup_message_format(config& conf); 

} // namespace unicomm

#endif // UNICOMM_USE_COMPLEX_XML

#endif // UNI_CONFIG_LOADER_HPP_
