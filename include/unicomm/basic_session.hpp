///////////////////////////////////////////////////////////////////////////////
// basic_session.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Basic unicomm session implementation.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_BASIC_SESSION_HPP_
#define UNI_BASIC_SESSION_HPP_

/** @file basic_session.hpp Session base class definition. */

#include <unicomm/session_base.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Represents session parameters absence. */
struct void_session_params { /* empty */ };

/** Session interface to be implemented
 *  by user's code to receive event notifications.
 *
 *  This is base class for user's objects. It receives different
 *  notifications about the communication process.
 *  This is the place where the business logic should live.
 *
 *  @tparam DerivedT Type that inherits unicomm::basic_session.
 *  @tparam SessionParamsT Parameters type that will be passed to the
 *    session constructor. By default unicomm::void_session_params is used.
 *
 *  @tparam BaseT Base type for the session. Default is unicomm::session_base.
 *
 *  @note Throwing an exception from any allowed handler interrupts current
 *    process iteration for the session object. An attempt to call left
 *    handlers will take place on next process iteration.
 *
 *  @see @ref sessions_section, unicomm::session_base,
 *    unicomm::extended_session.
 */
template <
  typename DerivedT,
  typename SessionParamsT = void_session_params,
  typename BaseT = session_base>
class basic_session : public BaseT
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Derived type to be created when connection occurs. */
  typedef DerivedT derived_type;

  /** Session parameters type. */
  typedef SessionParamsT session_params_type;

  /** Session base type. */
  typedef BaseT base_type;

  /** This type. */
  typedef basic_session<DerivedT, session_params_type, base_type> this_type;

  /** Smart pointer to a session. */
  typedef boost::shared_ptr<this_type> pointer_type;

public:
  /** Constructs session object. 
   *
   *  @param extra_params Additional session parameters. 
   */
  explicit basic_session(const SessionParamsT& extra_params):
    _params(extra_params)
  {
    // empty
  }

public:
  /** Used as factory method for the derived class.
   *
   *  @param params Connected handler parameters to be passed to the session
   *    constructor.
   *
   *  @param extra_params Additional session parameters. 
   *
   *  @return Smart pointer to the instance of derived_type.
   *
   *  @see unicomm::config::session_factory().
   */
  static unicomm::session_base::pointer_type create(
    const unicomm::connected_params& params, const SessionParamsT& extra_params)
  {
    return pointer_type(new DerivedT(params, extra_params));
  }

public:
  /** Returns a const reference to the session parameters. */
  virtual const SessionParamsT& parameters(void) const { return _params; }

private:
  SessionParamsT _params;
};

/** @brief Session interface to be implemented
 *    by user's code to receive event notifications.
 *
 *  Partial specialization to generic unicomm::basic_session class template.
 *
 *  This is base class for user's objects. It receives different
 *  notifications about the communication process.
 *  This is the place where the business logic should live.
 *
 *  @tparam DerivedT Type that inherits unicomm::basic_session.
 *  @tparam BaseT Base type for the session. Default is unicomm::session_base.
 *
 *  @note Throwing an exception from any allowed handler interrupts current
 *    process iteration for this session object. An attempt to call left
 *    handlers will take place on next process iteration.
 *
 *  @see @ref sessions_section, unicomm::session_base,
 *    unicomm::extended_session.
 */
template <typename DerivedT, typename BaseT>
class basic_session<DerivedT, void_session_params, BaseT> : public BaseT
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Derived type to be created when connection occurs. */
  typedef DerivedT derived_type;

  /** Session parameters type. */
  typedef void_session_params session_params_type;

  /** Session base type. */
  typedef BaseT base_type;

  /** This type. */
  typedef basic_session<DerivedT, session_params_type, base_type> this_type;

  /** Smart pointer to a session. */
  typedef boost::shared_ptr<this_type> pointer_type;

public:
  /** Used as factory method for the derived class.
   *
   *  @param params Connected handler parameters to be passed to the session
   *    constructor.
   *
   *  @return Smart pointer to the instance of derived_type.
   *
   *  @see unicomm::config::session_factory().
   */
  static unicomm::session_base::pointer_type
    create(const unicomm::connected_params& params)
  {
    return pointer_type(new DerivedT(params));
  }

public:
  /** Default constructor. */
  basic_session(void) { /* empty */ }
};

} // namespace unicomm

#endif // UNI_BASIC_SESSION_HPP_
