// Create a hypercubic graph with exogenous vertices and edges and
// filter out all of the edges. The result should be a graph with just
// one vertex.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1732456064
#define EDGE_MAGIC 2015789833

// a count of allocated edges to detect memory leaks
static uintptr_t edge_count;

// needed to lock the count
static pthread_mutex_t edge_lock;

// a count of allocated vertices to detect memory leaks
static uintptr_t vertex_count;

// needed to lock the count
static pthread_mutex_t vertex_lock;

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







edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(3191))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3192) : ++edge_count ? 0 : FAIL(3193))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(3194) : 0))
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
  if ((! e) ? FAIL(3195) : (e->e_magic != EDGE_MAGIC) ? FAIL(3196) : 0)
	 return;
  e->e_magic = MUGGLE(135);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3197) : edge_count-- ? 0 : FAIL(3198))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(3199) : 0)
	 return;
  free (e);
}








vertex
vertex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a vertex.
{
  vertex v;

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(3200))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3201) : ++vertex_count ? 0 : FAIL(3202))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(3203) : 0))
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
  if ((! v) ? FAIL(3204) : (v->v_magic != VERTEX_MAGIC) ? FAIL(3205) : 0)
	 return;
  v->v_magic = MUGGLE(136);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3206) : vertex_count-- ? 0 : FAIL(3207))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3208) : 0)
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
  if ((! a) ? FAIL(3209) : (a->v_magic != VERTEX_MAGIC) ? FAIL(3210) : 0)
	 return 0;
  if ((! b) ? FAIL(3211) : (b->v_magic != VERTEX_MAGIC) ? FAIL(3212) : 0)
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

  if ((! v) ? GLOBAL_FAIL(3213) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(3214) : 0)
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
	  // from that of the given vertex in exactly one bit. Number the
	  // edges consecutively from zero. This function cast to a
	  // cru_connector is passed to the cru library as b.connector in
	  // the builder b.
{
  uintptr_t outgoing_edge;
  vertex remote_vertex;
  edge e;

  if ((! given_vertex) ? FAIL(3215) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3216) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge), err);
		cru_connect (e = edge_of (outgoing_edge, err), remote_vertex, err);
	 }
}








uintptr_t
null (err)
	  int *err;

	  // Unconditionally return zero. This function is the used as the
	  // vacuous case in an edge reduction.
{
  return 0;
}







uintptr_t
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Raise an error and return 1 because there should be no edges.
{
  FAIL(3217);
  return 1;
}








uintptr_t
vertex_checker (edges_in, given_vertex, edges_out, err)
	  uintptr_t edges_in;
	  vertex given_vertex;
	  uintptr_t edges_out;
	  int *err;

	  // Validate a vertex based on there being no edges and the value
	  // being 0.
{
  if (*err ? 1 : (! given_vertex) ? FAIL(3218) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3219) : 0)
	 return 0;
  return ! (given_vertex->v_value ? FAIL(3220) : edges_in ? FAIL(3221) : edges_out ? FAIL(3222) : 0);
}






uintptr_t
fail (l, r, err)
	  uintptr_t l;
	  uintptr_t r;
	  int *err;

	  // Raise an error and return zero, because the set of vertices
	  // shouldn't need reduction.
{
  FAIL(3223);
  return 0;
}






int
valid (g, err)
	  cru_graph g;
	  int *err;

	  // Check for correct vertex counts, edge counts, and edge labels.
{
  struct cru_mapreducer_s m = {
	 .ma_prop = {
		.vertex = {
		  .reduction = (cru_bop) fail,
		  .map = (cru_top) vertex_checker},
		.incident = {
		  .reduction = (cru_bop) fail,
		  .vacuous_case = (cru_nop) null,
		  .map = (cru_top) edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) fail,
		  .vacuous_case = (cru_nop) null,
		  .map = (cru_top) edge_checker}}};

  if ((cru_vertex_count (g, LANES, err) == 1) ? 0 : FAIL(3224))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == 0) ? 0 : FAIL(3225))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == 1) ? 0 : FAIL(3226))
	 return 0;
  return 1;
}





uintptr_t
edge_test (local_vertex, connecting_edge, adjacent_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex adjacent_vertex;
	  int *err;

	  // Return 0 to filter out all edges.
{
  if ((! connecting_edge) ? FAIL(3227) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3228) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(3229) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3230) : 0)
	 return 0;
  if ((! adjacent_vertex) ? FAIL(3231) : (adjacent_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3232) : 0)
	 return 0;
  return 0;
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
	 FAIL(3233);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(3234);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(3235) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(3236) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(3237) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(3238) : *err);
}







int
main (argc, argv)
	  int argc;
	  char **argv;
{
  int vertex_lock_created;
  int edge_lock_created;
  uintptr_t limit;
  cru_graph g;
  int err;
  int v;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule,
	 .bu_sig = {
		.orders = {
		  .v_order = {
			 .hash = (cru_hash) vertex_hash,
			 .equal = (cru_bpred) equal_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge,
		  .v_free = (cru_destructor) free_vertex}}};

  struct cru_filter_s f = {
	 .fi_kernel = {
		.e_op = {
		  .map = (cru_top) edge_test}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  g = cru_filtered (cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err), &f, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(3239) : vertex_count ? THE_FAIL(3240) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3241);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(3242);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
