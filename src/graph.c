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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <nthm/nthm.h>
#include "edges.h"
#include "errs.h"
#include "graph.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "route.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"


// for exclusive access to routers' killed fields; the same one locks all of them
static pthread_rwlock_t graph_lock;


// --------------- initialization and teardown -------------------------------------------------------------




int
_cru_open_graph (err)
	  int *err;

	  // Initialize pthread resources.
{
  return ! (pthread_rwlock_init (&graph_lock, NULL) ? IER(1009) : 0);
}





void
_cru_close_graph (void)

	  // Do this when the process exits.
{
  if (pthread_rwlock_destroy (&graph_lock))
	 _cru_globally_throw (THE_IER(1010));
}





// --------------- allocation ------------------------------------------------------------------------------




cru_graph
_cru_graph_of (s, base, n, err)
	  cru_sig s;
	  node_list *base;
	  node_list n;
	  int *err;

	  // Create a new graph. The caller is responsible to reclaim the
	  // node list in case of an error.
{
  cru_graph g;

  if (*err ? 1 : (! base) ? IER(1011) : (! *base) ? 1 : (! n) ? 0 : n->previous ? IER(1012) : 0)
	 goto a;
  if ((! s) ? IER(1013) : (g = (cru_graph) _cru_malloc (sizeof (*g))) ? 0 : RAISE(ENOMEM))
	 goto a;
  memset (g, 0, sizeof (*g));
  memcpy (&(g->g_sig), s, sizeof (g->g_sig));
  if ((g->nodes = n))
	 n->previous = &(g->nodes);
  g->base_node = *base;
  g->glad = GRAPH_MAGIC;
  return g;
 a: return NULL;
}






// --------------- reclamation -----------------------------------------------------------------------------






void
_cru_free_now (g, err)
	  cru_graph g;
	  int *err;

	  // Free a graph sequentially.
{

  if ((! g) ? 1 : (*err == CRU_BADGPH))
	 return;
 if ((g->glad == GRAPH_MAGIC) ? 1 : ! IER(1014))
	_cru_free_nodes (g->nodes, &(g->g_sig.destructors), err);
  g->glad = MUGGLE(16);
  _cru_free (g);
}








static void
freeing_slacker (g)
	  cru_graph g;

	  // This function is used to free a graph in the background when
	  // passed to nthm_send.
{
  int err;

  err = 0;
  _cru_free_now (g, &err);
  _cru_globally_throw (err);
}







void
_cru_free_later (g, err)
	  cru_graph g;
	  int *err;

	  // Start a single background thread to reclaim all storage
	  // associated with a graph and return immediately.
{
  int dblx;

  dblx = 0;
  if (NOMEM ? 1 : ! nthm_send ((nthm_slacker) freeing_slacker, (void *) g, &dblx))
	 _cru_free_now (g, err);
  RAISE(dblx);
}







void *
_cru_freeing_task (s, err)
	  port s;
	  int *err;

	  // Free a graph co-operatively with other workers.
{
#define RECORDED(x) (_cru_member (x, seen) ? 1 : (! *err) ? 0 : _cru_listed (x, s->deletions))

  struct packet_list_s buffer;          // needed if only if there's not enough memory to allocate a packet
  packet_pod destinations;              // outgoing packets
  packet_list incoming;                 // incoming packets
  intptr_t status;
  node_list n, b;
  node_set seen;                        // previously received packet payloads
  router r;

  seen = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! s) ? IER(1015) : (s->gruntled != PORT_MAGIC) ? IER(1016) : 0)
	 goto a;
  if ((! (r = s->local)) ? IER(1017) : (r->valid != ROUTER_MAGIC) ? IER(1018) : 0)
	 goto a;
  if (((destinations = s->peers)) ? 0 : IER(1019))
	 goto b;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, destinations, err));)
	 {
		_cru_free_edges_and_labels (r->ro_sig.destructors.e_free, incoming->carrier, err);
		if ((n = (node_list) incoming->payload) ? RECORDED(n) : IER(1020))
		  goto c;
		if (! _cru_unscatterable (&(n->edges_out), n, r->ro_sig.destructors.e_free, destinations, err))
		  goto d;
		if (_cru_buffered (&buffer, incoming, &(destinations->deferrals), err))
		  goto e;
		if (_cru_transplanted_out ((node_list) buffer.payload, n, err))
		  goto d;
		if (! _cru_spun (&buffer, &incoming, &(destinations->deferrals), err))
		  goto e;
		continue;
	 d: if ((incoming != &buffer) ? (n == buffer.payload) : 0)
		  goto e;
		if (_cru_set_membership (_cru_half_severed (n), &seen, err))
		  _cru_free_nodes (n, &(r->ro_sig.destructors), err);
		else
		  _cru_pushed_node (n, &(s->deletions), err);
	 c: if (_cru_unbuffered (&buffer, &incoming, err))
		  continue;
	 e: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_free_nodes (s->deletions, &(r->ro_sig.destructors), err);
  _cru_forget_members (seen);
  s->deletions = NULL;
 a: status = *err;
  return (void *) status;
 b: return _cru_abort_status (s, destinations, err);
}






// --------------- analysis --------------------------------------------------------------------------------





int
_cru_bad (g, err)
	  cru_graph g;
	  int *err;

	  // Return non-zero if a graph is invalid. Empty graphs are
	  // represented by null cru_graph pointers. A non-empty graph
	  // must have both a base node and a list of nodes.
{
  if (! g)
	 return 0;
  if ((g->glad != GRAPH_MAGIC) ? RAISE(CRU_BADGPH) : 0)
	 return 1;
  return (((g->base_node ? g->nodes : NULL) ? 0 : (g->glad = MUGGLE(17))) ? RAISE(CRU_BADGPH) : 0);
}






int
_cru_half_duplex (g, err)
	  cru_graph g;
	  int *err;

	  // Return non-zero if the graph has no back edges. This test can
	  // be done in constant time if the whole graph is either half
	  // duplex or full duplex. In the latter case, every node has a
	  // back edge to each node preceding it. If the graph is empty,
	  // then it has no base node and no back edges. If it's non-empty
	  // but the base node has no outgoing edges, then there are no
	  // other nodes in the graph, so it can have no incoming edges and
	  // the graph should be considered half duplex. If it has outgoing
	  // edges, then any adjacent node to the base node should have a
	  // back edge to the base node unless the graph is half duplex.
{
  edge_list e;
  node_list n, m;

  if (! g)
	 return 1;
  if ((g->glad != GRAPH_MAGIC) ? IER(1021) : 0)
	 return 1;
  if (((n = g->base_node) ? 0 : IER(1022)) ? (g->glad = MUGGLE(18)) : 0)
	 return 0;
  if (! (e = n->edges_out))
	 return (n->edges_in ? IER(1023) : 1);
  if ((m = e->remote.node) ? 0 : IER(1024))
	 return 0;
  return (! (m->edges_in));
}






int
_cru_compatible (g, b, err)
	  cru_graph g;
	  cru_kernel b;
	  int *err;

	  // Return non-zero if a partial mutation to the graph would be
	  // compatible with its current destructors.
{
  if (! b)
	 return 1;
  if (g ? 0 : IER(1025))
	 return 0;
  if (g->g_sig.destructors.v_free != b->v_op.vertex.m_free)
	 return 0;
  if (g->g_sig.destructors.e_free != b->e_op.m_free)
	 return 0;
  return 1;
}







int
_cru_identical (l, r, err)
	  cru_sig l;
	  cru_sig r;
	  int *err;

	  // Return non-zero both have mostly the same fields, but allow
	  // the vertex limits to differ.
{
  if (l == r)
	 return 1;
  if ((! l) ? IER(1026) : (! r) ? IER(1027) : 0)
	 return 0;
  if ((l->orders.v_order.hash == r->orders.v_order.hash) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->orders.v_order.equal == r->orders.v_order.equal) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->orders.e_order.hash == r->orders.e_order.hash) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->orders.e_order.equal == r->orders.e_order.equal) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->destructors.v_free == r->destructors.v_free) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->destructors.v_free == r->destructors.v_free) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->destructors.e_free == r->destructors.e_free) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  if ((l->destructors.e_free == r->destructors.e_free) ? 0 : RAISE(CRU_INCSPB))
	 return 0;
  return 1;
}




// --------------- storage ---------------------------------------------------------------------------------





void
_cru_store (g, s, err)
	  cru_graph g;
	  void *s;
	  int *err;

	  // Associate arbitrary user-defined data with a graph.
{
  if ((! g) ? IER(1028) : (g->glad != GRAPH_MAGIC) ? IER(1029) : 0)
	 return;
  if (pthread_rwlock_wrlock (&graph_lock) ? IER(1030) : 0)
	 return;
  g->g_store = s;
  if (pthread_rwlock_unlock (&graph_lock))
	 IER(1031);
}






void *
_cru_retrieval (g, err)
	  cru_graph g;
	  int *err;

	  // Retrieve user-defined data previously associated with a graph.
{
  void *s;

  if ((! g) ? IER(1032) : (g->glad != GRAPH_MAGIC) ? IER(1033) : 0)
	 return NULL;
  if (pthread_rwlock_rdlock (&graph_lock) ? IER(1034) : 0)
	 return NULL;
  s = g->g_store;
  if (pthread_rwlock_unlock (&graph_lock))
	 IER(1035);
  return s;
}






// --------------- public facing API -----------------------------------------------------------------------








void
cru_free_later (g, err)
	  cru_graph g;
	  int *err;

	  // Start a single background thread to reclaim all storage
	  // associated with a graph and return immediately. This function
	  // needs to check for a null err parameter because it's callable
	  // from user code, which might neglect to assign it.
{
  int ignored;

  if (err ? NULL : (err = &ignored))
	 ignored = 0;
  if (! _cru_bad (g, err))
	 _cru_free_later (g, err);
}
