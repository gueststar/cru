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

#include <string.h>
#include "cthread.h"
#include "defo.h"
#include "duplex.h"
#include "edges.h"
#include "emu.h"
#include "errs.h"
#include "getset.h"
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
#include "sync.h"
#include "table.h"
#include "zones.h"
#include "wrap.h"

// Unordered means vertices can be mutated in any order.
#define UNORDERED (r->mutator.mu_plan.remote_first ? 0 : ! (r->mutator.mu_plan.local_first))

// Forward propagating means the origin of each edge is mutated before its terminus.
#define FORWARD_PROPAGATING ((! (r->mutator.mu_plan.zone.backwards)) == ! (r->mutator.mu_plan.remote_first))

// The prerequisites of a node are the edges connecting it with the nodes that must be mutated before it.
#define PREREQUISITES(x) (UNORDERED ? NULL : FORWARD_PROPAGATING ? (x)->edges_in : (x)->edges_out)

// The dependants of a node are the edges connecting it with the nodes that can't be mutated until after it is.
#define DEPENDANTS(x) (FORWARD_PROPAGATING ? (x)->edges_out : (x)->edges_in)

// The next definitions are of possible values of the marked field in a node during mutation.

// Mutated means the node has been mutated.
#define MUTATED (void *) 1

// Unreachable means the node is outside the zone of mutable nodes.
#define UNREACHABLE (void *) 2

// Blocked means an error occurred while the node was being mutated.
#define BLOCKED (void *) 3




// --------------- pre-mutation phase ----------------------------------------------------------------------





static void *
coverage_analyzing_task (source, err)
	  port source;
	  int *err;

	  // Raise an error if not every vertex is reachable. This test is
	  // needed only if a mutation changes the vertex or edge
	  // destructor but doesn't cover the whole graph.
{
  packet_pod destinations;    // for outgoing packets
  packet_list incoming;       // incoming packets
  unsigned sample;
  intptr_t status;
  node_set seen;
  node_list n;
  int killed;
  router r;

  sample = 0;
  killed = 0;
  seen = NULL;
  if ((! source) ? IER(1195) : (source->gruntled != PORT_MAGIC) ? IER(1196) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(1197) : (r->valid != ROUTER_MAGIC) ? IER(1198) : 0)
	 return NULL;
  if (! (destinations = source->peers) ? IER(1199) : *err)
	 return _cru_abort_status (source, destinations, err);
  _cru_set_storage (r->ro_store, err);
  _cru_set_kill_switch (&(r->killed), err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 {
		KILL_SITE(22);
		killed = (killed ? 1 : KILLED);
		if (killed)
		  goto a;
		if ((n = (node_list) incoming->payload) ? (_cru_test_and_set_membership (n, &seen, err) ? 1 : *err) : IER(1200))
		  goto a;
		_cru_scattered (n->edges_out, destinations, err);
		if (! _cru_member (n, source->reachable))
		  RAISE(CRU_TPCMUT);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  status = *err;
  return (void *) status;
}






static void
bound (n, r, i, o, s, d, np, err)
	  node_list n;
	  node_set r;         // set of reachable nodes
	  int i;              // non-zero means n is independent of incoming edges
	  int o;              // non-zero means n is independent of outgoing edges
	  node_list *s;
	  node_list *d;
	  int np;             // non-zero means the node has no prerequisites
	  int *err;

	  // Store nodes n on the initial boundary of the mutable region in
	  // s, and interior or unreachable nodes in d.
{
  if ((! n) ? IER(1201) : 0)
	 return;
  if ((! r) ? NULL : _cru_member (n, r) ? NULL : (n->marked = UNREACHABLE))
	 _cru_pushed_node (n, d, err);
  else if (np ? 1 : (i ? 1 : ! (n->edges_in)) ? (o ? 1 : ! (n->edges_out)) : 0)
	 _cru_pushed_node (n, s, err);
  else
	 _cru_pushed_node (n, d, err);
}






static void *
bounding_task (source, err)
	  port source;
	  int *err;

	  // Assuming reachability analysis has been done already, receive
	  // nodes from other workers, store those with no prerequisites in
	  // the survivors list, and store the rest in the deletions list
	  // of the port.
{
#define RECORDED(x) (\
  _cru_member (x, seen) ? 1 : \
  (! *err) ? 0 : \
  _cru_listed (x, source->survivors) ? 1 : \
  _cru_listed(x, source->deletions))

  int i;                // non-zero means vertex mutations don't depend on incident edges
  int o;                   // non-zero means vertex mutations don't depend on outgoing edges
  router r;
  int killed;
  node_list n;
  node_list *s;
  packet_pod d;
  node_set seen;
  intptr_t status;
  unsigned sample;
  packet_list incoming;
  struct packet_list_s buffer;          // needed if only if there's not enough memory to allocate a packet

  d = NULL;
  killed = 0;
  sample = 0;
  seen = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! source) ? IER(1202) : (source->gruntled != PORT_MAGIC) ? IER(1203) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(1204) : (r->valid != ROUTER_MAGIC) ? IER(1205) : 0)
	 return NULL;
  if ((! (d = source->peers)) ? IER(1206) : (r->tag != MUT) ? IER(1207) : 0)
	 return _cru_abort_status (source, d, err);
  _cru_set_storage (r->ro_store, err);
  _cru_set_kill_switch (&(r->killed), err);
  i = o = 0;
  if (! _cru_empty_fold (&(r->mutator.mu_kernel.e_op)))
	 if (! _cru_empty_prop (&(r->mutator.mu_kernel.v_op)))
		goto a;
  i = _cru_empty_fold (&(r->mutator.mu_kernel.v_op.incident));
  o = _cru_empty_fold (&(r->mutator.mu_kernel.v_op.outgoing));
 a: for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(23);
		killed = (killed ? 1 : KILLED);
		if (((n = (node_list) incoming->payload)) ? RECORDED(n) : IER(1208))
		  goto b;
		if (*err ? 0 : killed ? 0 : _cru_scattered (n->edges_out, d, err))
		  goto c;
		if (! _cru_unscatterable (&(n->edges_out), n, r->ro_sig.destructors.e_free, d, err))
		  goto c;
		if (_cru_buffered (&buffer, incoming, &(d->deferrals), err))
		  goto d;
		if (_cru_transplanted_out ((node_list) buffer.payload, n, err))
		  goto c;
		if (! _cru_spun (&buffer, &incoming, &(d->deferrals), err))
		  goto d;
		continue;
	 c: if ((incoming != &buffer) ? (n == buffer.payload) : 0)
		  goto d;
		_cru_set_membership (_cru_half_severed (n), &seen, err);
		bound (n, source->reachable, i, o, &(source->survivors), &(source->deletions), ! PREREQUISITES(n), err);
	 b: if (_cru_unbuffered (&buffer, &incoming, err))
		  continue;
	 d: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  status = *err;
  return (void *) status;
}





// --------------- mutations -------------------------------------------------------------------------------





static void
permeate (e, mutated_vertex, o, z, fwd, blocked, err)
	  edge_list e;
	  void *mutated_vertex;
	  cru_fold o;
	  cru_destructor_pair z;
	  int fwd;
	  int blocked;
	  int *err;

	  // Mutate the edge labels in e. Clear whatever can't be mutated.
{
#define MAP(w,x,y,z) (w->bmap ? APPLIED(w->bmap, x, y) : APPLIED(w->map, x, y, z))

  void *mutated_edge;
  edge_list t;
  void *ua;
  int ux;

  if ((! o) ? IER(1209) : (! (o->map ? 1 : ! ! (o->bmap))) ? IER(1210) : (! z) ? IER(1211) : 0)
	 return;
  for (t = e; *err ? NULL : e; e = e->next_edge)
	 {
		if (e->remote.node ? 0 : IER(1212))
		  break;
		mutated_edge = (blocked ? NULL : MAP(o, mutated_vertex, e->label, e->remote.node->vertex));
		if (*err)
		  {
			 if (o->m_free ? mutated_edge : NULL)
				APPLY(o->m_free, mutated_edge);
			 break;
		  }
		if ((! fwd) ? NULL : z->e_free ? e->label : NULL)
		  APPLY(z->e_free, e->label);
		e->label = mutated_edge;
	 }
  if (fwd)
	 _cru_free_labels (z->e_free, e, err);    // get rid of labels remaining unmutated
  if (*err ? fwd : 0)
	 _cru_free_labels (o->m_free, t, err);
}









static void
reflect (n, k, z, fwd, err)
	  node_list n;
	  cru_kernel k;
	  cru_destructor_pair z;
	  int fwd;
	  int *err;

	  // Synchronize edge labels with corresponding edges in the
	  // opposite directions.
{
  edge_list e, f;
  uintptr_t i;
  int ux;

  for (e = (fwd ? n->edges_out : n->edges_in); e; e = e->next_edge)
	 {
		if (e->remote.node ? 0 : IER(1213))
		  goto a;
		i = 0;
		for (f = fwd ? n->edges_out : n->edges_in; f != e; f = f->next_edge)
		  i += (f->remote.node == e->remote.node);
		f = (fwd ? e->remote.node->edges_in : e->remote.node->edges_out);
		do
		  {
			 for (; f ? (f->remote.node != n) : 0; f = f->next_edge);
			 if (f ? 0 : IER(1214))
				goto a;
			 f = (i ? f->next_edge : f);
		  }
		while (i--);
		if (fwd ? NULL : f->label ? z->e_free : NULL)
		  APPLY(z->e_free, f->label);
		f->label = e->label;
	 a: continue;
	 }
}










static void
undo_reflection (n, d, fwd, err)
	  node_list n;
	  cru_destructor d;
	  int fwd;
	  int *err;

	  // Get rid of all reflected labels assuming they've been mutated.
{
  int ux;
  edge_list e, f;

  if (n ? (! fwd) : ! IER(1215))
	 for (e = n->edges_in; e; e = e->next_edge)
		if (e->remote.node ? 1 : ! IER(1216))
		  for (f = e->remote.node->edges_out; f; f = f->next_edge)
			 if (f->remote.node == n)
				{
				  if (d ? f->label : NULL)
					 APPLY(d, f->label);
				  f->label = NULL;
				}
}








static void
mutate (n, k, z, fwd, blocked, err)
	  node_list n;
	  cru_kernel k;
	  cru_destructor_pair z;
	  int fwd;
	  int blocked;
	  int *err;

	  // Mutate an individual node and its edges in the direction of
	  // propagation assuming its prerequistes have been mutated.
{
  void *mutated_vertex;
  int mtv;                 // non-zero means no vertex mutation is defined
  int ux;

  if ((! k) ? IER(1217) : (! z) ? IER(1218) : (! n) ? IER(1219) : 0)
	 return;
  mtv = _cru_empty_prop (&(k->v_op));
  mutated_vertex = ((blocked ? 1 : mtv ? 1 : *err) ? NULL : _cru_mapped_node (&(k->v_op), n, err));
  _cru_lock_for_writing (&(n->marked), err);
  if (! _cru_empty_fold (&(k->e_op)))
	 {
		permeate (fwd ? n->edges_out : n->edges_in, mtv ? n->vertex : mutated_vertex, &(k->e_op), z, fwd, blocked, err);
		reflect (n, k, z, fwd, err);
		if (! *err)
		  goto b;
		undo_reflection (n, NO_LABEL_DESTRUCTOR, fwd, err);
		goto a;
	 }
 b: if (*err ? 1 : fwd ? 0 : blocked)
	 goto a;
  if (mtv ? NULL : n->vertex ? z->v_free : NULL)
	 APPLY(z->v_free, n->vertex);
  if (! mtv)
	 n->vertex = mutated_vertex;
  mutated_vertex = NULL;
 a: _cru_unlock_for_reading (&(n->marked), (*err ? 1 : blocked) ? BLOCKED : MUTATED, err);
  if (! *err)
	 return;
  if (mutated_vertex ? k->v_op.vertex.m_free : NULL)
	 APPLY(k->v_op.vertex.m_free, mutated_vertex);
}







// --------------- mutation phase --------------------------------------------------------------------------







static void
start (s, d, k, z, fwd, t, err)
	  node_list *s;
	  node_list *d;
	  cru_kernel k;
	  cru_destructor_pair z;
	  int fwd;
	  packet_pod t;
	  int *err;

	  // Mutate the nodes in s assuming they have no prerequisites. Move
	  // whatever can't be mutated to d.
{
#define NOT_BLOCKED 0

  node_list *m;

  if (d ? 0 : IER(1220))
	 return;
  for (m = s; *m; m = &((*m)->next_node))
	 {
		mutate (*m, k, z, fwd, NOT_BLOCKED, err);
		if (*err)
		  {
			 _cru_free_labels (k->e_op.m_free, (*m)->edges_out, err);
			 break;
		  }
		_cru_scattered (fwd ? (*m)->edges_out : (*m)->edges_in, t, err);
	 }
  if ((*d = _cru_cat_nodes (*m, *d)))
	 (*d)->previous = d;
  *m = NULL;
}









static int
all_quiescent (e, blocked, err)
	  edge_list e;
	  int *blocked;
	  int *err;

	  // Return non-zero if all adjacent nodes are mutated, blocked, or
	  // unreachable.
{
  void *t;

  if (blocked ? 0 : IER(1221))
	 return 0;
  for (*blocked = 0; e; e = e->next_edge)
	 {
		if (e->remote.node ? 0 : IER(1222))
		  goto a;
		if ((t = _cru_read (&(e->remote.node->marked), err)) == BLOCKED)
		  *blocked = 1;
		else if (! t)
		  goto a;
	 }
  return 1;
 a: return 0;
}







static void *
mutating_task (s, err)
	  port s;
	  int *err;

	  // Concurrently mutate the edges and vertices stored in one port
	  // when their prerequisites are ready.
{
  router r;
  node_list n;
  packet_pod d;
  cru_kernel k;
  unsigned sample;
  intptr_t status;
  packet_list incoming;
  int started, killed, blocked;

  if ((! s) ? IER(1223) : (s->gruntled != PORT_MAGIC) ? IER(1224) : (int) (sample = 0))
	 goto a;
  if ((! (r = s->local)) ? IER(1225) : (r->valid != ROUTER_MAGIC) ? IER(1226) : (killed = 0))
	 goto a;
  if ((!(d = s->peers)) ? IER(1227) : (r->tag != MUT) ? IER(1228) : (started = 0))
	 return _cru_abort_status (s, d, err);
  k = &(r->mutator.mu_kernel);
  _cru_set_storage (r->ro_store, err);
  _cru_set_kill_switch (&(r->killed), err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, d, err));)
	 {
		KILL_SITE(24);
		killed = (killed ? killed : KILLED);
		if ((n = (node_list) incoming->payload) ? 0 : ((s->own_index + 1) < r->lanes))
		  _cru_ping (r->ports[s->own_index + 1], err);
		if (started ? 0 : ++started)
		  start (&(s->survivors), &(s->deletions), k, &(r->ro_sig.destructors), FORWARD_PROPAGATING, d, err);
		if ((! n) ? 1 : ! ! (n->marked))
		  goto b;
		if (*err ? 0 : killed ? 0 : all_quiescent (PREREQUISITES(n), &blocked, err))
		  {
			 mutate (n, k, &(r->ro_sig.destructors), FORWARD_PROPAGATING, blocked, err);
			 if (*err ? 0 : ! blocked)
				_cru_pushed_node (_cru_severed (n, err), &(s->survivors), err);
			 if (*err ? 1 : killed)
				_cru_free_labels (k->e_op.m_free, n->edges_out, err);
			 else
				_cru_scattered (DEPENDANTS(n), d, err);
		  }
		if (killed ? 1 : *err ? _cru_killed (&(r->killed), err) : 1)
		  goto b;
		_cru_kill_internally (&(r->killed), err);
	 b: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_throw (r, err);
 a: status = *err;
  return (void *) status;
}




// --------------- post-mutation phase ---------------------------------------------------------------------



static void *
queuing_task (s, err)
	  port s;
	  int *err;

	  // Concurrently return an unmarked half-duplex queue of the
	  // survivors and deletions in a port.
{
  node_queue q, u;
  node_list n;
  router r;

  if ((! s) ? IER(1229) : (s->gruntled != PORT_MAGIC) ? IER(1230) : ! _cru_pingback (s, err))
	 return NULL;
  if ((! (r = s->local)) ? IER(1231) : (r->valid != ROUTER_MAGIC) ? IER(1232) : 0)
	 return NULL;
  if (*err ? 1 : ! (q = _cru_queue_of (s->deletions, err)))
	 goto a;
  s->deletions = NULL;
  if (! (u = _cru_queue_of (s->survivors, err)))
	 goto b;
  s->survivors = NULL;
  _cru_append_nodes (&q, u, err);
  for (n = q->front; n; n = n->next_node)
	 n->marked = NULL;
  return q;
 b: s->deletions = _cru_nodes_of (q, err);
 a: return NULL;
}




// --------------- error recovery --------------------------------------------------------------------------




static void
unbound (s, d, err)
	  port s;
	  cru_destructor_pair d;
	  int *err;

	  // Get rid of the survivors and deletions in a port assuming an
	  // error happened during the bounding phase.
{
  node_list n;

  if ((! s) ? IER(1233) : (s->gruntled != PORT_MAGIC) ? IER(1234) : (! d) ? IER(1235) : 0)
	 return;
  _cru_free_nodes (s->survivors, d, err);
  s->survivors = NULL;
  _cru_free_nodes (s->deletions, d, err);
  s->deletions = NULL;
}








static void *
unbounding_task (s, err)
	  port s;
	  int *err;

	  // Co-operatively get rid of the survivors and deletions in a
	  // port assuming an error happened during the bounding phase.
{
  router r;

  if ((! s) ? IER(1236) : (s->gruntled != PORT_MAGIC) ? IER(1237) : ! _cru_pingback (s, err))
		return NULL;
  if ((! (r = s->local)) ? IER(1238) : (r->valid != ROUTER_MAGIC) ? IER(1239) : (r->tag != MUT) ? IER(1240) : 0)
		return NULL;
  unbound (s, &(r->ro_sig.destructors), err);
  return NULL;
}







static void
retract (n, d, err)
	  node_list n;
	  cru_destructor d;
	  int *err;

	  // Get rid of individual remaining mutated edges mixed with
	  // unmutated edges in case of an error.
{
#define NOT_FORWARD_PROPAGATING 0

  if (n ? n->marked : NULL)
	 for (; n; n = n->next_node)
		undo_reflection (n, d, NOT_FORWARD_PROPAGATING, err);
}







static void *
retracting_task (s, err)
	  port s;
	  int *err;

	  // Concurrently get rid of individual remaining mutated edges
	  // mixed with unmutated edges in the deletions lists of
	  // co-workers' ports in case of an error indirectly by way of the
	  // survivor back edges.
{
  router r;

  if ((! s) ? IER(1241) : (s->gruntled != PORT_MAGIC) ? IER(1242) : ! _cru_pingback (s, err))
	 return NULL;
  if ((! (r = s->local)) ? IER(1243) : (r->valid != ROUTER_MAGIC) ? IER(1244) : (r->tag != MUT) ? IER(1245) : 0)
	 return NULL;
  retract (s->survivors, r->mutator.mu_kernel.e_op.m_free, err);
  return NULL;
}







static void
unmutate (s, err)
	  port s;
	  int *err;

	  // Get rid of the survivors and deletions in a port assuming an
	  // error happened.
{
  int ux;
  router r;
  struct cru_destructor_pair_s z;

  if ((! s) ? IER(1246) : (s->gruntled != PORT_MAGIC) ? IER(1247) : 0)
	 return;
  if ((! (r = s->local)) ? IER(1248) : (r->valid != ROUTER_MAGIC) ? IER(1249) : (r->tag != MUT) ? IER(1250) : 0)
	 return;
  memset (&z, 0, sizeof (z));
  if (_cru_empty_prop (&(r->mutator.mu_kernel.v_op)))
	 z.v_free = r->ro_sig.destructors.v_free;
  else
	 z.v_free = r->mutator.mu_kernel.v_op.vertex.m_free;
  if (_cru_empty_fold (&(r->mutator.mu_kernel.e_op)))
	 z.e_free = r->ro_sig.destructors.e_free;
  else
	 z.e_free = r->mutator.mu_kernel.e_op.m_free;
  if (s->survivors ? (! (s->survivors->marked)) : 0)
	 memcpy (&z, &(r->ro_sig.destructors), sizeof (z));
  _cru_free_nodes (s->deletions, &(r->ro_sig.destructors), err);
  _cru_free_nodes (s->survivors, &z, err);
  s->survivors = s->deletions = NULL;
}









static void *
unmutating_task (s, err)
	  port s;
	  int *err;

	  // Co-operatively get rid of the survivors and deletions in a
	  // port assuming an error happened.
{
  router r;

  if ((! s) ? IER(1251) : (s->gruntled != PORT_MAGIC) ? IER(1252) : ! _cru_pingback (s, err))
	 return NULL;
  unmutate (s, err);
  return NULL;
}




// --------------- combined phases -------------------------------------------------------------------------



static void
propagate (g, k, r, n, s, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  node_list n;           // initial node in the zone
	  cru_sig s;             // post-mutation sig
	  int *err;

	  // Assuming the graph is full-duplex and the router is
	  // initialized with reachabaility analysis done, perform the
	  // phases of bounding, mutating, reflecting, and queuing on the
	  // graph, or consume it in the event of any error.
{
  cru_hash h;
  unsigned i;
  uintptr_t q;

  if ((! r) ? IER(1253) : (r->valid != ROUTER_MAGIC) ? IER(1254) : (! g) ? IER(1255) : (! (g->base_node)) ? IER(1256) : *err)
	 return;
  if (((! (r->ports)) ? IER(1257) : (! (r->lanes)) ? IER(1258) : 0) ? (r->valid = MUGGLE(34)) : s ? 0 : IER(1259))
	 return;
  if ((r->tag != MUT) ? IER(1260) : 0)
	 return;
  _cru_reset (r, (task) bounding_task, err);
  if (! _cru_status_launched (k, g->base_node, _cru_scalar_hash (g->base_node), r, err))
	 return;
  g->nodes = NULL;
  if (*err)
	 goto a;
  memcpy (&(g->g_sig), s, sizeof (g->g_sig));
  if (! _cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) mutating_task, err)), err))
	 goto a;
  if (*err)
	 goto b;
  q = _cru_scalar_hash (n);
  if (_cru_queue_launched (k, g->base_node, q, _cru_reset (r, (task) queuing_task, err), &(g->nodes), err))
	 if (g->nodes)
		g->nodes->previous = &(g->nodes);
  if (*err)
	 goto b;
  return;
 a: g->base_node = NULL;
  if (_cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) unbounding_task, err)), err))
	 return;
  for (i = 0; i < r->lanes; i++)
	 unbound (r->ports[i], &(r->ro_sig.destructors), err);
  return;
 b: if (FORWARD_PROPAGATING ? NULL : r->mutator.mu_kernel.e_op.m_free)
	 if (! _cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) retracting_task, err)), err))
		for (i = 0; i < r->lanes; i++)
		  retract ((r->ports[i])->survivors, r->mutator.mu_kernel.e_op.m_free, err);
  if (_cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) unmutating_task, err)), err))
	 return;
  for (i = 0; i < r->lanes; i++)
	 unmutate (r->ports[i], err);
}






cru_graph
_cru_mutated (g, k, r, s, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  cru_sig s;       // the sig that will pertain to the graph after mutation
	  int *err;
{
  node_list n;

  if (*err ? 1 : (! g) ? IER(1261) : (! r) ? IER(1262) : (r->valid != ROUTER_MAGIC) ? IER(1263) : 0)
	 goto a;
  if ((r->tag != MUT) ? IER(1264) : ! (n = _cru_initial_node (g, k, r, err)))
	 goto a;
  if (n != g->base_node)
	 if (_cru_compatible (g, &(r->mutator.mu_kernel), err))
		if (*err ? 1 : ! _cru_launched (k, g->base_node, _cru_shared (_cru_reset (r, (task) coverage_analyzing_task, err)), err))
		  goto a;
  if (_cru_empty_fold (&(r->mutator.mu_kernel.v_op.incident)) ? _cru_empty_fold (&(r->mutator.mu_kernel.e_op)) : 0)
	 goto b;
  if (! _cru_half_duplex (g, err))
	 goto b;
  if (_cru_launched (k, n, _cru_router ((task) _cru_full_duplexing_task, r->lanes, err), err) ? *err : 0)
	 goto a;
 b: propagate (g, k, r, n, s, err);
 a: if (*err)
	 _cru_free_now (g, err);
  _cru_sweep (r, err);
  return (*err ? NULL : g);
}
