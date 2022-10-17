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
#include "brig.h"
#include "defo.h"
#include "duplex.h"
#include "edges.h"
#include "errs.h"
#include "filters.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "props.h"
#include "queue.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"
#include "zones.h"

// unused integers whose addresses are used

static int cut_back;
static int cut_forward;

// specialized messages between filtering workers differing from any possible pointer to the heap

#define CUT_BACK &cut_back
#define CUT_FORWARD &cut_forward



// --------------- initialization --------------------------------------------------------------------------



void *
_cru_populating_task (source, err)
	  port source;
	  int *err;

	  // Exhaustively populate the survivors in every
	  // port. Cf. guarding_task in compose.c.
{
#define RECORDED(x) (_cru_member (x, seen) ? 1 : (! *err) ? 0 : _cru_listed (x, source->survivors))

  router r;
  cru_prop i;
  node_list n;
  packet_pod d;
  node_set seen;
  intptr_t status;
  packet_list incoming;
  struct packet_list_s buffer;          // needed if only if there's not enough memory to allocate a packet

  d = NULL;
  seen = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! source) ? IER(923) : (source->gruntled != PORT_MAGIC) ? IER(924) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(925) : (r->valid != ROUTER_MAGIC) ? IER(926) : 0)
	 return NULL;
  if ((! (d = source->peers)) ? IER(927) : 0)
	 return _cru_abort_status (source, d, err);
  i = ((r->tag == EXT) ? &(r->stretcher.st_prop) : (r->tag == SPL) ? &(r->splitter.sp_prop) : NULL);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		if ((n = (node_list) incoming->payload) ? RECORDED(n) : IER(928))
		  goto a;
		if (*err ? 0 : _cru_scattered (n->edges_out, d, err))
		  goto b;
		if (! _cru_unscatterable (&(n->edges_out), n, r->ro_sig.destructors.e_free, d, err))
		  goto b;
		if (_cru_buffered (&buffer, incoming, &(d->deferrals), err))
		  goto c;
		if (_cru_transplanted_out ((node_list) buffer.payload, n, err))
		  goto b;
		if (! _cru_spun (&buffer, &incoming, &(d->deferrals), err))
		  goto c;
		continue;
	 b: if ((incoming != &buffer) ? (n == buffer.payload) : 0)
		  goto c;
		_cru_set_membership (_cru_half_severed (n), &seen, err);
		_cru_pushed_node (n, &(source->survivors), err);
	 a: if (_cru_unbuffered (&buffer, &incoming, err))
		  continue;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  if (*err ? i : NULL)
	 source->survivors = _cru_unpropped (source->survivors, i->vertex.m_free, err);
  _cru_forget_members (seen);
  status = *err;
  return (void *) status;
}








// --------------- filtering -------------------------------------------------------------------------------







static int
is_deletable (v, n, err)
	  cru_prop v;
	  node_list n;
	  int *err;

	  // Return non-zero iff a node should be deleted according to user
	  // supplied code v. Deletion is indicated when v returns NULL. If
	  // v is empty then all nodes are retained.
{
  void *undeletable;
  int ux;

  if (*err ? 1 : _cru_empty_prop (v) ? 1 : v ? 0 : IER(929))
	 return 0;
  if ((undeletable = _cru_mapped_node (v, n, err)) ? v->vertex.m_free : NULL)
	 APPLY(v->vertex.m_free, undeletable);
  return ! undeletable;
}








static void *
node_filtering_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively initialize the deleted node sets in the ports
	  // without deleting any edges or nodes. This has to be done first
	  // because testing the nodes for deletability depends on the
	  // presence of the edges.
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
  if ((! source) ? IER(930) : (source->gruntled != PORT_MAGIC) ? IER(931) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(932) : (r->valid != ROUTER_MAGIC) ? IER(933) : (killed = 0))
	 goto a;
  if ((!(d = source->peers)) ? IER(934) : (r->tag != FIL) ? IER(935) : 0)
	 return _cru_abort_status (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(13);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : (! (n = incoming->receiver)) ? IER(936) : 0)
		  goto c;
		if (source->reachable ? (! _cru_member (n, source->reachable)) : 0)
		  goto c;
		if (_cru_test_and_set_membership (n, &seen, err))
		  goto c;
		if (is_deletable (&(r->filter.fi_kernel.v_op), n, err) ? (! *err) : 0)
		  _cru_set_membership (n, &(source->deleted), err);
		if (r->filter.fi_zone.backwards)
		  _cru_scatter_in (n, d, err);
		else
		  _cru_scatter_out (n, d, err);
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
		if (killed ? 0 : *err ? (killed = 1) : 0)
		  _cru_kill_internally (&(r->killed), err);
	 }
  _cru_forget_members (seen);
  status = *err;
 a: return (void *) status;
}








static int
disconnection_acknowledged (incoming, c, err)
	  packet_list incoming;
	  edge_list *c;            // list of disconnected edges with reclamation deferred to avoid conflicts
	  int *err;

	  // Act on a message from another worker indicating that a remote
	  // node is to be deleted by disconnecting an edge touching it. In
	  // the case of forward traversals, the edge to be deleted can be
	  // detected by pointer equality with the incoming carrier. For
	  // backward traversals, the edge to be deleted has to be detected
	  // by comparison with the carrier's label and terminus.
{
  edge_list e;

  if ((! incoming) ? IER(937) : (! (incoming->receiver)) ? IER(938) : (! c) ? IER(939) : 0)
	 return 0;
  if (incoming->payload == CUT_FORWARD)
	 e = _cru_deleted_edge (incoming->carrier, &(incoming->receiver->edges_out), err);
  else if (incoming->payload == CUT_BACK)
	 e = _cru_disconnections (incoming->sender, incoming->carrier, &(incoming->receiver->edges_out), err);
  else
	 return 0;
  *c = _cru_cat_edges (e, *c);
  return 1;
}










static int
individually_filtered (test, t, v, e, c, err)
	  cru_fold test;
	  cru_qpred t;     // thinner relational predicate
	  void *v;
	  edge_list *e;
	  edge_list *c;    // failing edges
	  int *err;

	  // Retain the edges in a list that pass the test. Test each one
	  // individually using the map function in the fold without
	  // reducing them. If a relational predicate is defined, retain
	  // only the minumum edge.
{
  edge_list p;
  void *pass;
  void *ua;
  int ux;

  if ((! e) ? IER(940) : (! c) ? IER(941) : (! test) ? IER(942) : test->map ? 0 : IER(943))
	 return 0;
  for (p = NULL; *e;)
	 if (((*e)->remote.node ? 0 : IER(944)) ? 1 : ! (pass = APPLIED(test->map, v, (*e)->label, (*e)->remote.node->vertex)))
		*c = _cru_cat_edges (_cru_popped_edge (e, err), *c);
	 else if ((p = _cru_cat_edges (_cru_popped_edge (e, err), p)) ? test->m_free : NULL)
		APPLY(test->m_free, pass);
  if (! t)
	 *e = p;
  else if (p)
	 {
		*e = _cru_minimum_edge (t, &p, err);
		*c = _cru_cat_edges (p, *c);
	 }
 a: return ! *err;
}









static int
jointly_filtered (test, n, b, c, err)
	  cru_fold test;
	  node_list n;
	  brigade b;
	  edge_list *c;    // failing edges
	  int *err;

	  // Retain the edges in buckets whose reduction passes the test.
{
  edge_list e;
  void *pass;
  brigade o;
  void *ua;
  int ux;

  o = b;
  if ((! n) ? IER(945) : n->edges_out ? IER(946) : (! test) ? IER(947) : (! c) ? IER(948) : 0)
	 return 0;
  for (e = NULL; b; b = b->other_buckets)
	 if (! (pass = _cru_reduced_edges (test, n->vertex, b->bucket, err)))
		*c = _cru_cat_edges (b->bucket, *c);
	 else
		{
		  if (test->r_free)
			 APPLY(test->r_free, pass);
		  e = _cru_cat_edges (b->bucket, e);
		}
  _cru_free_brigade (o, err);
  n->edges_out = e;
 b: return ! *err;
}






static int
jointly_thinned (test, t, n, b, c, err)
	  cru_fold test;
	  cru_qpred t;     // thinner relational predicate
	  node_list n;
	  brigade b;
	  edge_list *c;    // failing edges
	  int *err;

	  // Retain the edges in the minimum bucket of whose members the
	  // reduction passes the test.
{
  brigade passing, minimum;
  void *pass;
  void *ua;
  int ux;

  if ((! n) ? IER(949) : n->edges_out ? IER(950) : (! test) ? IER(951) : (! c) ? IER(952) : (! t) ? IER(953) : 0)
	 return 0;
  for (passing = NULL; b;)
	 if (! (pass = _cru_reduced_edges (test, n->vertex, b->bucket, err)))
		{
		  *c = _cru_cat_edges (b->bucket, *c);
		  _cru_free_brigade (_cru_popped_bucket (&b, err), err);
		}
	 else
		{
		  if (test->r_free)
			 APPLY(test->r_free, pass);
		  passing = _cru_cat_brigades (_cru_popped_bucket (&b, err), passing);
		}
  minimum = (passing ? _cru_minimum_bucket (t, &passing, err) : NULL);
  n->edges_out = (minimum ? minimum->bucket : NULL);
  _cru_free_brigade (minimum, err);
  for (b = passing; b; b = b->other_buckets)
	 *c = _cru_cat_edges (b->bucket, *c);
  _cru_free_brigade (passing, err);
 b: return ! *err;
}












static int
filtered (n, f, d, c, err)
	  node_list n;
	  cru_filter f;
	  packet_pod d;
	  edge_list *c;  // failing edges to be deleted
	  int *err;

	  // Delete the outgoing edges from a node n that fail the filter
	  // test, either by testing them individually or in equivalence
	  // classes as requested, returning non-zero if successful.
{
  brigade b;
  cru_order r;
  cru_fold test;
  int unordered;

  if ((! n) ? IER(954) : (! f) ? IER(955) : (! c) ? IER(956) : *err)
	 return 0;
  if (_cru_empty_fold (test = &(f->fi_kernel.e_op)) ? (! (f->thinner)) : 0)       // no edges are to be deleted
	 return 1;
  unordered = _cru_empty_order (r = &(f->fi_order));
  if ((! unordered) ? 0 : (! (test->vacuous_case)) ? 1 : (test->vacuous_case == (cru_nop) _cru_undefined_nop))
	 return individually_filtered (test, f->thinner, n->vertex, &(n->edges_out), c, err);
  b = (unordered ? _cru_bundled (UNRELATED, &(n->edges_out), err) : _cru_rallied (r->hash, r->equal, &(n->edges_out), err));
  if (! (n->edges_out))
	 return (f->thinner ? jointly_thinned (test, f->thinner, n, b, c, err) : jointly_filtered (test, n, b, c, err));
  *c = _cru_cat_edges (*c, n->edges_out);
  n->edges_out = NULL;
}











static void
request_disconnection (incoming, backwards, d, err)
	  packet_list incoming;
	  int backwards;           // non-zero means the traversal is oppositely directed along the edges
	  packet_pod d;
	  int *err;

	  // Inform workers responsible for adjacent nodes that the
	  // receiver is to be deleted. If the traversal direction is
	  // forward, then the worker responsible for the receiver has
	  // received a packet whose sender is connected to the receiver by
	  // an outgoing edge from the sender, which is the carrier, so
	  // it's necessary to reply to the worker responsible for the
	  // sender with a message telling it to cut the carrier from the
	  // sender's outgoing edges. Only one message is needed in this
	  // case because it will happen again for each relevant sender.
	  // If the traversal direction is backwards, messages must be sent
	  // to the workers responsible for all predecessors of the
	  // receiver requesting that their outgoing edges to this receiver
	  // be deleted.
{
  edge_list e;

  if ((! incoming) ? IER(957) : (! (incoming->receiver)) ? IER(958) : 0)
	 return;
  if (! backwards)
	 _cru_received_by (CUT_FORWARD, incoming->carrier, incoming->sender, d, err);
  else
	 for (e = incoming->receiver->edges_in; e; e = e->next_edge)
		_cru_send_from (CUT_BACK, e, incoming->receiver, d, err);
}









static void *
edge_filtering_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively detect all edges that either fail a
	  // user-specified test or are connected to deletable nodes, and
	  // move them to the disconnections list in each port.
{
  packet_list incoming;
  unsigned sample;
  intptr_t status;
  node_set seen;
  packet_pod d;
  node_list n;
  int killed;                 // non-zero when the job is killed
  router r;
  int m;

  sample = 0;
  killed = 0;
  seen = NULL;
  if ((! source) ? IER(959) : (source->gruntled != PORT_MAGIC) ? IER(960) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(961) : (r->valid != ROUTER_MAGIC) ? IER(962) : 0)
	 goto a;
  if ((!(d = source->peers)) ? IER(963) : (r->tag != FIL) ? IER(964) : 0)
	 return _cru_abort (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(14);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : ! (n = incoming->receiver))  // could be null due to heap overflow in another thread
		  goto b;
		if (disconnection_acknowledged (incoming, &(source->disconnections), err) ? 1 : *err)
		  goto b;
		if (source->reachable ? (! _cru_member (n, source->reachable)) : 0)
		  goto b;
		m = _cru_member (n, source->deleted);
		if (_cru_test_and_set_membership (n, &seen, err))
		  goto c;
		if (! m)
		  {
			 if ((! (r->filter.thinner)) ? 1 : is_deletable (&(r->filter.fi_kernel.v_op), n, err))
				filtered (n, &(r->filter), d, &(source->disconnections), err);
		  }
		else
		  {
			 source->disconnections = _cru_cat_edges (n->edges_out, source->disconnections);
			 n->edges_out = NULL;
			 goto d;
		  } 
		if (r->filter.fi_zone.backwards)
		  _cru_scatter_in (n, d, err);
		else
		  _cru_scatter_out (n, d, err);
	 c: if (r->filter.thinner ? 1 : ! m)
		  goto b;
	 d: request_disconnection (incoming, r->filter.fi_zone.backwards, d, err);
	 b: _cru_nack (_cru_popped_packet (&incoming, err), err);
		if (killed ? 0 : *err ? (killed = 1) : 0)
		  _cru_kill_internally (&(r->killed), err);
	 }
  _cru_forget_members (seen);
 a: status = *err;
  return (void *) status;
}













router
_cru_filtered (g, r, k, err)
	  cru_graph *g;
	  router r;
	  cru_kill_switch k;
	  int *err;

	  // Get rid of everything that should be filtered out of a graph
	  // according to user supplied criteria in preparation for the
	  // next pass that prunes whatever is rendered unreachable. If the
	  // base node is reachable and deletable, then free the whole
	  // graph. If not, clear the graph node list, which might
	  // otherwise lead to dangling pointers. The algorithm requires a
	  // full duplex graph so add incident edges if necessary.
	  //
	  // Forward traversals must always start from the base node even
	  // if an alternative initial node has been specified, so as to
	  // prevent dangling forward edges into the zone from outside it.
{
  node_list i;    // initial node specified by user code, which determines the reachable zone for filtering
  node_list b;    // base node from which all nodes in the graph are reachable
  node_set s;     // the set of reachable nodes recorded in the port associated with the base node
  router z;
  task t;

  _cru_disable_killing (k, err);
  if (*err ? 1 : (! g) ? IER(965) : (! *g) ? IER(966) : (! r) ? IER(967) : (r->valid != ROUTER_MAGIC) ? IER(968) : 0)
	 goto a;
  if (((! (r->ports)) ? IER(969) : (! (r->lanes)) ? IER(970) : 0) ? (r->valid = MUGGLE(14)) : 0)
	 goto a;
  if ((r->tag != FIL) ? IER(971) : ! (i = _cru_initial_node (*g, k, r, err)))
	 goto a;
  s = (r->ports[_cru_scalar_hash (b = (*g)->base_node) % r->lanes])->reachable;
  if (! ((s ? _cru_member (b, s) : 1) ? r->filter.thinner ? 0 : is_deletable (&(r->filter.fi_kernel.v_op), b, err) : 0))
	 goto b;
  if (r->lanes == 1)
	 goto c;
  if (_cru_launched (UNKILLABLE, b, _cru_shared (_cru_reset (r, (task) _cru_freeing_task, err)), err))
	 (*g)->nodes = NULL;
  goto c;
 b: if (! _cru_half_duplex (*g, err))
	 goto e;
  if (_cru_launched (k, b, _cru_router ((task) _cru_full_duplexing_task, r->lanes, err), err) ? *err : 0)
	 goto c;
 e: if (_cru_launched (UNKILLABLE, b, _cru_shared (_cru_reset (r, (task) _cru_populating_task, err)), err))
	 (*g)->nodes = NULL;
  if (*err)
	 goto c;
  i = (r->ro_plan.zone.backwards ? i : b);
  if (r->filter.thinner ? 1 : _cru_empty_prop (&(r->filter.fi_kernel.v_op)))
	 t = (task) edge_filtering_task;
  else
	 t = (task) node_filtering_task;
  for (; t; t = ((t == (task) node_filtering_task) ? (task) edge_filtering_task : NULL))
	 if (! _cru_status_launched (k, i, _cru_scalar_hash (i), _cru_reset (r, t, err), err))
		goto c;
  z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err);
  if (_cru_launched (k, b, z, err) ? (! *err) : 0)
	 goto a;
 c: _cru_free_now (*g, err);
  *g = NULL;
 a: return r;
}






// --------------- pruning ---------------------------------------------------------------------------------








static node_queue
pruning_task (source, err)
	  port source;
	  int *err;

	  // Return a queue of every received node, severing each from the
	  // port's list of survivors.
{
  router r;
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
  if ((! source) ? IER(972) : (source->gruntled != PORT_MAGIC) ? IER(973) : (killed = 0))
	 return NULL;
  if ((!(r = source->local)) ? IER(974) : (r->valid != ROUTER_MAGIC) ? IER(975) : 0)
	 return NULL;
  if (((d = source->peers)) ? 0 : IER(976))
	 return (node_queue) _cru_abort (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(15);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : (n = (node_list) incoming->payload) ? 0 : IER(977))
		  goto a;
		if (_cru_member (n, seen) ? 1 : ! _cru_set_membership (n, &seen, err))
		  goto a;
		if (_cru_enqueued_node (_cru_severed (n, err), &q, err))
		  _cru_scattered (n->edges_out, d, err);
		else
		  _cru_pushed_node (n, &(source->survivors), err);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  if (killed ? 0 : ! *err)
	 goto d;
  _cru_free_node_queue (q, &(r->ro_sig.destructors), err);
  _cru_free_nodes (source->survivors, &(r->ro_sig.destructors), err);
  source->survivors = NULL;
 d: _cru_forget_members (seen);
  return (*err ? NULL : killed ? NULL : q);
}








int
_cru_pruned (g, r, k, err)
	  cru_graph g;
	  router r;
	  cru_kill_switch k;
	  int *err;

	  // Make a second pass through the graph to get rid of the nodes
	  // made unreachable by filtering, and consume the router. The
	  // graph's node list is assumed to be null or invalid on entry
	  // even though the base node is still required, with the nodes
	  // actually held temporarily in the survivors lists in the
	  // router's ports. The graph has to be made half duplex to avoid
	  // dangling back edges from nodes that are deleted due to being
	  // unreachable.
{
  router z;

  if (*err ? 1 : (! r) ? IER(978) : (r->valid != ROUTER_MAGIC) ? IER(979) : (! g) ? 1 : g->base_node ? 0 : IER(980))
	 goto a;
  if (((! (r->ports)) ? IER(981) : (! (r->lanes)) ? IER(982) : 0) ? (r->valid = MUGGLE(15)) : 0)
	 goto a;
  if (_cru_half_duplex (g, err))
	 goto b;
  z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err);
  if (_cru_launched (k, g->base_node, z, err))
	 goto a;
 b: _cru_reset (r, (task) pruning_task, err);
  if (_cru_queue_launched (k, g->base_node, _cru_scalar_hash (g->base_node), r, &(g->nodes), err))
	 if (g->nodes)
		g->nodes->previous = &(g->nodes);
 a: _cru_sweep (r, err);
  return ! *err;
}
