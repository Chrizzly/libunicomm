///////////////////////////////////////////////////////////////////////////////
// event_dispatcher.hpp
//
// Copyright (c) 2009 Igor Zimenkov, Dmitry Timoshenko.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
// 
// smart++ Multipurpose C++ Library.

/** @file event_dispatcher.hpp Event dispatcher implementation. 
 *  Based on boost::thread.
 *  
 *  Maps event to handler, and executes attached handler by
 *  the internal thread. Just an event dispatcher.
 */

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef SMART_EVENT_DISPATHCER_HPP_
#define SMART_EVENT_DISPATHCER_HPP_

#include <smart/event.hpp>
#include <smart/utils.hpp>

#include <smart/config/auto_sence.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (push)
# pragma warning (disable : 4244) //  warning C4244: 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
# pragma warning (disable : 4267) //  warning C4267: 'initializing' : conversion from 'size_t' to 'unsigned int', possible loss of data
#endif // SMART_VISUAL_CPP

#include <boost/thread/thread.hpp>

#ifdef SMART_VISUAL_CPP
# pragma warning (pop)
#endif // SMART_VISUAL_CPP

#include <boost/thread/locks.hpp>
//#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>

#include <queue>
#include <map>

/** @namespace smart Library root namespace. */
namespace smart
{

/** Event dispatcher.
 *
 *  Provides event dispatching to assigned handlers. 
 *  Boost thread wrapper.
 *
 *  @tparam EventT Type that objects of are stored by the 
 *    events dispatcher queue.
 *
 *  @tparam QueueT Internal events dispatcher's queue that 
 *    holds events.
 */
template <typename EventT = event<>, typename QueueT = std::queue<EventT> >
class event_dispatcher
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Event type. */
  typedef EventT event_type;

  /** Queue type. */
  typedef QueueT queue_type;

  /** Event traits type. */
  typedef event_traits<EventT> event_traits_type;

  /** Event handler type. 
   *
   *  @param signaled event.
   *  @return true means that event is handled and 
   *  there is no need to call default handler,
   *  false instead indicates to call default handler.
   */
  typedef boost::function<bool (const event_type&)> event_handler_type;

  /** Exception handler type. */
  typedef boost::function<void (void)> exception_handler_type;

public:
  //////////////////////////////////////////////////////////////////////////
  // predefined events initializers
  /** Exit event code. */
  static const typename event_traits_type::event_code_type& exit_event_code(void);       //= 0; 
  /** Timeout event code. */
  static const typename event_traits_type::event_code_type& timeout_event_code(void);    //= 1; 
  /** This value starts user events. */
  static const typename event_traits_type::event_code_type& user_event_base_code(void);  //= 2; 

  //////////////////////////////////////////////////////////////////////////
  // predefined events 
  static /*const*/ event_type& exit_event(void);      /**< Exit event. */
  static /*const*/ event_type& timeout_event(void);   /**< Timeout event. */
  static const boost::posix_time::time_duration& infinite_timeout(void); /**< Infinite timeout value. */

public:
  /** Creates event dispatcher object. 
   *
   *  Starts processing events if auto_start is true.
   *  @param wait_on_stop Specifies how long working thread to be waited
   *  by the stop caller thread.
   *  @param auto_start True causes event_dispatcher::start() is called.
   *  @note Stop is also called from event_dispatcher::~event_dispatcher.
   */
  explicit event_dispatcher(const boost::posix_time::time_duration& wait_on_stop = boost::posix_time::seconds(15),
    bool auto_start = false): 
      _wait_on_stop(wait_on_stop), 
      _timeout(infinite_timeout()), 
      _working(false)
  { 
    if (auto_start) 
    { 
      start(); 
    } 
  }

  /** Destructs an event_dispatcher object. Also if started calls event_dispatcher::stop(). */
  virtual ~event_dispatcher(void) { if (started()) { stop(); } } 

public:
  /** Sets an event with specified code into signaled state.
   *  @param event Event to be set into signaled state.
   */
  void signal_event(const event_type& event) 
  {
    signal_event(event, true);
  }

  /** Sets event handler.
   *
   *  Thread safe. To make it possible to easily change the
   *  event type use smart::code() instead of using event_type::code()
   *  in case of user event type.
   *
   *  @param event Event's code which handler to be called for.
   *  @param handler Event handler routine callback.
   *  @see smart::code().
   */
  void set_handler(const typename event_traits_type::event_code_type& event_code, const event_handler_type& handler)
  {
    boost::mutex::scoped_lock lock(_handlers_mutex);

    if (handler)
    {
      _handlers[event_code] = handler;
    } else
    {
      clear_handler(event_code);
    }
  }

  /** Unsubscribes from event.
   *
   *  This is thread safe function.
   *  
   *  @param event Event to unsubscribe from. 
   */
  void clear_handler(const typename event_traits_type::event_code_type& event_code)
  {  
    boost::mutex::scoped_lock lock(_handlers_mutex);

    _handlers.erase(event_code); 
  }

  /** Sets exception handler.
   *
   *  Thread safe.
   *  @param handler Event handler routine callback.
   *  @note this handler is called from virtual @code event_dispatcher::exception_handler(void) @endcode
   */
  void set_exception_handler(const exception_handler_type& handler)
  {
    set(_exc_handler, handler, _handlers_mutex);
  }

  /** Clears exception handler.
   *
   *  Thread safe.
   */
  void clear_exception_handler(void)
  {
    set(_exc_handler, exception_handler_type(0), _handlers_mutex);
  }

public:
  /** Starts events processing. 
   *
   *  Creates internal thread if not started yet.
   */
  void start(void)
  {
    BOOST_ASSERT(!started() && " - is already started, stop first");

    if (!started())
    {
      boost::mutex::scoped_lock lock(_stop_mutex);

      _thread.reset(new boost::thread(&event_dispatcher::thread_entry, this));
    }
  }

  /** Stops events processing. 
   *
   *  Signals exit_event and destroys thread object. If thread is busy it will not 
   *  peek the event up and process it until it frees. So, to reduce reaction to an event
   *  do not occupy the thread for long time tasks. 
   *  @code event_dispatcher::stop @endcode is equivalent to signal_event(exit_event) call. 
   *  You may use that way you like more.
   *
   *  Stop can also be called by working thread (from any event handler). 
   *  In this case exit_event is pushed into queue and no waiting for exit issued.
   *  Further handling depends on exit_event priority and other events priorities.
   *  So, if events priorities are equal every event is pushed out preserving chronological
   *  order. In other cases the event which has highest priority among events 
   *  currently contained by the queue is pushed out. So, changing events priority
   *  gives flexibility in events handling order and in overall behavior.
   *  
   *  For instance: Assigning exit_event highest priority will interrupt 
   *  any processing when you call stop. If the priority of the exit_event is equal to 
   *  other events priorities, any contained events will be anyway processed first and 
   *  stop will occur after that. If exit_event has lowest priority, it will never
   *  handled if there are any other events in the queue. Sometimes it may causes 
   *  that working thread will never stop.
   *
   *  It's quite safe to call stop from different threads and from dispatchers internal 
   *  thread too i.e. from any event handler. 
   *
   *  @param wait_on_stop Specifies how long working thread to be waited
   *  by the stop caller thread.
   *
   *  @note Event dispatcher's destructor calls stop. So if stop called from destructor
   *  returns false it's possible to get undefined behavior.
   *
   *  @return Returns true if working thread completed before wait_on_stop timeout elapses
   *  or dispatcher's thread is NOT started. Returns false, if timeout is elapsed 
   *  and thread is not finished yet.
   * 
   *  @see event_dispatcher::event_dispatcher(...)
   *
   *  @note Priorities only take place if you use container that considers elements 
   *  priorities.
   */
  bool stop(const boost::posix_time::time_duration& wait_on_stop) 
  {
    bool ok = !started();

    if (is_own_thread())
    {
      boost::mutex::scoped_try_lock lock(_stop_mutex);

      if (lock)
      {
        ok = try_stop(wait_on_stop);
      }
    } else
    {
      boost::mutex::scoped_lock lock(_stop_mutex);
      ok = try_stop(wait_on_stop);
    }
    
    return ok;
  }

  /** Stops events processing. 
   *
   *  Just calls event_dispatcher::stop(wait_on_stop) and passes the value 
   *  specified by constructor.
   *
   *  @see event_dispatcher::stop(const boost::posix_time::time_duration& wait_on_stop)
   */
  bool stop(void)
  {
    return stop(_wait_on_stop);
  }

  /** Whether thread in dispatcher loop.
   *
   *  on_started called when started is still false
   *  on_finished called when started is already false.
   */
  bool started(void) const { return _working; }

  /** Gets wait timeout.
   *
   *  When time is out predefined event (timeout_event) handler is called. 
   *  By default time out is infinite. Use infinite_timeout to indicate that you
   *  need not timeout.
   *
   *  @note This operation is thread safe.
   */
  const boost::posix_time::time_duration timeout(void) const
  { 
    return get(_timeout, _handlers_mutex);
  }

  /** Sets wait timeout.
   *
   *  When time is out predefined event (timeout_event) handler is called. 
   *  By default time out is infinite. Use infinite_timeout to indicate that you
   *  need not timeout.
   *
   *  @note This operation is thread safe.
   */
  void timeout(const boost::posix_time::time_duration& tout)
  { 
    set(_timeout, tout, _handlers_mutex);
  }

  /** Calls boost::thread::interrupt(). */
  void interrupt_thread(void)
  {
    boost::mutex::scoped_lock lock(_stop_mutex);

    _thread->interrupt();
  }

protected:
  /** Called if there is no registered handler found.
   *
   *  By default processes exit event only.
   */
  virtual void default_event_handler(const event_type& event) { if (is_exit_event(event)) { _working = false; } }

  /** Called by working thread and implements basic functionality. */
  virtual void dispatch_events(void) { dispatcher_loop(); }

  /** Exception handler. 
   *
   *  Called if an exception is risen by event handler.
   */
  virtual void exception_handler(void) { call_exc_handler(); }

  /** Called by working thread first of all. 
   *
   *  Called before main events dispatcher loop.
   */
  virtual void on_started(void) { /* empty */ }

  /** Called by working thread when thread function execution finishes. 
   *
   *  Called after main events dispatcher loop.
   */
  virtual void on_finished(void) { /* empty */ }

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  typedef std::map<typename event_traits_type::event_code_type, event_handler_type> handlers_collection_type;
  typedef queue_type events_collection_type;

private:
  //////////////////////////////////////////////////////////////////////////
  // queue private interface
  void push_event(const event_type& event) /*const*/ { _events.push(event); }
  //event_type& top_event(void) { return _events.front(); }
  const event_type& top_event(void) const { return _events.front(); }
  event_type pop_event(void) { event_type ev = top_event(); _events.pop(); return ev; }
  typename events_collection_type::size_type events_count(void) const { return _events.size(); }

private:
  /** Thread main */
  void thread_entry(void)
  {
    try
    {
      on_started();

      dispatch_events();

      on_finished();
    } 
    catch (...)
    {
      try
      {
        exception_handler();
      } catch (...)
      {
        BOOST_ASSERT(!"Exception handler has risen an exception");
      }
    }
  }

  /** Sets an event with specified code into signaled state.
   *  @param event Event to be set into signaled state.
   */
  void signal_event(const event_type& event, bool external_exit) 
  {
    if (is_exit_event(event) && external_exit)
    {
      stop();
    } else
    {
      boost::mutex::scoped_lock lock(_mutex);
      // put event into the queue
      push_event(event);
      // notify that event is set
      _condition.notify_one();
    }
  }

  //---------------------------------------------------------------------------
  bool try_stop(const boost::posix_time::time_duration& wait_on_stop)
  {
    bool ok = !started();

    if (started())
    {
      signal_event(exit_event(), false);

      if (started() && !is_own_thread())
      {
        // wait for thread execution to be completed
        if ((ok = _thread->timed_join(wait_on_stop)) != 0)
        {
          // delete thread object, if stop is called by another thread
          // instead do nothing
          _thread.reset();
        }
      }
    }

    return ok;
  }

  //---------------------------------------------------------------------------
  bool is_exit_event(const event_type& event) const { return code(event) == exit_event_code(); }

  //---------------------------------------------------------------------------
  bool is_own_thread(void) const { return _thread->get_id() == boost::this_thread::get_id(); }

  //---------------------------------------------------------------------------
  bool are_there_events(void) const { return events_count() != 0; }

  //---------------------------------------------------------------------------
  void call_handler(const event_type& event)
  {
    event_handler_type handler = 0;

    { // lock
      boost::mutex::scoped_lock lock(_handlers_mutex);

      typename handlers_collection_type::const_iterator cit = _handlers.find(code(event));
      if (cit != _handlers.end())
      {
        handler = cit->second;
        BOOST_ASSERT(handler && " - Handler is found it can not be null");
      }
    } // lock

    if (handler)
    {
      if (!handler(event))
      {
        // call default handler if necessary
        default_event_handler(event);
      }
    } else
    {
      // call default handler if necessary
      default_event_handler(event);
    }
  }

  //---------------------------------------------------------------------------
  void call_exc_handler(void)
  {
    if (_exc_handler) { _exc_handler(); }
  }

  //---------------------------------------------------------------------------
  void dispatcher_loop(void)
  {
    for (_working = true; _working; )
    {
      try
      {
        dispatcher_loop_iteration();
      } 
      catch (...)
      {
        try
        {
          exception_handler();
        } catch (...)
        {
          BOOST_ASSERT(!"Exception handler has risen an exception");
        }
      }
    }
  }

  //---------------------------------------------------------------------------
  bool is_infinite_timeout(const boost::posix_time::time_duration& tout)
  {
    return tout == infinite_timeout();
  }

  //---------------------------------------------------------------------------
  void dispatcher_loop_iteration(void)
  {
    boost::mutex::scoped_lock lock(_mutex);

    if (is_infinite_timeout(timeout()))
    {
      _condition.wait(lock, boost::bind(&event_dispatcher::are_there_events, this));
    } else
    {
      if (!_condition.timed_wait(lock, timeout(), boost::bind(&event_dispatcher::are_there_events, this)))
      {
        push_event(timeout_event());
      }
    }

    BOOST_ASSERT(are_there_events() && " - events count can not be zero");

    // make a copy of event to avoid synchronization
    const event_type event = pop_event();
    // unlock mutex
    lock.unlock();
    // call handler
    call_handler(event);
  }

private:
  boost::posix_time::time_duration _wait_on_stop;
  boost::mutex _mutex;
  //boost::recursive_mutex _stop_mutex;
  boost::mutex _stop_mutex;
  //mutable boost::recursive_mutex _handlers_mutex;
  mutable boost::mutex _handlers_mutex;
  boost::condition _condition;
  boost::shared_ptr<boost::thread> _thread;
  boost::posix_time::time_duration _timeout;
  events_collection_type _events;
  handlers_collection_type _handlers;
  exception_handler_type _exc_handler;
  volatile bool _working;
}; // class event_dispatcher

//////////////////////////////////////////////////////////////////////////
// predefined event codes

/** Exit event code. */
template <typename EventT, typename QueueT>
const typename event_traits<EventT>::event_code_type& event_dispatcher<EventT, QueueT>::exit_event_code(void)
{
  typedef typename event_traits<EventT>::event_code_type event_code_type;

  return smart::exit_event_code<event_code_type>();
}

/** Timeout event code. */
template <typename EventT, typename QueueT>
const typename event_traits<EventT>::event_code_type& event_dispatcher<EventT, QueueT>::timeout_event_code(void)
{
  typedef typename event_traits<EventT>::event_code_type event_code_type;

  return smart::timeout_event_code<event_code_type>();
}
/** This value starts user events. */
template <typename EventT, typename QueueT>
const typename event_traits<EventT>::event_code_type& event_dispatcher<EventT, QueueT>::user_event_base_code(void)
{
  typedef typename event_traits<EventT>::event_code_type event_code_type;

  return smart::user_event_base_code<event_code_type>();
}

//////////////////////////////////////////////////////////////////////////
// basic events

/** Exit event. */
template <typename EventT, typename QueueT>
EventT& smart::event_dispatcher<EventT, QueueT>::exit_event(void)
{
  static EventT event(exit_event_code()); return event;
}

/** Timeout event. */
template <typename EventT, typename QueueT>
EventT& smart::event_dispatcher<EventT, QueueT>::timeout_event(void)
{
  static EventT event(timeout_event_code()); return event;
}

/** Infinite timeout value */
template <typename EventT, typename QueueT>
const boost::posix_time::time_duration& smart::event_dispatcher<EventT, QueueT>::infinite_timeout(void)
{
  static const boost::posix_time::time_duration inf_timeout = boost::posix_time::pos_infin; return inf_timeout;
}

} // namespace smart

#endif // SMART_EVENT_DISPATHCER_HPP_

