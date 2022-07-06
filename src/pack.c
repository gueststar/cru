/*
  cru -- co-recursion utilities

  copyright (c) 2022 Dennis Furey

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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "edges.h"
#include "errs.h"
#include "nodes.h"
#include "pack.h"
#include "table.h"
#include "wrap.h"

// More packets might need to be allocated for an application to back
// out of a heap overflow if it happens when the graph is scattered
// among multiple workers with no common thread. To avoid deadlock,
// some packets are held in reserve. The reserves are checked and
// replenished in advance at each public API entry point. Any positive
// value of RESERVE_PACKET_LIMIT ensures that freeing a packet will
// make a packet available for the next packet allocation regardless
// of any other memory fragmentation issues. Higher values up to a
// point may be conducive to better performance when memory is
// constrained by enabling workers to spin less.

// maximum number of usable extra packets if the heap overflows
#define RESERVE_PACKET_LIMIT 0x100

// storage for extra packets if the heap overflows
static packet_list reserve_packets = NULL;

// the number of packets in reserve
static uintptr_t packet_count = 0;

// for exclusive access to the reserve packets and the packet count
static pthread_rwlock_t packet_lock;


// --------------- initialization and teardown -------------------------------------------------------------



int
_cru_open_packets (err)
	  int *err;

	  // Initialize pthread resources.
{
  return ! (pthread_rwlock_init (&packet_lock, NULL) ? IER(1238) : 0);
}







void
_cru_close_packets ()

	  // Release pthread resources.
{
  packet_list t;
  int err;

  for (err = 0; (t = reserve_packets); _cru_free (t))
	 {
		_cru_discount (t->seen_carriers);
		reserve_packets = t->next_packet;
		err = (packet_count-- ? err : THE_IER(1239));
	 }
  if (packet_count ? (! err) : 0)
	 err = THE_IER(1240);
  if (pthread_rwlock_destroy (&packet_lock) ? (! err) : 0)
	 err = THE_IER(1241);
  _cru_globally_throw (err);
}






// --------------- memory management -----------------------------------------------------------------------









static packet_list
new_packet (err)
	  int *err;

	  // Allocate a packet from the heap or the reserve list if
	  // possible. Raise an out-of-memory error if it's necessary to
	  // dip into the reserves regardless of whether they're exhausted.
{
  packet_list l;

  if ((l = (packet_list) _cru_malloc (sizeof (*l))) ? 1 : pthread_rwlock_wrlock (&packet_lock) ? IER(1242) : 0)
	 goto a;
  l = (((! reserve_packets) ? 1 : packet_count-- ? 0 : IER(1243)) ? NULL : _cru_popped_packet (&reserve_packets, err));
  if (pthread_rwlock_unlock (&packet_lock) ? IER(1244) : 1)
	 RAISE(ENOMEM);
 a: return l;
}








packet_list
_cru_packet_of (p, h, s, e, err)
	  void *p;
	  uintptr_t h;
	  node_list s;
	  edge_list e;
	  int *err;

	  // Create a unit packet list.
{
  packet_list l;

  if (! (l = new_packet (err)))
	 return NULL;
  memset (l, 0, sizeof (*l));
  l->hash_value = h;
  l->payload = p;
  if ((l->carrier = e))
	 l->receiver = e->remote.node;
  l->sender = s;
  return l;
}








packet_list
_cru_initial_packet_of (p, h, err)
	  void *p;
	  uintptr_t h;
	  int *err;

	  // Create a unit packet list determined by the payload.
{
  int dblx;
  packet_list l;

  dblx = 0;
  if ((l = _cru_packet_of (p, h, NO_SENDER, NO_CARRIER, &dblx)))
	 l->initial = 1;
  RAISE(dblx);
  return l;
}







void
_cru_nack (s, err)
	  packet_list s;
	  int *err;

	  // Reclaim a packet list.
{
  _cru_free_packets (s, NO_VERTEX_DESTRUCTOR, err);
}







void
_cru_free_packets (p, v, err)
	  packet_list p;
	  cru_destructor v;
	  int *err;

	  // Tear down a packet list and its payloads. If the reserve
	  // packets are running low, put the excess packets directly into
	  // the reserve packet list instead of freeing them. The latter
	  // event should be rare if the reserve packet limit is defined
	  // appropriately, but is relatively more expensive due to the
	  // need for a write lock on the packet count.
{
  packet_list t;
  int ok;             // non-zero means there are adequately many reserve packets
  int ux;

  while ((t = p))
	 {
		if (v ? p->payload : NULL)
		  APPLY(v, p->payload);
		_cru_discount (p->seen_carriers);
		p = p->next_packet;
		if (pthread_rwlock_rdlock (&packet_lock) ? IER(1245) : 0)
		  goto a;
		ok = (packet_count >= RESERVE_PACKET_LIMIT);
		if (pthread_rwlock_unlock (&packet_lock) ? IER(1246) : ok ? 1 : pthread_rwlock_wrlock (&packet_lock) ? IER(1247) : 0)
		  goto a;
		memset (t, 0, sizeof (*t));
		t->next_packet = reserve_packets;
		reserve_packets = t;
		packet_count++;
		if (pthread_rwlock_unlock (&packet_lock) ? IER(1248) : 1)
		  continue;
	 a: _cru_free (t);
	 }
}







uintptr_t
_cru_reserved_packets (err)
	  int *err;

	  // Return the current number of packets held in reserve.
{
  uintptr_t result;

  if (pthread_rwlock_rdlock (&packet_lock) ? IER(1249) : 0)
	 return 0;
  result = packet_count;
  if (pthread_rwlock_unlock (&packet_lock))
	 IER(1250);
  return result;
}






void
_cru_replenish_packets (err)
	  int *err;

	  // Create more reserve packets if necessary.
{
  packet_list l;
  int dblx;

  if (pthread_rwlock_wrlock (&packet_lock) ? IER(1251) : 0)
	 return;
  for (dblx = 0; dblx ? 0 : (packet_count < RESERVE_PACKET_LIMIT);)
	 if ((l = (packet_list) _cru_malloc (sizeof (*l))) ? 1 : ! (dblx = ENOMEM))
		{
		  memset (l, 0, sizeof (*l));
		  _cru_push_packet (l, &reserve_packets, &dblx);
		  packet_count++;
		}
  RAISE(dblx);
  if (pthread_rwlock_unlock (&packet_lock))
	 IER(1252);
}










// --------------- editing ---------------------------------------------------------------------------------





edge_list
_cru_carriers (p)
	  packet_list p;

	  // Destructively list all carriers in a packet list by
	  // contatenating them in place and returning the first one.
{
  edge_list e;

  for (e = NULL; p; p = p->next_packet)
	 e = _cru_cat_edges (p->carrier, e);
  return e;
}










node_list
_cru_receivers (p, err)
	  packet_list p;
	  int *err;

	  // Consume a packet list and return its receivers.
{
  node_list n;

  for (n = NULL; p; _cru_nack (_cru_popped_packet (&p, err), err))
	 n = _cru_cat_nodes (p->receiver, n);
  return n;
}







int
_cru_push_packet (h, t, err)
	  packet_list h;
	  packet_list *t;
	  int *err;

	  // Concatenate a unit packet list with another packet list.
{
  if ((! h) ? 1 : h->next_packet ? IER(1253) : (! t) ? IER(1254) : 0)
	 return 0;
  h->next_packet = *t;
  *t = h;
  return 1;
}







packet_list
_cru_popped_packet (p, err)
	  packet_list *p;
	  int *err;

	  // Take the first packet from a list.
{
  packet_list t;

  if ((! p) ? IER(1255) : (! *p) ? IER(1256) : 0)
	 return NULL;
  t = *p;
  *p = (*p)->next_packet;
  t->next_packet = NULL;
  return t;
}






int
_cru_buffered (b, i, d, err)
	  packet_list b;
	  packet_list i;
	  packet_list *d;
	  int *err;

	  // Return non-zero if an incoming payload can be stored in that
	  // of an unoccupied buffer, and if so, push the buffer to a list
	  // of deferred packets. This happens when not all of the outgoing
	  // edges from an incoming packet allow a packet to be scattered
	  // in their direction because there isn't enough memory to
	  // allocate a packet for each one.
{
  if ((! b) ? IER(1257) : (! i) ? IER(1258) : b->payload ? 1 : ! ! (b->receiver))
	 return 0;
  b->payload = i->payload;
  b->receiver = i->receiver;
  b->hash_value = i->hash_value;
  i->payload = NULL;
  i->receiver = NULL;
  _cru_push_packet (b, d, err);
  return 1;
}








int
_cru_spun (b, i, d, err)
	  packet_list b;
	  packet_list *i;
	  packet_list *d;
	  int *err;

	  // If the incoming packet points to the buffer, push it and
	  // return non-zero. This happens when there aren't enough packets
	  // to scatter one along each outgoing edge, and the incoming
	  // packet can't be buffered nor the outgoing edges from its
	  // payload transplanted because they already are, so the packet
	  // has to be deferred until after others are freed.
{
  if ((! i) ? IER(1259) : (*i != b))
	 return 0;
  _cru_push_packet (_cru_popped_packet (i, err), d, err);
  return 1;
}







int
_cru_unbuffered (b, i, err)
	  packet_list b;
	  packet_list *i;
	  int *err;

	  // Return non-zero if an incoming packet points to a buffer, and
	  // if so, clear the buffer and pop the packet. This happens when
	  // messages have been successfully scattered along all outgoing
	  // edges associated with a previously buffered packet.
{
  if ((! i) ? IER(1260) : (! b) ? IER(1261) : (b != *i) ? 1 : _cru_popped_packet (i, err) ? 0 : IER(1262))
	 return 0;
  memset (b, 0, sizeof (*b));
  return 1;
}
