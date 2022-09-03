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
#include <strings.h>
#include "edges.h"
#include "errs.h"
#include "cthread.h"
#include "filters.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "props.h"
#include "route.h"
#include "repl.h"
#include "scatter.h"
#include "split.h"
#include "table.h"
#include "wrap.h"


// unused integers whose addresses are outside the heap
static int create;
static int connect;
static int disconnect;

// a packet payload telling the worker to insert a new node into the graph
#define CREATE &create

// a packet payload telling the worker to attach a new outgoing edge to a node in the graph
#define CONNECT &connect

// a packet payload telling the worker to detach an dispose of an extant outgoing edge from a node in the graph
#define DISCONNECT &disconnect









static void
fan_out (n, s, d, a, z, err)
	  node_list n;
	  cru_splitter s;
	  packet_pod d;
	  node_list *a;             // a list of nodes for deferred reclamation of overwritten vertices
	  cru_destructor_pair z;
	  int *err;

	  // Create a copy of a given node if it's fissile, and delete,
	  // duplicate, or transfer its outgoing edges to the copy of the
	  // node. Send the copy of the node to another worker depending on
	  // its hash. Also allow all vertices and edge labels to be
	  // modified in place according to the user-specified splitter.
	  // Notify other workers via the edges created or remaining. If
	  // tests are being run, allow an allocation error to be simulated
	  // at each call site to the test code.
{
  void *l;
  void *t;
  void *ua;
  cru_ctop_pair o;
  edge_list *e;
  void *a_label;
  void *c_label;
  node_list m, c;
  int a_cond, c_cond, ux, ut, received;

  received = 0;
  if ((! n) ? IER(1508) : n->doppleganger ? IER(1509) : (! s) ? IER(1510) : (!(s->fissile)) ? IER(1511) : *err)
	 return;
  o = &(s->sp_ctops.outward);
  if ((! z) ? IER(1512) : ! PASSED(s->fissile, n->vertex, n->vertex_property))
	 goto a;
  if ((!(s->ana)) ? IER(1513) : (!(o->ana_labeler.tpred)) ? IER(1514) : (!(o->ana_labeler.top)) ? IER(1515) : 0)
	 return;
  if ((!(s->cata)) ? IER(1516) : (!(o->cata_labeler.tpred)) ? IER(1517) : (!(o->cata_labeler.top)) ? IER(1518) : 0)
	 return;
  t = APPLIED(s->cata, n->vertex, n->vertex_property);
  if (*err ? 1 : ! ((c = _cru_node_of (z, t, NO_EDGES_IN, NO_EDGES_OUT, err))))
	 return;
  if (_cru_propped (c, n->edges_in, n->edges_out, &(s->sp_prop), err) ? *err : 1)
	 goto b;
  if (! (received = _cru_received_by (CREATE, NO_CARRIER, c, d, err)))
	 goto b;
  if (z->v_free)
	 if (! ((m = _cru_node_of (NO_DESTRUCTORS, n->vertex, NO_EDGES_IN, NO_EDGES_OUT, err)) ? _cru_pushed_node (m, a, err) : 0))
		goto b;
  t = APPLIED(s->ana, n->vertex, n->vertex_property);
  if (*err)
	 goto c;
  n->doppleganger = c;
  _cru_write (&(n->vertex), t, err);
  for (e = &(n->edges_out); *err ? NULL : *e;)
	 {
		if (((*e)->remote.node) ? 0 : IER(1519))
		  return;
		l = (*e)->label;
		a_label = c_label = NULL;
		if ((a_cond = PASSED(o->ana_labeler.tpred, n->vertex_property, l, (*e)->remote.node->vertex_property)))
		  a_label = APPLIED(o->ana_labeler.top, n->vertex_property, l, (*e)->remote.node->vertex_property);
		if ((c_cond = PASSED(o->cata_labeler.tpred, n->vertex_property, l, (*e)->remote.node->vertex_property)))
		  c_label = APPLIED(o->cata_labeler.top, n->vertex_property, l, (*e)->remote.node->vertex_property);
		(*e)->label = (a_cond ? a_label : c_label);
		if (l ? z->e_free : NULL)
		  APPLY(z->e_free, l);
		if ((! a_cond) ? (! c_cond) : 0)
		  _cru_free_edges_and_labels (z->e_free, _cru_popped_edge (e, err), err);
		else if ((! a_cond) ? c_cond : 0)
		  _cru_push_edge (_cru_popped_edge (e, err), &(c->edges_out), err);
		else if (a_cond ? c_cond : 0)
		  _cru_push_edge (_cru_edge (z, c_label, NO_VERTEX, (*e)->remote.node, NO_NEXT_EDGE, err), &(c->edges_out), err);
		if (a_cond)
		  e = &((*e)->next_edge);
	 }
  if (! *err)
	 _cru_scatter_out (c, d, err);
 a: if (! *err)
	 _cru_scatter_out (n, d, err);
  return;
 c: if (z->v_free)
	 _cru_free_nodes (_cru_severed (m, err), NO_DESTRUCTORS, err);
 b: if (! received)
	 _cru_free_nodes (_cru_unpropped (c, s->sp_prop.vertex.m_free, err), z, err);
}








static void
fan_in (n, sender, carrier, s, d, z, err)
	  node_list n;
	  node_list sender;
	  edge_list carrier;
	  cru_splitter s;
	  packet_pod d;
	  cru_destructor_pair z;
	  int *err;

	  // Modify, transfer, request disconnection, or request connection
	  // of an incoming edge to a split vertex and its doppleganger. If
	  // tests are being run, allow an allocation error to be simulated
	  // at each call site to the test code.
{
  void *l;
  void *ua;
  cru_ctop_pair i;
  node_list c;
  void *a_label;
  void *c_label;
  int a_cond, c_cond, ux, ut;

  if (*err ? 1 : (! n) ? IER(1520) : (! s) ? IER(1521) : (! d) ? IER(1522) : ! ((c = n->doppleganger)))
	 return;
  i = &(s->sp_ctops.inward);
  if ((! z) ? IER(1523) : (sender ? carrier : NULL) ? 0 : sender ? IER(1524) : carrier ? IER(1525) : 1)
	 return;
  if ((! (i->ana_labeler.tpred)) ? IER(1526) : (! (i->ana_labeler.top)) ? IER(1527) : 0)
	 return;
  if ((! (i->cata_labeler.tpred)) ? IER(1528) : (! (i->cata_labeler.top)) ? IER(1529) : 0)
	 return;
  l = carrier->label;
  a_label = c_label = NULL;
  if ((a_cond = PASSED(i->ana_labeler.tpred, sender->vertex_property, l, n->vertex_property)))
	 a_label = APPLIED(i->ana_labeler.top, sender->vertex_property, l, n->vertex_property);
  if ((c_cond = PASSED(i->cata_labeler.tpred, sender->vertex_property, l, n->vertex_property)))
	 c_label = APPLIED(i->cata_labeler.top, sender->vertex_property, l, n->vertex_property);
  carrier->label = (a_cond ? a_label : c_label);
  if (l ? z->e_free : NULL)
	 APPLY(z->e_free, l);
  if ((! a_cond) ? (! c_cond) : 0)
	 _cru_received_by (DISCONNECT, carrier, sender, d, err);
  else if ((! a_cond) ? c_cond : 0)
	 carrier->remote.node = c;
  else if ((a_cond ? (! c_cond) : 0) ? 1 : ! (carrier = _cru_edge (z, c_label, NO_VERTEX, c, NO_NEXT_EDGE, err)))
	 return;
  else if (! _cru_received_by (CONNECT, carrier, sender, d, err))
	 _cru_free_edges_and_labels (z->e_free, carrier, err);
}








static void *
splitting_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively split the incoming nodes of a graph, and assist
	  // in updating the edges by responding accordingly to CONNECT,
	  // DISCONNECT, and CREATE instructions.
{
  cru_destructor_pair z;
  packet_list incoming;
  uintptr_t limit;            // maximum number of vertices allowed to be created by this worker
  uintptr_t count;            // number of vertices created by this worker
  unsigned sample;
  intptr_t status;
  node_set seen;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  router r;

  count = 0;
  sample = 0;
  seen = NULL;
  if ((! source) ? IER(1530) : (source->gruntled != PORT_MAGIC) ? IER(1531) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(1532) : (r->valid != ROUTER_MAGIC) ? IER(1533) : (killed = 0))
	 goto a;
  if ((!(d = source->peers)) ? IER(1534) : (r->tag != SPL) ? IER(1535) : 0)
	 return _cru_abort_status (source, d, err);
  limit = r->ro_sig.vertex_limit / r->lanes;
  limit = (limit ? limit : r->ro_sig.vertex_limit ? 1 : 0);
  if (limit)
	 for (n = source->survivors; n; n = n->next_node)
		count++;
  z = &(r->ro_sig.destructors);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(27);
		killed = (killed ? 1 : KILLED);
		if ((! (n = incoming->receiver)) ? IER(1536) : *err ? 1 : killed)
		  {
			 if (incoming->payload == CONNECT)
				_cru_free_edges_and_labels (z->e_free, incoming->carrier, err);
			 if (incoming->payload == CREATE)
				_cru_pushed_node (n, &(source->survivors), err);
		  }
		else if (incoming->payload == CONNECT)
		  _cru_push_edge (incoming->carrier, &(n->edges_out), err);
		else if (incoming->payload == DISCONNECT)
		  _cru_free_edges_and_labels (z->e_free, _cru_deleted_edge (incoming->carrier, &(n->edges_out), err), err);
		else if (incoming->payload != CREATE)
		  {
			 if (_cru_test_and_set_membership (n, &seen, err) ? 0 : ! *err)
				fan_out (n, &(r->splitter), d, &(source->survivors), z, err);
			 fan_in (n, incoming->sender, incoming->carrier, &(r->splitter), d, z, err);
		  }
		else if ((limit ? (count++ > limit) : 0) ? RAISE(CRU_INTOVF) : 1)
		  _cru_pushed_node (n, &(source->survivors), err);
		_cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
 a: status = *err;
  return (void *) status;
}








extern void
_cru_split (g, k, r, err)
	  cru_graph *g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Launch a split operation with a previously prepared
	  // router and block until finished.
{
  node_list b;

  _cru_disable_killing (k, err);
  if (*err ? 1 : (! g) ? IER(1537) : (! *g) ? 1 : (! (b = (*g)->base_node)) ? IER(1538) : 0)
	 goto a;
  if ((! r) ? IER(1539) : (r->valid != ROUTER_MAGIC) ? IER(1540) : (r->tag != SPL) ? IER(1541) : 0)
	 goto b;
  if ((! (r->ports)) ? IER(1542) : r->ro_sig.orders.v_order.hash ? 0 : IER(1543))
	 goto b;
  if (! _cru_set_properties (*g, k, r, err))
	 goto b;
  if (! _cru_launched (UNKILLABLE, b, _cru_shared (_cru_reset (r, (task) _cru_populating_task, err)), err))
	 {
		_cru_unset_properties (*g, r, err);
		goto b;
	 }
  (*g)->nodes = NULL;
  if (! *err)
	 _cru_status_launched (k, b, _cru_scalar_hash (b), _cru_reset (r, (task) splitting_task, err), err);
  _cru_clear_properties (r, err);
  if (*err ? 0 : _cru_pruned (*g, _cru_shared (r), k, err))
	 goto a;
 b: _cru_free_now (*g, err);
  *g = NULL;
 a: _cru_sweep (r, err);
}
