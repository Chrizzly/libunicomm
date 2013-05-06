///////////////////////////////////////////////////////////////////////////////
// umessage_old_content_handler.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2009, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_OLD_CONTENT_HANDLER_HPP_
#define SMART_COMPLEX_OLD_CONTENT_HANDLER_HPP_

#include <smart/data/complex.hpp>

#include "complex_content_handler_base.hpp"

namespace unicomm
{

namespace impl
{

// forward
struct moch_impl;
struct property_descriptor;

//////////////////////////////////////////////////////////////////////////
// message_content_handler class
class message_old_content_handler : public smart::data::message_content_handler_base
{
public:
  typedef message_content_handler_base::tag_attributes_collection_type tag_attributes_collection_type;

public:
  explicit message_old_content_handler(smart::data::complex *c);
  ~message_old_content_handler(void);

public:
  smart::data::complex* message(void) const;
  void message(smart::data::complex *m);

public:
  //void characters(const XMLCh* const chars, const unsigned int length);
  //void endDocument(void);
  //void endElement(const XMLCh* const /*uri*/, const XMLCh* const /*localname*/, const XMLCh* const /*qname*/);
  //void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
  //void processingInstruction(const XMLCh* const target, const XMLCh* const data);
  //void setDocumentLocator(const xercesc::Locator* const locator);
  //void startDocument(void);
  //void startElement(const XMLCh* const /*uri*/, const XMLCh* const /*localname*/, const XMLCh* const qname, const xercesc::Attributes& attrs);

  //void startPrefixMapping(const	XMLCh* const prefix, const XMLCh* const	uri) = 0;
  //void endPrefixMapping(const	XMLCh* const prefix) = 0;
  //void skippedEntity(const XMLCh* const	name) = 0;
  //InputSource* resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId);
  //void error(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
  //void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
  //void warning(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
  //void resetErrors(void);
  //void notationDecl(const XMLCh* const name, const XMLCh* const publicId, const XMLCh* const systemId);
  //void resetDocType(void);
  //void unparsedEntityDecl(const XMLCh* const name, const XMLCh* const publicId, const XMLCh* const systemId, const XMLCh* const notationName);
  //void comment(const XMLCh* const chars, const unsigned int length);
  //void endCDATA(void);
  //void endDTD(void);
  //void endEntity(const XMLCh* const name);
  //void startCDATA(void);
  //void startDTD(const XMLCh* const name, const XMLCh* const publicId, const XMLCh* const systemId);
  //void startEntity (const XMLCh* const name);
  //void elementDecl(const XMLCh* const name, const XMLCh* const model);
  //void attributeDecl(const XMLCh* const eName, const XMLCh* const aName, const XMLCh* const type, const XMLCh* const mode, const XMLCh* const value);
  //void internalEntityDecl(const XMLCh* const name, const XMLCh* const value);
  //void externalEntityDecl(const XMLCh* const name, const XMLCh* const publicId, const XMLCh* const systemId);

private:
  bool is_message_tag(const std::string& tag) const;
  bool is_property_tag(const std::string& tag) const;
  bool is_array_tag(const std::string& tag) const;
  bool is_array_item_tag(const std::string& tag) const;

  void on_message_start(const std::string& tag, const tag_attributes_collection_type& attrs);
  void on_property_start(const std::string& tag, const tag_attributes_collection_type& attrs);
  void on_array_start(const std::string& tag, const tag_attributes_collection_type& attrs);
  void on_array_item_start(const std::string& tag, const tag_attributes_collection_type& attrs);

  //void on_message_characters(const xml_string_type& chars, bool continuation);
  void on_property_characters(const smart::data::detail::xml_string_type& chars, bool continuation);
  //void on_array_characters(const xml_string_type& chars, bool continuation);
  void on_array_item_characters(const smart::data::detail::xml_string_type& chars, bool continuation);

  void on_message_end(const std::string& tag);
  void on_property_end(const std::string& tag);
  void on_array_end(const std::string& tag);
  void on_array_item_end(const std::string& tag);

//private:
//  const std::string& topelement(void) const;
//  /*const */property_descriptor& topdescriptor(void); // const is victim of convenience
//  /*const */types::complex_type& topcomplex(void);
//
//  void topelement(const std::string& el);
//  void topdescriptor(const property_descriptor& d);
//  void topcomplex(/*const */types::complex_type* c);
//
//  void popelement(void);

private:
  moch_impl *_impl;
};

} // namespace impl

} // namespace unicomm

#endif // SMART_COMPLEX_OLD_CONTENT_HANDLER_HPP_
