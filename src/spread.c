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

#include <stdlib.h>
#include <string.h>
#include "brig.h"
#include "build.h"
#include "edges.h"
#include "errs.h"
#include "getset.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "route.h"
#include "queue.h"
#include "scatter.h"
#include "spread.h"
#include "table.h"
#include "wrap.h"

// first parameter to a subconnector function
#define INITIAL 1
#define NOT_INITIAL 0

#define UNEQUAL(a,b) ((b ? 0 : IER(1556)) ? 0 : (a == b->vertex) ? 0 : FAILED(s->orders.v_order.equal, a, b->vertex))






static void
push_severed (n, t, err)
	  node_list n;
	  node_list *t;
	  int *err;

	  // Sever and push n into t, and disconnect its incoming edges.
{
  if (n ? 0 : IER(1562))
	 return;
  _cru_pushed_node (_cru_half_severed (n), t, err);
  _cru_free_edges (n->edges_in, err);
  n->edges_in = NULL;
}






static int
packed (i, s, p, t, d, err)
	  packet_list *i;
	  cru_sig s;
	  packet_table *p;
	  node_list *t;
	  packet_pod d;
	  int *err;

	  // Store an incoming packet carrying a previously unrecorded node
	  // in the collision table and send adjacent nodes in packets to
	  // other workers. If successful, also sever and store the node
	  // carried in the packet in a separate list t.
{
  int ux, ut, unequal;
  packet_list *c;
  edge_list e;

  if (*err ? 1 : (! i) ? IER(1563) : (! *i) ? IER(1564) : (! s) ? IER(1565) : (! (s->orders.v_order.equal)) ? IER(1566) : 0)
	 goto a;
  if ((! ((*i)->receiver)) ? IER(1567) : ! (c = _cru_collision ((*i)->hash_value, p, err)))
	 goto a;
  if (*c)
	 while (*err ? 0 : (unequal = UNEQUAL((*i)->receiver->vertex, (*c)->receiver)) ? (*c)->next_packet : NULL)
		c = &((*c)->next_packet);
  if (*err)
	 goto a;
  if (*c ? (! unequal) : 0)
	 goto c;
  if (! _cru_scattered_by_hashes ((*i)->receiver, s->orders.v_order.hash, d, BY_TERMINUS, err))
	 goto a;
  push_severed ((*i)->receiver, t, err);
  _cru_push_packet (_cru_popped_packet (i, err), *c ? &((*c)->next_packet) : c, err);
  goto b;
 c: _cru_nack (_cru_popped_packet (i, err), err);
 b: return 1;
 a: return 0;
}






void *
_cru_prespreading_task (source, err)
	  port source;
	  int *err;

	  // Build the collision table and the deletions list local to one
	  // worker by recording all incoming packets, and send packets
	  // carrying adjacent nodes to other workers. In the event of a
	  // memory overflow or other error, scrap the collision table but
	  // still build the deletions list so that the whole graph can be
	  // reclaimed. Assume an error has occurred if any incoming
	  // packet has a non-null payload. This task is a prerequisite to
	  // the spreading task.
{
#define RECORDED(x) (_cru_member ((x), seen) ? 1 : (! *err) ? 0 : _cru_listed ((x), source->deletions))

  struct packet_list_s buffer;
  packet_list incoming;
  unsigned sample;
  intptr_t status;
  node_set seen;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  cru_sig s;
  router r;

  d = NULL;
  s = NULL;
  sample = 0;
  killed = 0;
  seen = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! source) ? IER(1568) : (source->gruntled != PORT_MAGIC) ? IER(1569) : 0)
	 return NULL;
  if ((!(r = source->local)) ? IER(1570) : (r->valid != ROUTER_MAGIC) ? IER(1571) : 0)
	 return NULL;
  if ((r->tag == BUI) ? 0 : IER(1572))
	 goto d;
  s = &(r->builder.bu_sig);
  if ((!(d = source->peers)) ? IER(1573) : 0)
	 goto d;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(28);
		if (killed ? 0 : (killed = (*err ? 1 : KILLED)))
		  _cru_kill_internally (&(r->killed), err);
		if (((n = incoming->receiver)) ? RECORDED(n) : IER(1574))
		  goto a;
		if (incoming->payload)
		  {
			 _cru_forget_collisions (source->collided, err);
			 source->collided = NULL;
		  }
		else if (killed ? 0 : *err ? 0 : packed (&incoming, s, &(source->collided), &(source->deletions), d, err))
		  continue;
		if (killed ? 0 : *err ? 0 : _cru_scattered_by_hashes (n, s->orders.v_order.hash, d, BY_TERMINUS, err))
		  goto b;
		if (! _cru_unscatterable_by_hashes (n, s->orders.v_order.hash, s->destructors.e_free, d, NOT_INITIAL, err))
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
		push_severed (n, &(source->deletions), err);
		_cru_set_membership (n, &seen, err);
	 a: if (_cru_unbuffered (&buffer, &incoming, err))
		  continue;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
 d: if (! (status = *err))
	 return NULL;
  _cru_forget_collisions (source->collided, err);
  source->collided = NULL;
  return (void *) status;
}









static void
start (s, q, h, d, err)
	  node_list *s;
	  node_queue *q;
	  cru_hash h;
	  packet_pod d;
	  int *err;

	  // Enqueue the given node list and immediately notify other
	  // workers about all nodes adjacent to any in the list. This is
	  // done at most once on receipt of the first packet during a
	  // spread operation, when the list contains this worker's share
	  // of the graph's nodes prior to its creation of any new ones.
{
  node_list n;

  if ((! s) ? IER(1575) : (! q) ? IER(1576) : ! (*q = _cru_queue_of (*s, err)))
	 return;
  *s = NULL;
  for (n = (*q)->front; n; n = n->next_node)
	 if (! _cru_scattered_by_hashes (n, h, d, BY_TERMINUS, err))
		return;
}










static void
cancel_spreading (s, err)
	  port s;
	  int *err;

	  // Reclaim the collision table associated with a single port in
	  // the event of an error.
{
  node_list n;
  router r;

  if ((! s) ? IER(1577) : (s->gruntled != PORT_MAGIC) ? IER(1578) : 0)
	 return;
  if ((!(r = s->local)) ? IER(1579) : (r->valid != ROUTER_MAGIC) ? IER(1580) : 0)
	 return;
  _cru_forget_collisions (s->collided, err);
  s->collided = NULL;
}









static void *
canceling_task (s, err)
	  port s;
	  int *err;

	  // Reclaim the collision table associated with a single port and
	  // notify other workers to do the same with other ports
	  // concurrently.
{
  _cru_pingback (s, err);
  cancel_spreading (s, err);
  return NULL;
}









static int
break_out (i, b, d, err)
	  packet_list *i;
	  cru_builder b;
	  packet_pod d;
	  int *err;

	  // Take a packet i carrying a node not created in the course of
	  // the current spreading operation. If the node has no outgoing
	  // edges, call the user-defined connector or subconnector
	  // functions to create new adjacent vertices and send them to
	  // other workers for further elaboration. Cf,
	  // _cru_reached_new_node in build.c.
{
  edge_list *new_edges_out;
  node_list n;
  void *l;                // incident edge label
  int ux;

  if ((! i) ? IER(1581) : (! *i) ? IER(1582) : (! b) ? IER(1583) : (! (n = (*i)->receiver)) ? IER(1584) : ! ! (n->edges_out))
	 goto a;
  if (((! (b->connector)) == ! (b->subconnector)) ? IER(1585) : *err)
	 goto a;
  if (b->subconnector ? (b->bu_sig.orders.e_order.hash ? 0 : IER(1586)) : 0)
	 goto a;
  if ((new_edges_out = _cru_get_edges ()) ? (! ! (*new_edges_out = NULL)) : IER(1587))
	 goto a;
  if (b->connector)
	 APPLY(b->connector, n->vertex);
  else if ((*i)->initial ? IER(1588) : (*i)->carrier ? 0 : IER(1589))
	 goto a;
  else
	 {
		l = (*i)->carrier->label;
		_cru_record_edge ((b->bu_sig.orders.e_order.hash) (l), b->bu_sig.orders.e_order.equal, l, &((*i)->seen_carriers), err);
		APPLY(b->subconnector, NOT_INITIAL, l, n->vertex);
	 }
  n->edges_out = *new_edges_out;
  if (b->subconnector)
	 n->edges_out = _cru_deduplicated_edges (n->edges_out, &(b->bu_sig.orders.e_order), b->bu_sig.destructors.e_free, err);
  _cru_scatter_out_or_consume (n, b->bu_sig.orders.v_order.hash, &(b->bu_sig.destructors), d, err);
 a: _cru_nack (_cru_popped_packet (i, err), err);
  return ! *err;
}








void *
spreading_task (source, err)
	  port source;
	  int *err;

	  // Spread the graph starting from the nodes currently stored in
	  // this worker's collision table that presently have no outgoing
	  // edges.
{
  edge_list new_edges_out;    // thread specific storage location for created edges
  packet_list incoming;
  int ux, ut, unequal;
  uintptr_t limit;            // maximum number of vertices allowed to be created by this worker
  uintptr_t count;            // number of vertices created by this worker
  unsigned sample;
  packet_list *c;
  cru_builder b;
  packet_pod d;
  node_queue q;
  node_list n;
  int started;
  int killed;                 // non-zero when the job is killed
  context x;
  cru_sig s;
  router r;

  d = NULL;
  q = NULL;
  count = 0;
  sample = 0;
  killed = 0;
  started = 0;
  x = BUILDING;
  new_edges_out = NULL;
  if ((! source) ? IER(1590) : (source->gruntled != PORT_MAGIC) ? IER(1591) : 0)
	 return NULL;
  if ((!(r = source->local)) ? IER(1592) : (r->valid != ROUTER_MAGIC) ? IER(1593) : 0)
	 return NULL;
  b = &(r->builder);
  s = &(b->bu_sig);
  if ((!(d = source->peers)) ? IER(1594) : (r->tag != BUI) ? IER(1595) : 0)
	 goto a;
  if ((s->orders.v_order.equal) ? 0 : IER(1596))
	 goto a;
  if (_cru_set_destructors (&(b->bu_sig.destructors), err) ? 1 : _cru_set_kill_switch (&(r->killed), err))
	 goto a;
  if (_cru_set_context (&x, err) ? 1 : _cru_set_edges (&new_edges_out, err))
	 goto a;
  limit = b->bu_sig.vertex_limit / r->lanes;
  limit = (limit ? limit : b->bu_sig.vertex_limit ? 1 : 0);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(29);
		killed = (killed ? 1 : KILLED);
		if (started ? 0 : ++started)
		  start (&(source->deletions), &q, b->bu_sig.orders.v_order.hash, d, err);
		if (incoming->receiver ? break_out (&incoming, b, d, err) : 0)
		  continue;
		if (*err ? 1 : killed ? 1 : ! (c = _cru_collision (incoming->hash_value, &(source->collided), err)))
		  goto b;
		if (*c)
		  while (*err ? 0 : (unequal = UNEQUAL(incoming->payload, (*c)->receiver)) ? (*c)->next_packet : NULL)
			 c = &((*c)->next_packet);
		if (*err)
		  goto b;
		if (*c ? (! unequal) : 0)
		  _cru_reach_extant_node (&incoming, *c, b, d, err);
		else if (limit ? (count++ < limit) : 1)
		  _cru_push_packet (_cru_reached_new_node (&incoming, b, &q, d, err), *c ? &((*c)->next_packet) : c, err);
		else
		  RAISE(CRU_INTOVF);
		continue;
	 b: _cru_free_packets (incoming, s->destructors.v_free, err);
		if (killed ? 0 : (killed = 1))
		  _cru_kill_internally (&(r->killed), err);
		incoming = NULL;
	 }
  x = IDLE;
  _cru_forget_collisions (source->collided, err);
  source->collided = NULL;
  if ((! q) ? 0 : *err ? 0 : ! (b->connector))
	 for (n = q->front; n; n = n->next_node)
		n->edges_out = _cru_deduplicated_edges (n->edges_out, &(b->bu_sig.orders.e_order), b->bu_sig.destructors.e_free, err);
  if (*err)
	 _cru_free_node_queue (q, &(s->destructors), err);
  return (*err ? _cru_queue_of (NO_NODES, err) : q);
 a: _cru_forget_collisions (source->collided, err);
  source->collided = NULL;
  _cru_free_node_queue (q, &(s->destructors), err);
  return _cru_abort_new_packets (source, d, &(s->destructors), err);
}








cru_graph
_cru_spread (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Launch a prespreading operand and then a spreading operation
	  // on the given graph.
{
  unsigned i;
  uintptr_t h;

  _cru_disable_killing (k, err);
  if ((! r) ? IER(1597) : (r->valid != ROUTER_MAGIC) ? IER(1598) : (! (r->ro_sig.orders.v_order.hash)) ? IER(1599) : 0)
	 goto a;
  if ((! g) ? 1 : (! (g->base_node)) ? IER(1600) : 0)
	 goto a;
  if (! _cru_status_launched (k, g->base_node, h = (r->ro_sig.orders.v_order.hash) (g->base_node->vertex), r, err))
	 goto a;
  g->nodes = NULL;
  if (_cru_graph_launched (k, g->base_node->vertex, h, _cru_shared (_cru_reset (r, (task) spreading_task, err)), &g, err))
	 goto a;
  if (! _cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) canceling_task, err)), err))
	 for (i = 0; i < r->lanes; i++)
		cancel_spreading (r->ports[i], err);
 a: _cru_sweep (r, err);
  if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}
