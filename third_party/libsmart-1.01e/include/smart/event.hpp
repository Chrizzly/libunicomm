///////////////////////////////////////////////////////////////////////////////
// event.hpp
//
// Copyright (c) 2009 Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file event.hpp
 *  
 *  Event dispatcher's event class template implementation
 *  and accompanying stuff.
 */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_EVENT_HPP_
#define SMART_EVENT_HPP_

#include <smart/config/auto_sence.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) // warning C4267: 'argument' : conversion from 'size_t' to 'unsigned int', possible loss of data
#endif // SMART_VISUAL_CPP

#include <boost/lexical_cast.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

#include <limits>
#include <functional>

#include <cstddef>

#ifdef max
# undef max
#endif 

/** @namespace smart 
 *  Library root namespace.
 */
namespace smart
{

/** Default event type.
 *  
 *  By default all events has the same priority.
 *  If you need to use priorities override 
 *  event::priority to make this possible to be. 
 *  Also, there is priority_event which already implements 
 *  event::priority and holds priority value.
 * 
 *  @tparam EventCodeT event code type. 
 *  @tparam PriorityT priority type.
 */
template <typename EventCodeT = std::size_t, typename PriorityT = std::size_t>
struct event
{
//////////////////////////////////////////////////////////////////////////
// public interface
public:
  /** Event code type. */
  typedef EventCodeT event_code_type;
  /** Priority type. */
  typedef PriorityT priority_type;

public:
  /** Constructs an event with given event code. */
  explicit event(const event_code_type& ec = event_code_type()): _code(ec) { /* empty */ }
  virtual ~event(void) { /* empty */ }

public:
  /** Returns event code. */
  const event_code_type& code(void) const { return _code; }

public:
  /** Returns the event priority.
   *
   *  Initially all events have the same priority. There is
   *  no object that stores an event priority. If you need such object use
   *  prioirty_event instead or make your own event type which is either based on 
   *  this class or on something else.
   */
  virtual priority_type priority(void) const { return priority_type(); }

  /** To be implemented by derived class. */
  virtual void priority(const priority_type& /*p*/) { /* empty */ }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  /*const */event_code_type _code; // to make the assignment to be possible
};

/** Event traits. */
template <typename T>
struct event_traits 
{
  /** Event type. */
  typedef T event_type;
  /** Event code type. */
  typedef typename T::event_code_type event_code_type;
  /** Priority type. */
  typedef typename T::priority_type priority_type;
};

/** Event traits size_t specialization. */
template <>
struct event_traits<size_t>
{
  /** Event type. */
  typedef size_t event_type;
  /** Event code type. */
  typedef size_t event_code_type;
  /** Priority type. */
  typedef size_t priority_type;
};

/** Event interface being used by dispatcher. 
 *
 *  Returns an event code.
 */
template <typename EventT> 
typename event_traits<EventT>::event_code_type code(const EventT& event) 
{ 
  return event.code(); 
}

/** Returns an event code. */
inline size_t code(size_t event) { return event; }

/** Returns an event priority. */
template <typename EventT> 
typename event_traits<EventT>::priority_type priority(const EventT& event) 
{ 
  return event.priority(); 
}

/** Returns an event priority. */
inline size_t priority(size_t event) { return event; }

/** Default event initializer.
 *
 *  Used to make initialization independent from immediate initializer type.
 */
template <typename EventT, typename InitializerT>
inline typename event_traits<EventT>::event_code_type init_event(const InitializerT& value)
{
  return boost::lexical_cast<typename event_traits<EventT>::event_code_type>(value);
}

template <typename EventCodeT, typename InitializerT>
inline EventCodeT init_event_code(const InitializerT& value)
{
#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4267) // warning C4267: 'argument' : conversion from 'size_t' to 'unsigned int', possible loss of data
#endif // SMART_VISUAL_CPP

  return boost::lexical_cast<EventCodeT>(value);

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP
}

//////////////////////////////////////////////////////////////////////////
// predefined event codes

/** Exit event code. */
inline size_t exit_event_code(void) { return 0; }

/** Timeout event code. */
inline size_t timeout_event_code(void) { return 1; }

/** This value starts user events. */
inline size_t user_event_base_code(void) { return 2; }

/** Exit event code. */
template <typename EventCodeT>
const EventCodeT& exit_event_code(void)
{
  static const EventCodeT code = init_event_code<EventCodeT>(exit_event_code()); return code;
}

/** Timeout event code. */
template <typename EventCodeT>
const EventCodeT& timeout_event_code(void)
{
  static const EventCodeT code = init_event_code<EventCodeT>(timeout_event_code()); return code;
}
/** This value starts user events. */
template <typename EventCodeT>
const EventCodeT& user_event_base_code(void)
{
  static const EventCodeT code = init_event_code<EventCodeT>(user_event_base_code()); return code;
}

/** Priority event.
 *
 *  Holds priority member.
 */
template <typename EventCodeT = std::size_t, typename PriorityT = std::size_t>
struct priority_event : public event<EventCodeT, PriorityT>
{
public:
  /** Event code type. */
  typedef typename event<EventCodeT, PriorityT>::event_code_type event_code_type;
  /** Priority type. */
  typedef typename event<EventCodeT, PriorityT>::priority_type priority_type;

public:
  /** By default all events have equal priority. */
  explicit priority_event(const event_code_type& ec = event_code_type(), const priority_type& p = std::numeric_limits<priority_type>::max()): 
    event<EventCodeT, PriorityT>(ec),
    _priority(p)
  {
    // empty
  }
    
public:
  /** Returns the event priority. */
  virtual priority_type priority(void) const { return _priority; }

  /** To be implemented by derived class. */
  virtual void priority(const priority_type& p) { _priority = p; }

private:
  priority_type _priority;
};

/** Less. */
template <typename EventCodeT, typename PriorityT> 
inline bool operator<(const event<EventCodeT, PriorityT>& l, const event<EventCodeT, PriorityT>& r) 
{ 
  return l.priority() < r.priority(); 
}

/** Greater. */
template <typename EventCodeT, typename PriorityT> 
inline bool operator>(const event<EventCodeT, PriorityT>& l, const event<EventCodeT, PriorityT>& r) 
{  
  return l.priority() > r.priority();
}

/** Equal. */
template <typename EventCodeT, typename PriorityT> 
inline bool operator==(const event<EventCodeT, PriorityT>& l, const event<EventCodeT, PriorityT>& r) 
{  
  return l.priority() == r.priority();
}

/** Functional object to check whether the event is fully equal. */
template <typename EventT> 
struct is_equal : std::binary_function<EventT, EventT, bool>
{
  bool operator()(const EventT& first, const EventT& second) const
  {
    return code(first) == code(second) && priority(first) == priority(second);
  }
};

} // namespace smart

#endif // SMART_EVENT_HPP_

