/*
  cru -- co-recursion utilities

  copyright (c) 2022-2024 Dennis Furey

  Cru is free software: you can redistribute it and/or modify it under
  the terms of version 3 of the GNU General Public License as
  published by the Free Software Foundation.

  Cru is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with cru. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CRU_PACK_H
#define CRU_PACK_H 1

// Items in a packet list each specify a quantum of work for a worker
// thread. During the time a graph is being built, the workers store
// previously received packets in a packet table as defined in table.h
// so they can ignore repeated instances of the same packet payload.
// If the graph is being built by subconnector functions instead of
// connectors (as defined in include/cru/function_types.h), then
// repeated instances of the same packet payload are acted upon
// provided they arrive by different carriers, hence the need to store
// previously seen carriers in a multiset table as defined in table.h.

#include "ptr.h"

// default values for unspecified packet fields
#define NO_HASH (uintptr_t) 0
#define NO_SENDER NULL
#define NO_CARRIER NULL
#define NO_PAYLOAD NULL

// default argument to _cru_free_packets
#define NO_VERTEX_DESTRUCTOR NULL

struct packet_list_s
{
  int initial;                   // non-zero only for the first packet sent to start a job
  void *payload;                 // a vertex in the graph during building but otherwise usually a node
  node_list sender;              // during full duplex building, the node from whose vertex the edge originates
  edge_list carrier;             // the edge whose terminus is the receiver, if any
  node_list receiver;            // the node at the other end of the carrier edge, if any
  uintptr_t hash_value;          // used for establishing inequality between payloads
  packet_list next_packet;       // next packet to be processed after this one
  multiset_table seen_carriers;  // carriers previously associated with packets delivering the same payload
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization and teardown -------------------------------------------------------------

// initialize pthread resources
extern int
_cru_open_packets (int *err);

// release pthread resources
extern void
_cru_close_packets (void);

// --------------- memory management -----------------------------------------------------------------------

// create a unit packet list with payload p, hash h, sender s, and carrier e
extern packet_list
_cru_packet_of (void *p, uintptr_t h, node_list s, edge_list e, int *err);

// create a unit packet list with no sender or carrier
extern packet_list
_cru_initial_packet_of (void *p, uintptr_t h, int *err);

// reclaim a packet list but not its payloads
extern void
_cru_nack (packet_list s, int *err);

// reclaim a packet list and its payloads
extern void
_cru_free_packets (packet_list p, cru_destructor v, int *err);

// return the current number of packets held in reserve
extern uintptr_t
_cru_reserved_packets (int *err);

// create more reserve packets if necessary
extern void
_cru_replenish_packets (int *err);

// --------------- editing ---------------------------------------------------------------------------------

// destructively list all carriers in a packet list
extern edge_list
_cru_carriers (packet_list p);

// consume a packet list and return its receivers
extern node_list
_cru_receivers (packet_list p, int *err);

// insert a unit packet list at the front of a packet list
extern int
_cru_push_packet (packet_list h, packet_list *t, int *err);

// return a unit list of the first packet in a non-empty list and remove it from the list
extern packet_list
_cru_popped_packet (packet_list *p, int *err);

// return non-zero if an incoming payload can be stored in that of an unoccupied buffer, and if so, push the buffer
extern int
_cru_buffered (packet_list b, packet_list i, packet_list *d, int *err);

// if the incoming packet points to the buffer, push it and return non-zero
extern int
_cru_spun (packet_list b, packet_list *i, packet_list *d, int *err);

// return non-zero if an incoming packet points to a buffer, and if so, clear the buffer and pop the packet.
extern int
_cru_unbuffered (packet_list b, packet_list *i, int *err);

#ifdef __cplusplus
}
#endif
#endif
