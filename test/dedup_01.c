// Create a hypercubic graph with exogenous vertices and some extra
// exogenous edges using a subconnector function, get rid of the
// extras by deduplicating the graph the easy way with an empty
// merger, and check that it worked.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define EDGES_PER_VERTEX (((uintptr_t) DIMENSION) >> ((DIMENSION & 1) ? 0 : 1))

#define VERTEX_MAGIC 1681134602
#define EDGE_MAGIC 1647408281

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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(2617))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2618) : ++edge_count ? 0 : FAIL(2619))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(2620) : 0))
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
  if ((! e) ? FAIL(2621) : (e->e_magic != EDGE_MAGIC) ? FAIL(2622) : 0)
	 return;
  e->e_magic = MUGGLE(109);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2623) : edge_count-- ? 0 : FAIL(2624))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(2625) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(2626))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2627) : ++vertex_count ? 0 : FAIL(2628))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(2629) : 0))
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
  if ((! v) ? FAIL(2630) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2631) : 0)
	 return;
  v->v_magic = MUGGLE(110);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2632) : vertex_count-- ? 0 : FAIL(2633))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2634) : 0)
	 return;
  free (v);
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

  if ((! e) ? GLOBAL_FAIL(2635) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(2636) : 0)
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

	  // Compare two edges and return non-zero if they are equal. This
	  // function cast to a cru_bpred is passed to the cru library
	  // as b.bu_sig.orders.e_order.equal in the builder b.
{
  if ((! a) ? FAIL(2637) : (a->e_magic != EDGE_MAGIC) ? FAIL(2638) : 0)
	 return 0;
  if ((! b) ? FAIL(2639) : (b->e_magic != EDGE_MAGIC) ? FAIL(2640) : 0)
	 return 0;
  return a->e_value == b->e_value;
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

  if ((! v) ? GLOBAL_FAIL(2641) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(2642) : 0)
	 return 0;
  u = v->v_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
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
  if ((! a) ? FAIL(2643) : (a->v_magic != VERTEX_MAGIC) ? FAIL(2644) : 0)
	 return 0;
  if ((! b) ? FAIL(2645) : (b->v_magic != VERTEX_MAGIC) ? FAIL(2646) : 0)
	 return 0;
  return a->v_value == b->v_value;
}







void
building_rule (initial, incident, given_vertex, err)
	  int initial;                                       // non-zero for the initial vertex
	  edge incident;                                     // label on the incident edge to the given vertex
	  vertex given_vertex;                               // from which outgoing edges are to be created
	  int *err;

	  // Declare one outgoing edge from the given vertex along each
	  // dimensional axis to a vertex whose binary encoding differs
	  // from that of the given vertex in exactly one bit, but only if
	  // the incoming edge label differs by one from that of a
	  // consecutively numbered outgoing edge. This function cast to a
	  // cru_subconnector is passed to the cru library as
	  // b.subconnector in the builder b. Only the initial vertex
	  // should have a zero value. For a dimension of N, the result is
	  // a graph with 2^N vertices and 2 + N2^N edges if N is even, or
	  // 2 + N2^(N+1) edges if N is odd.
{
  uintptr_t i, e;

  if ((! given_vertex) ? FAIL(2647) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2648) : 0)
	 return;
  if (initial)
	 {
		if (given_vertex->v_value ? FAIL(2649) : 0)
		  return;
		if (incident ? FAIL(2650) : 0)
		  return;
		e = 0;            // the incident edge is ignored
	 }
  else
	 {
		if ((! incident) ? FAIL(2651) : (incident->e_magic != EDGE_MAGIC) ? FAIL(2652) : 0)
		  return;
		e = incident->e_value;
	 }
  for (i = 0; i < DIMENSION; i++)
	 if ((i == ((e + 1) % DIMENSION)) ? 1 : (e == ((i + 1) % DIMENSION)))
		cru_connect (edge_of (i, err), vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << i), err), err);
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(2653) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2654) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(2655) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2656) : 0)
	 return 0;
  if ((! remote_vertex) ? FAIL(2657) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2658) : 0)
	 return 0;
  return ! (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(2659) : 0);
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
  if (*err ? 1 : (! given_vertex) ? FAIL(2660) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2661) : 0)
	 return 0;
  return ! ((edges_in != EDGES_PER_VERTEX) ? FAIL(2662) : (edges_out != EDGES_PER_VERTEX) ? FAIL(2663) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(2664) : (s < r) ? FAIL(2665) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(2666))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == (EDGES_PER_VERTEX * NUMBER_OF_VERTICES)) ? 0 : FAIL(2667))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(2668))
	 return 0;
  return 1;
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
	 FAIL(2669);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2670);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2671) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2672) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2673) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2674) : *err);
}








int
main (argc, argv)
	  int argc;
	  char **argv;
{
  struct cru_merger_s c;
  int vertex_lock_created;
  int edge_lock_created;
  uintptr_t limit;
  cru_graph g;
  int err;
  int v;

  struct cru_builder_s b = {
	 .subconnector = (cru_subconnector) building_rule,
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
 
  err = 0;
  memset (&c, 0, sizeof (c));
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  g = cru_merged (g, &c, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(2675) : vertex_count ? THE_FAIL(2676) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2677);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2678);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
