///////////////////////////////////////////////////////////////////////////////
// factory.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file factory.hpp
 *
 *  Simple factory that allows to create objects of one base type by 
 *  specifed identifier object. Maps creators to identifiers.
 */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_FACTORY_HPP_
#define SMART_FACTORY_HPP_

#include <map>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <stdexcept>

/** @namespace smart smart++ Library root namespace. */
namespace smart
{

/** Invalid type exception. */
class invalid_type_error : public std::runtime_error
{
public:
  explicit invalid_type_error(const std::string& what): std::runtime_error(what) { /* empty */ }
};

/** Factory.
 *
 *  Allows to register a creator and then create registered type.
 *
 *  @tparam CreatedT Data type that objects are created of.
 *  @tparam KeyT Key type to identify created objects.
 */
template <typename CreatedT, typename KeyT = std::string>
class factory
{
//////////////////////////////////////////////////////////////////////////
// factory interface
public:
  /** Type that object to be created of by the factory. */
  typedef CreatedT created_type;

  /** Smart pointer to created object. */
  typedef boost::shared_ptr<created_type> created_pointer_type;

  /** Predefined creator object type. */
  typedef boost::function<created_pointer_type (void)> creator_type;

  /** Key type which creators are mapped to. */
  typedef KeyT key_type;

  /** Easy init. */
  //typedef factory_easy_init<created_type, key_type> easy_init_type;
  
public:
  /** Creates a factory object. 
   *
   *  @param default_creator If there is no mapped creation object
   *  found this will be called. If this is null exceptions is thrown 
   *  by create operation.
   */
  explicit factory(const creator_type& default_creator = creator_type()): 
    _default_creator(default_creator)
  {  
    // empty
  }

  /** Whether factory is empty. */
  bool empty(void) const { return _creators.empty(); }

  /** Register new creator or overwrite existing. 
   *
   *  @return *this;
   */
  factory& reg(const key_type& id, const creator_type& f) 
    { _creators[id] = f; return *this; }

  /** Unregister creator. 
   *
   *  @return *this;
   */
  factory& unreg(const key_type& id) { _creators.erase(id); return *this; }

  /** Clears factory. */
  void unreg_all(void) { _creators.clear(); }

  /** Creates an object corresponding to the given identifier. 
   *
   *  @param id Created type identifier.
   *  @return Smart pointer to a created object.
   *  @throw smart::invalid_type_error is risen if there no given identifier found and default 
   *  creation object is null.
   */
  created_pointer_type create(const key_type& id) const
  {
    creator_type creator = _default_creator;

    typename creators_collection_type::const_iterator cit = _creators.find(id);
  
    if (cit != _creators.end() && cit->second)
    {
      creator = cit->second;
    }

    if (!creator)
    {
      throw invalid_type_error("factory: There is no specified type creator found");
    }

    return creator();
  }

  /** Registers a creator object with given identifier. */
  factory& operator()(const key_type& id, const creator_type& f)
    { reg(id, f); return *this; }

  /** Unregisters a creator object with given identifier. */
  factory& operator()(const key_type& id)
    { unreg(id); return *this; }

//////////////////////////////////////////////////////////////////////////
// private section
private:
  typedef std::map<key_type, creator_type> creators_collection_type;

private:
  creators_collection_type _creators;
  creator_type _default_creator;
}; // class factory

/** Default creator function, can be used with the factory. */
#ifndef SMART_BORLAND
template <typename T> typename factory<T>::created_pointer_type create(void) 
  { return factory<T>::created_pointer_type(new T()); }
#else
template <typename T> boost::shared_ptr<T> create(void) 
  { return boost::shared_ptr<T>(new T()); }
#endif // SMART_BORLAND

} // namespace smart

#endif // _FACTORY_HPP_

