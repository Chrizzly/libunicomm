///////////////////////////////////////////////////////////////////////////////
// comm_container.cpp
//
// unicomm - Unified Communication protocol C++ library.
//
// Communicator container. Provides a thread safe access 
// to communicators collection.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// 2012, (c) Dmitry Timoshenko.

#include <unicomm/comm_container.hpp>
#include <unicomm/handler_types.hpp>
#include <unicomm/message_base.hpp>
#include <unicomm/comm.hpp>
#include <unicomm/except.hpp>

#include <boost/assert.hpp>
#include <boost/bind.hpp>

#include <utility>
#include <algorithm>

using std::make_pair;

using boost::recursive_mutex;

namespace 
{

typedef std::pair<unicomm::comm_container::comm_collection_type::iterator, bool> 
  insert_result_type;

/** Message sender - for_each functor. */
struct message_sender
{
//////////////////////////////////////////////////////////////////////////
// interface
public:
  typedef void result_type;

public:
  const unicomm::full_messageid_map_type& sended(void) const { return _sended; }

public:
  result_type operator()(
    const unicomm::comm_container::comm_collection_type::value_type &commpair, 
    const unicomm::message_base& message)
  {
    _sended.insert(make_pair(commpair.first,  send(commpair, message)));
  }

  //---------------------------------------------------------------------------
  result_type operator()(
    const unicomm::comm_container::comm_collection_type::value_type &commpair, 
    const unicomm::message_base& message, 
    const unicomm::message_sent_handler_type& handler)
  {
    _sended.insert(make_pair(commpair.first, send(commpair, message, handler)));
  }

  //////////////////////////////////////////////////////////////////////////
  // private stuff
private:
  unicomm::messageid_type send(
    const unicomm::comm_container::comm_collection_type::value_type &commpair, 
    const unicomm::message_base& message)
  {
    return commpair.second->send(message);
  }

  //-----------------------------------------------------------------------------
  unicomm::messageid_type send(
    const unicomm::comm_container::comm_collection_type::value_type &commpair, 
    const unicomm::message_base& message, 
    const unicomm::message_sent_handler_type& handler)
  {
    return commpair.second->send(message, handler);
  }

private:
  //////////////////////////////////////////////////////////////////////////
  // data
  unicomm::full_messageid_map_type _sended;
};

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////
// communicator container
unicomm::comm_container::comm_ptr unicomm::comm_container::take_out(void)
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  const comm_ptr comm = clients().empty()? 
    comm_ptr(): clients().begin()->second;

  if (comm)
  {
    // erase original primary collection
    clients().erase(comm->id());

    // insert into reserved collection
    insert_client(comm, excluded_clients());
  }

  return comm;
}

////------------------------------------------------------------------------
//void unicomm::comm_container::get_back(commid_type id)
//{
//  boost::recursive_mutex::scoped_lock lock(_clients_mutex);
//
//  BOOST_ASSERT(mt_excluded_client_exists(id) && 
//    " - Requested communicator id is not found in excluded collection");
//
//  if (mt_excluded_client_exists(id))
//  {
//    // erase original primary collection
//    const comm_ptr comm = excluded_clients().find(id)->second;
//    excluded_clients().erase(id);
//
//    // insert into primary collection
//    insert_client(comm, clients());
//  }
//}

//------------------------------------------------------------------------
void unicomm::comm_container::get_back(commid_type id)
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  if (excluded_client_exists(id))
  {
    // erase original primary collection
    const comm_ptr comm = excluded_clients().find(id)->second;
    excluded_clients().erase(id);

    // insert into primary collection
    insert_client(comm, clients());
  }
}

//------------------------------------------------------------------------
void unicomm::comm_container::get_back_all(void)
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  clients().insert(excluded_clients().begin(), excluded_clients().end());

  excluded_clients().clear();
}

//------------------------------------------------------------------------
unicomm::comm_container::comm_ptr 
unicomm::comm_container::get(commid_type id) const
{
  recursive_mutex::scoped_lock lock(_clients_mutex);

  comm_collection_type::const_iterator it = clients().find(id);
  if (it == clients().end())
  {
    it = excluded_clients().find(id);
    if (it == excluded_clients().end())
    {
      throw session_not_found("Invalid client identifier has been specified");
    }
  }

  BOOST_ASSERT(it->second && " - Communicator pointer can't be null");

  return it->second;
}

//------------------------------------------------------------------------
void unicomm::comm_container::insert(const comm_ptr& comm)
{
  recursive_mutex::scoped_lock lock(_clients_mutex);

  if (!client_exists(comm->id()))
  {
    insert_client(comm, clients());
  }
}

//------------------------------------------------------------------------
unicomm::full_messageid_map_type 
unicomm::comm_container::send_all(const unicomm::message_base& message) const
{
  full_messageid_map_type primary_result  = send_all(message, clients());
  full_messageid_map_type excluded_result = send_all(message, excluded_clients());

  primary_result.insert(excluded_result.begin(), excluded_result.end());

  return primary_result;
}

//------------------------------------------------------------------------
unicomm::full_messageid_map_type 
unicomm::comm_container::send_all(const unicomm::message_base& message, 
                                  const message_sent_handler_type& handler) const
{
  full_messageid_map_type primary_result  = send_all(message, handler, clients());
  full_messageid_map_type excluded_result = send_all(message, handler, excluded_clients());

  primary_result.insert(excluded_result.begin(), excluded_result.end());

  return primary_result;
}

//------------------------------------------------------------------------
unicomm::full_messageid_map_type 
unicomm::comm_container::send_all(const message_base& message, 
                                  const comm_collection_type& clients) const
{
  message_sender ms;

  { // lock clients collection
    recursive_mutex::scoped_lock lock(_clients_mutex);

    for_each(clients.begin(), clients.end(), boost::bind(boost::ref(ms), _1, boost::ref(message)));
  }

  return ms.sended();  
}

//------------------------------------------------------------------------
unicomm::full_messageid_map_type 
unicomm::comm_container::send_all(const message_base& message, 
                                  const message_sent_handler_type& handler, 
                                  const comm_collection_type& clients) const
{
  message_sender ms;

  { // lock clients collection
    recursive_mutex::scoped_lock lock(_clients_mutex);

    for_each(clients.begin(), clients.end(), 
      boost::bind(boost::ref(ms), _1, boost::ref(message), handler));
  }

  return ms.sended();
}

//------------------------------------------------------------------------
size_t unicomm::comm_container::size(void) const
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  return clients().size() + excluded_clients().size();
}

//------------------------------------------------------------------------
bool unicomm::comm_container::empty(void) const
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  return clients().empty() && excluded_clients().empty();
}

//------------------------------------------------------------------------
void unicomm::comm_container::clear(void)
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  clients().clear();
  excluded_clients().clear();
}

//------------------------------------------------------------------------
bool unicomm::comm_container::erase(commid_type id)
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  size_t erased = clients().erase(id);
  erased |= excluded_clients().erase(id);

  return erased != 0;
}

//-----------------------------------------------------------------------------
unicomm::comm_container::comm_collection_type 
unicomm::comm_container::communicators(void) const 
{ 
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  comm_collection_type comms = excluded_clients();
  comms.insert(clients().begin(), clients().end());

  return comms;
  //std::copy(clients.begin(), clients().end(), std::inserter(comms));
}

//------------------------------------------------------------------------
bool unicomm::comm_container::exists(commid_type id) const 
{ 
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  return client_exists(id);
}

//------------------------------------------------------------------------
void unicomm::comm_container::insert_client(const comm_ptr& comm, 
                                            comm_collection_type& clients)
{
  UNICOMM_IFDEF_DEBUG(const insert_result_type ok = ) 

  clients.insert(make_pair(comm->id(), comm));

  UNICOMM_IFDEF_DEBUG(BOOST_ASSERT(ok.second && 
    " - Communicator already exists in collection"));
}

//------------------------------------------------------------------------
bool unicomm::comm_container::mt_client_exists(
  commid_type id, const comm_collection_type& clients) const
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  return client_exists(id, clients); 
}

//------------------------------------------------------------------------
bool unicomm::comm_container::client_exists(
  commid_type id, const comm_collection_type& clients) const
{
  return clients.find(id) != clients.end(); 
}

//------------------------------------------------------------------------
bool unicomm::comm_container::client_exists(commid_type id) const
{
  return client_exists(id, clients()) || client_exists(id, excluded_clients());
}

//-----------------------------------------------------------------------------
bool unicomm::comm_container::excluded_client_exists(commid_type id) const
{
  return client_exists(id, excluded_clients());
}

//-----------------------------------------------------------------------------
bool unicomm::comm_container::mt_excluded_client_exists(commid_type id) const
{
  return mt_client_exists(id, excluded_clients());
}

//-----------------------------------------------------------------------------
void unicomm::comm_container::disconnect_client_1(
  const comm_collection_type::value_type &commpair)
{
  disconnect_client(*commpair.second);
}

//-----------------------------------------------------------------------------
void unicomm::comm_container::disconnect_client(communicator& comm)
{
  comm.disconnect();
}

//-----------------------------------------------------------------------------
void unicomm::comm_container::disconnect_all(void)
{
  recursive_mutex::scoped_lock lock(_clients_mutex);

  disconnect_all(clients());
  disconnect_all(excluded_clients());
}

//-----------------------------------------------------------------------------
void unicomm::comm_container::disconnect_all(const comm_collection_type& clients)
{
  for_each(clients.begin(), clients.end(), 
    boost::bind(&comm_container::disconnect_client_1, this, _1));
}

#ifdef UNICOMM_FORK_SUPPORT

//-----------------------------------------------------------------------------
void unicomm::comm_container::fork_support_all(void)
{
  fork_support_all(clients());
  fork_support_all(excluded_clients());
} 

//-----------------------------------------------------------------------------
void unicomm::comm_container::fork_support_all(const comm_collection_type& clients)
{
  boost::recursive_mutex::scoped_lock lock(_clients_mutex);

  for_each(clients.begin(), clients.end(), 
    boost::bind(&comm_container::fork_support_1, this, _1));
}

//-----------------------------------------------------------------------------
void unicomm::comm_container::fork_support_1(
  const comm_collection_type::value_type &commpair)
{
  fork_support(*commpair.second);
}

//-----------------------------------------------------------------------------
void unicomm::comm_container::fork_support(communicator& comm)
{
  comm.fork_parent();
}

#endif // UNICOMM_FORK_SUPPORT
