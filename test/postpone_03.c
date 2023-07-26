// Create an acyclic hypercubic graph with exogenous vertices and
// labels, transfer a copy of each even numbered edge to the terminus
// of each odd numbered sibling edge labeled by a greater number than
// the even edge, and repeat until a fixed point is reached. The result
// should nodes with varying out-degrees but all edges from any given
// node labeled even or odd.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1191526580
#define EDGE_MAGIC 1914862091
#define REDEX_MAGIC 1627670529
#define MAPEX_MAGIC 1947009769

// a count of allocated edges to detect memory leaks
static uintptr_t edge_count;

// needed to lock the count
static pthread_mutex_t edge_lock;

// a count of allocated vertices to detect memory leaks
static uintptr_t vertex_count;

// needed to lock the count
static pthread_mutex_t vertex_lock;

// a count of allocated redexes to detect memory leaks
static uintptr_t redex_count;

// needed to lock the count
static pthread_mutex_t redex_lock;

typedef struct edge_s
{
  int e_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t e_value;
} *edge;

typedef struct vertex_s
{
  int v_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t v_value;
} *vertex;

typedef struct redex_s
{
  int r_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t r_value;
} *redex;








redex
redex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a redex.
{
  redex r;

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(4965))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(4966) : ++redex_count ? 0 : FAIL(4967))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(4968) : 0))
	 return r;
 a: free (r);
  return NULL;
}







void
free_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex.
{
  if ((! r) ? FAIL(4969) : (r->r_magic != REDEX_MAGIC) ? FAIL(4970) : 0)
	 return;
  r->r_magic = MUGGLE(204);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(4971) : redex_count-- ? 0 : FAIL(4972))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(4973) : 0)
	 return;
  free (r);
}




void
free_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex.
{
  if ((! r) ? FAIL(4974) : (r->r_magic != MAPEX_MAGIC) ? FAIL(4975) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
}







edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(4976))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4977) : ++edge_count ? 0 : FAIL(4978))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(4979) : 0))
	 return e;
 a: free (e);
  return NULL;
}







void
free_edge (e, err)
	  edge e;
	  int *err;

	  // Reclaim an edge. This function cast to a cru_destructor is
	  // passed to the cru library as b.bu_sig.destructors.e_free in the builder
	  // b.
{
  if ((! e) ? FAIL(4980) : (e->e_magic != EDGE_MAGIC) ? FAIL(4981) : 0)
	 return;
  e->e_magic = MUGGLE(205);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4982) : edge_count-- ? 0 : FAIL(4983))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4984) : 0)
	 return;
  free (e);
}







uintptr_t
edge_hash (e)
	  edge e;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed as b.bu_sig.orders.e_order.hash in the builder b,
	  // but is needed only for composing and not building.
{
  size_t i;
  uintptr_t u;

  if ((! e) ? GLOBAL_FAIL(4985) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(4986) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







int
equal_edges (a, b, err)
	  edge a;
	  edge b;
	  int *err;

	  // Compare two edges and return non-zero if they are
	  // equal. This function cast to a cru_bpred is passed to the
	  // cru library as b.bu_sig.orders.e_order.equal in the builder b,
	  // but is needed only for composing.
{
  if ((! a) ? FAIL(4987) : (a->e_magic != EDGE_MAGIC) ? FAIL(4988) : 0)
	 return 0;
  if ((! b) ? FAIL(4989) : (b->e_magic != EDGE_MAGIC) ? FAIL(4990) : 0)
	 return 0;
  return a->e_value == b->e_value;
}







vertex
vertex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a vertex.
{
  vertex v;

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(4991))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4992) : ++vertex_count ? 0 : FAIL(4993))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(4994) : 0))
	 return v;
 a: free (v);
  return NULL;
}








void
free_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex. This function cast to a cru_destructor is
	  // passed to the cru library as b.bu_sig.destructors.v_free in the builder
	  // b.
{
  if ((! v) ? FAIL(4995) : (v->v_magic != VERTEX_MAGIC) ? FAIL(4996) : 0)
	 return;
  v->v_magic = MUGGLE(206);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4997) : vertex_count-- ? 0 : FAIL(4998))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(4999) : 0)
	 return;
  free (v);
}







int
equal_vertices (a, b, err)
	  vertex a;
	  vertex b;
	  int *err;

	  // Compare two vertices and return non-zero if they are
	  // equal. This function cast to a cru_bpred is passed to the
	  // cru library as b.bu_sig.orders.v_order.equal in the builder b.
{
  if ((! a) ? FAIL(5000) : (a->v_magic != VERTEX_MAGIC) ? FAIL(5001) : 0)
	 return 0;
  if ((! b) ? FAIL(5002) : (b->v_magic != VERTEX_MAGIC) ? FAIL(5003) : 0)
	 return 0;
  return a->v_value == b->v_value;
}









uintptr_t
vertex_hash (v)
	  vertex v;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed to the cru library as b.bu_sig.orders.v_order.hash
	  // in the builder b.
{
  size_t i;
  uintptr_t u;

  if ((! v) ? GLOBAL_FAIL(5004) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(5005) : 0)
	 return 0;
  u = v->v_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







void
building_rule (given_vertex, err)
	  vertex given_vertex;
	  int *err;

	  // Declare one outgoing edge from the given vertex along each
	  // dimensional axis to a vertex whose binary encoding differs
	  // from that of the given vertex in exactly one bit, but only if
	  // the remote vertex has more 1s in its encoding. Number the
	  // edges consecutively from zero. This function cast to a
	  // cru_connector is passed to the cru library as b.connector in
	  // the builder b.
{
  uintptr_t outgoing_edge, v, remote_value, local_bits, remote_bits;
  vertex r;
  edge e;

  if ((! given_vertex) ? FAIL(5006) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5007) : 0)
	 return;
  v = given_vertex->v_value;
  for (local_bits = 0; v; v >>= 1)
	 local_bits += (v & 1);
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		v = remote_value = given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge);
		for (remote_bits = 0; v; v >>= 1)
		  remote_bits += (v & 1);
		if (remote_bits <= local_bits)
		  continue;
		r = vertex_of (remote_value, err);
		cru_connect (edge_of (outgoing_edge, err), r, err);
	 }
}










redex
edge_offsetter (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return the redex of 2 plus the edge label.
{
  uintptr_t e;
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(5008) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(5009) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5010) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5011) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5012) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5013) : 0)
	 return NULL;
  if ((r = redex_of (connecting_edge->e_value + 2, err)))
	 r->r_magic = MAPEX_MAGIC;
  return r;
}








redex
vertex_checker (edges_in, given_vertex, edges_out, err)
	  redex edges_in;
	  vertex given_vertex;
	  redex edges_out;
	  int *err;

	  // Validate a vertex based on the incoming and outgoing
	  // edges being valid.
{
  if (*err ? 1 : (! given_vertex) ? FAIL(5014) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5015) : 0)
	 return NULL;
  if (edges_in ? FAIL(5016) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5017) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5018) : 0)
	 return NULL;
  if ((edges_out->r_value != 1) ? FAIL(5019) : 0)
	 return NULL;
  return redex_of (1, err);
}







redex
sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // Compute the sum of two numbers checking for overflow and other
	  // errors.
{
  uintptr_t s;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(5020) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5021) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5022) : (s < (r ? r->r_value : 0)) ? FAIL(5023) : 0)
	 return NULL;
  return redex_of (s, err);
}







redex
matching_parity (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // Return non-zero if two numnbers greater than 1 are both even
	  // or both odd, or if the right one is 1.
{
  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != MAPEX_MAGIC) ? FAIL(5024) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5025) : 0)
	 return NULL;
  return redex_of ((! l) ? 0 : (! r) ? 0 : (! (r->r_value)) ? 1 : ((l->r_value & 1) == (r->r_value & 1)), err);
}





redex
one (err)
	  int *err;

	  // Return a redex with a value of 1, for use as the vacuous case in
	  // a mapreduction over redexes.
{
  return redex_of (1, err);
}






int
valid (g, err)
	  cru_graph g;
	  int *err;

	  // Check for correct vertex counts, edge counts, and edge labels
	  // the hard way by using redexes for numbers.
{
  struct cru_mapreducer_s m = {
	 .ma_prop = {
		.vertex = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) vertex_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .vacuous_case = (cru_nop) one,
		  .map = (cru_top) edge_offsetter,
		  .reduction = (cru_bop) matching_parity,
		  .m_free = (cru_destructor) free_mapex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(5026))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((((uintptr_t) DIMENSION) * NUMBER_OF_VERTICES) >> 1)) ? 0 : FAIL(5027))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(5028))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(5029))
	 return 0;
  if (!(v = (r->r_value == NUMBER_OF_VERTICES)))
	 FAIL(5030);
  free_redex (r, err);
  return v;
}










int
initialized (count, lock, lock_created, err)
	  uintptr_t *count;
	  pthread_mutex_t *lock;
	  int *lock_created;
	  int *err;
{
  pthread_mutexattr_t mutex_attribute;

  if (count ? (*count = 0) : 1)
	 FAIL(5031);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(5032);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(5033) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(5034) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(5035) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(5036) : *err);
}









edge
copied_mark (l, r, err)
	  edge l;
	  edge r;
	  int *err;

	  // Return a copy of the left edge.
{
  if ((! l) ? FAIL(5037) : (l->e_magic != EDGE_MAGIC) ? FAIL(5038) : 0)
	 return 0;
  return edge_of (l->e_value, err);
}







int
even_label (o, l, t, err)
	  vertex o;
	  edge l;
	  vertex t;
	  int *err;

	  // Treat all edges with an even numbered label as postponable.
{
  if ((! o) ? FAIL(5039) : (o->v_magic != VERTEX_MAGIC) ? FAIL(5040) : 0)
	 return 0;
  if ((! t) ? FAIL(5041) : (t->v_magic != VERTEX_MAGIC) ? FAIL(5042) : 0)
	 return 0;
  if ((! l) ? FAIL(5043) : (l->e_magic != EDGE_MAGIC) ? FAIL(5044) : 0)
	 return 0;
  return ! (l->e_value & 1);
}










int
main (argc, argv)
	  int argc;
	  char **argv;
{
  int vertex_lock_created;
  int redex_lock_created;
  int edge_lock_created;
  uintptr_t limit;
  cru_graph g;
  int v, err;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule,
	 .bu_sig = {
		.orders = {
		  .e_order = {
			 .hash = (cru_hash) edge_hash,
			 .equal = (cru_bpred) equal_edges},
		  .v_order = {
			 .hash = (cru_hash) vertex_hash,
			 .equal = (cru_bpred) equal_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge,
		  .v_free = (cru_destructor) free_vertex}}};

  struct cru_postponer_s p = {
	 .po_fix = 1,
	 .postponement = {
		.bop = (cru_bop) copied_mark},
	 .postponable = (cru_tpred) even_label};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  if (! initialized (&redex_count, &redex_lock, &redex_lock_created, &err))
	 goto a;
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  g = cru_postponed (g, &p, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(5045) : vertex_count ? THE_FAIL(5046) : redex_count ? THE_FAIL(5047) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(5048);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(5049);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
