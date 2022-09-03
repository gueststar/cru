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

#include "crew.h"
#include "errs.h"
#include "graph.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "ports.h"
#include "repl.h"
#include "route.h"
#include "runners.h"
#include "stretch.h"
#include "sync.h"
#include "wrap.h"






int
_cru_launched (k, i, r, err)
	  cru_kill_switch k;
	  node_list i;         // if non-null, the hash determines the worker to receive the initial packet
	  router r;
	  int *err;

	  // Launch a traversal, block until finished, and consume the
	  // router. The router's reachability analysis is assumed to be
	  // done already if necessary. Return non-zero if the operation
	  // starts successfully.
{
  packet_list p;
  uintptr_t q;
  int started;
  int dblx;
  crew c;

  started = dblx = 0;
  if ((! r) ? 1 : (r->valid != ROUTER_MAGIC) ? IER(1053) : r->ports ? 0 : IER(1054))
	 goto a;
  if (! _cru_crewed (c = _cru_crew_of (&dblx), (runner) _cru_status_runner, r, &dblx))
	 goto b;
  if (! i)
	 started = _cru_ping (r->ports[0], &dblx);
  else if ((p = _cru_initial_packet_of (i, q = _cru_scalar_hash (i), &dblx)))
	 started = _cru_assigned (r->ports[q % r->lanes], &p, &dblx);
  if (started)
	 _cru_wait_for_quiescence (k, r, &dblx);
  _cru_dismiss (r, &dblx);
 b: _cru_status_disjunction (c, &dblx);
  RAISE(r->ro_status);
 a: _cru_free_router (r, &dblx);
  RAISE(dblx);
  return started;
}










int
_cru_status_launched (k, i, h, r, err)
	  cru_kill_switch k;
	  node_list i;
	  uintptr_t h;
	  router r;
	  int *err;

	  // Mutate with a specified hash for the initial packet and return
	  // the status disjunction.
{
  crew c;
  int dblx;
  int started;
  packet_list p;

  started = dblx = 0;
  if ((! r) ? IER(1055) : (r->valid != ROUTER_MAGIC) ? IER(1056) : 0)
	 return 0;
  if (((! (r->ports)) ? IER(1057) : (! (r->lanes)) ? IER(1058) : 0) ? (r->valid = MUGGLE(27)) : 0)
	 return 0;
  if (! _cru_crewed (c = _cru_crew_of (&dblx), (runner) _cru_status_runner, r, &dblx))
	 goto a;
  if (((p = _cru_packet_of ((r->tag == BUI) ? NULL : i, h, NO_SENDER, NO_CARRIER, &dblx))) ? (p->receiver = i) : NULL)
	 if ((started = _cru_assigned (r->ports[h % r->lanes], &p, &dblx)))
		_cru_wait_for_quiescence (k, r, &dblx);
  _cru_dismiss (r, &dblx);
 a: _cru_status_disjunction (c, &dblx);
  RAISE(r->ro_status);
  RAISE(dblx);
  return started;
}










int
_cru_count_launched (k, i, a, q, r, count, err)
	  cru_kill_switch k;
	  node_list i;
	  void *a;             // initial payload
	  uintptr_t q;
	  router r;
	  uintptr_t *count;
	  int *err;

	  // Launch with a specified hash for the initial packet and set
	  // the count to the summation of counts returned by workers. If
	  // there's an initial node but no initial payload, then set the
	  // receiver of the initial packet to the initial node.
{
  packet_list p;
  int started;
  int dblx;
  crew c;

  started = dblx = 0;
  if ((! r) ? 1 : (r->valid != ROUTER_MAGIC) ? IER(1059) : count ? 0 : IER(1060))
	 goto a;
  if (((! (r->ports)) ? IER(1061) : (! (r->lanes)) ? IER(1062) : 0) ? (r->valid = MUGGLE(28)) : 0)
	 goto a;
  *count = 0;
  if (! _cru_crewed (c = _cru_crew_of (&dblx), (runner) _cru_count_runner, r, &dblx))
	 goto b;
  if (! i)
	 started = _cru_ping (r->ports[0], &dblx);
  else if (((p = _cru_packet_of (a, q, NO_SENDER, NO_CARRIER, &dblx))) ? (a ? 1 : ! ! (p->receiver = i)) : 0)
	 started = _cru_assigned (r->ports[q % r->lanes], &p, err);
  if (started)
	 _cru_wait_for_quiescence (k, r, &dblx);
  _cru_dismiss (r, &dblx);
 b: *count = _cru_summation (c, &dblx);
  RAISE(r->ro_status);
  RAISE(dblx);
 a: _cru_free_router (r, err);
  return started;
}










int
_cru_queue_launched (k, i, q, r, u, err)
	  cru_kill_switch k;
	  node_list i;
	  uintptr_t q;
	  router r;
	  node_list *u;
	  int *err;

	  // Launch with a specified hash for the initial packet and set
	  // the node list.
{
  packet_list p;
  int started;
  int dblx;
  crew c;

  started = dblx = 0;
  if ((! r) ? IER(1063) : (r->valid != ROUTER_MAGIC) ? IER(1064) : 0)
	 return 0;
  if (((! (r->ports)) ? IER(1065) : (! (r->lanes)) ? IER(1066) : 0) ? (r->valid = MUGGLE(29)) : 0)
	 return 0;
  if ((! i) ? IER(1067) : (! u) ? IER(1068) : *u ? IER(1069) : 0)
	 return 0;
  if (! _cru_crewed (c = _cru_crew_of (&dblx), (runner) _cru_queue_runner, r, &dblx))
	 goto a;
  if (r->work != (task) _cru_pruning_task)
	 p = _cru_initial_packet_of (i, q, &dblx);
  else if ((p = _cru_packet_of (i->vertex, q, NO_SENDER, NO_CARRIER, &dblx)))
	 p->receiver = i;
  if (p ? ((started = _cru_assigned (r->ports[q % r->lanes], &p, &dblx))) : 0)
	 _cru_wait_for_quiescence (k, r, &dblx);
  _cru_dismiss (r, &dblx);
 a: *u = _cru_node_union (c, NO_BASE_BAY, NO_BASE, &dblx);
  RAISE(r->ro_status);
  RAISE(dblx);
  return started;
}









int
_cru_maybe_disjunction_launched (k, i, r, result, err)
	  cru_kill_switch k;
	  node_list i;
	  router r;
	  void **result;
	  int *err;

	  // Return at most one result from a router whose workers return
	  // maybe types.
{
  int started, dblx;
  packet_list p;
  uintptr_t q;
  crew c;

  dblx = started = 0;
  if ((! result) ? IER(1070) : 0)
	 goto a;
  *result = NULL;
  if ((! r) ? IER(1071) : (r->valid != ROUTER_MAGIC) ? IER(1072) : 0)
	 goto a;
  if (((! (r->ports)) ? IER(1073) : (! (r->lanes)) ? IER(1074) : 0) ? (r->valid = MUGGLE(30)) : 0)
	 return 0;
  if (! _cru_crewed (c = _cru_crew_of (&dblx), (runner) _cru_maybe_runner, r, &dblx))
	 goto b;
  if ((p = _cru_initial_packet_of (i, q = _cru_scalar_hash (i), &dblx)))
	 if ((started = _cru_assigned (r->ports[q % r->lanes], &p, &dblx)))
		_cru_wait_for_quiescence (k, r, &dblx);
  _cru_dismiss (r, &dblx);
 b: _cru_maybe_disjunction (c, result, &dblx);
  RAISE(r->ro_status);
 a: RAISE(dblx);
  return started;
}









int
_cru_maybe_reduction_launched (k, i, r, result, err)
	  cru_kill_switch k;
	  node_list i;
	  router r;
	  void **result;
	  int *err;

	  // Return a combination of the results from a router whose
	  // workers return maybe types.
{
  int started, dblx;
  packet_list p;
  uintptr_t q;
  crew c;

  dblx = started = 0;
  if ((! result) ? IER(1075) : 0)
	 goto a;
  *result = NULL;
  if ((! r) ? IER(1076) : (r->valid != ROUTER_MAGIC) ? IER(1077) : (r->tag != MAP) ? IER(1078) : 0)
	 goto a;
  if (((! (r->ports)) ? IER(1079) : (! (r->lanes)) ? IER(1080) : 0) ? (r->valid = MUGGLE(31)) : 0)
	 return 0;
  if (! _cru_crewed (c = _cru_crew_of (&dblx), (runner) _cru_maybe_runner, r, &dblx))
	 goto b;
  if ((p = _cru_initial_packet_of (i, q = _cru_scalar_hash (i), &dblx)))
	 if ((started = _cru_assigned (r->ports[q % r->lanes], &p, &dblx)))
		_cru_wait_for_quiescence (k, r, &dblx);
  _cru_dismiss (r, &dblx);
 b: _cru_maybe_reduction (c, &(r->mapreducer.ma_prop.vertex), result, &dblx);
  RAISE(r->ro_status);
 a: RAISE(dblx);
  return started;
}











extern int
_cru_graph_launched (k, v, q, r, g, err)
	  cru_kill_switch k;
	  void *v;
	  uintptr_t q;
	  router r;
	  cru_graph *g;
	  int *err;

	  // Launch a traversal to build or spread a graph from the base
	  // v. Consume v and the router. If *g is NULL, then assume the an
	  // existing graph is being spread. Otherwise assume a new graph
	  // is being built.
{
  node_list n, b;
  packet_list p;
  int started;
  crew c;
  int ux;

  b = NULL;
  started = 0;
  if ((! r) ? IER(1081) : (r->valid != ROUTER_MAGIC) ? IER(1082) : 0)
	 return 0;
  if (((! (r->ports)) ? IER(1083) : (! (r->lanes)) ? IER(1084) : 0) ? (r->valid = MUGGLE(32)) : (! g) ? IER(1085) : 0)
	 return 0;
  if (*err)
	 goto a;
  if (! _cru_crewed (c = _cru_crew_of (err), (runner) _cru_queue_runner, r, err))
	 goto b;
  if ((p = _cru_initial_packet_of (v, q, err)))
	 {
		if (*g)
		  p->receiver = (*g)->base_node;
		if ((started = _cru_assigned (r->ports[q % r->lanes], &p, err)))
		  {
			 v = NULL;
			 if (*g)
				(*g)->nodes = NULL;
			 _cru_wait_for_quiescence (k, r, err);
		  }
	 }
  _cru_dismiss (r, err);
 b: n = _cru_node_union (c, (unsigned) (q % r->lanes), &b, err);
  RAISE(r->ro_status);
  if (*err ? NULL : *g)
	 (*g)->nodes = n;
  else if (*err ? 1 : ! (*g = _cru_graph_of (&(r->ro_sig), &b, n, err)))
	 {
		_cru_free_nodes (n, &(r->ro_sig.destructors), err);
		if (*g)
		  {
			 _cru_free_now (*g, err);
			 v = NULL;
		  }
		*g = NULL;
	 }
 a: if ((r->tag == FAB) ? NULL : (r->tag == CRO) ? NULL : (r->tag == BUI) ? NULL : r->ro_sig.destructors.v_free ? v : NULL)
	 APPLY(r->ro_sig.destructors.v_free, v);
  _cru_free_router (r, err);
  return started;
}
