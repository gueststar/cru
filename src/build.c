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
#include "table.h"
#include "wrap.h"

// first parameter to a subconnector function
#define INITIAL 1
#define NOT_INITIAL 0

#define UNEQUAL(a,b) ((a == b) ? 0 : FAILED(s->orders.v_order.equal, a, b))






static edge_list
unique (e, s, err)
	  edge_list e;
	  cru_sig s;
	  int *err;

	  // Deduplicate edges by label and remote vertex. Time is
	  // quadratic in the cardinality of the largest edge label
	  // equivalence class, making it possibly a bottleneck if
	  // connectors create large numbers of identically labeled
	  // outgoing edges between the same vertices.
{
  brigade b, t;           // each bucket has only one edge label but multiple remote vertices
  edge_list *l;           // all edges in a bucket
  edge_list p;            // next edge in a bucket
  edge_list r;            // cumulative unique edges
  int ux, ut;

  if (s ? 0 : IER(566))
	 return NULL;
  t = _cru_rallied (s->orders.e_order.hash, s->orders.e_order.equal, &e, err);
  _cru_free_edges_and_termini (&(s->destructors), e, err);
  for (r = NULL; (b = _cru_popped_bucket (&t, err)); _cru_free_brigade (b, err))
	 for (l = &(b->bucket); (p = (*l ? _cru_popped_edge (l, err) : NULL));)
		{
		  for (e = b->bucket; e ? UNEQUAL(p->remote.vertex, e->remote.vertex) : 0; e = e->next_edge);
		  if (e ? 1 : *err)
			 _cru_free_edges_and_termini (&(s->destructors), p, err);
		  else
			 _cru_push_edge (p, &r, err);
		}
  return r;
}











void
_cru_reach_extant_node (i, c, b, d, err)
	  packet_list *i;
	  packet_list c;       // colliding packet
	  cru_builder b;
	  packet_pod d;
	  int *err;

	  // Discard a packet i whose payload vertex matches one already in
	  // the graph recorded in a packet c, and connect its sender node
	  // to the latter. If the graph is being built by subconnector
	  // functions and the label on the edge leading to the vertex
	  // hasn't been seen already, then call the subconnector function
	  // and percolate messages to other workers through any new edges
	  // it creates.
{
  int ux;
  void *l;
  uintptr_t h;
  node_list n;
  edge_list *new_edges_out;
  edge_list extant_edges_out;

  if ((! i) ? IER(567) : (! *i) ? IER(568) : (! ((*i)->carrier)) ? IER(569) : c ? 0 : IER(570))
	 goto a;
  if ((! b) ? IER(571) : (new_edges_out = _cru_get_edges ()) ? (! ! (*new_edges_out = NULL)) : IER(572))
	 goto b;
  if (b->bu_sig.destructors.v_free ? (*i)->payload : NULL)           // get rid of the extra copy of the vertex
	 APPLY(b->bu_sig.destructors.v_free, (*i)->payload);
  (*i)->payload = NULL;
  if (((n = c->receiver)) ? 0 : IER(573))
	 goto b;
  if (b->connector ? (b->subconnector ? IER(574) : 1) : 0)
	 goto b;
  if (b->subconnector ? (b->bu_sig.orders.e_order.hash ? 0 : IER(575)) : IER(576))
	 goto b;
  h = (b->bu_sig.orders.e_order.hash) (l = (*i)->carrier->label);
  if ((*i)->initial ? IER(577) : _cru_already_recorded (h, b->bu_sig.orders.e_order.equal, l, c->seen_carriers, err))
	 goto b;
  _cru_record_edge (h, b->bu_sig.orders.e_order.equal, l, &(c->seen_carriers), err);
  APPLY(b->subconnector, NOT_INITIAL, l, c->receiver->vertex);                                  // make new edges
  extant_edges_out = c->receiver->edges_out;
  c->receiver->edges_out = unique (*new_edges_out, &(b->bu_sig), err);
  *new_edges_out = NULL;
  _cru_scatter_out_or_consume (c->receiver, b->bu_sig.orders.v_order.hash, &(b->bu_sig.destructors), d, err);
  c->receiver->edges_out = _cru_cat_edges (c->receiver->edges_out, extant_edges_out);
 b: (*i)->carrier->remote.node = c->receiver;
 a: _cru_nack (_cru_popped_packet (i, err), err);
}













packet_list
_cru_reached_new_node (i, b, q, d, err)
	  packet_list *i;
	  cru_builder b;
	  node_queue *q;       // the output queue where all newly created nodes are stored
	  packet_pod d;
	  int *err;

	  // Create a new node for the graph, store it in the receiver
	  // field of the top packet, connect the sender node in the packet
	  // to the new node if applicable, send adjacent vertices to other
	  // workers in other packets, and pop the packet.
{
#define NO_INCIDENT_EDGE_LABEL NULL

  edge_list *new_edges_out;
  node_list n;
  void *l;                // incident edge label
  int ux;

  if ((! i) ? IER(578) : (! *i) ? IER(579) : (! b) ? IER(580) : 0)
	 goto a;
  if (((! (b->connector)) == ! (b->subconnector)) ? IER(581) : 0)
	 goto a;
  if (b->subconnector ? (b->bu_sig.orders.e_order.hash ? 0 : IER(582)) : 0)
	 goto a;
  if ((new_edges_out = _cru_get_edges ()) ? (! ! (*new_edges_out = NULL)) : IER(583))
	 goto a;
  if (b->connector)
	 APPLY(b->connector, (*i)->payload);
  else if ((*i)->initial)
	 APPLY(b->subconnector, INITIAL, NO_INCIDENT_EDGE_LABEL, (*i)->payload);
  else if ((*i)->carrier ? 0 : IER(584))
	 goto a;
  else
	 {
		l = (*i)->carrier->label;
		_cru_record_edge ((b->bu_sig.orders.e_order.hash) (l), b->bu_sig.orders.e_order.equal, l, &((*i)->seen_carriers), err);
		APPLY(b->subconnector, NOT_INITIAL, l, (*i)->payload);
	 }
  n = _cru_node_of (&(b->bu_sig.destructors), (*i)->payload, NO_EDGES_IN, *new_edges_out, err);
  *new_edges_out = NULL;
  if (! _cru_enqueued_node (n, q, err))
	 {
		_cru_free_adjacencies (&n, &(b->bu_sig.destructors), err);
		goto a;
	 }
  if ((*i)->carrier)
	 (*i)->carrier->remote.node = n;
  n->edges_out = unique (n->edges_out, &(b->bu_sig), err);
  _cru_scatter_out_or_consume ((*i)->receiver = n, b->bu_sig.orders.v_order.hash, &(b->bu_sig.destructors), d, err);
  return _cru_popped_packet (i, err);
 a: _cru_nack (_cru_popped_packet (i, err), err);
  return NULL;
}









void *
_cru_building_task (source, err)
	  port source;
	  int *err;

	  // Receive packets carrying graph vertices from other workers,
	  // check for their presence in the graph, create adjacent
	  // vertices to those not previously present, and send the created
	  // adjacent vertices to other workers.
{
  packet_table collisions;    // previous incoming packets
  edge_list new_edges_out;    // thread specific storage location for created edges
  packet_list incoming;
  int ux, ut, unequal;
  uintptr_t limit;            // maximum number of vertices allowed to be created by this worker
  uintptr_t count;            // number of vertices created by this worker
  unsigned sample;
  packet_list *c;
  cru_builder b;
  node_queue q;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  context x;
  cru_sig s;
  router r;

  q = NULL;
  d = NULL;
  count = 0;
  sample = 0;
  killed = 0;
  x = BUILDING;
  collisions = NULL;
  new_edges_out = NULL;
  if ((! source) ? IER(585) : (source->gruntled != PORT_MAGIC) ? IER(586) : 0)
	 return NULL;
  if ((!(r = source->local)) ? IER(587) : (r->valid != ROUTER_MAGIC) ? IER(588) : 0)
	 return NULL;
  b = &(r->builder);
  if ((!(d = source->peers)) ? IER(589) : (r->tag != BUI) ? IER(590) : ! (s = &(b->bu_sig)))
	 goto a;
  if ((s->orders.v_order.equal) ? 0 : IER(591))
	 goto a;
  if (_cru_set_destructors (&(b->bu_sig.destructors), err) ? 1 : _cru_set_kill_switch (&(r->killed), err) ? 1 : 0)
	 goto a;
  if (_cru_set_context (&x, err) ? 1 : _cru_set_edges (&new_edges_out, err) ? 1 : 0)
	 goto a;
  limit = b->bu_sig.vertex_limit / r->lanes;
  limit = (limit ? limit : b->bu_sig.vertex_limit ? 1 : 0);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(1);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : ! (c = _cru_collision (incoming->hash_value, &collisions, err)))
		  goto b;
		if (*c)
		  while (*err ? 0 : (unequal = UNEQUAL(incoming->payload, (*c)->payload)) ? (*c)->next_packet : NULL)
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
  _cru_forget_collisions (collisions, err);
  if ((! q) ? 0 : *err ? 0 : ! (b->connector))
	 for (n = q->front; n; n = n->next_node)
		n->edges_out = _cru_deduplicated_edges (n->edges_out, &(b->bu_sig.orders.e_order), b->bu_sig.destructors.e_free, err);
  if (*err)
	 _cru_free_node_queue (q, &(s->destructors), err);
  return (*err ? NULL : q);
 a: return _cru_abort_packets (source, d, &(r->ro_sig.destructors), err);
}









cru_graph
_cru_built (v, k, r, err)
	  void *v;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Concurrently build the graph of all nodes reachable from the
	  // initial vertex, and consume the initial vertex and the router.
{
  cru_graph g;
  int ux;

  g = NULL;
  _cru_disable_killing (k, err);
  if ((! r) ? IER(592) : (r->valid != ROUTER_MAGIC) ? IER(593) : (! (r->ro_sig.orders.v_order.hash)) ? IER(594) : 0)
	 goto a;
  if (! _cru_graph_launched (k, v, (r->ro_sig.orders.v_order.hash) (v), r, &g, err))
	 if (v ? r->ro_sig.destructors.v_free : NULL)
		APPLY(r->ro_sig.destructors.v_free, v);
  if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
 a: _cru_free_router (r, err);
  return NULL;
}
