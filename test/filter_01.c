// Create a hypercubic graph with exogenous vertices and edges and
// filter out any edge terminating on a vertex whose bit count is half
// the dimension, which eliminates the major diagonal hyperplane and
// anything on the other side of it. The vertices on the frontier
// should also lose some of their outgoing and incoming edges.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1749875071
#define EDGE_MAGIC 1188891083

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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(2895))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2896) : ++edge_count ? 0 : FAIL(2897))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(2898) : 0))
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
  if ((! e) ? FAIL(2899) : (e->e_magic != EDGE_MAGIC) ? FAIL(2900) : 0)
	 return;
  e->e_magic = MUGGLE(125);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2901) : edge_count-- ? 0 : FAIL(2902))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(2903) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(2904))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2905) : ++vertex_count ? 0 : FAIL(2906))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(2907) : 0))
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
  if ((! v) ? FAIL(2908) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2909) : 0)
	 return;
  v->v_magic = MUGGLE(126);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2910) : vertex_count-- ? 0 : FAIL(2911))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2912) : 0)
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
  if ((! a) ? FAIL(2913) : (a->v_magic != VERTEX_MAGIC) ? FAIL(2914) : 0)
	 return 0;
  if ((! b) ? FAIL(2915) : (b->v_magic != VERTEX_MAGIC) ? FAIL(2916) : 0)
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

  if ((! v) ? GLOBAL_FAIL(2917) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(2918) : 0)
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

  if ((! given_vertex) ? FAIL(2919) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2920) : 0)
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

	  // Return 1 if an edge is labeled by the index of the bit in
	  // which its endpoints differ, and 0 otherwise.
{
  if (*err ? 1 : (! connecting_edge) ? FAIL(2921) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2922) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(2923) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2924) : 0)
	 return 0;
  if ((! remote_vertex) ? FAIL(2925) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2926) : 0)
	 return 0;
  return ! (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(2927) : 0);
}








uintptr_t
vertex_checker (edges_in, given_vertex, edges_out, err)
	  uintptr_t edges_in;
	  vertex given_vertex;
	  uintptr_t edges_out;
	  int *err;

	  // Validate a vertex based on the incoming and outgoing
	  // edges being valid.
{
  int v, bits;

  if (*err ? 1 : (! given_vertex) ? FAIL(2928) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2929) : 0)
	 return 0;
  v = (int) given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if (bits < ((DIMENSION >> 1) - 1))
	 return ! ((edges_in != DIMENSION) ? FAIL(2930) : (edges_out != DIMENSION) ? FAIL(2931) : 0);
  return ! ((edges_in != bits) ? FAIL(2932) : (edges_out != bits) ? FAIL(2933) : 0);
}




uintptr_t
null (err)
	  int *err;

	  // Unconditionally return zero. This function is used as as the
	  // vacuous case in a map reduction.
{
  return 0;
}






uintptr_t
sum (l, r, err)
	  uintptr_t l;
	  uintptr_t r;
	  int *err;

	  // Compute the sum of two numbers checking for overflow and other
	  // errors.
{
  uintptr_t s;

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(2934) : (s < r) ? FAIL(2935) : 0) ? 0 : s);
}






uintptr_t
choose (a, b)
	  uintptr_t a;
	  uintptr_t b;

	  // Return a!/b!(a-b)! ignoring overflow.
{
  uintptr_t c, d;

  for (d = c = 1; b; b--)
	 c = (c * a--) / d++;
  return c;
}






int
valid (g, err)
	  cru_graph g;
	  int *err;

	  // Check for correct vertex counts, edge counts, and edge labels.
{
#define REMAINING_VERTICES ((NUMBER_OF_VERTICES - (choose (DIMENSION, DIMENSION >> 1) << (DIMENSION & 1))) >> 1)
#define DISCONNECTIONS (choose (DIMENSION, (DIMENSION >> 1) - 1) * (((DIMENSION + 1) >> 1) + 1))
#define REMAINING_EDGES ((REMAINING_VERTICES * DIMENSION) - DISCONNECTIONS)

  struct cru_mapreducer_s m = {
	 .ma_prop = {
		.vertex = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) vertex_checker},
		.incident = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) null,
		  .map = (cru_top) edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) null,
		  .map = (cru_top) edge_checker}}};

  if ((cru_vertex_count (g, LANES, err) == REMAINING_VERTICES) ? 0 : FAIL(2936))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == REMAINING_EDGES) ? 0 : FAIL(2937))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == REMAINING_VERTICES) ? 0 : FAIL(2938))
	 return 0;
  return 1;
}







uintptr_t
edge_test (local_vertex, connecting_edge, adjacent_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex adjacent_vertex;
	  int *err;

 	  // Return non-zero if the number of 1 bits in the remote vertex
	  // differs from DIMENSION >> 1. This function cast as a cru_top
	  // is passed to the cru library as f.fi_kernel.e_op.map in the filter
	  // f.
{
  int bits;
  int b;

  if ((! connecting_edge) ? FAIL(2939) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2940) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(2941) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2942) : 0)
	 return 0;
  if ((! adjacent_vertex) ? FAIL(2943) : (adjacent_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2944) : 0)
	 return 0;
  b = (int) adjacent_vertex->v_value;
  for (bits = 0; b; b >>= 1)
	 bits += (b & 1);
  return (bits != (DIMENSION >> 1));
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
	 FAIL(2945);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2946);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2947) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2948) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2949) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2950) : *err);
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
	 err = (edge_count ? THE_FAIL(2951) : vertex_count ? THE_FAIL(2952) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2953);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2954);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
