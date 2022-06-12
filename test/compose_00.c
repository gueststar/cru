// Create a hypercubic graph with exogenous vertices and edges and
// iterating connect every vertex to every vertex adjacent to any
// adjacent vertex thereof by two edges having the same label.
// The net effect is to connect each vertex to itself in addition
// to its other connections.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1531825135
#define EDGE_MAGIC 1754440576
#define REDEX_MAGIC 1766270808

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(2048))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(2049) : ++redex_count ? 0 : FAIL(2050))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(2051) : 0))
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
  if ((! r) ? FAIL(2052) : (r->r_magic != REDEX_MAGIC) ? FAIL(2053) : 0)
	 return;
  r->r_magic = MUGGLE(96);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(2054) : redex_count-- ? 0 : FAIL(2055))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(2056) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(2057))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2058) : ++edge_count ? 0 : FAIL(2059))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(2060) : 0))
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
  if ((! e) ? FAIL(2061) : (e->e_magic != EDGE_MAGIC) ? FAIL(2062) : 0)
	 return;
  e->e_magic = MUGGLE(97);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(2063) : edge_count-- ? 0 : FAIL(2064))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(2065) : 0)
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

  if ((! e) ? GLOBAL_FAIL(2066) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(2067) : 0)
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
  if ((! a) ? FAIL(2068) : (a->e_magic != EDGE_MAGIC) ? FAIL(2069) : 0)
	 return 0;
  if ((! b) ? FAIL(2070) : (b->e_magic != EDGE_MAGIC) ? FAIL(2071) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(2072))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2073) : ++vertex_count ? 0 : FAIL(2074))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(2075) : 0))
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
  if ((! v) ? FAIL(2076) : (v->v_magic != VERTEX_MAGIC) ? FAIL(2077) : 0)
	 return;
  v->v_magic = MUGGLE(98);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(2078) : vertex_count-- ? 0 : FAIL(2079))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(2080) : 0)
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
  if ((! a) ? FAIL(2081) : (a->v_magic != VERTEX_MAGIC) ? FAIL(2082) : 0)
	 return 0;
  if ((! b) ? FAIL(2083) : (b->v_magic != VERTEX_MAGIC) ? FAIL(2084) : 0)
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

  if ((! v) ? GLOBAL_FAIL(2085) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(2086) : 0)
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

  if ((! given_vertex) ? FAIL(2087) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2088) : 0)
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

  if (*err ? 1 : (! connecting_edge) ? FAIL(2089) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2090) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(2091) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2092) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(2093) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2094) : 0)
	 return NULL;
  if (connecting_edge->e_value == DIMENSION)
	 return redex_of (1, err);
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(2095) : 0)
	 return NULL;
  return redex_of (1, err);
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
  if (*err ? 1 : (! given_vertex) ? FAIL(2096) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2097) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(2098) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(2099) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(2100) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(2101) : 0)
	 return NULL;
  if ((edges_in->r_value != (DIMENSION + 1)) ? FAIL(2102) : 0)
	 return NULL;
  if ((edges_out->r_value != (DIMENSION + 1)) ? FAIL(2103) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(2104) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(2105) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(2106) : (s < (r ? r->r_value : 0)) ? FAIL(2107) : 0)
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
		  .r_free = (cru_destructor) free_redex,
		  .m_free = (cru_destructor) free_redex},
		.incident = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(2108))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((DIMENSION + 1) * NUMBER_OF_VERTICES)) ? 0 : FAIL(2109))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(2110))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(2111))
	 return 0;
  if (!(v = (r->r_value == NUMBER_OF_VERTICES)))
	 FAIL(2112);
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
	 FAIL(2113);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2114);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2115) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2116) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2117) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2118) : *err);
}






int
qual (local_redex, adjacent_redex, local_label, adjacent_label, err)
	  redex local_redex;
	  redex adjacent_redex;
	  edge local_label;
	  edge adjacent_label;
	  int *err;

	  // Enable bypassing when two edges have the same label.
{
  return equal_edges (local_label, adjacent_label, err);
}





void *
qop (local_redex, adjacent_redex, local_label, adjacent_label, err)
	  redex local_redex;
	  redex adjacent_redex;
	  edge local_label;
	  edge adjacent_label;
	  int *err;

	  // Create a bypassing edge labeled by the dimension.
{
  if ((! local_label) ? FAIL(2119) : (local_label->e_magic != EDGE_MAGIC) ? FAIL(2120) : 0)
	 return NULL;
  if ((! adjacent_label) ? FAIL(2121) : (adjacent_label->e_magic != EDGE_MAGIC) ? FAIL(2122) : 0)
	 return NULL;
  return edge_of (DIMENSION, err);
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
	 .labeler = {
		.qop = (cru_qop) qop,
		.qpred = (cru_qpred) qual}};

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
	 err = (edge_count ? THE_FAIL(2123) : vertex_count ? THE_FAIL(2124) : redex_count ? THE_FAIL(2125) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2126);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2127);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
