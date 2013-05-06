///////////////////////////////////////////////////////////////////////////////
// umessage_adv_content_handler.cpp
//
// smart++ - Multipurpose C++ Library.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)//
//
// 2010, (c) Dmitry Timoshenko.

#include "detail/complex_default_content_handler.hpp"
#include "detail/complex_helper.hpp"
#include "detail/xercesc_string.hpp"
#include <smart/data/complex_serializer.hpp>

#include <smart/debug_out.hpp>

#include <xercesc/sax2/Attributes.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
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

using smart::data::types::complex_type_name;
using smart::data::types::string_type_name;
using smart::data::types::double_type_name;
using smart::data::types::int_type_name;
using smart::data::types::uint_type_name;
using smart::data::types::complex_array_type;
using smart::data::types::complex_type;
using smart::data::types::string_type;
using smart::data::types::double_type;
using smart::data::types::int_type;
using smart::data::types::uint_type;
using smart::data::array_attr;
using smart::data::name_attr;
using smart::data::name_property;

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
// fixme: It's necessary to put resources (see mach_impl class) into PropertyDescriptors
// Resources should be available through pointers. If pointer is null, it means
// there is no requested resource in the descriptor
struct default_property_descriptor
{
public:
  typedef vector<string> strings_type;
  typedef strings_type::size_type size_type;

public:
  default_property_descriptor(const string &name, const string &type, bool ar, const string &value = ""):
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
  bool is_complex(void) const { return _proptype == complex_type_name(); }
  bool is_int(void) const { return _proptype == int_type_name(); }
  bool is_uint(void) const { return _proptype == uint_type_name(); }
  bool is_string(void) const { return _proptype == string_type_name(); }
  bool is_double(void) const { return _proptype == double_type_name(); }

private:
  string _propname;
  string _proptype;
  bool _propar;
  string _propval;
  mutable strings_type _propvalar;
};

//////////////////////////////////////////////////////////////////////////
// Content Handler Implementation

struct mdch_impl
{
  typedef default_property_descriptor property_descriptor_type;
  typedef stack<complex_type*> complex_stack_type;
  typedef stack<property_descriptor_type> descriptor_stack_type;
  typedef vector<shared_ptr<complex_type> > complex_watchers_vector_type;

  //mdch_impl(void) { /*empty*/ }

  //-----------------------------------------------------------------------------
  /*const */property_descriptor_type& top_descriptor(void)
  {
    return _descriptors.top();
  }

  //-----------------------------------------------------------------------------
  /*const */complex_type& top_complex(void)
  {
    return *_complexes.top();
  }

  //-----------------------------------------------------------------------------
  void top_descriptor(const property_descriptor_type& d)
  {
    _descriptors.push(d);
  }

  //-----------------------------------------------------------------------------
  void top_complex(complex_type* c)
  {
    _complexes.push(c);
  }

  //-----------------------------------------------------------------------------
  void push_watcher(const complex_watchers_vector_type::value_type& watcher)
  {
    _watchers.push_back(watcher);
  }

  //-----------------------------------------------------------------------------
  void init(void)
  {
    _descriptors = descriptor_stack_type();
    _complexes   = complex_stack_type();
    _watchers.clear();
    _complex.clear();
  }

  //////////////////////////////////////////////////////////////////////////
  // data
  complex_stack_type _complexes;
  descriptor_stack_type _descriptors;
  complex_watchers_vector_type _watchers;

  complex_type _complex;
};

} // namespace detail

} // namespace data

} // namespace smart


using smart::data::detail::mdch_impl;

namespace
{

typedef mdch_impl::property_descriptor_type property_descriptor_type;
typedef mdch_impl::complex_stack_type complex_stack_type;
typedef mdch_impl::descriptor_stack_type descriptor_stack_type;
typedef mdch_impl::complex_watchers_vector_type complex_watchers_vector_type;
typedef smart::data::message_default_content_handler::tag_attributes_collection_type tag_attributes_collection_type;

//////////////////////////////////////////////////////////////////////////
// Auxiliary routines

string get_attr(const tag_attributes_collection_type& attrs, const string& attr_name)
{
  xml_string_type val;// = std_to_xml_string(wstring(L"0"));

  tag_attributes_collection_type::const_iterator cit = attrs.find(attr_name);

  // we have no mandatory attributes, every attribute is optional
  if (cit != attrs.end())
  {
    val = cit->second;

    if (attr_name == array_attr())
    {
      // looking for array attribute

      // does g++ implement ADL right? why doesn't it find declared operators?
      //    if (val == L"true")
      //    {
      //      val = L"1";
      //    }
      // it seems it doesn't
      // fixme: to think about allowed values? throw or not if not allowed.
      // at this time everything is allowed.
      // "", "false", "0" assumed to false; remain values set are considered as true
      if (smart::data::detail::operator==(val, L"false") || smart::data::detail::operator==(val, L""))
      {
        val = std_to_xml_string(wstring(L"0"));
      }
      else if (smart::data::detail::operator!=(val, L"0"))
      {
        val = std_to_xml_string(wstring(L"1"));
      }

      BOOST_ASSERT(!val.empty() && " - Value could not be empty here");
    }
  } else
  {
    if (attr_name == array_attr())
    {
      val = std_to_xml_string(wstring(L"0"));
    }
  }

  return xml_to_std_string(val);
}


//////////////////////////////////////////////////////////////////////////
// serves complex properties
void set_property(const complex_type &what, complex_type &where)
{
  where.set_property(what.get_property<string_type>(name_property()), what);
}

//////////////////////////////////////////////////////////////////////////
// serves complex properties arrays
void set_property(const string& name, const complex_array_type &array, complex_type &where)
{
  where.set_property(name, array);
}

//////////////////////////////////////////////////////////////////////////
// serves simple properties and simple properties arrays
template <typename T>
void set_property(complex_type &c, const property_descriptor_type &pd)
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
void set_property(complex_type &c, const property_descriptor_type &pd)
{
  if (pd.is_int())
  {
    // int
    set_property<smart::data::types::int_type>(c, pd);
  }
  else if (pd.is_uint())
  {
    // unsigned int
    set_property<smart::data::types::uint_type>(c, pd);
  }
  else if (pd.is_double())
  {
    // double
    set_property<smart::data::types::double_type>(c, pd);
  }
  else if (pd.is_string())
  {
    // string
    set_property<smart::data::types::string_type>(c, pd);
  }
  //else if (pd.is_complex())
  //{
  //  // complex
  //}
}

//-----------------------------------------------------------------------------
void set_property(mdch_impl::complex_stack_type& complexes, descriptor_stack_type& descriptors)
{
  if (descriptors.top().is_complex())
  {
    complex_type &c = *complexes.top();

    if (descriptors.top().is_array())
    {
      const string name                             = c.get_property<string_type>(name_property());
      const bool propex                             = c.prop_exists<complex_array_type>(name);
      const complex_array_type& car = propex? c.get_property<complex_array_type>(name): complex_array_type();

      complexes.pop();
      descriptors.pop();

      set_property(name, car, *complexes.top());
    } else
    {
      // complex type
      // look upper, what is there?
      // current descriptor is not needed anymore
      descriptors.pop();
      complexes.pop();

      if (descriptors.top().is_array())
      {
        // cparent is temporary storage of the current array. It won't be included into result set
        complex_type &cparent  = *complexes.top();
        const string name      = cparent.get_property<string_type>(name_property());

        const bool propex      = cparent.prop_exists<complex_array_type>(name);
        // fixme: it's necessary to provide less expensive way to setup a complex array
        complex_array_type car = propex? cparent.get_property<complex_array_type>(name): complex_array_type();

        car.add(c);

        set_property(name, car, cparent);
      }
      else
      {
        set_property(c, *complexes.top());
      }
    }
  } else
  {
    // simple types and simple type arrays
    set_property(*complexes.top(), descriptors.top());

    descriptors.pop();
  }
}

//-----------------------------------------------------------------------------
bool is_property_tag(const string& tag)
{
  return
    string_type_name() == tag ||
    int_type_name() == tag ||
    uint_type_name() == tag ||
    double_type_name() == tag ||
    complex_type_name() == tag;
}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// message_default_content_handler class

smart::data::message_default_content_handler::message_default_content_handler(void):
  _impl(new mdch_impl)
{
  // empty
}

//-----------------------------------------------------------------------------
smart::data::message_default_content_handler::~message_default_content_handler(void)
{
  delete _impl;
}

//-----------------------------------------------------------------------------
const smart::data::complex& smart::data::message_default_content_handler::complex(void) const
{
  return _impl->_complex;
}

//-----------------------------------------------------------------------------
smart::data::complex& smart::data::message_default_content_handler::complex(void)
{
  return _impl->_complex;
}

//-----------------------------------------------------------------------------
bool smart::data::message_default_content_handler::is_message_tag(const std::string& tag) const
{
  return tag == smart::data::message_tag();
}

//-----------------------------------------------------------------------------
bool smart::data::message_default_content_handler::is_property_tag(const std::string& tag) const
{
  return ::is_property_tag(tag);
}

//-----------------------------------------------------------------------------
bool smart::data::message_default_content_handler::is_array_tag(const std::string& tag) const
{
  return tag == smart::data::array_tag();
}

//-----------------------------------------------------------------------------
bool smart::data::message_default_content_handler::is_array_item_tag(const std::string& tag) const
{
  return tag == smart::data::array_item_tag();
}

// fixme: check tags invariant until xml-schema is not thought out
//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_message_start(const std::string& /*tag*/, const tag_attributes_collection_type& attrs)
{
  // <message>
  SMART_DEBUG_OUT("[xml parser]: <message> started");
  // message has started
  _impl->top_complex(&complex());
  // insert descriptor
  _impl->top_descriptor(property_descriptor_type(get_attr(attrs, name_attr()), complex_type_name()/*getattr(attrs, uni_message::type_attr())*/, false /*getattr(attrs, message::array_attr)*/));
  SMART_DEBUG_OUT("[xml parser]: message name = " << _impl->top_descriptor().name() << "; type = " << _impl->top_descriptor().type());
  // set predefined 'name' property
  set_property(_impl->top_complex(), property_descriptor_type(name_property(), string_type_name(), false, get_attr(attrs, name_attr())));
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_property_start(const std::string& tag, const tag_attributes_collection_type& attrs)
{
  // <property>
  SMART_DEBUG_OUT("[xml parser]: <" << tag << "> property started");

  _impl->top_descriptor(property_descriptor_type(get_attr(attrs, name_attr()), tag/*getattr(attrs, uni_message::type_attr())*/, false/*lexical_cast<bool>(get_attr(attrs, array_attr()))*/));

  SMART_DEBUG_OUT("[xml parser]: property name = " << _impl->top_descriptor().name() << "; type = " << _impl->top_descriptor().type());

  if (_impl->top_descriptor().is_complex())
  {
    // complex type has just begun
    complex_watchers_vector_type::value_type c(new complex_type());
    _impl->push_watcher(c);
    _impl->top_complex(c.get());

    // set predefined 'name' property
    set_property(_impl->top_complex(), property_descriptor_type(name_property(), string_type_name(), false, get_attr(attrs, name_attr())));
  }
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_array_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/)
{
  // array <items>
  SMART_DEBUG_OUT("[xml parser]: <items> array started");

  _impl->top_descriptor().is_array(true);
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_array_item_start(const std::string& /*tag*/, const tag_attributes_collection_type& /*attrs*/)
{
  // array <item>
  SMART_DEBUG_OUT("[xml parser]: <item> array started");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

//-----------------------------------------------------------------------------
//void unicomm::impl::message_default_content_handler::on_message_characters(const xml_string_type& chars, bool continuation) { ; }

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_property_characters(const xml_string_type& chars, bool continuation)
{
  // property
  property_descriptor_type& topdesc = _impl->top_descriptor();

  // ignore characters for complex objects
  if (!topdesc.is_complex())
  {
    const string value           = xml_to_std_string(chars);

    continuation? topdesc.value(topdesc.value() + value): topdesc.value(value);

    SMART_DEBUG_OUT("[xml parser]: property value = " << topdesc.value());
  }
}

//-----------------------------------------------------------------------------
//void unicomm::impl::message_default_content_handler::on_array_characters(const xml_string_type& chars, bool continuation) { ; }

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_array_item_characters(const xml_string_type& chars, bool continuation)
{
  // array item
  property_descriptor_type& topdesc = _impl->top_descriptor();

  // ignore characters for items in complex object arrays
  if (!topdesc.is_complex())
  {
    // simple type array item
    // fixme: to consider data continuation for the item - to process large data blocks
    //continuation? topdesc.
    SMART_IFDEF_DEBUG(const property_descriptor_type::size_type i =) topdesc.add_value(xml_to_std_string(chars));
    SMART_IFDEF_DEBUG(i;)
    SMART_DEBUG_OUT("[xml parser]: array item value = " << topdesc.value_at(i));
  }
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_message_end(const std::string& /*tag*/)
{
  // <message>
  SMART_DEBUG_OUT("[xml parser]: </message> finshed; name = " << _impl->top_descriptor().name());
  // message has finished
  BOOST_ASSERT(_impl->_complexes.size() == 1 && " - stack size should be equal to 1");
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_property_end(const std::string& /*tag*/)
{
  // <property>
  SMART_DEBUG_OUT("[xml parser]: </property> finshed; name = " << _impl->top_descriptor().name());
  // property has finished
  set_property(_impl->_complexes, _impl->_descriptors);
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_array_end(const std::string& /*tag*/)
{
  // array
  SMART_DEBUG_OUT("[xml parser]: </items> array finshed");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

//-----------------------------------------------------------------------------
void smart::data::message_default_content_handler::on_array_item_end(const std::string& /*tag*/)
{
  // array item
  SMART_DEBUG_OUT("[xml parser]: </item> array finshed");

  BOOST_ASSERT(_impl->top_descriptor().is_array() && " - top element is not an array");
  if (!_impl->top_descriptor().is_array())
  {
    throw runtime_error("Top element is not an array");
  }
}

