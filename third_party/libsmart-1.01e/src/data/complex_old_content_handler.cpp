///////////////////////////////////////////////////////////////////////////////
// umessage_old_content_handler.cpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2009, (c) Dmitry Timoshenko.

#include "detail/complex_old_content_handler.hpp"
#include "detail/complex_helper.hpp"
#include "detail/xercesc_string.hpp"
#include <smart/data/complex.hpp>
#include <smart/data/complex_serializer.hpp>
#include <smart/debug_out.hpp>

#include <xercesc/sax2/Attributes.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <stack>
#include <string>
#include <vector>
#include <stdexcept>

#if !defined (NDEBUG) && !defined (SMART_COMPLEX_PARSER_VERBOSE) && defined (SMART_DEBUG_OUT)
# undef SMART_DEBUG_OUT
# define SMART_DEBUG_OUT(expr)
#endif // !defined NDEBUG && !defined SMART_COMPLEX_PARSER_VERBOSE && defined SMART_DEBUG_OUT

using std::stack;
using std::runtime_error;
using std::equal_to;
using std::bind2nd;
using std::wstring;
using std::string;
using std::vector;

using boost::shared_ptr;
using boost::lexical_cast;

using smart::data::detail::std_to_xml_string;
using smart::data::detail::xml_to_std_string;
using smart::data::detail::xml_string_type;

namespace unicomm
{

namespace impl
{

//////////////////////////////////////////////////////////////////////////
// Auxiliary types
//
// Property Descriptor
//
// fixme: It's necessary to put resources (see mch_impl class) into PropertyDescriptors
// Resources should be available through pointers. If pointer is null, it means
// there is no requested resource in the descriptor
struct old_property_descriptor
{
public:
  typedef vector<string> strings_type;
  typedef strings_type::size_type size_type;

public:
  old_property_descriptor(const string &name, const string &type, bool ar, const string &value = ""):
    _propname(name),
    _proptype(type),
    _propar(ar),
    _propval(value)
  {
    /*empty*/
  }

  const string& name(void) const { return _propname; }
  const string& type(void) const { return _proptype; }
  bool is_array(void) const { return _propar; }
  const string& value(void) const { return _propval; }
  const string& value_at(size_type i) const { return _propvalar.at(i); }
  string& value_at(size_type i) { return _propvalar.at(i); }
  size_type values_count(void) const { return _propvalar.size(); }
  const strings_type& values(void) const { return _propvalar; }
  //
  void name(const string &n) { _propname = n; }
  void type(const string &t) { _proptype = t; }
  void is_array(bool ar) { _propar = ar; }
  void value(const string &v) { _propval = v; }
  size_type add_value(const string &v) { _propvalar.push_back(v); return _propvalar.size() - 1; }
  void clear_value(void) { _propvalar.clear(); _propval = ""; }
  //
  bool is_complex(void) const { return _proptype == smart::data::types::complex_type_name(); }
  bool is_int(void) const { return _proptype == smart::data::types::int_type_name(); }
  bool is_uint(void) const { return _proptype == smart::data::types::uint_type_name(); }
  bool is_string(void) const { return _proptype == smart::data::types::string_type_name(); }
  bool is_double(void) const { return _proptype == smart::data::types::double_type_name(); }

private:
  string _propname;
  string _proptype;
  bool _propar;
  string _propval;
  mutable strings_type _propvalar;
};

//////////////////////////////////////////////////////////////////////////
// Content Handler Implementation

struct moch_impl
{
  typedef old_property_descriptor property_descriptor_type;
  typedef stack<smart::data::types::complex_type*> complex_stack_type;
  typedef stack<property_descriptor_type> descriptor_stack_type;
  typedef vector<shared_ptr<smart::data::types::complex_type> > complex_watchers_vector_type;

  //
  moch_impl(smart::data::complex *c): _complex(c)/*, _chunks(0)*/ { /*empty*/ }

  //-----------------------------------------------------------------------------
  /*const */property_descriptor_type& top_descriptor(void)
  {
    return _descriptors.top();
  }

  //-----------------------------------------------------------------------------
  /*const */smart::data::types::complex_type& top_complex(void)
  {
    return *_complexes.top();
  }

  //-----------------------------------------------------------------------------
  void top_descriptor(const property_descriptor_type& d)
  {
    _descriptors.push(d);
  }

  //-----------------------------------------------------------------------------
  void top_complex(smart::data::types::complex_type* c)
  {
    _complexes.push(c);
  }

  //-----------------------------------------------------------------------------
  void push_watcher(const complex_watchers_vector_type::value_type& watcher)
  {
    _watchers.push_back(watcher);
  }

  //////////////////////////////////////////////////////////////////////////
  // data
  complex_stack_type _complexes;
  descriptor_stack_type _descriptors;
  complex_watchers_vector_type _watchers;

  smart::data::complex *_complex;

  //smart::data::complex _message;
};

} // namespace impl

} // namespace unicomm

namespace
{

typedef unicomm::impl::moch_impl::property_descriptor_type property_descriptor_type;
typedef unicomm::impl::moch_impl::complex_stack_type complex_stack_type;
typedef unicomm::impl::moch_impl::descriptor_stack_type descriptor_stack_type;
typedef unicomm::impl::moch_impl::complex_watchers_vector_type complex_watchers_vector_type;
typedef unicomm::impl::message_old_content_handler::tag_attributes_collection_type tag_attributes_collection_type;


//////////////////////////////////////////////////////////////////////////
// Auxiliary routines
xml_string_type getattr(const xercesc::Attributes &attrs, const xml_string_type &attrname)
{
  smart::data::throwif(attrs.getIndex(attrname.c_str()), bind2nd(equal_to<int>(), -1), runtime_error("Requested attribute doesn't exist"));

  return attrs.getValue(attrname.c_str());
}

//-----------------------------------------------------------------------------
string getattr(const xercesc::Attributes &attrs, const string &attrname)
{
  const xml_string_type an = std_to_xml_string(attrname);

  xml_string_type val = std_to_xml_string(wstring(L"0"));

  if (attrname != smart::data::array_attr())
  {
    smart::data::throwif(attrs.getIndex(an.c_str()), bind2nd(equal_to<int>(), -1), runtime_error("Requested attribute doesn't exist"));
    val = attrs.getValue(an.c_str());
  } else if (attrs.getIndex(an.c_str()) != -1)
  {
    val = attrs.getValue(an.c_str());

    // does g++ implement ADL right? why doesn't it find declared operators?
//    if (val == L"true")
//    {
//      val = L"1";
//    }
    // it seems it doesn't
    if (smart::data::detail::operator==(val, L"true"))
    {
      val = std_to_xml_string(wstring(L"1"));
    }
  }

  return xml_to_std_string(val);
}

//-----------------------------------------------------------------------------
string get_attr(const tag_attributes_collection_type& attrs, const string& attr_name)
{
  xml_string_type val = std_to_xml_string(wstring(L"0"));

  tag_attributes_collection_type::const_iterator cit = attrs.find(attr_name);

  if (attr_name != smart::data::array_attr())
  {
    // fixme: if array attribute requested - throw exception if attribute doesn't exist... why?
    if (cit == attrs.end())
    {
      throw runtime_error("Requested attribute doesn't exist");
    }

    val = cit->second;
  } else if (cit != attrs.end())
  {
    val = cit->second;

    // does g++ implement ADL right? why doesn't it find declared operators?
    //    if (val == L"true")
    //    {
    //      val = L"1";
    //    }
    // it seems it doesn't
    if (smart::data::detail::operator==(val, L"true"))
    {
      val = std_to_xml_string(wstring(L"1"));
    }
  }

  return xml_to_std_string(val);
}


//////////////////////////////////////////////////////////////////////////
// serves complex properties
void setproperty(const smart::data::types::complex_type &what, smart::data::types::complex_type &where)
{
  where.set_property(what.get_property<smart::data::types::string_type>(smart::data::name_property()), what);
}

//////////////////////////////////////////////////////////////////////////
// serves complex properties arrays
void setproperty(const string& name, const smart::data::types::complex_array_type &array, smart::data::types::complex_type &where)
{
  where.set_property(name, array);
}

//////////////////////////////////////////////////////////////////////////
// serves simple properties and simple properties arrays
template <typename T>
void setproperty(smart::data::types::complex_type &c, const property_descriptor_type &pd)
{
  if (pd.is_array())
  {
    typename smart::data::types::rebind<T>::array_type ar;

    for (property_descriptor_type::size_type i = 0; i < pd.values_count(); ++i)
    {
      ar.add(lexical_cast<T>(pd.value_at(i)));
    }

    c.set_property(pd.name(), ar);
  } else
  {
    c.set_property(pd.name(), lexical_cast<T>(pd.value()));
  }
}

//-----------------------------------------------------------------------------
void setproperty(smart::data::types::complex_type &c, const property_descriptor_type &pd)
{
  using namespace unicomm;

  if (pd.is_int())
  {
    // int
    setproperty<smart::data::types::int_type>(c, pd);
  }
  else if (pd.is_uint())
  {
    // unsigned int
    setproperty<smart::data::types::uint_type>(c, pd);
  }
  else if (pd.is_double())
  {
    // double
    setproperty<smart::data::types::double_type>(c, pd);
  }
  else if (pd.is_string())
  {
    // string
    setproperty<smart::data::types::string_type>(c, pd);
  }
  //else if (pd.is_complex())
  //{
  //  // complex
  //}
}

//-----------------------------------------------------------------------------
void setproperty(complex_stack_type& complexes, descriptor_stack_type& descriptors)
{
  if (descriptors.top().is_complex())
  {
    if (descriptors.top().is_array())
    {
      smart::data::types::complex_type &c               = *complexes.top();
      const string name                             = c.get_property<smart::data::types::string_type>(smart::data::name_property());
      const bool propex                             = c.prop_exists<smart::data::types::complex_array_type>(name);
      const smart::data::types::complex_array_type& car = propex? c.get_property<smart::data::types::complex_array_type>(name): smart::data::types::complex_array_type();

      complexes.pop();
      descriptors.pop();

      setproperty(name, car, *complexes.top());
    } else
    {
      // complex type
      // look upper, what is there?
      descriptors.pop();

      if (descriptors.top().is_array())
      {
        smart::data::types::complex_type &c         = *complexes.top();
        complexes.pop();
        // cparent is temporary storage of the current array. It won't be included into result set
        smart::data::types::complex_type &cparent   = *complexes.top();
        const string name                           = cparent.get_property<smart::data::types::string_type>(smart::data::name_property());

        const bool propex                           = cparent.prop_exists<smart::data::types::complex_array_type>(name);
        smart::data::types::complex_array_type car  = propex? cparent.get_property<smart::data::types::complex_array_type>(name): smart::data::types::complex_array_type();

        car.add(c);

        setproperty(name, car, cparent);
      }
      else
      {
        smart::data::types::complex_type &c = *complexes.top();
        complexes.pop();

        setproperty(c, *complexes.top());
      }
    }
  } else
  {
    // simple types and simple type arrays
    setproperty(*complexes.top(), descriptors.top());

    descriptors.pop();
  }
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// message_content_handler class

unicomm::impl::message_old_content_handler::message_old_content_handler(smart::data::complex *c):
  _impl(new impl::moch_impl(c))
{
  BOOST_ASSERT(_impl->_complex != 0 && "message could not be null");
}

//-----------------------------------------------------------------------------
unicomm::impl::message_old_content_handler::~message_old_content_handler(void)
{
  delete _impl;
}

//-----------------------------------------------------------------------------
smart::data::complex* unicomm::impl::message_old_content_handler::message(void) const
{
  return _impl->_complex;
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::message(smart::data::complex *c)
{
  _impl->_complex = c;

  BOOST_ASSERT(_impl->_complex != 0 && "Message could not be null");
}

////-----------------------------------------------------------------------------
//const string& unicomm::impl::message_old_content_handler::topelement(void) const
//{
//  return _impl->_elements.top();
//}
//
////-----------------------------------------------------------------------------
///*const */property_descriptor_type& unicomm::impl::message_old_content_handler::topdescriptor(void)
//{
//  return _impl->_descriptors.top();
//}
//
////-----------------------------------------------------------------------------
///*const */unicomm::types::complex_type& unicomm::impl::message_old_content_handler::topcomplex(void)
//{
//  return *_impl->_complexes.top();
//}
//
////-----------------------------------------------------------------------------
//void unicomm::impl::message_old_content_handler::topelement(const string& el)
//{
//  _impl->_elements.push(el);
//}
//
////-----------------------------------------------------------------------------
//void unicomm::impl::message_old_content_handler::topdescriptor(const unicomm::impl::property_descriptor& d)
//{
//  _impl->_descriptors.push(d);
//}
//
////-----------------------------------------------------------------------------
//void unicomm::impl::message_old_content_handler::topcomplex(types::complex_type* c)
//{
//  _impl->_complexes.push(c);
//}
//
////-----------------------------------------------------------------------------
//void unicomm::impl::message_content_handler::popelement(void)
//{
//  _impl->_elements.pop();
//}

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
//void unicomm::impl::message_content_handler::error(const XERCES_CPP_NAMESPACE::SAXParseException& exc)
//{
//  throw runtime_error("Unicomm xml parser error: " + xml_to_std_string(xml_string_type(exc.getMessage())));
//}
//
////-----------------------------------------------------------------------------
//void unicomm::impl::message_content_handler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exc)
//{
//  throw runtime_error("Unicomm xml parser error: " + xml_to_std_string(xml_string_type(exc.getMessage())));
//}
//
////-----------------------------------------------------------------------------
//void unicomm::impl::message_content_handler::warning(const XERCES_CPP_NAMESPACE::SAXParseException& /*exc*/)
//{
//  //throw runtime_error(xml_to_std_string(xml_string_type(exc.getMessage())));
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool unicomm::impl::message_old_content_handler::is_message_tag(const std::string& tag) const
{
  return tag == smart::data::message_tag();
}

//-----------------------------------------------------------------------------
bool unicomm::impl::message_old_content_handler::is_property_tag(const std::string& tag) const
{
  return tag == smart::data::property_tag();
}

//-----------------------------------------------------------------------------
bool unicomm::impl::message_old_content_handler::is_array_tag(const std::string& tag) const
{
  return tag == smart::data::array_tag();
}

//-----------------------------------------------------------------------------
bool unicomm::impl::message_old_content_handler::is_array_item_tag(const std::string& tag) const
{
  return tag == smart::data::array_item_tag();
}

// fixme: check tags invariant until xml-schema is not thought out
//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_message_start(const std::string& /*tag*/, const tag_attributes_collection_type& attrs)
{
  // <message>
  SMART_DEBUG_OUT("[xml parser]: <message> started");
  // message has started
  _impl->top_complex(message());
  // insert descriptor
  _impl->top_descriptor(property_descriptor_type(get_attr(attrs, smart::data::name_attr()), get_attr(attrs, smart::data::type_attr()), false /*getattr(attrs, message::array_attr)*/));
  SMART_DEBUG_OUT("[xml parser]: message name = " << _impl->top_descriptor().name() << "; type = " << _impl->top_descriptor().type());
  // set predefined Name property
  setproperty(_impl->top_complex(), property_descriptor_type(smart::data::name_property(), smart::data::types::string_type_name(), false, get_attr(attrs, smart::data::name_attr())));
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_property_start(const std::string& /*tag*/, const tag_attributes_collection_type& attrs)
{
  // <property>
  SMART_DEBUG_OUT("[xml parser]: <property> started");

  _impl->top_descriptor(property_descriptor_type(get_attr(attrs, smart::data::name_attr()), get_attr(attrs, smart::data::type_attr()), lexical_cast<bool>(get_attr(attrs, smart::data::array_attr()))));

  SMART_DEBUG_OUT("[xml parser]: property name = " << _impl->top_descriptor().name() << "; type = " << _impl->top_descriptor().type());

  if (_impl->top_descriptor().is_complex())
  {
    // complex type has just begun
    complex_watchers_vector_type::value_type c(new smart::data::types::complex_type());
    _impl->push_watcher(c);
    _impl->top_complex(c.get());

    // set predefined 'name' property
    setproperty(_impl->top_complex(), property_descriptor_type(smart::data::name_property(), smart::data::types::string_type_name(), false, get_attr(attrs, smart::data::name_attr())));
  }
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_array_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/)
{
  // array <items>
  SMART_DEBUG_OUT("[xml parser]: <items> array started");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - Top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_array_item_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/)
{
  // array <item>
  SMART_DEBUG_OUT("[xml parser]: <item> array started");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - Top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

//-----------------------------------------------------------------------------
//void on_message_characters(const xml_string_type& chars, bool continuation) { ; }
//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_property_characters(const xml_string_type& chars, bool continuation)
{
  // property
  property_descriptor_type& topdesc = _impl->top_descriptor();
  const string value                = xml_to_std_string(chars);//string(s.begin(), s.end());

  continuation? topdesc.value(topdesc.value() + value): topdesc.value(value);

  SMART_DEBUG_OUT("[xml parser]: property value = " << topdesc.value());
}
//-----------------------------------------------------------------------------
//void on_array_characters(const xml_string_type& chars, bool continuation) { ; }
//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_array_item_characters(const xml_string_type& chars, bool continuation)
{
  // array item
  property_descriptor_type& topdesc = _impl->top_descriptor();
  if (!topdesc.is_complex())
  {
    // simple type array item
    // fixme: consider continuation
    const property_descriptor_type::size_type i = topdesc.add_value(xml_to_std_string(chars));
    i; // make compiler stay quiet in release -> avoid 'unreferenced variable warning'
    SMART_DEBUG_OUT("[xml parser]: array item value = " << topdesc.value_at(i));
  }
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_message_end(const std::string& /*tag*/)
{
  // <message>
  SMART_DEBUG_OUT("[xml parser]: </message> finished; name = " << _impl->top_descriptor().name());
  // message has finished
  BOOST_ASSERT(_impl->_complexes.size() == 1 && " - Stack size should be equal to 1");
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_property_end(const std::string& /*tag*/)
{
  // <property>
  SMART_DEBUG_OUT("[xml parser]: </property> finshed; name = " << _impl->top_descriptor().name());
  // property has finished
  setproperty(_impl->_complexes, _impl->_descriptors);
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_array_end(const std::string& /*tag*/)
{
  // array
  SMART_DEBUG_OUT("[xml parser]: </items> array finshed");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - Top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

//-----------------------------------------------------------------------------
void unicomm::impl::message_old_content_handler::on_array_item_end(const std::string& /*tag*/)
{
  // array item
  SMART_DEBUG_OUT("[xml parser]: </item> array finshed");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - Top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

