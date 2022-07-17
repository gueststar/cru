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
#include <stdlib.h>
#include <string.h>
#include "edges.h"
#include "errs.h"
#include "fab.h"
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
#include "table.h"
#include "wrap.h"








static packet_list
new_node (i, a, q, d, z, err)
	  packet_list *i;
	  cru_fabricator a;
	  node_queue *q;       // the output queue where all newly created nodes are stored
	  packet_pod d;
	  cru_destructor_pair z;
	  int *err;

	  // Create a new node based on the payload of the incoming packet
	  // and create outgoing edges from it corresponding the outgoing
	  // edges of the given node. Send a packet to another worker for
	  // each outgoing edge, but if not all of them can be sent
	  // successfully, then clear the labels and termini of the edges
	  // along which no packets were sent.
{
  node_list o, n;
  edge_list e, f;
  void *ua;
  void *v;
  int ux;

  if ((! i) ? IER(900) : (! *i) ? IER(901) : 0)
	 return NULL;
  if ((! (o = (node_list) (*i)->payload)) ? IER(902) : *err)
	 goto a;
  if ( (! a) ? IER(903) : (! (a->v_fab)) ? IER(904) : (! (a->e_fab)) ? IER(905) : 0)
	 goto a;
  n = NULL;
  v = APPLIED(a->v_fab, o->vertex);
  if (*err ? 1 : (n = _cru_node_of (z, v, NO_EDGES_IN, NO_EDGES_OUT, err)) ? *err : 1)
	 goto b;
  for (e = o->edges_out; *err ? NULL : e; e = e->next_edge)
	 if ((f = _cru_edge (z, APPLIED(a->e_fab, e->label), NO_VERTEX, e->remote.node, NO_NEXT_EDGE, err)))
		n->edges_out = _cru_cat_edges (f, n->edges_out);
  if (*err ? 1 : ! _cru_enqueued_node (n, q, err))
	 goto b;
  if ((*i)->carrier)
	 (*i)->carrier->remote.node = n;
  for (e = f = _cru_scatter_out ((*i)->receiver = n, d, err); f; f = f->next_edge)
	 f->remote.node = NULL;
  _cru_free_labels (z->e_free, e, err);
  return _cru_popped_packet (i, err);
 b: _cru_free_nodes (n, z, err);
 a: if ((*i)->carrier)
	 (*i)->carrier->remote.node = NULL;
  _cru_nack (_cru_popped_packet (i, err), err);
  return NULL;
}








static void *
fabricating_task (s, err)
	  port s;
	  int *err;

	  // Fabricate a new graph based on the original graph
	  // co-operatively with other workers. The first time a node from
	  // the original graph is received, make a new node with new
	  // outgoing edges connected to the adjacent nodes in the original
	  // graph, and scatter along the outgoing edges. Also point the
	  // carrierq on the incoming packet to the newly created node.
	  // For each subsequent receipt of a previously received node,
	  // don't create a new one but point the carrier to the
	  // corresponding previously created node. In the event of an
	  // error, point the carrier to NULL and free its label.
{
  packet_table collisions;
  packet_list incoming;       // incoming packets
  unsigned sample;
  packet_list *c;
  packet_list *p;
  packet_pod d;               // outgoing packets
  node_queue q;
  node_list n;
  int killed;
  int unequal;
  router r;
  int ux;

  q = NULL;
  killed = 0;
  sample = 0;
  collisions = NULL;
  if ((! s) ? IER(906) : (s->gruntled != PORT_MAGIC) ? IER(907) : 0)
	 return NULL;
  if ((! (r = s->local)) ? IER(908) : (r->valid != ROUTER_MAGIC) ? IER(909) : 0)
	 return NULL;
  if ((! (d = s->peers)) ? IER(910) : (r->tag != FAB) ? IER(911) : 0)
	 return _cru_abort (s, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, d, err));)
	 {
		KILL_SITE(12);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : ! (c = _cru_collision (incoming->hash_value, &collisions, err)))
		  goto a;
		if (*c)
		  while ((unequal = (incoming->payload != (*c)->payload)) ? (*c)->next_packet : NULL)
			 c = &((*c)->next_packet);
		if (*c ? (! unequal) : 0)
		  goto b;
		p = (*c ? &((*c)->next_packet) : c);
		_cru_push_packet (new_node (&incoming, &(r->fabricator), &q, d, &(r->ro_sig.destructors), err), p, err);
		continue;
	 b: if (incoming->carrier ? 1 : ! IER(912))
		  incoming->carrier->remote.node = (*c)->receiver;
		incoming->carrier = NULL;
	 a: if (! (incoming->carrier))
		  goto c;
		incoming->carrier->remote.node = NULL;
		if (incoming->carrier->label ? r->ro_sig.destructors.e_free : NULL)
		  APPLY(r->ro_sig.destructors.e_free, incoming->carrier->label);
		incoming->carrier->label = NULL;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_collisions (collisions, err);
  if (*err)
	 _cru_free_node_queue (q, &(r->ro_sig.destructors), err);
  return (*err ? NULL : q);
}








cru_graph
_cru_fabricated (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Return a graph fabricated from the given graph with the
	  // supplied router.
{
  cru_graph f;

  f = NULL;
  _cru_disable_killing (k, err);
  if ((! r) ? 1 : (! g) ? IER(913) : 0)
	 goto a;
  _cru_graph_launched (k, g->base_node, _cru_scalar_hash (g->base_node), _cru_reset (r, (task) fabricating_task, err), &f, err);
  if (*err)
	 _cru_free_now (f, err);
  return (*err ? NULL : f);
 a: _cru_free_router (r, err);
  return NULL;
}
