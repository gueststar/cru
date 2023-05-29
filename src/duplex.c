/*
  cru -- co-recursion utilities

  copyright (c) 2022-2023 Dennis Furey

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

#include <string.h>
#include <strings.h>
#include <errno.h>
#include "duplex.h"
#include "edges.h"
#include "errs.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "queue.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "stretch.h"
#include "table.h"
#include "wrap.h"

#define EQUAL_CARRIERS (cru_bpred) _cru_equality_bpred




// --------------- edge creation ---------------------------------------------------------------------------



void *
_cru_full_duplexing_task (source, err)
	  port source;
	  int *err;

	  // Insert back edges. This function cast to a task is used to
	  // create a router passed to _cru_launched in entry.c and filters.c.
	  // See route.c and mutate.c.
{
  packet_pod destinations;    // outgoing packets
  packet_list incoming;       // incoming packets
  unsigned sample;
  intptr_t status;
  node_set seen;              // previously received packet payloads
  node_list n;
  edge_list e;
  int killed;
  router r;

  seen = NULL;
  killed = 0;
  sample = 0;
  if ((! source) ? IER(826) : (source->gruntled != PORT_MAGIC) ? IER(827) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(828) : (r->valid != ROUTER_MAGIC) ? IER(829) : 0)
	 return NULL;
  if (((destinations = source->peers)) ? 0 : IER(830))
	 return _cru_abort_status (source, destinations, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 {
		KILL_SITE(10);
		killed = (killed ? 1 : KILLED);
		if (killed ? 1 : (n = (node_list) incoming->payload) ? 0 : IER(831))
		  goto a;
		if (_cru_test_and_set_membership (n, &seen, err))
		  goto b;
		if (*err)
		  goto a;
		_cru_scatter_out (n, destinations, err);
		_cru_free_edges (n->edges_in, err);
		n->edges_in = NULL;
	 b: if (*err ? 0 : ((!(incoming->carrier)) == !(incoming->sender)) ? (! ! (incoming->carrier)) : ! IER(832))
		  if ((e = _cru_edge (NO_SIG, incoming->carrier->label, NO_VERTEX, incoming->sender, NO_NEXT_EDGE, err)))
			 _cru_push_edge (e, &(n->edges_in), err);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  status = *err;
  return (void *) status;
}




// --------------- edge removal ----------------------------------------------------------------------------




void *
_cru_half_duplexing_task (source, err)
	  port source;
	  int *err;

	  // Delete the back edges from incoming nodes and send adjacent
	  // nodes to other workers.
{
  packet_pod destinations;    // outgoing packets
  packet_list incoming;       // incoming packets
  unsigned sample;
  intptr_t status;
  node_set seen;              // previously received packet payloads
  node_list n;
  int killed;
  router r;

  seen = NULL;
  if ((! source) ? IER(833) : (source->gruntled != PORT_MAGIC) ? IER(834) : (killed = 0))
	 return NULL;
  if ((! (r = source->local)) ? IER(835) : (r->valid != ROUTER_MAGIC) ? IER(836) : (int) (sample = 0))
	 return NULL;
  if (((destinations = source->peers)) ? 0 : IER(837))
	 return _cru_abort_status (source, destinations, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 {
		KILL_SITE(11);
		killed = (killed ? 1 : KILLED);
		if ((! (n = (node_list) incoming->payload)) ? IER(838) : _cru_test_and_set_membership (n, &seen, err))
		  goto a;
		if (killed ? 0 : ! *err)
		  _cru_scatter_out (n, destinations, err);
		_cru_free_edges (n->edges_in, err);
		n->edges_in = NULL;
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  status = *err;
  return (void *) status;
}






// --------------- deduplication ---------------------------------------------------------------------------






static void *
populating_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively distribute a portion of the nodes in a graph to
	  // each port according to their vertex hashes in advance so that
	  // workers in the next pass can selectively delete them without
	  // locking.
{
#define NOT_INITIAL 0
#define RECORDED(x) (_cru_member (x, seen) ? 1 : (! *err) ? 0 : _cru_listed (x, source->survivors))

  router r;
  cru_hash h;
  node_list n;
  packet_pod d;
  node_set seen;
  intptr_t status;
  packet_list incoming;
  struct packet_list_s buffer;          // needed if only if there's not enough memory to allocate a packet

  d = NULL;
  seen = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! source) ? IER(839) : (source->gruntled != PORT_MAGIC) ? IER(840) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(841) : (r->valid != ROUTER_MAGIC) ? IER(842) : 0)
	 return NULL;
  if ((! (d = source->peers)) ? IER(843) : 0)
	 return _cru_abort_status (source, d, err);
  h = r->ro_sig.orders.v_order.hash;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		if ((n = incoming->receiver) ? RECORDED(n) : IER(844))
		  goto a;
		if (*err ? 0 : _cru_scattered_by_hashes (n, h, d, BY_TERMINUS, err))
		  goto b;
		if (! _cru_unscatterable_by_hashes (n, h, r->ro_sig.destructors.e_free, d, NOT_INITIAL, err))
		  goto b;
		if (_cru_buffered (&buffer, incoming, &(d->deferrals), err))
		  goto c;
		if (_cru_transplanted_out (buffer.receiver, n, err))
		  goto b;
		if (! _cru_spun (&buffer, &incoming, &(d->deferrals), err))
		  goto c;
		continue;
	 b: if ((incoming != &buffer) ? (n == buffer.receiver) : 0)
		  goto c;
		_cru_set_membership (_cru_half_severed (n), &seen, err);
		_cru_pushed_node (n, &(source->survivors), err);
	 a: if (_cru_unbuffered (&buffer, &incoming, err))
		  continue;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  status = *err;
  return (void *) status;
}








static void
graft (i, c, s, d, e, err)
	  packet_list i;               // incoming packet
	  packet_list c;               // packet containing a previously stored node
	  cru_sig s;
	  node_list *d;
	  edge_list *e;
	  int *err;

	  // Record the carriers on an incoming packet in a previously
	  // stored packet holding an equal or equivalent node. If the
	  // incoming node is equivalent but not equal, discard it,
	  // associate its outgoing edges with the previously stored node,
	  // and point the outgoing edges from its predecessor nodes to the
	  // previously stored node.
{
  if ((! i) ? IER(845) : (! (i->carrier)) ? IER(846) : (! (i->receiver)) ? IER(847) : (! e) ? IER(848) : 0)
	 return;
  if ((! c) ? IER(849) : (! (c->receiver)) ? IER(850) : (i == c) ? IER(851) : s ? 0 : IER(852))
	 return;
  _cru_record_edge (_cru_scalar_hash (i->carrier), EQUAL_CARRIERS, (void *) i->carrier, &(c->seen_carriers), err);
  if (c->receiver == i->receiver)
	 return;
  _cru_redirect (i->seen_carriers, c->receiver, err);
  c->receiver->edges_out = _cru_cat_edges (i->receiver->edges_out, c->receiver->edges_out);
  i->receiver->edges_out = NULL;
  *e = _cru_cat_edges (_cru_repeating_labels (&(s->orders.e_order), c->receiver->edges_out, err), *e);
  i->carrier->remote.node = c->receiver;
  _cru_record_edge (_cru_scalar_hash (i->carrier), EQUAL_CARRIERS, (void *) i->carrier, &(c->seen_carriers), err);
  _cru_pushed_node (_cru_severed (i->receiver, err), d, err);
  i->receiver = NULL;
}








static void *
deduplicating_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively deduplicate vertices and edges. Read nodes from
	  // the receiver fields of incoming packets and store their
	  // vertices in a hash table. The first time a node is received,
	  // deduplicate its outgoing edges and send packets to its
	  // adjacent nodes. Ignore subsequent receipt of identical nodes,
	  // but fuse distinct nodes whose vertices match those previously
	  // stored in the hash table.
{
#define UNEQUAL(a,b) \
(*err ? 0 : (! b) ? (! IER(853)) : (a == b->vertex) ? 0 : FAILED(s->orders.v_order.equal, a, b->vertex))

  packet_table collisions;    // previous incoming packets
  packet_list incoming;
  int ux, ut, unequal;
  unsigned sample;
  intptr_t status;
  packet_list *c;
  packet_list *p;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  cru_sig s;
  router r;

  sample = 0;
  collisions = NULL;
  if ((! source) ? IER(854) : (source->gruntled != PORT_MAGIC) ? IER(855) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(856) : (r->valid != ROUTER_MAGIC) ? IER(857) : (killed = 0))
	 goto a;
  if (((d = source->peers)) ? ((r->ro_sig.orders.v_order.equal) ? 0 : IER(858)) : IER(859))
	 return _cru_abort_status (source, d, err);
  s = &(r->ro_sig);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(12);
		killed = (killed ? 1 : KILLED);
		if (killed ? 1 : *err ? 1 : (! (n = incoming->receiver)) ? IER(860) : 0)
		  goto b;
		if (! (c = _cru_collision (incoming->hash_value, &collisions, err)))
		  goto b;
		if (*c)
		  while ((unequal = UNEQUAL(n->vertex, (*c)->receiver)) ? (*c)->next_packet : NULL)
			 c = &((*c)->next_packet);
		if (*err)
		  goto b;
		if (*c ? (n == (*c)->receiver) : 0)
		  goto d;
		n->edges_out = _cru_deduplicated_edges (n->edges_out, &(s->orders.e_order), s->destructors.e_free, err);
		if (! _cru_scattered_by_hashes (n, s->orders.v_order.hash, d, BY_TERMINUS, err))
		  goto b;
	 d: if (*c ? (! unequal) : 0)
		  {
			 graft (incoming, *c, s, &(source->deletions), &(source->disconnections), err);
			 goto c;
		  }
		if (_cru_push_packet (_cru_popped_packet (&incoming, err), p = (*c ? &((*c)->next_packet) : c), err) ? 1 : ! IER(861))
		  if (*p ? 1 : ! IER(862))
			 {
				_cru_record_edge (_cru_scalar_hash ((*p)->carrier), EQUAL_CARRIERS, (*p)->carrier, &((*p)->seen_carriers), err);
				continue;
			 }
	 b: _cru_nack (_cru_unpacked (collisions), err);
		collisions = NULL;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
 	 }
  _cru_forget_collisions (collisions, err);
  status = *err;
  a: return (void *) status;
}








cru_graph
_cru_deduplicated (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Launch a deduplication operation with a previously prepared
	  // router and block until finished.
{
  crew c;
  task t;
  router z;
  cru_hash h;
  node_list b;
  uintptr_t q;
  packet_list p;

  _cru_disable_killing (k, err);
  if (*err ? 1 : (! g) ? 1 : (! (b = g->base_node)) ? IER(863) : 0)
	 goto a;
  if ((! r) ? IER(864) : (r->valid != ROUTER_MAGIC) ? IER(865) : 0)
	 goto a;
  if ((! (r->ports)) ? IER(866) : ((h = r->ro_sig.orders.v_order.hash)) ? 0 : IER(867))
	 goto a;
  if (_cru_half_duplex (g, err))
	 goto b;
  z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err);
  if (! _cru_launched (k, b, z, err))
	 goto a;
 b: for (t = (task) populating_task; t; t = ((t == (task) populating_task) ? deduplicating_task : NULL))
	 {
		if (! _cru_status_launched (UNKILLABLE, b, q = h (b->vertex), _cru_reset (r, t, err), err))
		  goto a;
		g->nodes = NULL;
	 }
  if (_cru_queue_launched (k, b, q, _cru_reset (r, (task) _cru_pruning_task, err), &(g->nodes), err))
	 if (g->nodes)
		g->nodes->previous = &(g->nodes);
 a: _cru_sweep (r, err);
  if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}
