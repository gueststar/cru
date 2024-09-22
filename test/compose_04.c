// Create a hypercubic graph with exogenous vertices and edges, and
// then iterate the operation of creating a bypassing edge from any
// vertex to the successor of any adjacent vertex having the opposite
// parity and deleting the original outgoing edge.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1174051543
#define EDGE_MAGIC 1815008869
#define REDEX_MAGIC 1845352406

#define DEGREE ((((uintptr_t) DIMENSION) - 1) << 1)

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(2462))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(2463) : ++redex_count ? 0 : FAIL(2464))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(2465) : 0))
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
  if ((! r) ? FAIL(2466) : (r->r_magic != REDEX_MAGIC) ? FAIL(2467) : 0)
	 return;
  r->r_magic = MUGGLE(104);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(2468) : redex_count-- ? 0 : FAIL(2469))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(2470) : 0)
	 return;
  free (r);
}






edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(2471))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2472) : ++edge_count ? 0 : FAIL(2473))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(2474) : 0))
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
  if ((! e) ? FAIL(2475) : (e->e_magic != EDGE_MAGIC) ? FAIL(2476) : 0)
	 return;
  e->e_magic = MUGGLE(105);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2477) : edge_count-- ? 0 : FAIL(2478))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(2479) : 0)
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

  if ((! e) ? GLOBAL_FAIL(2480) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(2481) : 0)
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
  if ((! a) ? FAIL(2482) : (a->e_magic != EDGE_MAGIC) ? FAIL(2483) : 0)
	 return 0;
  if ((! b) ? FAIL(2484) : (b->e_magic != EDGE_MAGIC) ? FAIL(2485) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(2486))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2487) : ++vertex_count ? 0 : FAIL(2488))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(2489) : 0))
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
  if ((! v) ? FAIL(2490) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2491) : 0)
	 return;
  v->v_magic = MUGGLE(106);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2492) : vertex_count-- ? 0 : FAIL(2493))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2494) : 0)
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
  if ((! a) ? FAIL(2495) : (a->v_magic != VERTEX_MAGIC) ? FAIL(2496) : 0)
	 return 0;
  if ((! b) ? FAIL(2497) : (b->v_magic != VERTEX_MAGIC) ? FAIL(2498) : 0)
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

  if ((! v) ? GLOBAL_FAIL(2499) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(2500) : 0)
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

  if ((! given_vertex) ? FAIL(2501) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2502) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge), err);
		cru_connect (edge_of (outgoing_edge, err), remote_vertex, err);
	 }
}










redex
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return 1 if an edge is labeled by the index of the bit in
	  // which its endpoints differ, and 0 otherwise.
{
  uintptr_t e;

  if (*err ? 1 : (! connecting_edge) ? FAIL(2503) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2504) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(2505) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2506) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(2507) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2508) : 0)
	 return NULL;
  if (! (connecting_edge->e_value))
	 goto a;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 + (1 << connecting_edge->e_value))) ? FAIL(2509) : 0)
  	 return NULL;
 a: return redex_of (1, err);
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
  if (*err ? 1 : (! given_vertex) ? FAIL(2510) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2511) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(2512) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(2513) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(2514) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(2515) : 0)
	 return NULL;
  if ((edges_in->r_value != DEGREE) ? FAIL(2516) : (edges_out->r_value != DEGREE) ? FAIL(2517) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(2518) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(2519) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(2520) : (s < (r ? r->r_value : 0)) ? FAIL(2521) : 0)
	 return NULL;
  return redex_of (s, err);
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
		.incident = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .r_free = (cru_destructor) free_redex,
		  .m_free = (cru_destructor) free_redex},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if ((cru_vertex_count (g, LANES, err) == (NUMBER_OF_VERTICES >> 1)) ? 0 : FAIL(2522))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == (DEGREE * (NUMBER_OF_VERTICES >> 1))) ? 0 : FAIL(2523))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(2524))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(2525))
	 return 0;
  if (!(v = (r->r_value == (NUMBER_OF_VERTICES >> 1))))
	 FAIL(2526);
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
	 FAIL(2527);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2528);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2529) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2530) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2531) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2532) : *err);
}








redex
parity (edges_in, given_vertex, edges_out, err)
	  redex edges_in;
	  vertex given_vertex;
	  redex edges_out;
	  int *err;

	  // Let the local and adjacent conditions for a bypassing edge consist
	  // only in the parity of the given vertex.
{
  if (*err ? 1 : (! given_vertex) ? FAIL(2533) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2534) : 0)
	 return NULL;
  if (edges_in ? FAIL(2535) : edges_out ? FAIL(2536) : 0)
	 return NULL;
  return redex_of (given_vertex->v_value & 1, err);
}








redex
anti_parity (edges_in, given_vertex, edges_out, err)
	  redex edges_in;
	  vertex given_vertex;
	  redex edges_out;
	  int *err;

	  // Let the local and adjacent conditions for a bypassing edge consist
	  // only in the parity of the given vertex.
{
  if (*err ? 1 : (! given_vertex) ? FAIL(2537) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2538) : 0)
	 return NULL;
  if (edges_in ? FAIL(2539) : edges_out ? FAIL(2540) : 0)
	 return NULL;
  return redex_of (! (given_vertex->v_value & 1), err);
}







int
qual (local_redex, local_label, adjacent_redex, adjacent_label, err)
	  redex local_redex;
	  edge local_label;
	  redex adjacent_redex;
	  edge adjacent_label;
	  int *err;

	  // Enable a bypassing edge across adjacent vertices of
	  // opposite parities.
{
  if ((! local_redex) ? FAIL(2541) : (local_redex->r_magic != REDEX_MAGIC) ? FAIL(2542) : 0)
	 return 0;
  if ((! adjacent_redex) ? FAIL(2543) : (adjacent_redex->r_magic != REDEX_MAGIC) ? FAIL(2544) : 0)
	 return 0;
  return (local_redex->r_value == adjacent_redex->r_value);
}





void *
qop (local_redex, local_label, adjacent_redex, adjacent_label, err)
	  redex local_redex;
	  edge local_label;
	  redex adjacent_redex;
	  edge adjacent_label;
	  int *err;

	  // Create a bypassing edge with the same label as the launching edge.
{
  if ((! local_label) ? FAIL(2545) : (local_label->e_magic != EDGE_MAGIC) ? FAIL(2546) : 0)
	 return NULL;
  if ((! adjacent_label) ? FAIL(2547) : (adjacent_label->e_magic != EDGE_MAGIC) ? FAIL(2548) : 0)
	 return NULL;
  return edge_of (local_label->e_value, err);
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

  struct cru_composer_s c = {
	 .co_fix = 1,
	 .destructive = 1,
	 .labeler = {
		.qop = (cru_qop) qop,
		.qpred = (cru_qpred) qual},
	 .co_props = {
		.local = {
		  .vertex = {
			 .map = (cru_top) parity,
			 .m_free = (cru_destructor) free_redex}},
		.adjacent = {
		  .vertex = {
			 .map = (cru_top) anti_parity,
			 .m_free = (cru_destructor) free_redex}}}};
 
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
  g = cru_composed (g, &c, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(2549) : vertex_count ? THE_FAIL(2550) : redex_count ? THE_FAIL(2551) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2552);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2553);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
