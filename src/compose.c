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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "compose.h"
#include "duplex.h"
#include "edges.h"
#include "errs.h"
#include "filters.h"
#include "graph.h"
#include "killers.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "launch.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"






static void
guard_it (n, c, visited, err)
	  node_list n;
	  cru_composer c;
	  node_set *visited;
	  int *err;

 	  // Compute the props associated with a single vertex in a graph.
{
  mapex_pair g;
  void *a;
  int ux;

  if (*err ? 1 : (! n) ? IER(638) : (! c) ? IER(639) : ! _cru_set_membership (n, visited, err))
	 return;
  if ((g = (mapex_pair) _cru_malloc (sizeof (*g))) ? 0 : RAISE(ENOMEM))
	 return;
  memset (g, 0, sizeof (*g));
  g->local_mapex = _cru_mapped_node (&(c->co_props.local), n, err);
  if (*err)
	 goto c;
  g->adjacent_mapex = _cru_mapped_node (&(c->co_props.adjacent), n, err);
  if (*err)
	 goto d;
  n->props = g;
  return;
 d: if (g->adjacent_mapex ? c->co_props.adjacent.vertex.m_free : NULL)
	 APPLY(c->co_props.adjacent.vertex.m_free, g->adjacent_mapex);
 c: if (g->local_mapex ? c->co_props.local.vertex.m_free : NULL)
	 APPLY(c->co_props.local.vertex.m_free, g->local_mapex);
 b: _cru_free (g);
  _cru_clear_membership (n, visited);
}






static void
free_props (n, c, visited, err)
	  node_list n;
	  cru_composer c;
	  node_set *visited;
	  int *err;

	  // Free the guard values from one node in a graph.
{
  mapex_pair g;
  int ux;

  if ((! c) ? IER(640) : (! visited) ? IER(641) : (! n) ? IER(642) : ! _cru_member (n, *visited))
	 return;
  if (! (g = n->props))
	 return;
  n->props = NULL;
  _cru_clear_membership (n, visited);
  _cru_forget_members (g->deletable_edges);
  if (g->local_mapex ? c->co_props.local.vertex.m_free : NULL)
	 APPLY(c->co_props.local.vertex.m_free, g->local_mapex);
  if (g->adjacent_mapex ? c->co_props.adjacent.vertex.m_free : NULL)
	 APPLY(c->co_props.adjacent.vertex.m_free, g->adjacent_mapex);
  _cru_free (g);
}








static void *
guarding_task (source, err)
	  port source;
	  int *err;

	  // Initialize all the guard mapexes in a graph and store the
	  // nodes thus initialized in the survivors list of the port.
{
#define RECORDED(x) (_cru_member (x, seen) ? 1 : (*err ? 0 : ! killed) ? 0 : _cru_listed (x, source->survivors))

  router r;
  int killed;
  node_list n;
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
  if ((! source) ? IER(643) : (source->gruntled != PORT_MAGIC) ? IER(644) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(645) : (r->valid != ROUTER_MAGIC) ? IER(646) : 0)
	 return NULL;
  if ((! (d = source->peers)) ? IER(647) : (r->tag != COM) ? IER(648) : 0)
	 return _cru_abort_status (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(3);
		killed = (killed ? 1 : KILLED);
		if ((n = (node_list) incoming->payload) ? RECORDED(n) : IER(649))
		  goto a;
		if (*err ? 0 : killed ? 0 : _cru_scattered (n->edges_out, d, err))
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
		guard_it (n, &(r->composer), &(source->visited), err);
		_cru_pushed_node (n, &(source->survivors), err);
	 a: if (_cru_unbuffered (&buffer, &incoming, err))
		  continue;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  if (! (*err ? 1 : killed))
	 goto d;
  for (n = source->survivors; n; n = n->next_node)
	 free_props (n, &(r->composer), &(source->visited), err);
 d: _cru_forget_members (seen);
  status = *err;
  return (void *) status;
}








static void
compose (l, n, c, g, f, b, z, err)
	  void *l;
	  node_list n;
	  cru_composer c;
	  mapex_pair g;
	  edge_list *f;               // front of a queue
	  edge_list *b;               // back of a queue, reused across calls
	  cru_destructor_pair z;
	  int *err;

	  // Construct new edges from the compositions of existing edges
	  // meeting the user-supplied qualification, and schedule the
	  // existing edges for deletion if requested.
{
#define MARKED_FOR_DELETION(x,d) (_cru_member ((node_list) x, d) ? 1 : _cru_set_membership ((node_list) x, &(d), err))

  void *t;
  void *ua;
  int ux, ut;
  node_list m;
  edge_list e;
  mapex_pair r;

  if ((! n) ? IER(650) : (! c) ? IER(651) : ! g)
	 return;
  if ((! (c->labeler.qpred)) ? IER(652) : (! (c->labeler.qop)) ? IER(653) : 0)
	 return;
  for (e = n->edges_out; *err ? NULL : e; e = e->next_edge)
	 if ((m = e->remote.node) ? (r = m->props) : NULL)
		if (PASSED(c->labeler.qpred, g->local_mapex, l, r->adjacent_mapex, e->label))
		  if (c->destructive ? MARKED_FOR_DELETION(l, g->deletable_edges) : 1)
			 {
				t = APPLIED(c->labeler.qop, g->local_mapex, l, r->adjacent_mapex, e->label);
				_cru_enqueue_edge (_cru_edge (z, t, NO_VERTEX, m, NO_NEXT_EDGE, err), f, b, err);
			 }
}








static void *
composing_task (s, err)
	  port s;
	  int *err;

	  // Compute the composed edges originating from graph nodes stored
	  // in the given port's survivors list. This task is done
	  // concurrently with other workers but need not communicate with
	  // them.
{
  unsigned sample;
  cru_composer c;
  edge_list e, b;
  node_list n;
  int killed;
  router r;

  sample = 0;
  killed = 0;
  if ((! s) ? IER(654) : (s->gruntled != PORT_MAGIC) ? IER(655) : ! _cru_pingback (s, err))
	 return NULL;
  if ((! (r = s->local)) ? IER(656) : (r->valid != ROUTER_MAGIC) ? IER(657) : (! (r->ports)) ? IER(658) : 0)
	 return NULL;
  if ((r->tag != COM) ? IER(659) : (! (r->lanes)) ? IER(660) : (s->own_index >= r->lanes) ? IER(661) : 0)
	 return _cru_abort (s, NO_POD, err);
  n = s->survivors;
  c = &(r->composer);
  for (; n; n = n->next_node)
	 {
		KILL_SITE(4);
		killed = (killed ? 1 : KILLED);
		if (killed ? 1 : *err ? 1 : n->edges_by ? IER(662) : 0)
		  continue;
		b = NULL;
		for (e = n->edges_out; *err ? NULL : e; e = e->next_edge)
		  compose (e->label, e->remote.node, c, n->props, &(n->edges_by), &b, &(r->ro_sig.destructors), err);
		n->edges_by = _cru_deduplicated_edges (n->edges_by, &(r->ro_sig.orders.e_order), r->ro_sig.destructors.e_free, err);
	 }
  return NON_NULL;
}







int
bypassed (n, o, d, err)
	  node_list n;
	  cru_order o;
	  cru_destructor d;
	  int *err;

	  // Include the bypassing edges in the adjacency list and
	  // deduplicate. Return non-zero if there are any new ones.
{
  edge_list *t;
  mapex_pair p;
  uintptr_t before, after;

  if ((! n) ? IER(663) : (! o) ? IER(664) : ! (p = n->props))
	 return 0;
  before = _cru_degree (n->edges_out);
  n->edges_out = _cru_cat_edges (n->edges_out, n->edges_by);
  n->edges_by = NULL;
  if (! *err)
	 n->edges_out = _cru_deduplicated_edges (n->edges_out, o, d, err);
  after = _cru_degree (n->edges_out);
  if (! (p->deletable_edges))
	 goto a;
  for (t = &(n->edges_out); *t;)
	 if (_cru_member ((node_list) (*t)->label, p->deletable_edges))
		_cru_free_edges_and_labels (d, _cru_popped_edge (t, err), err);
	 else
		t = &((*t)->next_edge);
 a: return (*err ? 0 : (before != after));
}








static void
decompose (n, c, visited, z, err)
	  node_list *n;
	  cru_composer c;
	  node_set *visited;
	  cru_destructor_pair z;
	  int *err;

	  // Get rid of unused bypassing edges.
{
  node_list t;

  if ((! z) ? IER(665) : n ? 0 : IER(666))
	 return;
  for (t = *n; t; t = t->next_node)
	 {
		free_props (t, c, visited, err);
		_cru_free_edges_and_labels (z->e_free, t->edges_by, err);
		t->edges_by = NULL;
	 }
  _cru_free_nodes (*n, z, err);
  *n = NULL;
}









static void *
bypassing_task (s, err)
	  port s;
	  int *err;

	  // Connect newly computed composed edges originating from graph
	  // nodes stored in the given port's survivors list to the
	  // appropriate termini, and also delete any edges previously
	  // scheduled for deletion. This task is done concurrently with
	  // other workers after all workers have completed their composing
	  // tasks, but need not communicate with them.
{
  uintptr_t changed;
  unsigned sample;
  node_list n;
  int killed;
  router r;

  sample = 0;
  killed = 0;
  changed = 0;
  if ((! s) ? IER(667) : (s->gruntled != PORT_MAGIC) ? IER(668) : ! _cru_pingback (s, err))
	 return NULL;
  if ((! (r = s->local)) ? IER(669) : (r->valid != ROUTER_MAGIC) ? IER(670) : 0)
	 return NULL;
  if ((r->tag != COM) ? IER(671) : 0)
	 return NULL;
  for (n = s->survivors; n; n = n->next_node)
	 {
		KILL_SITE(5);
		killed = (killed ? 1 : KILLED);
		changed = (bypassed (n, &(r->ro_sig.orders.e_order), r->ro_sig.destructors.e_free, err) ? 1 : changed);
		free_props (n, &(r->composer), &(s->visited), err);
	 }
  if (*err)
	 decompose (&(s->survivors), &(r->composer), &(s->visited), &(r->ro_sig.destructors), err);
  return (void *) changed;
}











cru_graph
_cru_composed (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Launch a composition with a previously prepared router and
	  // block until finished.
{
  task t;
  router z;
  node_list n;
  int iterating;
  packet_list p;
  uintptr_t count;
  unsigned worker;

  count = 0;
  _cru_disable_killing (k, err);
  if (*err ? 1 : ! g)
	 goto a;
  if ((! r) ? IER(672) : (r->valid != ROUTER_MAGIC) ? IER(673) : (r->tag != COM) ? IER(674) : (! (r->ports)) ? IER(675) : 0)
	 goto a;
  if (_cru_half_duplex (g, err))
	 goto b;
  if (! (z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err)))
	 goto a;
  if (! _cru_launched (k, g->base_node, z, err))
	 goto a;
 b: for (iterating = 1; iterating;)
	 {
		if (! _cru_launched (UNKILLABLE, g->base_node, _cru_shared (_cru_reset (r, (task) guarding_task, err)), err))
		  break;
		g->nodes = NULL;
		for (t = (task) composing_task; t; t = ((t == (task) composing_task) ? (task) bypassing_task : NULL))
		  if (_cru_count_launched (k, NO_BASE, NO_PAYLOAD, NO_HASH, _cru_shared (_cru_reset (r, t, err)), &count, err))
			 iterating = (count ? r->composer.co_fix : 0);
		  else
			 iterating = 0;
		if (*err ? 1 : ! _cru_pruned (g, _cru_shared (r), k, err))
		  break;
		if (! iterating)
		  goto c;
	 }
  for (worker = 0; worker < r->lanes; worker++)
	 decompose (&((r->ports[worker])->survivors), &(r->composer), &((r->ports[worker])->visited), &(r->ro_sig.destructors), err);
 a: _cru_free_now (g, err);
  g = NULL;
 c: _cru_sweep (r, err);
  return g;
}
