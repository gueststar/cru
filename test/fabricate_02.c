// Create a hypercubic graph with exogenous vertices and edges,
// fabricate a copy with flipped vertices and check its consistency.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 2071755565
#define FLIPPED_VERTEX_MAGIC 1578221296
#define EDGE_MAGIC 1578697261

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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(2724))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2725) : ++edge_count ? 0 : FAIL(2726))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(2727) : 0))
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
  if ((! e) ? FAIL(2728) : (e->e_magic != EDGE_MAGIC) ? FAIL(2729) : 0)
	 return;
  e->e_magic = MUGGLE(120);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2730) : edge_count-- ? 0 : FAIL(2731))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(2732) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(2733))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2734) : ++vertex_count ? 0 : FAIL(2735))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(2736) : 0))
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
  if ((! v) ? FAIL(2737) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2738) : 0)
	 return;
  v->v_magic = MUGGLE(121);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2739) : vertex_count-- ? 0 : FAIL(2740))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2741) : 0)
	 return;
  free (v);
}







void
free_flipped_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex. This function cast to a cru_destructor is
	  // passed to the cru library as b.bu_sig.destructors.v_free in the builder
	  // b.
{
  if ((! v) ? FAIL(2742) : (v->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2743) : 0)
	 return;
  v->v_magic = MUGGLE(122);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2744) : vertex_count-- ? 0 : FAIL(2745))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2746) : 0)
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
  if ((! a) ? FAIL(2747) : (a->v_magic != VERTEX_MAGIC) ? FAIL(2748) : 0)
	 return 0;
  if ((! b) ? FAIL(2749) : (b->v_magic != VERTEX_MAGIC) ? FAIL(2750) : 0)
	 return 0;
  return a->v_value == b->v_value;
}







uintptr_t
vertex_hash (v, err)
	  vertex v;
	  int *err;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed to the cru library as b.bu_sig.orders.v_order.hash
	  // in the builder b.
{
  size_t i;
  uintptr_t u;

  if ((! v) ? FAIL(2751) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2752) : 0)
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

  if ((! given_vertex) ? FAIL(2753) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2754) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge), err);
		cru_connect (edge_of (outgoing_edge, err), remote_vertex, err);
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(2755) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2756) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(2757) : (local_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2758) : 0)
	 return 0;
  if ((! remote_vertex) ? FAIL(2759) : (remote_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2760) : 0)
	 return 0;
  return ! (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(2761) : 0);
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
  if (*err ? 1 : (! given_vertex) ? FAIL(2762) : (given_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2763) : 0)
	 return 0;
  return ! ((edges_in != DIMENSION) ? FAIL(2764) : (edges_out != DIMENSION) ? FAIL(2765) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(2766) : (s < r) ? FAIL(2767) : 0) ? 0 : s);
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
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) vertex_checker},
		.incident = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker}}};

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(2768))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == DIMENSION * NUMBER_OF_VERTICES) ? 0 : FAIL(2769))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(2770))
	 return 0;
  return 1;
}








vertex
vflip (given_vertex, err)
	  vertex given_vertex;
	  int *err;

	  // Return the bitwise complement of the given vertex.
{
  vertex v;

  if (*err ? 1 : (! given_vertex) ? FAIL(2771) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2772) : 0)
	 return NULL;
  if ((v = vertex_of (given_vertex->v_value ^ (uintptr_t) (NUMBER_OF_VERTICES - 1), err)))
	 v->v_magic = FLIPPED_VERTEX_MAGIC;
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
	 FAIL(2773);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2774);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2775) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2776) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2777) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2778) : *err);
}







int
main (argc, argv)
	  int argc;
	  char **argv;
{
  int vertex_lock_created;
  int edge_lock_created;
  uintptr_t limit;
  cru_graph g, f;
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

  struct cru_fabricator_s a = {
	 .v_fab = (cru_uop) vflip,
	 .fa_sig = {
		.destructors = {
		  .v_free = (cru_destructor) free_flipped_vertex}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  f = cru_fabricated (g, &a, UNKILLABLE, LANES, &err);
  v = valid (f, &err);
  cru_free_now (f, LANES, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  err = (edge_count ? THE_FAIL(2779) : vertex_count ? THE_FAIL(2780) : err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2781);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2782);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
