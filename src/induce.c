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
#include "cthread.h"
#include "edges.h"
#include "errs.h"
#include "induce.h"
#include "killers.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "maybe.h"
#include "launch.h"
#include "route.h"
#include "scatter.h"
#include "sync.h"
#include "table.h"
#include "wrap.h"
#include "zones.h"







static void
visit (n, b, lock, visited, i, result, err)
	  node_list n;
	  node_list b;
	  pthread_rwlock_t *lock;
	  node_set *visited;
	  cru_inducer i;
	  maybe *result;
	  int *err;

 	  // Compute the value associated with a single vertex in a graph
	  // based on the vertex and the values associated with its
	  // prerequisites. Store this value in the node's accumulator
	  // field.
{
  edge_list e;
  void *ua;
  void *a;
  int ux;

  if ((! n) ? IER(993) : (! b) ? IER(994) : (! i) ? IER(995) : (! result) ? IER(996) : *result ? IER(997) : *err)
	 return;
  if (((e = (i->in_zone.backwards ? n->edges_in : n->edges_out))) ? NULL : i->boundary_value)
	 a = APPLIED(i->boundary_value, n->vertex);
  else
	 a = _cru_reduced_edges_by_accumulator (&(i->in_fold), n->vertex, e, err);
  if (*err)
	 goto a;
  if (n != b)
	 goto b;
  if (! (*result = _cru_new_maybe (PRESENT, a, err)))
	 goto a;
  return;
 b: n->accumulator = a;
  if (pthread_rwlock_wrlock (lock) ? IER(998) : 0)
	 return;
  _cru_set_membership (n, visited, err);
  if (pthread_rwlock_unlock (lock))
	 IER(999);
  return;
 a: if (a ? i->in_fold.r_free : NULL)
	 APPLY(i->in_fold.r_free, a);
}










void *
_cru_inducing_task (s, err)
	  port s;
	  int *err;

	  // Co-operatively perform a computation for each reachable vertex
	  // in a graph. In the event of an error, ignore further packets
	  // and disable the port.
{
  packet_pod destinations;    // outgoing packets
  packet_list incoming;       // incoming packets
  int unvisitable;
  unsigned sample;
  node_set seen;
  maybe result;
  node_list n;
  int killed;
  router r;

  seen = NULL;
  result = NULL;
  if ((! s) ? IER(1000) : (s->gruntled != PORT_MAGIC) ? IER(1001) : (int) (sample = 0))
	 return NULL;
  if ((! (r = s->local)) ? IER(1002) : (r->valid != ROUTER_MAGIC) ? IER(1003) : (killed = 0))
	 return NULL;
  if ((!(destinations = s->peers)) ? IER(1004) : (r->tag != IND) ? IER(1005) : (unvisitable = 0))
	 return _cru_abort (s, destinations, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, destinations, err));)
	 {
		KILL_SITE(16);
		killed = (killed ? 1 : KILLED);
		if (s->disabled)
		  goto a;
		if (*err ? 1 : killed ? 1 : (n = (node_list) incoming->payload) ? 0 : IER(1006))
		  goto b;
		if (_cru_member (n, s->visited))
		  goto a;
		if (_cru_member (n, seen))
		  goto c;
		if (! _cru_set_membership (n, &seen, err))
		  goto b;
		if (! _cru_scattered (r->ro_plan.zone.backwards ? n->edges_in : n->edges_out, destinations, err))
		  goto b;
	 c: if (_cru_visitable (n, s, &unvisitable, err))
		  goto e;
		if (*err ? 1 : unvisitable)
		  goto b;
		_cru_push_packet (_cru_popped_packet (&incoming, err), &(destinations->deferrals), err);
		continue;
	 e: visit (n, r->base_register, &(s->p_lock), &(s->visited), &(r->inducer), &result, err);
		if (! *err)
		  goto a;
	 b: _cru_set (&(s->p_lock), &(s->disabled), err);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_nack (s->deferred, err);
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  s->deferred = NULL;
  if (result ? 0 : *err)
	 result = _cru_new_maybe (ABSENT, NULL, err);
  _cru_throw (r, err);
  return (void *) result;
}









static void *
deducing_task (s, err)
	  port s;
	  int *err;

	  // Free the partial results computed during an induction. This
	  // task ignores the kill signal other than to report it, because
	  // stopping would cause a memory leak requiring more time to be
	  // wasted by the caller freeing them sequentially.
{
  packet_list incoming;       // incoming packets
  unsigned sample;
  intptr_t status;
  node_set seen;              // previously received packet payloads
  packet_pod d;               // outgoing packets
  node_list n;
  int killed;
  router r;
  void *v;
  int ux;

  seen = NULL;
  if ((! s) ? IER(1007) : (s->gruntled != PORT_MAGIC) ? IER(1008) : (killed = 0))
	 return NULL;
  if ((! (r = s->local)) ? IER(1009) : (r->valid != ROUTER_MAGIC) ? IER(1010) : (int) (sample = 0))
	 return NULL;
  if ((! (d = s->peers)) ? IER(1011) : (r->tag != IND) ? IER(1012) : 0)
	 return _cru_abort_status (s, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, d, err));)
	 {
		KILL_SITE(17);
		killed = (killed ? 1 : KILLED);
		if ((n = (node_list) incoming->payload) ? 0 : IER(1013))
		  goto a;
		if (_cru_test_and_set_membership (n, &seen, err) ? 0 : ! *err)
		  _cru_scattered (r->ro_plan.zone.backwards ? n->edges_in : n->edges_out, d, err);
		if (n->accumulator ? r->inducer.in_fold.r_free : NULL)
		  APPLY(r->inducer.in_fold.r_free, n->accumulator);
		n->accumulator = NULL;
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  status = *err;
  return (void *) status;
}











void *
_cru_induce (k, i, n, r, err)
	  cru_kill_switch k;
	  node_list i;          // initial node for the purpos of induction
	  node_list n;          // all nodes in a graph
	  router r;
	  int *err;

	  // Launch a traversal for induction, block until finished, and
	  // consume the router. The router's reachability analysis is
	  // assumed to be done already if necessary.
{
  void *result;
  int ux;

  result = NULL;
  _cru_disable_killing (k, err);
  if (! _cru_maybe_disjunction_launched (k, i, r, &result, err))
	 goto a;
  if (_cru_launched (k, i, _cru_shared (_cru_reset (r, (task) deducing_task, err)), err) ? *err : 1)
	 for (; n; n = n->next_node)
		{
		  if (n->accumulator ? r->inducer.in_fold.r_free : NULL)
			 APPLY(r->inducer.in_fold.r_free, n->accumulator);
		  n->accumulator = NULL;
		}
 a: _cru_free_router (r, err);
  return result;
}
