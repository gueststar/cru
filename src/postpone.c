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
#include <nthm/nthm.h>
#include "edges.h"
#include "errs.h"
#include "duplex.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "postpone.h"
#include "route.h"
#include "repl.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"

// unused
static int attach;

// a packet payload telling a worker to attach a new outgoing edge to a node in the graph
#define ATTACH &attach





static void
bipartition (n, postponable, stationary, movable, err)
	  node_list n;
	  cru_tpred postponable;
	  edge_list *stationary;
	  edge_list *movable;
	  int *err;

	  // Using the user-supplied postponable predicate, transfer
	  // postponable edges from the outgoing edges of n to the movable
	  // list and the rest fo the stationary list, except for the edges
	  // that point back to n, which should be left as they are. This
	  // operation is done in preparation for that of relocating the
	  // edges based on a test of every pair of stationary and movable
	  // edges.
{
  edge_list *e;
  int ux, ut;

  if ((! n) ? IER(1317) : (! postponable) ? IER(1318) : 0)
	 return;
  for (e = &(n->edges_out); *err ? NULL : *e;)
	 if ((*e)->remote.node == n)
		e = &((*e)->next_edge);
	 else if (! ((*e)->remote.node))
		IER(1319);
	 else if PASSED(postponable, n->vertex, (*e)->label, (*e)->remote.node->vertex)
		_cru_push_edge (_cru_popped_edge (e, err), movable, err);
	 else
		_cru_push_edge (_cru_popped_edge (e, err), stationary, err);
}









static int
postponed (l, e, s, d, z, err)
	  edge_list l;                 // one movable edge
	  edge_list e;                 // all stationary siblings of l
	  cru_cbop s;
	  packet_pod d;
	  cru_destructor_pair z;
	  int *err;

	  // Send a packet with an edge labeled by one derived from the
	  // labels of a movable edge l and a stationary edge in e for
	  // every pair meeting the user-supplied condition to the worker
	  // responsible for the node terminating the stationary edge. If
	  // tests are being run, allow an allocation error to be simulated
	  // at each call site to the test code.
{
  int result, ux, ut;
  edge_list carrier;
  void *ua;

  if ((! s) ? IER(1320) : (! (s->bpred)) ? IER(1321) : (! (s->bop)) ? IER(1322) : (! l) ? IER(1323) : (! z) ? IER(1324) : 0)
	 return 0;
  for (result = 0; *err ? NULL : e; e = e->next_edge)
	 if (PASSED(s->bpred, l->label, e->label))
		{
		  carrier = _cru_edge (z, APPLIED(s->bop, l->label, e->label), NO_VERTEX, l->remote.node, NO_NEXT_EDGE, err);
		  if ((! carrier) ? 0 : _cru_received_by (ATTACH, carrier, e->remote.node, d, err) ? (! (result = 1)) : 1)
			 _cru_free_edges_and_labels (z->e_free, carrier, err);
		}
  return result;
}









static void
postpone (n, p, d, z, err)
	  node_list n;
	  cru_postponer p;
	  packet_pod d;
	  cru_destructor_pair z;
	  int *err;

	  // Identify the stationary and movable edges outgoing from n,
	  // relocate the movable ones to the termini of relevant
	  // stationary ones, and leave the rest where they are.
{
  edge_list stationary, movable;

  if ((! n) ? IER(1325) : (! p) ? IER(1326) : (! z) ? IER(1327) : *err)
	 return;
  if ((! (n->edges_out)) ? 1 : ! (n->edges_out->next_edge))
	 return;
  stationary = movable = NULL;
  bipartition (n, p->postponable, &stationary, &movable, err);
  if (stationary ? 0 : ! movable)
	 return;
  if (! stationary)
	 goto a;
  while (movable)
	 if (postponed (movable, stationary, &(p->postponement), d, z, err))
		_cru_free_edges_and_labels (z->e_free, _cru_popped_edge (&movable, err), err);
	 else
		_cru_push_edge (_cru_popped_edge (&movable, err), &(n->edges_out), err);
  n->edges_out = _cru_cat_edges (n->edges_out, stationary);
  return;
  a: n->edges_out = _cru_cat_edges (n->edges_out, movable);
}










static void *
postponing_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively move edges around in the graph by responding to
	  // packets instructing the worker either to connect the incoming
	  // carrier to a receiving node or to send corresponding messages
	  // to workers responding to adjacent nodes.
{
  packet_list incoming;
  unsigned sample;
  intptr_t status;
  node_set seen;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  router r;

  sample = 0;
  seen = NULL;
  if ((! source) ? IER(1328) : (source->gruntled != PORT_MAGIC) ? IER(1329) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(1330) : (r->valid != ROUTER_MAGIC) ? IER(1331) : (killed = 0))
	 goto a;
  if ((!(d = source->peers)) ? IER(1332) : (r->tag != POS) ? IER(1333) : 0)
	 return _cru_abort_status (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(24);
		killed = (killed ? 1 : KILLED);
		if ((n = incoming->receiver) ? 0 : IER(1334))
		  goto b;
		if (incoming->payload == ATTACH)
		  _cru_push_edge (incoming->carrier, &(n->edges_postponed), err);
		else if (*err ? 0 : killed ? 0 : _cru_test_and_set_membership (n, &seen, err) ? 0 : ! *err)
		  {
			 _cru_scatter_out (n, d, err);
			 postpone (n, &(r->postponer), d, &(r->ro_sig.destructors), err);
		  }
	 b: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
 a: status = *err;
  return (void *) status;
}










static void *
absorbing_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively merge the postponed edges with the outgoing
	  // edges of every node, deduplicating as needed, and return
	  // non-zero if there are any postponed edges.
{
#define ALL_EDGES(x) _cru_cat_edges (x->edges_postponed, x->edges_out)

  uintptr_t changed, count;
  packet_list incoming;
  unsigned sample;
  node_set seen;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  router r;
  int ux;

  sample = 0;
  seen = NULL;
  changed = 0;
  if ((! source) ? IER(1335) : (source->gruntled != PORT_MAGIC) ? IER(1336) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(1337) : (r->valid != ROUTER_MAGIC) ? IER(1338) : (killed = 0))
	 goto a;
  if ((!(d = source->peers)) ? IER(1339) : (r->tag != POS) ? IER(1340) : 0)
	 goto b;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(25);
		killed = (killed ? 1 : KILLED);
		if ((! (n = incoming->receiver)) ? IER(1341) : _cru_member (n, seen))
		  goto c;
		if (! _cru_set_membership (n, &seen, err))
		  goto c;
		if (*err ? 1 : killed ? 1 : ! (n->edges_postponed))
		  goto d;
		count = (changed ? 0 : _cru_degree (n->edges_out));
		n->edges_out = _cru_deduplicated_edges (ALL_EDGES(n), &(r->ro_sig.orders.e_order), r->ro_sig.destructors.e_free, err);
		n->edges_postponed = NULL;
		changed = (changed ? 1 : (_cru_degree (n->edges_out) > count));
	 d: if (*err ? 0 : ! killed)
		  _cru_scatter_out (n, d, err);
		if (*err ? 0 : ! killed)
		  goto c;
		if (n->vertex ? r->ro_sig.destructors.v_free : NULL)
		  APPLY(r->ro_sig.destructors.v_free, n->vertex);
		n->vertex = NULL;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
 a: return (void *) changed;
 b: return _cru_abort (source, d, err);
}








static void
free_now (g, err)
	  cru_graph g;
	  int *err;

	  // Free a graph with postponed edges sequentially.
{
  node_list n;

  if (! g)
	 return;
  for (n = g->nodes; n; n = n->next_node)
	 {
		n->edges_out = _cru_cat_edges (n->edges_postponed, n->edges_out);
		n->edges_postponed = NULL;
	 }
  _cru_free_now (g, err);
}








static void
freeing_slacker (g)
	  cru_graph g;

	  // This function is used to free a graph with postponed edges in
	  // the background when passed to nthm_send.
{
  int err;

  err = 0;
  free_now (g, &err);
  _cru_globally_throw (err);
}









extern void
_cru_postpone (g, k, r, err)
	  cru_graph *g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Launch a postponement operation with a previously prepared
	  // router and block until finished. The operation requires two
	  // passes through the graph for each iteration, one to identify
	  // and transfer copies of the movable edges, and one to merge
	  // them into the extant outgoing edges of each node. If a fixed
	  // point is requested, the two passes are iterated until no
	  // further changes are detected.
{
  task t;
  router z;
  int dblx;
  node_list b;
  uintptr_t count;

  _cru_disable_killing (k, err);
  if (*err ? 1 : (! g) ? IER(1342) : (! *g))
	 goto a;
  b = (*g)->base_node;
  if ((! r) ? IER(1343) : (r->valid != ROUTER_MAGIC) ? IER(1344) : (r->tag != POS) ? IER(1345) : 0)
	 goto b;
  if (_cru_half_duplex (*g, err))
	 goto c;
  if ((z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err)) ? 0 : IER(1346))
	 goto b;
  if (! _cru_launched (k, b, z, err))
	 goto b;
 c: while (! *err)
	 {
		for (t = (task) postponing_task; t; t = ((t == (task) postponing_task) ? (task) absorbing_task : NULL))
		  _cru_count_launched (k, b, NO_PAYLOAD, _cru_scalar_hash (b), _cru_shared (_cru_reset (r, t, err)), &count, err);
		if (*err ? 0 : r->postponer.po_fix ? (! count) : 1)
		  goto a;
	 }
 b: dblx = 0;
  if ((*err != CRU_INTKIL) ? 1 : NOMEM ? 1 : ! nthm_send ((nthm_slacker) freeing_slacker, (void *) *g, &dblx))
	 free_now (*g, err);
  RAISE(dblx);
  *g = NULL;
 a: _cru_sweep (r, err);
}
