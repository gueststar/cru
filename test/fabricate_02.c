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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(2817))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2818) : ++edge_count ? 0 : FAIL(2819))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(2820) : 0))
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
  if ((! e) ? FAIL(2821) : (e->e_magic != EDGE_MAGIC) ? FAIL(2822) : 0)
	 return;
  e->e_magic = MUGGLE(116);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2823) : edge_count-- ? 0 : FAIL(2824))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(2825) : 0)
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
  if ((! a) ? FAIL(2826) : (a->e_magic != EDGE_MAGIC) ? FAIL(2827) : 0)
	 return 0;
  if ((! b) ? FAIL(2828) : (b->e_magic != EDGE_MAGIC) ? FAIL(2829) : 0)
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

  if ((! e) ? GLOBAL_FAIL(2830) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(2831) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(2832))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2833) : ++vertex_count ? 0 : FAIL(2834))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(2835) : 0))
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
  if ((! v) ? FAIL(2836) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2837) : 0)
	 return;
  v->v_magic = MUGGLE(117);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2838) : vertex_count-- ? 0 : FAIL(2839))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2840) : 0)
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
  if ((! v) ? FAIL(2841) : (v->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2842) : 0)
	 return;
  v->v_magic = MUGGLE(118);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2843) : vertex_count-- ? 0 : FAIL(2844))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2845) : 0)
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
  if ((! a) ? FAIL(2846) : (a->v_magic != VERTEX_MAGIC) ? FAIL(2847) : 0)
	 return 0;
  if ((! b) ? FAIL(2848) : (b->v_magic != VERTEX_MAGIC) ? FAIL(2849) : 0)
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

  if ((! v) ? FAIL(2850) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2851) : 0)
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

  if ((! given_vertex) ? FAIL(2852) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2853) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(2854) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2855) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(2856) : (local_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2857) : 0)
	 return 0;
  if ((! remote_vertex) ? FAIL(2858) : (remote_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2859) : 0)
	 return 0;
  return ! (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(2860) : 0);
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
  if (*err ? 1 : (! given_vertex) ? FAIL(2861) : (given_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(2862) : 0)
	 return 0;
  return ! ((edges_in != DIMENSION) ? FAIL(2863) : (edges_out != DIMENSION) ? FAIL(2864) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(2865) : (s < r) ? FAIL(2866) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(2867))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((uintptr_t) DIMENSION) * NUMBER_OF_VERTICES) ? 0 : FAIL(2868))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(2869))
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

  if (*err ? 1 : (! given_vertex) ? FAIL(2870) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2871) : 0)
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
	 FAIL(2872);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2873);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2874) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2875) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2876) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2877) : *err);
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
		  .e_order = {
			 .hash = (cru_hash) edge_hash,
			 .equal = (cru_bpred) equal_edges},
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
  err = (edge_count ? THE_FAIL(2878) : vertex_count ? THE_FAIL(2879) : err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2880);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2881);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
