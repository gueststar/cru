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
#include "cross.h"
#include "edges.h"
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
#include "table.h"
#include "wrap.h"


// --------------- products --------------------------------------------------------------------------------


/*
	The product struct holds the payload for packets exchanged among
	crossing task workers. The head of each node list is a node in each
	of two separate graphs. The terminal nodes of each pair of edges in
	the cartesian product of their outgoing edge sets determine a
	further product.

	The product vertex isn't stored in this structure, but computed and
	stored in a hash table only when a worker receives a packet
	carrying the associated payload for the first time. Postponing
	evaluation until then prevents repeatedly computing product
	vertices reached by multiple paths. The hash is derived necessarily
	from the factors and not the result, so duplicate vertices in the
	product graph are possible unless the application code prevents
	them or deduplicates them afterwards.
*/

typedef struct product_s
{
  node_list multiplicand;
  node_list multiplier;
} *product;





static product
product_of (l, r, err)
	  node_list l;
	  node_list r;
	  int *err;

	  // Allocate a new product.
{
  product result;

  if (*err ? 1 : (! l) ? IER(757) : (! r) ? IER(758) : 0)
	 return NULL;
  if ((result = (product) _cru_malloc (sizeof (*result))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (result, 0, sizeof (*result));
  result->multiplicand = l;
  result->multiplier = r;
  return result;
}







static void
free_product (p, err)
	  product p;
	  int *err;

	  // Free a product. This function is used as a destructor passed
	  // to _cru_free_packets, hence the unused error parameter.
{
  _cru_free (p);
}






static uintptr_t
rash (i)
	  product i;

	  // Generate a hash value for a product. The hash is derived from
	  // the addresses of the factors unless they're equal, in which
	  // case it's derived from the address of just one of them.
{
  if (! i)
	 return 0;
  if (i->multiplicand == i->multiplier)
	 return _cru_scalar_hash (i->multiplicand);
  return _cru_scalar_hash (i->multiplicand) ^ _cru_scalar_hash (i->multiplier);
}






// --------------- packets ---------------------------------------------------------------------------------





static void
free_productive_packets (p, z, err)
	  packet_list p;
	  cru_destructor_pair z;
	  int *err;

	  // Free a packet list whose payloads are products and whose
	  // carriers are freshly created by the e_prod function in a
	  // crosser and not otherwise accessible.
{
  packet_list o;

  if (z ? 0 : IER(759))
	 return;
  for (o = p; o; o = o->next_packet)
	 {
		if (o->payload)
		  _cru_free ((product) o->payload);
		_cru_free_edges_and_labels (z->e_free, o->carrier, err);
	 }
  _cru_nack (p, err);
}








static packet_list
productive_packets_of (i, x, err)
	  product i;
	  cru_crosser x;
	  int *err;

	  // Given a product and a crosser, generate a packet list holding
	  // all of the products and new edges implied by it. If tests are
	  // being run, allow an allocation error to be simulated at each
	  // call site to the test code.
{
  packet_list result, p;
  edge_list l, r, c;
  product v;
  void *e;
  int ux, ut;
  void *ua;

  result = NULL;
  if ((! i) ? IER(760) : (! x) ? IER(761) : (! (x->e_prod.bop)) ? IER(762) : 0)
	 return NULL;
  if ((! (x->e_prod.bpred)) ? IER(763) : (! (i->multiplicand)) ? IER(764) : (! (i->multiplier)) ? IER(765) :  0)
	 return NULL;
  for (l = i->multiplicand->edges_out; *err ? NULL : l; l = l->next_edge)
	 for (r = i->multiplier->edges_out; *err ? NULL : r; r = r->next_edge)
		if (PASSED(x->e_prod.bpred, l->label, r->label))
		  {
			 if ((v = product_of (l->remote.node, r->remote.node, err)) ? *err : 1)
				goto a;
			 e = APPLIED(x->e_prod.bop, l->label, r->label);
			 if (*err)
				goto b;
			 if ((c = _cru_edge (NO_DESTRUCTORS, e, NO_VERTEX, NO_NODE, NO_NEXT_EDGE, err)) ? *err : 1)
				goto c;
			 if ((p = _cru_packet_of (v, rash (v), NO_SENDER, c, err)) ? *err : 1)
				goto d;
			 p->receiver = NULL;
			 _cru_push_packet (p, &result, err);
			 if (! *err)
				continue;
		  d: _cru_nack (p, err);
		  c: _cru_free_edges (c, err);
		  b: if (e ? x->cr_sig.destructors.e_free : NULL)
				APPLY(x->cr_sig.destructors.e_free, e);
		  a: if (v)
				_cru_free (v);
		  }
  if (*err)
	 free_productive_packets (result, &(x->cr_sig.destructors), err);
  return (*err ? NULL : result);
}





// --------------- crossing --------------------------------------------------------------------------------






static void
reach_extant_node (i, c, d, err)
	  packet_list *i;
	  packet_list c;       // colliding packet
	  packet_pod d;
	  int *err;

	  // Discard a packet i whose payload vertex matches one already in
	  // the graph recorded in a packet c, and connect its sender to
	  // the latter.
{
  if ((! i) ? IER(766) : (! *i) ? IER(767) : (! ((*i)->payload)) ? IER(768) : 0)
	 goto a;
  _cru_free ((product) (*i)->payload);
  if ((! c) ? IER(769) : (!((*i)->carrier)) ? IER(770) : 0)
	 goto a;
  (*i)->carrier->remote.node = c->receiver;                           // overwrite a vertex field with a node field
 a: _cru_nack (_cru_popped_packet (i, err), err);
}








static packet_list
reached_new_node (i, x, q, d, err)
	  packet_list *i;
	  cru_crosser x;
	  node_queue *q;       // the output queue where all newly created nodes are stored
	  packet_pod d;
	  int *err;

	  // Given an incoming packet holding a product not previously
	  // detected, return the packet for it to be stored in the
	  // collision table. Allocate and enqueue a node with a vertex
	  // obtained from the generated product vertex and all outgoing
	  // edges implied by the product. Scatter packets containng the
	  // edges and further products to other workers. If tests are
	  // being run, allow an allocation error to be simulated at each
	  // call site to the test code.
{
  packet_list p;
  node_list n;
  product l;
  int ux;
  void *ua;

  if ((! i) ? IER(771) : (! *i) ? IER(772) : (! (l = (product) (*i)->payload)) ? IER(773) : 0)
	 goto a;
  if ((! (l->multiplicand)) ? IER(774) : (! (l->multiplier)) ? IER(775) : 0)
	 goto a;
  if ((! x) ? IER(776) : (! (x->v_prod)) ? IER(777) : 0)
	 goto a;
  p = productive_packets_of (l, x, err);
  if (*err)
	 goto b;
  if ((n = _cru_node_of (&(x->cr_sig.destructors), NO_VERTEX, NO_EDGES_IN, NO_EDGES_OUT, err)) ? *err : 1)
	 goto c;
  n->vertex = APPLIED(x->v_prod, l->multiplicand->vertex, l->multiplier->vertex);
  if (*err)
	 goto c;
  if (_cru_enqueued_node (n, q, err) ? *err : 1)
	 goto c;
  n->edges_out = _cru_carriers (p);    // assignment delayed until now to avoid a double free if allocation fails
  if ((*i)->carrier)
	 (*i)->carrier->remote.node = n;
  (*i)->receiver = n;
  _cru_scatter (p, d, err);
  return _cru_popped_packet (i, err);
 c: _cru_free_nodes (n, &(x->cr_sig.destructors), err);
 b: free_productive_packets (p, &(x->cr_sig.destructors), err);
 a: return NULL;
}









void *
_cru_crossing_task (source, err)
	  port source;
	  int *err;

	  // Receive packets carrying products from other workers, check
	  // for their presence in the graph, generate adjacent vertices,
	  // and send them to other workers.
{
#define UNEQUAL(a,b) \
(*err ? 0 : (! (a)) ? (! IER(778)) : (! (b)) ? (! IER(779)) : \
((a)->multiplicand != (b)->multiplicand) ? 1 : ((a)->multiplier != (b)->multiplier))

  packet_table collisions;    // previous incoming packets
  packet_list incoming;
  uintptr_t limit;            // maximum number of vertices allowed to be created by this worker
  uintptr_t count;            // number of vertices created by this worker
  unsigned sample;
  packet_list *c;
  cru_crosser x;
  node_queue q;
  packet_pod d;
  int unequal;
  int killed;                 // non-zero when the job is killed
  cru_sig s;
  router r;

  q = NULL;
  d = NULL;
  count = 0;
  sample = 0;
  killed = 0;
  collisions = NULL;
  if ((! source) ? IER(780) : (source->gruntled != PORT_MAGIC) ? IER(781) : 0)
	 return NULL;
  if ((!(r = source->local)) ? IER(782) : (r->valid != ROUTER_MAGIC) ? IER(783) : 0)
	 return NULL;
  x = &(r->crosser);
  if ((!(d = source->peers)) ? IER(784) : (r->tag != CRO) ? IER(785) : ! (s = &(x->cr_sig)))
	 goto a;
  if ((s->orders.v_order.equal) ? 0 : IER(786))
	 goto a;
  _cru_set_storage (r->ro_store, err);
  _cru_set_kill_switch (&(r->killed), err);
  limit = x->cr_sig.vertex_limit / r->lanes;
  limit = (limit ? limit : x->cr_sig.vertex_limit ? 1 : 0);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(9);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : incoming->payload ? 0 : IER(787))
		  goto b;
		if (! (c = _cru_collision (incoming->hash_value, &collisions, err)))
		  goto b;
		if (*c)
		  while ((unequal = UNEQUAL((product) incoming->payload, (product) (*c)->payload)) ? (*c)->next_packet : NULL)
			 c = &((*c)->next_packet);
		if (*err)
		  goto b;
		if (*c ? (! unequal) : 0)
		  reach_extant_node (&incoming, *c, d, err);
		else if (limit ? (count++ < limit) : 1)
		  _cru_push_packet (reached_new_node (&incoming, x, &q, d, err), *c ? &((*c)->next_packet) : c, err);
		else 
		  RAISE(CRU_INTOVF);
		continue;
	 b: _cru_free_packets (_cru_unpacked (collisions), (cru_destructor) free_product, err);
		_cru_free_packets (incoming, (cru_destructor) free_product, err);
		collisions = NULL;
		incoming = NULL;
	 }
  _cru_free_packets (_cru_unpacked (collisions), (cru_destructor) free_product, err);
  if (*err)
	 _cru_free_node_queue (q, &(s->destructors), err);
  return (*err ? NULL : q);
 a: return _cru_abort (source, d, err);
}









cru_graph
_cru_cross (g, h, k, r, err)
	  cru_graph g;
	  cru_graph h;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Return the product of two graphs computed concurrently using a
	  // prepared router.
{
  cru_graph x;
  product i;

  x = NULL;
  _cru_disable_killing (k, err);
  if ((! g) ? 1 : (! h) ? 1 : (! r) ? 1 : ! (i = product_of (g->base_node, h->base_node, err)))
	 goto a;
  _cru_graph_launched (k, i, rash (i), r, &x, err);
  if (*err)
	 _cru_free_now (x, err);
  return (*err ? NULL : x);
 a: _cru_free_router (r, err);
  return NULL;
}
