///////////////////////////////////////////////////////////////////////////////
// umessage_content_handler_base.cpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2009, (c) Dmitry Timoshenko.

#include "detail/complex_content_handler_base.hpp"

#include <smart/data/complex_except.hpp>
#include <smart/debug_out.hpp>

#include <xercesc/sax2/Attributes.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>

#include <stack>
#include <stdexcept>

#if defined (SMART_DEBUG) && !defined (SMART_COMPLEX_PARSER_VERBOSE) && defined (SMART_DEBUG_OUT)
# undef SMART_DEBUG_OUT
# define SMART_DEBUG_OUT(expr)
#endif // !defined NDEBUG && !defined UNI_PARSER_VERBOSE && defined SMART_DEBUG_OUT
//
using std::stack;
using std::runtime_error;
using std::string;
using std::make_pair;

using boost::shared_ptr;

using smart::data::detail::std_to_xml_string;
using smart::data::detail::xml_to_std_string;
using smart::data::detail::xml_string_type;

namespace smart
{

namespace data
{

namespace detail
{

//////////////////////////////////////////////////////////////////////////
// Auxiliary types
//
// Property Descriptor
//
// fixme: It's necessary to put resources (see mch_impl class) into PropertyDescriptors
// Resources should be available through pointers. If pointer is null, it means
// there is no requested resource in the descriptor

//////////////////////////////////////////////////////////////////////////
// Content Handler Implementation

struct mchb_impl
{
  mchb_impl(void): _chunks(false) { /*empty*/ }

  //-----------------------------------------------------------------------------
  const string& top_element(void) const
  {
    return _elements.top();
  }

  //-----------------------------------------------------------------------------
  void top_element(const string& element)
  {
    _elements.push(element);
  }

  //-----------------------------------------------------------------------------
  void pop_element(void)
  {
    _elements.pop();
  }

  //////////////////////////////////////////////////////////////////////////
  // data
  stack<string> _elements;
  //size_t _chunks;
  bool _chunks;
};

} // namespace detail

} // namespace data

} // namespace smart

using smart::data::detail::mchb_impl;

//////////////////////////////////////////////////////////////////////////
// message_content_handler class

smart::data::message_content_handler_base::message_content_handler_base(void):
  _impl(new mchb_impl)
{
  // empty
}

//-----------------------------------------------------------------------------
smart::data::message_content_handler_base::~message_content_handler_base(void)
{
  delete _impl;
}

//-----------------------------------------------------------------------------
const string& smart::data::message_content_handler_base::top_element(void) const
{
  return _impl->_elements.top();
}

//-----------------------------------------------------------------------------
smart::data::message_content_handler_base::tag_attributes_collection_type smart::data::message_content_handler_base::get_attribs(const xercesc::Attributes& attrs) const
{
  tag_attributes_collection_type attrs_res;

  for (unsigned int/*size_t*/ i = 0; i < attrs.getLength(); ++i)
  {
    const string attr_name           = xml_to_std_string(xml_string_type(attrs.getQName(i)));
    const xml_string_type attr_value = xml_string_type(attrs.getValue(i));
    attrs_res.insert(make_pair(attr_name, attr_value));
  }

  return attrs_res;
}


//-----------------------------------------------------------------------------
//void unicomm::message_content_handler::endDocument(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void unicomm::message_content_handler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
//{
//}
//
////-----------------------------------------------------------------------------
//void unicomm::message_content_handler::processingInstruction(const XMLCh* const target, const XMLCh* const data)
//{
//}
//
////-----------------------------------------------------------------------------
//void unicomm::message_content_handler::setDocumentLocator(const xercesc::Locator* const locator)
//{
//}
//
////-----------------------------------------------------------------------------
//void unicomm::message_content_handler::startDocument(void)
//{
//}


//-----------------------------------------------------------------------------
void smart::data::message_content_handler_base::error(const XERCES_CPP_NAMESPACE::SAXParseException& exc)
{
  throw parser_error("Unicomm xml parser error: " + xml_to_std_string(xml_string_type(exc.getMessage())));
}

//-----------------------------------------------------------------------------
void smart::data::message_content_handler_base::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exc)
{
  throw parser_error("Unicomm xml parser error: " + xml_to_std_string(xml_string_type(exc.getMessage())));
}

//-----------------------------------------------------------------------------
void smart::data::message_content_handler_base::warning(const XERCES_CPP_NAMESPACE::SAXParseException& /*exc*/)
{
  //throw runtime_error(xml_to_std_string(xml_string_type(exc.getMessage())));
}

//-----------------------------------------------------------------------------
void smart::data::message_content_handler_base::startElement(const XMLCh* const /*uri*/, const XMLCh* const /*localname*/, const XMLCh* const qname, const xercesc::Attributes& attrs)
{
  // fixme: check tags invariant until xml-schema is not thought out

  const xml_string_type tmp = qname;
  const string tag          = xml_to_std_string(tmp);
  const tag_attributes_collection_type attribs = get_attribs(attrs);

  _impl->top_element(tag);
  _impl->_chunks = false;

  if (is_message_tag(tag))
  {
    // <message>
    SMART_DEBUG_OUT("[xml parser]: <message> started");

    on_message_start(tag, attribs);
  }
  else if (is_property_tag(tag))
  {
    // <property>
    SMART_DEBUG_OUT("[xml parser]: <property> started");

    on_property_start(tag, attribs);
  }
  else if (is_array_tag(tag))
  {
    // array <items>
    SMART_DEBUG_OUT("[xml parser]: <items> array started");

    on_array_start(tag, attribs);
  }
  else if (is_array_item_tag(tag))
  {
    // array <item>
    SMART_DEBUG_OUT("[xml parser]: <item> started");

    on_array_item_start(tag, attribs);
  }
}

//-----------------------------------------------------------------------------
void smart::data::message_content_handler_base::characters(const XMLCh* const chars, const unsigned int length)
{
  const string& tag       = top_element();
  const xml_string_type s = xml_string_type(chars, length);

  if (is_message_tag(tag))
  {
    // message
    SMART_DEBUG_OUT("[xml parser]: message characters");

    on_message_characters(s, _impl->_chunks);
  }
  else if (is_property_tag(tag))
  {
    // property
    SMART_DEBUG_OUT("[xml parser]: property characters");

    on_property_characters(s, _impl->_chunks);
  }
  else if (is_array_tag(tag))
  {
    // array
    SMART_DEBUG_OUT("[xml parser]: items characters");

    on_array_characters(s, _impl->_chunks);
  }
  else if (is_array_item_tag(tag))
  {
    // array item
    SMART_DEBUG_OUT("[xml parser]: item characters");

    on_array_item_characters(s, _impl->_chunks);
  }

  _impl->_chunks = true;
}

//-----------------------------------------------------------------------------
void smart::data::message_content_handler_base::endElement(const XMLCh* const /*uri*/, const XMLCh* const /*localname*/, const XMLCh* const qname)
{
  _impl->_chunks = false;

  const string& tag = top_element();

  BOOST_ASSERT(xml_to_std_string(xml_string_type(qname)) == tag && "Start and End tags should be identical");

  if (is_message_tag(tag))
  {
    // <message>
    SMART_DEBUG_OUT("[xml parser]: </message> finished");

    on_message_end(tag);
  }
  else if (/*uni_message::property_tag() == topelement()*/is_property_tag(tag))
  {
    // <property>
    SMART_DEBUG_OUT("[xml parser]: </property> finished");

    on_property_end(tag);
  }
  else if (/*uni_message::array_tag() == topelement()*/is_array_tag(tag))
  {
    // array
    SMART_DEBUG_OUT("[xml parser]: </items> array finished");

    on_array_end(tag);
  }
  else if (/*uni_message::array_item_tag() == topelement()*/is_array_item_tag(tag))
  {
    // array item
    SMART_DEBUG_OUT("[xml parser]: </item> finished");

    on_array_item_end(tag);
  }

  _impl->pop_element();
}

////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::MessageStarted(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::PropertyStarted(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::ItemStarted(void)
//{
//
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::MessageValue(void)
//{
//
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::PropertyValue(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::ItemValue(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::MessageFinished(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::PropertyFinished(void)
//{
//}
//
////-----------------------------------------------------------------------------
//void smart::data::message_content_handler::ItemFinished(void)
//{
//}
