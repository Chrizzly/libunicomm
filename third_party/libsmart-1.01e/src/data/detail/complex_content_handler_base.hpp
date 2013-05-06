///////////////////////////////////////////////////////////////////////////////
// umessage_content_handler_base.hpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2010, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_COMPLEX_CONTENT_HANDLER_BASE_HPP_
#define SMART_COMPLEX_CONTENT_HANDLER_BASE_HPP_

#include "xercesc_string.hpp"

#include <xercesc/sax2/DefaultHandler.hpp>

//#include <boost/function.hpp>

#include <string>
#include <map>

/** @namespace smart Library root namespace. */
namespace smart
{

/** @namespace data smart_data library namespace. */
namespace data
{

// forward
namespace detail { struct mchb_impl; }

//struct property_descriptor;

//////////////////////////////////////////////////////////////////////////
// message_content_handler class
class message_content_handler_base : public xercesc::DefaultHandler
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  explicit message_content_handler_base(void);
  ~message_content_handler_base(void);

//////////////////////////////////////////////////////////////////////////
// protected interface
protected:
  //typedef xercesc::Attributes tag_attributes_type;
  typedef std::map<std::string, detail::xml_string_type> tag_attributes_collection_type;

protected:
  const std::string& top_element(void) const;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  //typedef boost::function<void (void)> handler_type;

private:
  virtual bool is_message_tag(const std::string& /*tag*/) const { return false; }
  virtual bool is_property_tag(const std::string& /*tag*/) const { return false; }
  virtual bool is_array_tag(const std::string& /*tag*/) const { return false; }
  virtual bool is_array_item_tag(const std::string& /*tag*/) const { return false; }

  virtual void on_message_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/) { ; }
  virtual void on_property_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/) { ; }
  virtual void on_array_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/) { ; }
  virtual void on_array_item_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/) { ; }

  virtual void on_message_characters(const detail::xml_string_type& /*chars*/, bool /*continuation*/) { ; }
  virtual void on_property_characters(const detail::xml_string_type& /*chars*/, bool /*continuation*/) { ; }
  virtual void on_array_characters(const detail::xml_string_type& /*chars*/, bool /*continuation*/) { ; }
  virtual void on_array_item_characters(const detail::xml_string_type& /*chars*/, bool /*continuation*/) { ; }

  virtual void on_message_end(const std::string& /*tag*/) { ; }
  virtual void on_property_end(const std::string& /*tag*/) { ; }
  virtual void on_array_end(const std::string& /*tag*/) { ; }
  virtual void on_array_item_end(const std::string& /*tag*/) { ; }

private:
  void characters(const XMLCh* const chars, const unsigned int length);
  //void endDocument(void);
  void endElement(const XMLCh* const /*uri*/, const XMLCh* const /*localname*/, const XMLCh* const /*qname*/);
  //void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
  //void processingInstruction(const XMLCh* const target, const XMLCh* const data);
  //void setDocumentLocator(const xercesc::Locator* const locator);
  //void startDocument(void);
  void startElement(const XMLCh* const /*uri*/, const XMLCh* const /*localname*/, const XMLCh* const qname, const xercesc::Attributes& attrs);

  //void startPrefixMapping(const	XMLCh* const prefix, const XMLCh* const	uri) = 0;
  //void endPrefixMapping(const	XMLCh* const prefix) = 0;
  //void skippedEntity(const XMLCh* const	name) = 0;
  //InputSource* resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId);
  void error(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
  void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
  void warning(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
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
  tag_attributes_collection_type get_attribs(const xercesc::Attributes& attrs) const;

private:
  smart::data::detail::mchb_impl *_impl;
};

} // namespace data

} // namespace smart

#endif // SMART_COMPLEX_CONTENT_HANDLER_BASE_HPP_
