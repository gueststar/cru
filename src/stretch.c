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
#include "cthread.h"
#include "duplex.h"
#include "edges.h"
#include "emap.h"
#include "errs.h"
#include "getset.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "props.h"
#include "route.h"
#include "queue.h"
#include "repl.h"
#include "scatter.h"
#include "stretch.h"
#include "table.h"
#include "wrap.h"



#define EQUAL_CARRIERS (cru_bpred) _cru_equality_bpred





static int
scattered (n, x, s, d, err)
	  node_list n;
	  cru_stretcher x;
	  cru_sig s;
	  packet_pod d;
	  int *err;

	  // Return non-zero if a non-stretch marked node is sent to the
	  // worker responsible for each adjacent node to n and an stretch
	  // packet is sent to the relevant worker for each new node
	  // requested by the user-defined stretcher. This is done exactly
	  // once for each node received with a non-stretch class mark.
{
  node_list m;
  edge_list *e, l;
  int deletable, ux, ut;
  edge_map *new_edge_map;

  if ((! n) ? IER(1534) : n->edges_in ? IER(1535) : 0)
	 return 0;
  l = NULL;
  if ((! s) ? IER(1536) : 0)
	 return 0;
  if (! _cru_scattered_by_hashes (n, s->orders.v_order.hash, d, BY_TERMINUS, err))
	 return 0;
  if ((! x) ? IER(1537) : (! (x->expander)) ? IER(1538) : 0)
	 return 0;
  if ((new_edge_map = _cru_get_edge_maps ()) ? 0 : IER(1539))
	 return 0;
  for (e = &(n->edges_out); *err ? NULL : *e;)
	 {
		if (((m = (*e)->remote.node)) ? 0 : IER(1540))
		  return 0;
		*new_edge_map = NULL;
		deletable = (*err ? 1 : PASSED(x->expander, n->vertex_property, (*e)->label, m->vertex_property));
		if (! *err)
		  l = _cru_cat_edges (_cru_scatter_and_stretch (&(s->destructors), *new_edge_map, m, s->orders.v_order.hash, d, err), l);
		if (*err ? 0 : deletable)
		  _cru_free_edges_and_labels (s->destructors.e_free, _cru_popped_edge (e, err), err);
		else
		  e = &((*e)->next_edge);
	 }
  n->edges_out = _cru_cat_edges (n->edges_out, l);
  return 1;
}








static packet_list
grown (i, limit, count, err)
	  packet_list *i;              // incoming packet
	  uintptr_t limit;
	  uintptr_t *count;
	  int *err;

	  // Store a node obtained from in incoming packet i and consume
	  // the packet. This is done to every node containing a vertex not
	  // previously detected.
{
  packet_list c;

  if (count ? (limit ? ((*count)++ >= limit) : 0) : ! IER(1541))
	 RAISE(CRU_INTOVF);
  if ((c = _cru_popped_packet (i, err)) ? 0 : IER(1542))
	 return NULL;
  c->receiver = _cru_half_severed (c->receiver);
  _cru_record_edge (_cru_scalar_hash (c->carrier), EQUAL_CARRIERS, (void *) c->carrier, &(c->seen_carriers), err);
  return c;
}









static void
graft (i, c, s, err)
	  packet_list i;               // incoming packet
	  packet_list c;               // packet containing a previously stored node
	  cru_sig s;
	  int *err;

	  // Discard the node in an incoming packet, point its predecessor
	  // to an equivalent previously stored node, and connect the
	  // stored node additionally to the incoming node's
	  // successors. This is done when an incoming node with an
	  // stretch class mark matches one previously stored.
{
  if ((! i) ? IER(1543) : (! (i->carrier)) ? IER(1544) : (! (i->receiver)) ? IER(1545) : 0)
	 return;
  if ((! c) ? IER(1546) : (! (c->receiver)) ? IER(1547) : (i == c) ? IER(1548) : s ? 0 : IER(1549))
	 return;
  c->receiver->edges_out = _cru_cat_edges (i->receiver->edges_out, c->receiver->edges_out);
  i->receiver->edges_out = NULL;
  if (! *err)
	 c->receiver->edges_out = _cru_deduplicated_edges (c->receiver->edges_out, &(s->orders.e_order), s->destructors.e_free, err);
  i->carrier->remote.node = c->receiver;
  _cru_record_edge (_cru_scalar_hash (i->carrier), EQUAL_CARRIERS, (void *) i->carrier, &(c->seen_carriers), err);
  i->receiver->class_mark = NULL;
  _cru_free_nodes (_cru_half_severed (i->receiver), &(s->destructors), err);
  i->receiver = NULL;
}








static void
overwrite (i, c, s, d, err)
	  packet_list i;              // incoming packet
	  packet_list c;              // packet containing stored node
	  cru_sig s;
	  cru_destructor d;           // property destructor
	  int *err;

	  // Replace a previously stored node with one obtained from an
	  // incoming packet, attaching the previously stored incident and
	  // outgoing edges to the replacement. This is done to nodes
	  // previously received with an stretch class mark but replaceable
	  // by an equivalant one recieved later with a non-stretch
	  // class mark.
{
  if ((! i) ? IER(1550) : (! (i->receiver)) ? IER(1551) : (! c) ? IER(1552) : (! (c->receiver)) ? IER(1553) : s ? 0 : IER(1554))
	 return;
  _cru_redirect (c->seen_carriers, i->receiver, err);
  i->receiver->edges_out = _cru_cat_edges (c->receiver->edges_out, i->receiver->edges_out);
  c->receiver->edges_out = NULL;
  if (! *err)
	 i->receiver->edges_out = _cru_deduplicated_edges (i->receiver->edges_out, &(s->orders.e_order), s->destructors.e_free, err);
  _cru_free_nodes (_cru_unpropped (c->receiver, d, err), &(s->destructors), err);
  c->receiver = _cru_half_severed (i->receiver);
  _cru_record_edge (_cru_scalar_hash (i->carrier), EQUAL_CARRIERS, (void *) i->carrier, &(c->seen_carriers), err);
  i->receiver = NULL;
}






void
_cru_cull (i, b, h, q, d, by_class, err)
	  packet_list *i;
	  packet_list b;             // statically allocated packet
	  cru_hash h;
	  node_list *q;              // the destination queue for all incoming nodes and maybe some previously received
	  packet_pod d;
	  int by_class;
	  int *err;

	  // Collect incoming nodes into the queue in anticipation of
	  // tearing down the graph. This is done only in the event of an
	  // error.
{
  node_list n;

  if ((! i) ? IER(1555) : (! *i) ? IER(1556) : 0)
	 return;
  if ((! b) ? IER(1557) : (! q) ? IER(1558) : (! d) ? IER(1559) : (! (n = (*i)->receiver)) ? IER(1560) : 0)
	 goto a;
  if (n->class_mark == RETIRED)
	 goto b;
  if ((n->class_mark == SCATTERED) ? 1 : (n->class_mark == STRETCHED))
	 goto c;
  if (! (n->class_mark))
	 _cru_write (&(n->class_mark), SEEN, err);
  if (_cru_scattered_by_hashes (n, h, d, by_class, err))
	 goto d;
  if (_cru_buffered (b, *i, &(d->deferrals), err))
	 goto a;
  if (_cru_transplanted_out (b->receiver, n, err))
	 goto e;
  if (! _cru_spun (b, i, &(d->deferrals), err))
	 goto a;
  return;
 e: _cru_write (&(b->receiver->class_mark), SEEN, err);
 d: _cru_write (&(n->class_mark), SCATTERED, err);
 c: if ((*i != b) ? (n == b->receiver) : 0)
	 goto a;
  _cru_pushed_node (_cru_half_severed (n), q, err);
  _cru_write (&(n->class_mark), RETIRED, err);
 b: if (_cru_unbuffered (b, i, err))
	 return;
 a: _cru_nack (_cru_popped_packet (i, err), err);
}









static void *
stretching_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively stretch the graph with other workers. Return
	  // non-null if there is a net increase in the number of vertices.
{
#define UNEQUAL(a,b) \
((! b) ? (! IER(1561)) : (a == b->vertex) ? 0 : FAILED(r->ro_sig.orders.v_order.equal, a, b->vertex))

  struct packet_list_s buffer;  // needed if only if there's not enough memory to allocate a packet
  packet_table collisions;      // previous incoming packets
  edge_map new_edge_map;        // thread specific storage area
  packet_list incoming;
  int ux, ut, unequal;
  uintptr_t changed;
  uintptr_t limit;              // maximum number of vertices allowed to be created by this worker
  uintptr_t count;              // number of vertices created by this worker
  unsigned sample;
  node_list n, q;
  packet_list *c;
  packet_list *p;
  packet_pod d;
  int killed;                   // non-zero when the job is killed
  context x;
  router r;

  q = NULL;
  killed = 0;
  sample = 0;
  x = STRETCHING;
  collisions = NULL;
  changed = count = 0;
  new_edge_map = NULL;
  _cru_set_context (&x, err);
  memset (&buffer, 0, sizeof (buffer));
  _cru_set_edge_maps (&new_edge_map, err);
  if ((! source) ? IER(1562) : (source->gruntled != PORT_MAGIC) ? IER(1563) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(1564) : (r->valid != ROUTER_MAGIC) ? IER(1565) : 0)
	 goto b;
  if ((!(d = source->peers)) ? IER(1566) : (r->tag != EXT) ? IER(1567) : 0)
	 goto b;
  if ((r->ro_sig.orders.v_order.equal) ? 0 : IER(1568))
	 goto b;
  limit = r->ro_sig.vertex_limit / r->lanes;
  _cru_set_destructors (&(r->ro_sig.destructors), err);
  _cru_set_kill_switch (&(r->killed), err);
  limit = (limit ? limit : r->ro_sig.vertex_limit ? 1 : 0);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(28);
		killed = (killed ? 1 : KILLED);
		if ((! (n = incoming->receiver)) ? IER(1569) : 0)
		  goto c;
		if (*err ? 1 : killed ? 1 : (c = _cru_collision (incoming->hash_value, &collisions, err)) ? *err : 1)
		  goto d;
		if (*c)
		  while (*err ? 0 : (unequal = UNEQUAL(n->vertex, (*c)->receiver)) ? (*c)->next_packet : NULL)
			 c = &((*c)->next_packet);
		if (*c ? (n == (*c)->receiver) : 0)
		  goto c;
		if (n->class_mark == STRETCHED)
		  goto e;
		if (*err ? 1 : scattered (n, &(r->stretcher), &(r->ro_sig), d, err) ? *err : 1)
		  goto d;
		_cru_write (&(n->class_mark), SCATTERED, err);
	 e: if (*c ? (! unequal) : 0)
		  goto f;
		_cru_push_packet (grown (&incoming, limit, &count, err), p = (*c ? &((*c)->next_packet) : c), err);
		if ((! *p) ? IER(1570) : ((*p)->receiver->class_mark != STRETCHED) ? 1 : ++changed ? 1 : IER(1571))
		  continue;
	 f: if (n->class_mark == STRETCHED)
		  graft (incoming, *c, &(r->ro_sig), err);
		else if (changed-- ? 1 : IER(1572))
		  overwrite (incoming, *c, &(r->ro_sig), r->stretcher.st_prop.vertex.m_free, err);
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
		continue;
	 d: if (q ? NULL : (q = _cru_receivers (_cru_unpacked (collisions), err)))
		  for (n = q; n; n = n->next_node)
			 _cru_write (&(n->class_mark), RETIRED, err);
		collisions = NULL;
		_cru_cull (&incoming, &buffer, r->ro_sig.orders.v_order.hash, &q, d, BY_TERMINUS, err);
	 }
  x = IDLE;
  source->survivors = (q ? q :  _cru_receivers (_cru_unpacked (collisions), err));
  if ((source->survivors = _cru_unpropped (source->survivors, r->stretcher.st_prop.vertex.m_free, err)))
	 source->survivors->previous = &(source->survivors);
 a: return (void *) changed;
 b: return _cru_abort (source, d, err);
}









node_queue
_cru_pruning_task (source, err)
	  port source;
	  int *err;

	  // Return a queue of every received node, severing each from the
	  // port's list of survivors. Cf. pruning_task in filters.c.
{
  router r;
  cru_sig s;
  int killed;
  node_list n;
  node_queue q;
  packet_pod d;
  node_set seen;
  unsigned sample;
  packet_list incoming;

  q = NULL;
  d = NULL;
  sample = 0;
  seen = NULL;
  if ((! source) ? IER(1573) : (source->gruntled != PORT_MAGIC) ? IER(1574) : (killed = 0))
	 return NULL;
  if ((!(r = source->local)) ? IER(1575) : (r->valid != ROUTER_MAGIC) ? IER(1576) : 0)
	 return NULL;
  s = &(r->ro_sig);
  if (((d = source->peers)) ? 0 : IER(1577))
	 return (node_queue) _cru_abort (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(29);
		killed = (killed ? 1 : KILLED);
		if (killed ? 1 : (! (n = incoming->receiver)) ? IER(1578) : _cru_test_and_set_membership (n, &seen, err) ? 1 : *err)
		  goto a;
		if (_cru_enqueued_node (_cru_severed (n, err), &q, err))
		  _cru_scattered_by_hashes (n, s->orders.v_order.hash, d, (r->tag == CLU) ? BY_CLASS : BY_TERMINUS, err);
		else
		  _cru_pushed_node (n, &(source->survivors), err);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  return q;
}












cru_graph
_cru_stretched (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Launch an extension operation with a previously prepared
	  // router and block until finished.
{
  router z;
  uintptr_t q;
  node_list b;
  int iterating;
  uintptr_t count;   // non-zero when any worker detects a change

  _cru_disable_killing (k, err);
  if (*err ? 1 : (! g) ? 1 : (! (b = g->base_node)) ? IER(1579) : 0)
	 goto a;
  if ((! r) ? IER(1580) : (r->valid != ROUTER_MAGIC) ? IER(1581) : (r->tag != EXT) ? IER(1582) : 0)
	 goto a;
  if ((! (r->ports)) ? IER(1583) : r->ro_sig.orders.v_order.hash ? 0 : IER(1584))
	 goto a;
  if (_cru_half_duplex (g, err))
	 goto b;
  if (! (z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err)))
	 goto a;
  if (! _cru_launched (k, b, z, err))
	 goto a;
 b: q = (r->ro_sig.orders.v_order.hash) (b->vertex);
  for (iterating = 1; *err ? 0 : iterating;)
	 {
		if (_cru_set_properties (g, k, r, err) ? *err : 1)
		  break;
		if (_cru_reset (r, (task) stretching_task, err) ? *err : 1)
		  break;
 		if (_cru_count_launched (UNKILLABLE, b, NO_PAYLOAD, q, _cru_shared (r), &count, err))
		  g->nodes = NULL;
		else
		  _cru_unset_properties (g, r, err);
		if (*err)
		  break;
		iterating = (count ? r->stretcher.st_fix : 0);
		if (_cru_queue_launched (k, b, q, _cru_reset (r, (task) _cru_pruning_task, err), &(g->nodes), err))
		  if (g->nodes)
			 g->nodes->previous = &(g->nodes);
	 }
 a: if (*err == CRU_INTKIL)
	 _cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  _cru_sweep (r, err);
  return (*err ? NULL : g);
}
