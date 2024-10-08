// Create a hypercubic graph with exogenous vertices and edges and
// filter out all of the vertices. The result should be an empty
// graph.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1480926779
#define EDGE_MAGIC 1261959105

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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(3384))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3385) : ++edge_count ? 0 : FAIL(3386))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(3387) : 0))
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
  if ((! e) ? FAIL(3388) : (e->e_magic != EDGE_MAGIC) ? FAIL(3389) : 0)
	 return;
  e->e_magic = MUGGLE(137);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3390) : edge_count-- ? 0 : FAIL(3391))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(3392) : 0)
	 return;
  free (e);
}






int
equal_edges (a, b, err)
	  edge a;
	  edge b;
	  int *err;

	  // Compare two edges and return non-zero if they are equal. This
	  // function cast to a cru_bpred is passed to the cru library
	  // as b.bu_sig.orders.e_order.equal in the builder b.
{
  if ((! a) ? FAIL(3393) : (a->e_magic != EDGE_MAGIC) ? FAIL(3394) : 0)
	 return 0;
  if ((! b) ? FAIL(3395) : (b->e_magic != EDGE_MAGIC) ? FAIL(3396) : 0)
	 return 0;
  return a->e_value == b->e_value;
}







uintptr_t
edge_hash (e)
	  edge e;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed to the cru library as b.bu_sig.orders.e_order.hash
	  // in the builder b.
{
  size_t i;
  uintptr_t u;

  if ((! e) ? GLOBAL_FAIL(3397) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(3398) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}








vertex
vertex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a vertex.
{
  vertex v;

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(3399))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3400) : ++vertex_count ? 0 : FAIL(3401))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(3402) : 0))
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
  if ((! v) ? FAIL(3403) : (v->v_magic != VERTEX_MAGIC) ? FAIL(3404) : 0)
	 return;
  v->v_magic = MUGGLE(138);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3405) : vertex_count-- ? 0 : FAIL(3406))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3407) : 0)
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
  if ((! a) ? FAIL(3408) : (a->v_magic != VERTEX_MAGIC) ? FAIL(3409) : 0)
	 return 0;
  if ((! b) ? FAIL(3410) : (b->v_magic != VERTEX_MAGIC) ? FAIL(3411) : 0)
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

  if ((! v) ? GLOBAL_FAIL(3412) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(3413) : 0)
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

  if ((! given_vertex) ? FAIL(3414) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3415) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge), err);
		cru_connect (e = edge_of (outgoing_edge, err), remote_vertex, err);
	 }
}








uintptr_t
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Raise an error and return 1 because there should be no edges.
{
  FAIL(3416);
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
  FAIL(3417);
  return 0;
}






uintptr_t
fail (l, r, err)
	  uintptr_t l;
	  uintptr_t r;
	  int *err;

	  // Raise an error and return zero, because the set of vertices
	  // shouldn't need reduction.
{
  FAIL(3418);
  return 0;
}





uintptr_t
null (err)
	  int *err;

	  // Unconditionally return zero. This function is used as as the
	  // vacuous case in a vertex map reduction.
{
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
		  .map = (cru_top) vertex_checker,
		  .vacuous_case = (cru_nop) null},
		.incident = {
		  .reduction = (cru_bop) fail,
		  .map = (cru_top) edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) fail,
		  .map = (cru_top) edge_checker}}};

  if (cru_vertex_count (g, LANES, err) ? FAIL(3419) : 0)
	 return 0;
  if (cru_edge_count (g, LANES, err) ? FAIL(3420) : 0)
	 return 0;
  if (cru_mapreduced (g, &m, UNKILLABLE, LANES, err) ?  FAIL(3421) : 0)
	 return 0;
  return 1;
}






uintptr_t
vertex_test (edges_in, given_vertex, edges_out, err)
	  uintptr_t edges_in;
	  vertex given_vertex;
	  uintptr_t edges_out;
	  int *err;

	  // Return zero to make all vertices fail the_test.
{
  if ((! given_vertex) ? FAIL(3422) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3423) : 0)
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
	 FAIL(3424);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(3425);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(3426) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(3427) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(3428) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(3429) : *err);
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
		  .e_order = {
			 .hash = (cru_hash) edge_hash,
			 .equal = (cru_bpred) equal_edges},
		  .v_order = {
			 .hash = (cru_hash) vertex_hash,
			 .equal = (cru_bpred) equal_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge,
		  .v_free = (cru_destructor) free_vertex}}};

  struct cru_filter_s f = {
	 .fi_kernel = {
		.v_op = {
		  .vertex = {
			 .map = (cru_top) vertex_test}}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  g = cru_filtered (g, &f, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(3430) : vertex_count ? THE_FAIL(3431) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3432);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(3433);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
