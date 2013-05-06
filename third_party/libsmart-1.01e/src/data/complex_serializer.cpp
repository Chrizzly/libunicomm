///////////////////////////////////////////////////////////////////////////////
// umessage_serializer.cpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#include <smart/data/complex_serializer.hpp>
#include <smart/data/complex_serializer_details.hpp>
#include <smart/data/complex_aux.hpp>
#include <smart/data/complex_except.hpp>

#include "detail/xercesc_string.hpp"
#include "detail/complex_default_content_handler.hpp"
#include "detail/complex_old_content_handler.hpp"

#ifdef UNI_GNUG
# pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp> // warning: comparison between signed and unsigned integer expressions

#ifdef UNI_GNUG
# pragma GCC diagnostic warning "-Wsign-compare"
#endif

#include <xercesc/internal/XSerializeEngine.hpp>
#include <xercesc/util/BinFileInputStream.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/dom/DOM.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4245) // signed/unsigned conversion mismatch
#endif // UNI_VISUAL_CPP

#include <boost/filesystem/convenience.hpp>

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

#include <ios>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iterator>

using std::ifstream;
using std::ofstream;
using std::vector;
using std::string;
using std::runtime_error;
using std::ios;

using boost::shared_ptr;
using boost::lexical_cast;
using boost::filesystem::file_size;
using boost::filesystem::path;
using boost::filesystem::create_directories;
using boost::filesystem::exists;


using smart::data::property;
using smart::data::complex;

using smart::data::message_tag;
using smart::data::string_tag;
using smart::data::int_tag;
using smart::data::uint_tag;
using smart::data::double_tag;
using smart::data::complex_tag;
using smart::data::property_tag;
using smart::data::array_tag;
using smart::data::array_item_tag;
using smart::data::name_attr;
using smart::data::type_attr;
using smart::data::array_attr;
using smart::data::serializer_error;
using smart::data::serializer_not_initialized_error;

using smart::data::detail::std_to_xml_string;
using smart::data::detail::xml_to_std_string;
using smart::data::detail::xml_string_type;

namespace sdt = smart::data::types;

//////////////////////////////////////////////////////////////////////////
// constants

// define this macros to reduce service information size
#ifndef SMART_COMPLEX_SHORT_XML_TAGS

/** Message tag. */
const string& smart::data::message_tag(void) { static const string s = "message"; return s; }
/** Property tag. */
const string& smart::data::property_tag(void) { static const string s = "property"; return s; }
/** Array tag. */
const string& smart::data::array_tag(void) { static const string s = "items"; return s; }
/** Array item tag. */
const string& smart::data::array_item_tag(void) { static const string s = "item"; return s; }
/** Name attribute. */
const string& smart::data::name_attr(void) { static const string s = "name"; return s; }
/** Type attribute. */
const string& smart::data::type_attr(void) { static const string s = "type"; return s; }
/** Array attribute. */
const string& smart::data::array_attr(void) { static const string s = "array"; return s; }

#else // SMART_COMPLEX_SHORT_XML_TAGS

/** Message tag. */
const string& smart::data:message_tag(void) { static const string s = "m"; return s; }
/** Property tag. */
const string& smart::data::property_tag(void) { static const string s = "p"; return s; }
/** Array tag. */
const string& smart::data::array_tag(void) { static const string s = "is"; return s; }
/** Array item tag. */
const string& smart::data::array_item_tag(void) { static const string s = "it"; return s; }
/** Name attribute. */
const string& smart::data::name_attr(void) { static const string s = "n"; return s; }
/** Type attribute. */
const string& smart::data::type_attr(void) { static const string s = "t"; return s; }
/** Array attribute. */
const string& smart::data::array_attr(void) { static const string s = "a"; return s; }

#endif // SMART_COMPLEX_SHORT_XML_TAGS

/** Int xml tag name. */
const string& smart::data::int_tag(void) { return types::int_type_name(); }
/** Uint xml tag name. */
const string& smart::data::uint_tag(void) { return types::uint_type_name(); }
/** Double xml tag name. */
const string& smart::data::double_tag(void) { return types::double_type_name(); }
/** String xml tag name. */
const string& smart::data::string_tag(void) { return types::string_type_name(); }
/** Complex xml tag name. */
const string& smart::data::complex_tag(void) { return types::complex_type_name(); }

//////////////////////////////////////////////////////////////////////////
// implementation details
namespace
{

//////////////////////////////////////////////////////////////////////////
// Xercesc Resource Sentinel
template <typename T>
class xercesc_resource_watcher
{
public:
  explicit xercesc_resource_watcher(T *t): _t(t) { /*empty*/ }
  ~xercesc_resource_watcher(void)
  {
    try
    {
      _t->release();
    }
    catch (...)
    {
      BOOST_ASSERT(!" - UNEXPECTED EXCEPTION!");
    }
  }

public:
  T* get(void) { return _t; }

  T* operator->(void) { return _t; }
  const T* operator->(void) const { return _t; }

  T& operator*(void) { return *_t; }
  const T& operator*(void) const { return *_t; }

private:
  T *_t;
};

//////////////////////////////////////////////////////////////////////////
// Auxiliary routines

void addattr(xercesc::DOMElement *el, const string &name, const string &value)
{
  el->setAttribute(std_to_xml_string(name).c_str(), std_to_xml_string(value).c_str());
}

//-----------------------------------------------------------------------------
template <typename T>
void addpropattr(const property<T> &p, xercesc::DOMElement *el)
{
  // property name attribute
  if (!p.name().empty())
  {
    addattr(el, name_attr(), p.name());
  }
}

//-----------------------------------------------------------------------------
template <typename T>
void addpropattr(const property<smart::data::array<T> > &p, xercesc::DOMElement *el)
{
  // property name attribute
  if (!p.name().empty())
  {
    addattr(el, name_attr(), p.name());
  }
}

//////////////////////////////////////////////////////////////////////////
// forward - serialize complex type
void serialize(const sdt::complex_type &c, xercesc::DOMDocument *doc, xercesc::DOMElement *parent);

//////////////////////////////////////////////////////////////////////////
// general template
template <typename T>
void serialize(const property<T> &p, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  if (p.name() != smart::data::name_property())
  {
    // new element which represents property
    xercesc::DOMElement *el = doc->createElement(std_to_xml_string(p.type_name()).c_str());
    // adding element to DOM tree
    parent->appendChild(el);
    // add attributes
    addpropattr(p, el);
    // set node value
    el->setTextContent(std_to_xml_string(lexical_cast<string>(p.value())).c_str());
  }
}

//////////////////////////////////////////////////////////////////////////
// complex type property specialization
template <>
void serialize(const sdt::complex_property_type &p, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  // new element which represents property
  xercesc::DOMElement *el = doc->createElement(std_to_xml_string(p.type_name()).c_str());
  // adding element to DOM tree
  parent->appendChild(el);
  // add attributes
  addpropattr(p, el);
  // serialize further
  serialize(p.value(), doc, el);
}

//////////////////////////////////////////////////////////////////////////
// arrays general template
//template <typename T>
//void serialize(const typename property<typename uct::rebind<T>::array_type>::value_type &ar, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
//void serialize(const typename uct::rebind<T>::array_type &ar, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
template <typename T>
void serialize(const smart::data::array<T> &ar, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  // iterate elements in the array
  for (typename property<typename sdt::rebind<T>::array_type>::value_type::size_type i = 0; i < ar.size(); ++i)
  {
    // took array item
    typename property<typename sdt::rebind<T>::array_type>::value_type::const_reference item = ar.at(i);
    // create xml element
    xercesc::DOMElement *el = doc->createElement(std_to_xml_string(array_item_tag()).c_str());
    // set xml element value
    el->setTextContent(std_to_xml_string(lexical_cast<string>(item)).c_str());
    // add node to parent
    parent->appendChild(el);
  }
}

//////////////////////////////////////////////////////////////////////////
// complex type array specialization
template <>
void serialize(const sdt::complex_array_type &ar, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  // iterate through the array
  for (sdt::complex_array_property_type::value_type::size_type i = 0; i < ar.size(); ++i)
  {
    // create xml element
    xercesc::DOMElement *el = doc->createElement(std_to_xml_string(array_item_tag()).c_str());
    // took array item
    sdt::complex_array_property_type::value_type::const_reference c = ar.at(i);
    // reassign actual work
    sdt::complex_property_type prop(smart::data::get_safe_name(c), c);
    serialize(prop, doc, el);
    // adding element to DOM tree
    parent->appendChild(el);
  }
}

//////////////////////////////////////////////////////////////////////////
// array property specialization
//template <typename T>
//void serialize(const property<typename uct::rebind<T>::array_type> &p, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
// ^^^ it is not compiled, but it should have been compiled. I have no idea why
template <typename T>
void serialize(const property<smart::data::array<T> > &p, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  // serialize array
  // new array <items> element
  xercesc::DOMElement *itemsel = doc->createElement(std_to_xml_string(array_tag()).c_str());

  serialize(p.value(), doc, itemsel);
  // new element which represents property
  xercesc::DOMElement *propel = doc->createElement(std_to_xml_string(p.type_name()).c_str());
  // adding element to DOM tree
  propel->appendChild(itemsel);
  // add attributes
  addpropattr(p, propel);
  // add node to xml
  parent->appendChild(propel);
}

//-----------------------------------------------------------------------------
template <typename IterT>
void serialize(smart::data::property_const_iterator<IterT> first,
               smart::data::property_const_iterator<IterT> last,
               xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  // unfortunately overloading doesn't work
  //for_each(first, last, boost::bind(&serialize, doc, parent));

  for ( ; first != last; ++first)
  {
    serialize(*first, doc, parent);
  }
}

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4396) // warning C4396: 'smart::data::operator ==' : the inline specifier cannot be used when a friend declaration refers to a specialization of a function template
#endif // SMART_VISUAL_CPP

//-----------------------------------------------------------------------------
void serialize(const sdt::complex_type &c, xercesc::DOMDocument *doc, xercesc::DOMElement *parent)
{
  serialize(c.begin<sdt::int_type>(), c.end<sdt::int_type>(), doc, parent);
  serialize(c.begin<sdt::uint_type>(), c.end<sdt::uint_type>(), doc, parent);
  serialize(c.begin<sdt::double_type>(), c.end<sdt::double_type>(), doc, parent);
  serialize(c.begin<sdt::string_type>(), c.end<sdt::string_type>(), doc, parent);
  serialize(c.begin<sdt::complex_type>(), c.end<sdt::complex_type>(), doc, parent);
  // arrays
  serialize(c.begin<sdt::int_array_type>(), c.end<sdt::int_array_type>(), doc, parent);
  serialize(c.begin<sdt::uint_array_type>(), c.end<sdt::uint_array_type>(), doc, parent);
  serialize(c.begin<sdt::double_array_type>(), c.end<sdt::double_array_type>(), doc, parent);
  serialize(c.begin<sdt::string_array_type>(), c.end<sdt::string_array_type>(), doc, parent);
  // complex type array
  serialize(c.begin<sdt::complex_array_type>(), c.end<sdt::complex_array_type>(), doc, parent);
}

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

//-----------------------------------------------------------------------------
void serialize(const sdt::complex_type &c, xercesc::DOMDocument *doc)
{
  xercesc::DOMElement *root = doc->getDocumentElement();

  root->setAttribute(std_to_xml_string(name_attr()).c_str(), std_to_xml_string(get_name(c)).c_str());

  serialize(c, doc, root);
}

//-----------------------------------------------------------------------------
string serialize(const sdt::complex_type& c, bool pretty_format, bool canonical_xml)
{
  if (!smart::data::serialization_helper::instance().initialized())
  {
    smart::data::serialization_helper::instance().init();
    //throw serializer_not_initialized_error("Serialization engine is not initialized. Initialize it first.");
  }

  using namespace xercesc;
  //using XERCES_CPP_NAMESPACE::XMLUni;
  //using XERCES_CPP_NAMESPACE::SAX2XMLReader;
  //using XERCES_CPP_NAMESPACE::XMLReaderFactory;
  //using XERCES_CPP_NAMESPACE::XMLString;
  //using XERCES_CPP_NAMESPACE::MemBufInputSource;
  //using XERCES_CPP_NAMESPACE::SAXException;
  //using XERCES_CPP_NAMESPACE::XMLException;

  static const XMLCh ls[]   = { xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull };

  string s;
  try
  {
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(ls);

    xercesc_resource_watcher<xercesc::DOMDocument> doc(impl->createDocument(0, std_to_xml_string(message_tag()).c_str(), 0));
    xercesc_resource_watcher<DOMWriter> writer(impl->createDOMWriter());

    //const uni_config& config  = uni_config::instance();

    // optionally, set some DOMWriter features
    // set the format-pretty-print feature
    if (pretty_format && writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
    {
      writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    }

    // set the canonical form feature
    if (canonical_xml && writer->canSetFeature(XMLUni::fgDOMWRTCanonicalForm, true))
    {
      writer->setFeature(XMLUni::fgDOMWRTCanonicalForm, true);
    }

    // set the byte-order-mark feature
    if (writer->canSetFeature(XMLUni::fgDOMWRTBOM, true))
    {
      writer->setFeature(XMLUni::fgDOMWRTBOM, true);
    }

    const xml_string_type utf8 = std_to_xml_string("UTF-8");

    doc->setEncoding(utf8.c_str());
    writer->setEncoding(utf8.c_str());
    // serialize message
    serialize(c, doc.get());
    // serialize the DOMNode to a UTF-16 string
    xercesc::MemBufFormatTarget membuf;
    writer->writeNode(&membuf, *doc);

    s.assign(reinterpret_cast<string::const_pointer>(membuf.getRawBuffer()), membuf.getLen());
  }
  catch (const DOMException &e)
  {
    throw serializer_error(xml_to_std_string(xml_string_type(e.getMessage())));
  }
  catch (const std::exception&)
  {
    throw; // rethrow
  }
  catch (...)
  {
    BOOST_ASSERT(!"Unknown error");

    throw runtime_error("Unknown error");
  }

  return s;
}

//-----------------------------------------------------------------------------
void unserialize(const string& complex_str, smart::data::message_content_handler_base& ch)
{
  if (!smart::data::serialization_helper::instance().initialized())
  {
    smart::data::serialization_helper::instance().init();
    //throw serializer_not_initialized_error("Serialization engine is not initialized. Initialize it first.");
  }
  //using namespace xercesc;
  using XERCES_CPP_NAMESPACE::XMLUni;
  using XERCES_CPP_NAMESPACE::SAX2XMLReader;
  using XERCES_CPP_NAMESPACE::XMLReaderFactory;
  using XERCES_CPP_NAMESPACE::XMLString;
  using XERCES_CPP_NAMESPACE::MemBufInputSource;
  using XERCES_CPP_NAMESPACE::SAXException;
  using XERCES_CPP_NAMESPACE::XMLException;

  shared_ptr<SAX2XMLReader> parser(XMLReaderFactory::createXMLReader());

  try
  {
    parser->setContentHandler(&ch);

    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgXercesDynamic, true);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);

    //enabled_exceptions(*parser);

    // fixme: to think about schema
    //const uni_config& config  = uni_config::instance();
    //const string& schemefn    = config.schemefn();
    //if (!schemefn.empty())
    //{
    //  parser->setFeature(XMLUni::fgXercesSchema, true);
    //  //parser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

    //  scoped_array<XMLCh> propval(XMLString::transcode(&*schemefn.begin()));
    //  parser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, propval.get());
    //}

    parser->setErrorHandler(&ch);

    static const xml_string_type fake_id = std_to_xml_string(string("a fake id"));

    const MemBufInputSource source(reinterpret_cast<const XMLByte*>(complex_str.c_str()), static_cast<unsigned int>(complex_str.size()), fake_id.c_str());
    parser->parse(source);
  }
  catch (const SAXException &e)
  {
    throw runtime_error(xml_to_std_string(xml_string_type(e.getMessage())));
  }
  catch (const XMLException &e)
  {
    throw runtime_error(xml_to_std_string(xml_string_type(e.getMessage())));
  }
  catch (const std::exception&)
  {
    throw;
  }
  catch (...)
  {
    BOOST_ASSERT(!"Unknown error");

    throw runtime_error("Unknown error");
  }
}

//-----------------------------------------------------------------------------
//void enabled_exceptions(XERCES_CPP_NAMESPACE::SAX2XMLReader& parser)
//{
//  using XERCES_CPP_NAMESPACE::XMLUni;
//
//  parser.setFeature(XMLUni::fgArrayIndexOutOfBoundsException_Name, true);
//  parser.setFeature(XMLUni::fgEmptyStackException_Name, true);
//  parser.setFeature(XMLUni::fgIllegalArgumentException_Name, true);
//  parser.setFeature(XMLUni::fgInvalidCastException_Name, true);
//  parser.setFeature(XMLUni::fgIOException_Name, true);
//  parser.setFeature(XMLUni::fgNoSuchElementException_Name, true);
//  parser.setFeature(XMLUni::fgNullPointerException_Name, true);
//  parser.setFeature(XMLUni::fgXMLPlatformUtilsException_Name, true);
//  parser.setFeature(XMLUni::fgRuntimeException_Name, true);
//  parser.setFeature(XMLUni::fgTranscodingException_Name, true);
//  parser.setFeature(XMLUni::fgUnexpectedEOFException_Name, true);
//  parser.setFeature(XMLUni::fgUnsupportedEncodingException_Name, true);
//  parser.setFeature(XMLUni::fgUTFDataFormatException_Name, true);
//  parser.setFeature(XMLUni::fgNetAccessorException_Name, true);
//  parser.setFeature(XMLUni::fgMalformedURLException_Name, true);
//  parser.setFeature(XMLUni::fgNumberFormatException_Name, true);
//  parser.setFeature(XMLUni::fgParseException_Name, true);
//  parser.setFeature(XMLUni::fgInvalidDatatypeFacetException_Name, true);
//  parser.setFeature(XMLUni::fgInvalidDatatypeValueException_Name, true);
//  parser.setFeature(XMLUni::fgSchemaDateTimeException_Name, true);
//  parser.setFeature(XMLUni::fgXPathException_Name, true);
//  parser.setFeature(XMLUni::fgXSerializationException_Name, true);
//}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// Default complex serializer
string smart::data::complex_default_serializer::serialize(const complex& c)
{
  return ::serialize(c, _pretty_format, _canonical_xml);
}

//-----------------------------------------------------------------------------
void smart::data::complex_default_serializer::unserialize(complex& c, const string& complex_str)
{
  smart::data::message_default_content_handler ch;

  ::unserialize(complex_str, ch);

  c = static_cast<const smart::data::message_default_content_handler&>(ch).complex();
}

//////////////////////////////////////////////////////////////////////////
// Old complex serializer
string smart::data::complex_old_serializer::serialize(const complex& /*c*/)
{
  return "Not implemented";//::serialize(c, _pretty_format, _canonical_xml);
}

//-----------------------------------------------------------------------------
void smart::data::complex_old_serializer::unserialize(complex& c, const string& complex_str)
{
  unicomm::impl::message_old_content_handler ch(&c);

  ::unserialize(complex_str, ch);
}

//////////////////////////////////////////////////////////////////////////
// wrappers

/** Serializes complex to a string using given serializer. */
string smart::data::serialize(const complex& c, complex_serializer_base& serializer)
{
  return serializer.serialize(c);
}

/** Unserializes complex from a string using given serializer. */
smart::data::complex& smart::data::unserialize(complex& c, const std::string& complex_str, complex_serializer_base& serializer)
{
  serializer.unserialize(c, complex_str);

  return c;
}

/** Serializes complex to a string using default serializer. */
string smart::data::default_serialize(const complex& c)
{
  complex_default_serializer serializer;

  return serialize(c, serializer);
}

/** Unserializes complex from a string using given serializer. */
smart::data::complex& smart::data::default_unserialize(complex& c, const std::string& complex_str)
{
  complex_default_serializer serializer;

  return unserialize(c, complex_str, serializer);
}

//////////////////////////////////////////////////////////////////////////
// convinient rountines
smart::data::complex& smart::data::from_file(complex_serializer_base& serializer, complex& c, const std::string& fn)
{
  ifstream f(fn.c_str());

  if (!f)
  {
    throw runtime_error("Unable to open file [" + fn + "]");
  }

  vector<char> buf(static_cast<vector<char>::size_type>(file_size(path(fn))));

#ifdef UNI_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4996) // function call with parameter that could be unsafe
#endif // UNI_VISUAL_CPP

  f.read(&*buf.begin(), static_cast<std::streamsize>(buf.size()));

#ifdef UNI_VISUAL_CPP
# pragma warning (pop)
#endif // UNI_VISUAL_CPP

  if (!f && !f.eof())
  {
    throw runtime_error("Error has occurred while reading file [" + fn + "]");
  }

  return unserialize(c, string(buf.begin(), buf.end()), serializer);
}

//-----------------------------------------------------------------------------
const smart::data::complex& smart::data::to_file(complex_serializer_base& serializer, const complex& c, const std::string& fn, bool create_dirs)
{
  if (create_dirs)
  {
    create_directories(path(fn).parent_path());
  }

  ofstream f(fn.c_str(), ios::trunc);

  if (!f)
  {
    throw runtime_error("Unable to open file [" + fn + "]");
  }

  f << serialize(c, serializer);

  return c;
}

//-----------------------------------------------------------------------------
smart::data::complex& smart::data::from_string(complex_serializer_base& serializer, complex& c, const string &s)
{
  return unserialize(c, s, serializer);
}

//-----------------------------------------------------------------------------
string smart::data::to_string(complex_serializer_base& serializer, const complex& c)
{
  return serialize(c, serializer);
}

