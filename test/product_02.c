// Create a hypercubic graph with exogenous vertices and edges and
// cross it with itself to construct a graph of 2^(2D - 1) vertices
// and D^2 edges from each vertex. Take the opportunity to use
// different magic numbers in the result types to check that cru
// doesn't make a type error. This test is very memory intensive, with
// ten dimensional cube taking north of 8 GB and each increment to the
// dimension quadrupling that requirement. To save memory, this test
// uses only half the dimension defined in readme.h.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define HALF_DIMENSION (DIMENSION >> 1)
#define VERTEX_MAGIC 1982665750
#define EDGE_MAGIC 1882637523
#define REDEX_MAGIC 1331378400
#define CROSS_VERTEX_MAGIC 1269026545
#define CROSS_EDGE_MAGIC 1687895397

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(5122))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5123) : ++redex_count ? 0 : FAIL(5124))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(5125) : 0))
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
  if ((! r) ? FAIL(5126) : (r->r_magic != REDEX_MAGIC) ? FAIL(5127) : 0)
	 return;
  r->r_magic = MUGGLE(210);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5128) : redex_count-- ? 0 : FAIL(5129))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(5130) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(5131))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5132) : ++edge_count ? 0 : FAIL(5133))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(5134) : 0))
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
  if ((! e) ? FAIL(5135) : (e->e_magic != EDGE_MAGIC) ? FAIL(5136) : 0)
	 return;
  e->e_magic = MUGGLE(211);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5137) : edge_count-- ? 0 : FAIL(5138))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(5139) : 0)
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
  if ((! a) ? FAIL(5140) : (a->e_magic != EDGE_MAGIC) ? FAIL(5141) : 0)
	 return 0;
  if ((! b) ? FAIL(5142) : (b->e_magic != EDGE_MAGIC) ? FAIL(5143) : 0)
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

  if ((! e) ? GLOBAL_FAIL(5144) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(5145) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







void
free_cross_edge (e, err)
	  edge e;
	  int *err;

	  // as above for cross edges
{
  if ((! e) ? FAIL(5146) : (e->e_magic != CROSS_EDGE_MAGIC) ? FAIL(5147) : 0)
	 return;
  e->e_magic = EDGE_MAGIC;
  free_edge (e, err);
}








vertex
vertex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a vertex.
{
  vertex v;

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(5148))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5149) : ++vertex_count ? 0 : FAIL(5150))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(5151) : 0))
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
  if ((! v) ? FAIL(5152) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5153) : 0)
	 return;
  v->v_magic = MUGGLE(212);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5154) : vertex_count-- ? 0 : FAIL(5155))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(5156) : 0)
	 return;
  free (v);
}








void
free_cross_vertex (v, err)
	  vertex v;
	  int *err;

	  // as above for cross vertices
{
  if ((! v) ? FAIL(5157) : (v->v_magic != CROSS_VERTEX_MAGIC) ? FAIL(5158) : 0)
	 return;
  v->v_magic = VERTEX_MAGIC;
  free_vertex (v, err);
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
  if ((! a) ? FAIL(5159) : (a->v_magic != VERTEX_MAGIC) ? FAIL(5160) : 0)
	 return 0;
  if ((! b) ? FAIL(5161) : (b->v_magic != VERTEX_MAGIC) ? FAIL(5162) : 0)
	 return 0;
  return a->v_value == b->v_value;
}







int
equal_cross_vertices (a, b, err)
	  vertex a;
	  vertex b;
	  int *err;

	  // as above for cross vertices
{
  if ((! a) ? FAIL(5163) : (a->v_magic != CROSS_VERTEX_MAGIC) ? FAIL(5164) : 0)
	 return 0;
  if ((! b) ? FAIL(5165) : (b->v_magic != CROSS_VERTEX_MAGIC) ? FAIL(5166) : 0)
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

  if ((! v) ? GLOBAL_FAIL(5167) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(5168) : 0)
	 return 0;
  u = v->v_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}









uintptr_t
cross_vertex_hash (v)
	  vertex v;

	  // as above for cross vertices
{
  size_t i;
  uintptr_t u;

  if ((! v) ? GLOBAL_FAIL(5169) : (v->v_magic != CROSS_VERTEX_MAGIC) ? GLOBAL_FAIL(5170) : 0)
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

  if ((! given_vertex) ? FAIL(5171) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5172) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < HALF_DIMENSION; outgoing_edge++)
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
#define HI(x) (x >> HALF_DIMENSION)
#define LO(x) (x & ((1 << HALF_DIMENSION) - 1))

  if (*err ? 1 : (! connecting_edge) ? FAIL(5173) : (connecting_edge->e_magic != CROSS_EDGE_MAGIC) ? FAIL(5174) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5175) : (local_vertex->v_magic != CROSS_VERTEX_MAGIC) ? FAIL(5176) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5177) : (remote_vertex->v_magic != CROSS_VERTEX_MAGIC) ? FAIL(5178) : 0)
	 return NULL;
  if (((HI(local_vertex->v_value) ^ HI(remote_vertex->v_value)) != (1 << HI(connecting_edge->e_value))) ? FAIL(5179) : 0)
	 return NULL;
  if (((LO(local_vertex->v_value) ^ LO(remote_vertex->v_value)) != (1 << LO(connecting_edge->e_value))) ? FAIL(5180) : 0)
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
  if (*err ? 1 : (! given_vertex) ? FAIL(5181) : (given_vertex->v_magic != CROSS_VERTEX_MAGIC) ? FAIL(5182) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(5183) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(5184) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5185) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5186) : 0)
	 return NULL;
  if ((edges_in->r_value != (HALF_DIMENSION * HALF_DIMENSION)) ? FAIL(5187) : 0)
	 return NULL;
  if ((edges_out->r_value != (HALF_DIMENSION * HALF_DIMENSION)) ? FAIL(5188) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(5189) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5190) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5191) : (s < (r ? r->r_value : 0)) ? FAIL(5192) : 0)
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
#define V (1 << ((HALF_DIMENSION << 1) - 1))
#define E (V * HALF_DIMENSION * HALF_DIMENSION)

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
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if ((cru_vertex_count (g, LANES, err) == V) ? 0 : FAIL(5193))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == E) ? 0 : FAIL(5194))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(5195))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(5196))
	 return 0;
  if (! (v = (r->r_value == V)))
	 FAIL(5197);
  free_redex (r, err);
  return v;
}






vertex
vertex_product (left_vertex, right_vertex, err)
	  vertex left_vertex;
	  vertex right_vertex;
	  int *err;

	  // Return one of two equal vertices.
{
  vertex v;

  if ((! left_vertex) ? FAIL(5198) : (left_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5199) : 0)
	 return NULL;
  if ((! right_vertex) ? FAIL(5200) : (right_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5201) : 0)
	 return NULL;
  if ((v = vertex_of ((left_vertex->v_value << HALF_DIMENSION) + right_vertex->v_value, err)))
	 v->v_magic = CROSS_VERTEX_MAGIC;
  return v;
}








edge
edge_product (left_edge, right_edge, err)
	  edge left_edge;
	  edge right_edge;
	  int *err;

	  // Return one of two equal edges.
{
  edge e;

  if ((! left_edge) ? FAIL(5202) : (left_edge->e_magic != EDGE_MAGIC) ? FAIL(5203) : 0)
	 return NULL;
  if ((! right_edge) ? FAIL(5204) : (right_edge->e_magic != EDGE_MAGIC) ? FAIL(5205) : 0)
	 return NULL;
  if ((e = edge_of ((left_edge->e_value << HALF_DIMENSION) + right_edge->e_value, err)))
	 e->e_magic = CROSS_EDGE_MAGIC;
  return e;
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
	 FAIL(5206);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(5207);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(5208) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(5209) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(5210) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(5211) : *err);
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
  cru_graph g, h;
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

  struct cru_crosser_s c = {
	 .e_prod = {
		.bop = (cru_bop) edge_product},
	 .v_prod = (cru_bop) vertex_product,
	 .cr_sig = {
		.orders = {
		  .v_order = {
			 .hash = (cru_hash) cross_vertex_hash,
			 .equal = (cru_bpred) equal_cross_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_cross_edge,
		  .v_free = (cru_destructor) free_cross_vertex}}};

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
  v = valid (h = cru_crossed (g, g, &c, UNKILLABLE, LANES, &err), &err);
  cru_free_now (g, LANES, &err);
  cru_free_now (h, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(5212) : vertex_count ? THE_FAIL(5213) : redex_count ? THE_FAIL(5214) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(5215);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(5216);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
