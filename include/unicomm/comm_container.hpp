///////////////////////////////////////////////////////////////////////////////
// comm_container.hpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Communicator container. Provides thread safe access 
// to communicators collection.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.

#ifdef _MSC_VER
# pragma once
#endif // _MSC_VER

#ifndef UNI_COMM_CONTAINER_HPP_
#define UNI_COMM_CONTAINER_HPP_

/** @file comm_container.hpp Communicators collection. */

#include <unicomm/config/auto_link.hpp>
#include <unicomm/basic.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/handler_types.hpp>

#include <boost/thread/recursive_mutex.hpp>

/** @namespace unicomm Unicomm library root namespace. */
namespace unicomm
{

/** Communicators container. 
 *
 *  Provides thread safe access to the communicators collection.
 */
class comm_container
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  /** Smart pointer to a communicator object. */
  typedef comm_pointer_type comm_ptr;

  /** Communicators collection type. */
  typedef std::map<commid_type, comm_ptr> comm_collection_type;

  /** Communicator identifiers collection type. */
  typedef std::vector<commid_type> commid_collection_type;

public:
  /** Takes out a client from the primary collection. 
   *
   *  @return Smart pointer to the client.
   *  @see comm_ptr.
   */
  comm_ptr take_out(void);

  /** Get back the communicator to the primary collection.
   *
   *  @param id Communicator to put back into the collection.
   */
  void get_back(commid_type id);

  /** Get back all the communicators to the primary collection. */
  void get_back_all(void);

  /** Returns a pointer to the communicator with given id. 
   *
   *  @param id Communicator object identifier.
   *  @return A pointer to the communicator with given identifier.
   *  @throw session_not_found exception is thrown if requested 
   *    communicator is not found.
   */
  comm_ptr get(commid_type id) const;

  /** Inserts communicator to the collection. 
   *
   *  Does NOT overwrite existing one if there is.
   *
   *  @param comm Pointer to the communicator to be inserted to the container.
   */
  void insert(const comm_ptr& comm);

  /** Sends the message to all currently connected clients. 
   *
   *  Actually calls communicator::send() for each communicator in the 
   *  collection.
   *
   *  @param message Message to send.
   *  @return Map of the sent message identifiers. The map contains 
   *    <commid, messagid> pairs. 
   *
   *  @throw The same as unicomm::communicator::send().
   */
  full_messageid_map_type send_all(const message_base& message) const;

  /** Sends the message to all currently connected clients. 
   *
   *  Actually calls communicator::send() for each communicator in the 
   *  collection.
   *
   *  @param message Message to send.
   *  @param handler Handler to be called when the message is actually sent.
   *  @return Map of the sent message identifiers. The map contains 
   *    <commid, messagid> pairs. 
   *
   *  @throw The same as unicomm::communicator::send().
   */
  full_messageid_map_type send_all(const message_base& message, 
    const message_sent_handler_type& handler) const;

  /** Disconnects all currently connected clients if there are. */
  void disconnect_all(void);

#ifdef UNICOMM_FORK_SUPPORT

  /** Actually calls communicator::fork_parent() for each communicator. */
  void fork_support_all(void);

#endif // UNICOMM_FORK_SUPPORT

  /** Whether client with given identifier exists. 
   *
   *  @param id Communicator identifier to check.
   *  @return True if communicator is present in the collection and 
   *    false otherwise.
   */
  bool exists(commid_type id) const;

  /** Returns the size of the collection. 
   *
   *  @return Size of the collection.
   */
  size_t size(void) const;

  /** Whether collection is empty. 
   *
   *  @return True if collection is empty and false instead.
   */
  bool empty(void) const;

  /** Clears the collection. */
  void clear(void);

  /** Remove element with given identifier from the collection. 
   *
   *  @param id Communicator identifier.
   *  @return Returns true if object with the specified id existed
   *    and false if it didn't.
   */
  bool erase(commid_type id);

  /** Retrieves a copy of the communicators collection. 
   *
   *  @return Communicator objects collection.
   */
  comm_collection_type communicators(void) const;

//////////////////////////////////////////////////////////////////////////
// private stuff
private:
  void insert_client(const comm_ptr& comm, comm_collection_type& clients);
  bool client_exists(commid_type id, const comm_collection_type& clients) const;
  bool client_exists(commid_type id) const;
  bool excluded_client_exists(commid_type id) const;
  bool mt_client_exists(commid_type id, const comm_collection_type& clients) const;
  bool mt_excluded_client_exists(commid_type id) const;
  
  comm_collection_type& clients(void) { return _clients; }
  comm_collection_type& excluded_clients(void) { return _excluded_clients; }
  const comm_collection_type& clients(void) const { return _clients; }
  const comm_collection_type& excluded_clients(void) const 
    { return _excluded_clients; }

  void disconnect_client_1(const comm_collection_type::value_type &commpair);
  void disconnect_client(communicator& comm);
  void disconnect_all(const comm_collection_type& clients);

  full_messageid_map_type send_all(const message_base& message, 
    const comm_collection_type& clients) const;

  full_messageid_map_type send_all(const message_base& message, 
    const message_sent_handler_type& handler, 
    const comm_collection_type& clients) const;

#ifdef UNICOMM_FORK_SUPPORT

  void fork_support_all(const comm_collection_type& clients);
  void fork_support_1(const comm_collection_type::value_type &commpair);
  void fork_support(communicator& comm);

#endif // UNICOMM_FORK_SUPPORT

private:
  mutable boost::recursive_mutex _clients_mutex;
  comm_collection_type _clients;
  comm_collection_type _excluded_clients;
};

} // namespace unicomm

#endif // UNI_COMM_CONTAINER_HPP_

