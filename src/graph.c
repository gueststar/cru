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

  if (*err ? 1 : (! base) ? IER(977) : (! *base) ? 1 : (! n) ? 0 : n->previous ? IER(978) : 0)
	 goto a;
  if ((! s) ? IER(979) : (g = (cru_graph) _cru_malloc (sizeof (*g))) ? 0 : RAISE(ENOMEM))
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
 if ((g->glad == GRAPH_MAGIC) ? 1 : ! IER(980))
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
#define RECORDED(x) (_cru_member (x, seen) ? 1 : _cru_listed (x, s->deletions))

  struct packet_list_s buffer;          // needed if only if there's not enough memory to allocate a packet
  packet_pod destinations;              // outgoing packets
  packet_list incoming;                 // incoming packets
  intptr_t status;
  node_list n, b;
  node_set seen;                        // previously received packet payloads
  router r;

  seen = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! s) ? IER(981) : (s->gruntled != PORT_MAGIC) ? IER(982) : 0)
	 goto a;
  if ((! (r = s->local)) ? IER(983) : (r->valid != ROUTER_MAGIC) ? IER(984) : 0)
	 goto a;
  if (((destinations = s->peers)) ? 0 : IER(985))
	 goto b;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, destinations, err));)
	 {
		_cru_free_edges_and_labels (r->ro_sig.destructors.e_free, incoming->carrier, err);
		if ((n = (node_list) incoming->payload) ? RECORDED(n) : IER(986))
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
  if ((g->glad != GRAPH_MAGIC) ? IER(987) : 0)
	 return 1;
  if (((n = g->base_node) ? 0 : IER(988)) ? (g->glad = MUGGLE(18)) : 0)
	 return 0;
  if (! (e = n->edges_out))
	 return (n->edges_in ? IER(989) : 1);
  if ((m = e->remote.node) ? 0 : IER(990))
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
  if (g ? 0 : IER(991))
	 return 0;
  if (g->g_sig.destructors.v_free != b->v_op.vertex.m_free)
	 return 0;
  if (g->g_sig.destructors.e_free != b->e_op.m_free)
	 return 0;
  return 1;
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
