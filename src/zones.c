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

#include <strings.h>
#include <stdlib.h>
#include "defo.h"
#include "duplex.h"
#include "edges.h"
#include "errs.h"
#include "graph.h"
#include "killers.h"
#include "maybe.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "sync.h"
#include "table.h"
#include "wrap.h"
#include "zones.h"




// --------------- initial node discovery ------------------------------------------------------------------



static maybe
base_searching_task (source, err)
	  port source;
	  int *err;

	  // Scan incoming packets for a node containing the initial
	  // vertex. Kill the job and and return the node if found. This
	  // job is unkillable in production builds but samples the kill
	  // switch during testing to simulate other possible errors.
{
  int ut, ux, found, dblx;
  packet_list incoming;       // incoming packets
  node_list result;
  unsigned sample;
  cru_bpred e;                // vertex equality
  node_set seen;
  packet_pod d;               // for outgoing packets
  node_list n;
  int killed;
  cru_zone z;
  router r;

  sample = 0;
  seen = NULL;
  result = NULL;
  killed = found = dblx = 0;
  if ((! source) ? IER(1744) : (source->gruntled != PORT_MAGIC) ? IER(1745) : 0)
	 return NULL;
  if ((!(r = source->local)) ? IER(1746) : (r->valid != ROUTER_MAGIC) ? IER(1747) : 0)
	 return NULL;
  z = &(r->ro_plan.zone);
  if ((! (d = source->peers)) ? IER(1748) : (e = r->ro_sig.orders.v_order.equal) ? *err : IER(1749))
	 {
		_cru_abort (source, d, err);
		return _cru_new_maybe (ABSENT, NULL, err);
	 }
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(33);
		killed = (killed ? 1 : KILLED);
		if (killed ? 1 : found)
		  goto a;
		if ((n = (node_list) incoming->payload) ? (_cru_test_and_set_membership (n, &seen, err) ? 1 : *err) : IER(1750))
		  goto a;
		_cru_scattered (n->edges_out, d, err);
		if (! (found = ((z->initial_vertex == n->vertex) ? 1 : PASSED(e, z->initial_vertex, n->vertex))))
		  goto a;
		_cru_kill_internally (&(r->killed), err);
		result = n;
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  return ((result ? 1 : *err) ? _cru_new_maybe (result ? PRESENT : ABSENT, (void *) result, err) : NULL);
}





static node_list
located (b, r, err)
	  node_list b;
	  router r;
	  int *err;

	  // Initialize the initial node to one that points to the
	  // mutator's initial vertex if any by searching the graph for it
	  // starting from the base node. If the result is null and the
	  // workers have raised an error, report it. If the result is null
	  // and the workers haven't raised an error, report that the
	  // initial vertex wasn't found.
{
  node_list result;

  result = NULL;
  if (! _cru_maybe_disjunction_launched (UNKILLABLE, b, r, (void **) &result, err))
	 return NULL;
  if (result)
	 return result;
  _cru_catch (r, err);
  if (*err != ENOMEM)
	 RAISE(CRU_INCINV);
  return NULL;
}





// --------------- reachability analysis -------------------------------------------------------------------






static void *
reachability_analyzing_task (source, err)
	  port source;
	  int *err;

	  // Initialize the source port's reachable set and those of its
	  // co-workers by adding every incoming packet's payload to it.
{
  packet_pod destinations;    // for outgoing packets
  packet_list incoming;       // incoming packets
  unsigned sample;
  intptr_t status;
  node_list n;
  int killed;
  router r;

  sample = 0;
  killed = 0;
  if ((! source) ? IER(1751) : (source->gruntled != PORT_MAGIC) ? IER(1752) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(1753) : (r->valid != ROUTER_MAGIC) ? IER(1754) : 0)
	 return NULL;
  if (! (destinations = source->peers) ? IER(1755) : *err)
	 return _cru_abort_status (source, destinations, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 {
		KILL_SITE(34);
		killed = (killed ? 1 : KILLED);
		if (killed ? 1 : (n = (node_list) incoming->payload) ? 0 : IER(1756))
		  goto a;
		if (_cru_test_and_set_membership (n, &(source->reachable), err) ? 0 : ! *err)
		  _cru_scattered (r->ro_plan.zone.backwards ? n->edges_in : n->edges_out, destinations, err);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
//  if (*err ? 0 : killed)
//	 RAISE(CRU_INTKIL);
  _cru_nack (incoming, err);
  status = *err;
  return (void *) status;
}










node_list
_cru_initial_node (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Find the initial node in the zone given the corresponding
	  // user-supplied vertex, and perform reachability analysis on the
	  // router if necessary. It's necessary only in preparation for
	  // operations using constrained traversal orders, such as
	  // filtering and mutation.
{
  cru_plan w;
  cru_zone z;
  node_list result;
  cru_destructor_pair d;

  if ((! r) ? 1 : (r->valid != ROUTER_MAGIC) ? IER(1757) : 0)
	 return NULL;
  z = &((w = &(r->ro_plan))->zone);
  if (! _cru_half_duplex (g, err))
	 goto a;
  if ((r->tag == FIL) ? 1 : (r->tag == MUT) ? 1 : z->backwards)
	 goto b;
  if ((r->tag == MAP) ? _cru_empty_fold (&(r->mapreducer.ma_prop.incident)) : 0)
	 goto a;
 b: if (_cru_launched (k, g->base_node, _cru_router ((task) _cru_full_duplexing_task, r->lanes, err), err))
	 goto a;
  d = &(r->ro_sig.destructors);
  _cru_launched (k, g->base_node, _cru_razing_router (d, (task) _cru_half_duplexing_task, r->lanes, err), err);
  return NULL;
 a: if ((z->offset ? (g->base_node->vertex == z->initial_vertex) : 1) ? (result = g->base_node) : NULL)
	 goto c;
  if (! (result = located (g->base_node, _cru_reset (r, (task) base_searching_task, err), err)))
	 return NULL;
  if (result == g->base_node)
	 goto c;
  if (! (w->remote_first ? 1 : w->local_first))
	 goto c;
  if (! _cru_launched (k, result, _cru_shared (_cru_reset (r, (task) reachability_analyzing_task, err)), err))
	 return NULL;
 c: r->base_register = result;
  return (*err ? NULL : result);
}




// --------------- reachability querying -------------------------------------------------------------------





static int
all_visited (e, r, s, unvisitable, err)
	  edge_list e;
	  router r;
	  port s;               // the current worker thread's port, whose visited set can be read without locking
	  int *unvisitable;
	  int *err;

	  // Return non-zero if no more remote nodes in the edge list can
	  // be visited because they've all been visited. Set unvisitable
	  // to non-zero if it can be ascertained that at least one will
	  // never be visited due to a worker being disabled. Ignore
	  // unreachable nodes.
{
  port p;
  node_list o;
  int visited;

  if ((! r) ? IER(1758) : (r->valid != ROUTER_MAGIC) ? IER(1759) : 0)
	 return 0;
  if (((! r->ports) ? IER(1760) : (! (r->lanes)) ? IER(1761) : 0) ? (r->valid = MUGGLE(75)) : 0)
	 return 0;
  if ((! unvisitable) ? IER(1762) : *unvisitable ? IER(1763) : 0)
	 return 0;
  for (visited = 1; (! e) ? 0 : *unvisitable ? 0 : visited; e = e->next_edge)
	 {
		if (((p = r->ports[_cru_scalar_hash (o = e->remote.node) % r->lanes]) ? 0 : IER(1764)) ? (r->valid = MUGGLE(76)) : 0)
		  return 0;
		if ((p->gruntled != PORT_MAGIC) ? IER(1765) : 0)
		  return 0;
		if (p->reachable ? (! _cru_member (o, p->reachable)) : 0)
		  continue;
		if ((p == s) ? 0 : (pthread_rwlock_rdlock (&(p->p_lock)) ? IER(1766) : 0) ? (p->gruntled = MUGGLE(77)) : 0)
		  return 0;
		if (! (*unvisitable = p->disabled))
		  visited = _cru_member (o, p->visited);
		if ((p == s) ? 0 : (pthread_rwlock_unlock (&(p->p_lock)) ? IER(1767) : 0) ? (p->gruntled = MUGGLE(78)) : 0)
		  return 0;
	 }
  return (*unvisitable ? 0 : visited);
}









static int
visited (e, s, unvisitable, err)
	  edge_list e;
	  port s;
	  int *unvisitable;
	  int *err;

	  // Return non-zero if all remote nodes in the given edge list
	  // have been visited. If so, enable the worker to resume at full
	  // speed by resetting the backoff parameter.
{
  if ((! s) ? IER(1768) : (s->gruntled != PORT_MAGIC) ? IER(1769) : ! all_visited (e, s->local, s, unvisitable, err))
	 return 0;
  if ((pthread_mutex_lock (&(s->suspension)) ? IER(1770) : 0) ? (s->gruntled = MUGGLE(79)) : 0)
	 return 1;
  if (s->backoff)
	 s->backoff = 1;
  if (pthread_mutex_unlock (&(s->suspension)) ? IER(1771) : 0)
	 s->gruntled = MUGGLE(80);
  return 1;
}







int
_cru_visitable (n, s, unvisitable, err)
	  node_list n;
	  port s;
	  int *unvisitable;
	  int *err;

	  // Return non-zero if a node n is ready to be visited because all
	  // of its reachable prerequisites have been visited. Visitability
	  // is always implied if the traversal order is unconstrained.
{
  cru_plan w;
  router r;

  if ((! s) ? IER(1772) : (s->gruntled != PORT_MAGIC) ? IER(1773) : 0)
	 return 0;
  if ((! n) ? IER(1774) : (! (r = s->local)) ? IER(1775) : (r->valid != ROUTER_MAGIC) ? IER(1776) : 0)
	 return 0;
  if (! ((w = &(r->ro_plan))->remote_first ? 1 : w->local_first))             // traversal order is unconstrained
	 return 1;
  return visited (((! (w->remote_first)) == ! (w->zone.backwards)) ? n->edges_in : n->edges_out, s, unvisitable, err);
}






int
_cru_retirable (n, s, unvisitable, err)
	  node_list n;
	  port s;
	  int *unvisitable;
	  int *err;

	  // Return non-zero if all reachable nodes adjacent to the given
	  // node via outgoing edges have been visited.
{
  return ((n ? 0 : IER(1777)) ? 0 : visited (n->edges_out, s, unvisitable, err));
}
