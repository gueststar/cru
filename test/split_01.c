// Create a hypercubic graph with exogenous vertices and labels, split
// it with a splitter that makes copies of every vertex and edge, and
// check that the result is a hypercube of double the original size.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1437713919
#define EDGE_MAGIC 1662565575
#define REDEX_MAGIC 1955686701
#define MAPEX_MAGIC 1435615277

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(5272))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5273) : ++redex_count ? 0 : FAIL(5274))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(5275) : 0))
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
  if ((! r) ? FAIL(5276) : (r->r_magic != REDEX_MAGIC) ? FAIL(5277) : 0)
	 return;
  r->r_magic = MUGGLE(213);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5278) : redex_count-- ? 0 : FAIL(5279))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(5280) : 0)
	 return;
  free (r);
}








void
free_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex.
{
  if ((! r) ? FAIL(5281) : (r->r_magic != MAPEX_MAGIC) ? FAIL(5282) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(5283))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5284) : ++edge_count ? 0 : FAIL(5285))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(5286) : 0))
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
  if ((! e) ? FAIL(5287) : (e->e_magic != EDGE_MAGIC) ? FAIL(5288) : 0)
	 return;
  e->e_magic = MUGGLE(214);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5289) : edge_count-- ? 0 : FAIL(5290))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(5291) : 0)
	 return;
  free (e);
}







uintptr_t
edge_hash (e)
	  edge e;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed as b.bu_sig.orders.e_order.hash in the builder b,
	  // but is needed only for merging and not building.
{
  size_t i;
  uintptr_t u;

  if ((! e) ? GLOBAL_FAIL(5292) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(5293) : 0)
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
	  // but is needed only for merging.
{
  if ((! a) ? FAIL(5294) : (a->e_magic != EDGE_MAGIC) ? FAIL(5295) : 0)
	 return 0;
  if ((! b) ? FAIL(5296) : (b->e_magic != EDGE_MAGIC) ? FAIL(5297) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(5298))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5299) : ++vertex_count ? 0 : FAIL(5300))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(5301) : 0))
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
  if ((! v) ? FAIL(5302) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5303) : 0)
	 return;
  v->v_magic = MUGGLE(215);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5304) : vertex_count-- ? 0 : FAIL(5305))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(5306) : 0)
	 return;
  free (v);
}








void
free_fused_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex resulting from a fusion operation.
{
  if ((! v) ? FAIL(5307) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5308) : 0)
	 return;
  v->v_magic = MUGGLE(216);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5309) : vertex_count-- ? 0 : FAIL(5310))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(5311) : 0)
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
  if ((! a) ? FAIL(5312) : (a->v_magic != VERTEX_MAGIC) ? FAIL(5313) : 0)
	 return 0;
  if ((! b) ? FAIL(5314) : (b->v_magic != VERTEX_MAGIC) ? FAIL(5315) : 0)
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

  if ((! v) ? GLOBAL_FAIL(5316) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(5317) : 0)
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

  if ((! given_vertex) ? FAIL(5318) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5319) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(5320) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(5321) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5322) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5323) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5324) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5325) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(5326) : 0)
	 return NULL;
  return redex_of (1, err);
}










redex
serial_edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Simulate a different result type.
{
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(5327) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(5328) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5329) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5330) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5331) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5332) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(5333) : 0)
	 return NULL;
  if ((r = redex_of (1, err)))
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
  if (*err ? 1 : (! given_vertex) ? FAIL(5334) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5335) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(5336) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(5337) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5338) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5339) : 0)
	 return NULL;
  if ((edges_in->r_value != (uintptr_t) (DIMENSION << 1)) ? FAIL(5340) : 0)
	 return NULL;
  if ((edges_out->r_value != (uintptr_t) (DIMENSION << 1)) ? FAIL(5341) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(5342) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5343) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5344) : (s < (r ? r->r_value : 0)) ? FAIL(5345) : 0)
	 return NULL;
  return redex_of (s, err);
}









redex
serial_sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // as above but sequential
{
  uintptr_t s;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != MAPEX_MAGIC) ? FAIL(5346) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5347) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5348) : (s < (r ? r->r_value : 0)) ? FAIL(5349) : 0)
	 return NULL;
  return redex_of (s, err);
}







redex
zero (err)
	  int *err;

	  // Return a redex with a value of zero, for use as the vacuous case in
	  // a mapreduction over redexes.
{
  return redex_of (0, err);
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
		  .vacuous_case = (cru_nop) zero,
		  .map = (cru_top) vertex_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.incident = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) zero,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .vacuous_case = (cru_nop) zero,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if (*err ? 1 : (cru_vertex_count (g, LANES, err) == (NUMBER_OF_VERTICES << 1)) ? 0 : FAIL(5350))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == (((uintptr_t) (DIMENSION << 1)) * (NUMBER_OF_VERTICES << 1))) ? 0 : FAIL(5351))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(5352))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(5353))
	 return 0;
  if (!(v = (r->r_value == (NUMBER_OF_VERTICES << 1))))
	 FAIL(5354);
  free_redex (r, err);
  return v;
}






redex
r_copy (edges_in, v, edges_out, err)
	  redex edges_in;
	  vertex v;
	  redex edges_out;
	  int *err;

	  // This function used as a vertex map in the property function of a
	  // splitter s creates a vertex property consisting of a single redex
	  // numbered the same as the given vertex.
{
  if ((! edges_in) ? FAIL(5355) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(5356) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5357) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5358) : 0)
	 return NULL;
  if ((! (edges_in->r_value)) ? 0 : (edges_in->r_value != DIMENSION) ? FAIL(5359) : 0)
	 return NULL;
  if ((edges_out->r_value != DIMENSION) ? FAIL(5360) : 0)
	 return NULL;
  if ((! v) ? FAIL(5361) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5362) : 0)
	 return NULL;
  return redex_of (v->v_value, err);
}






vertex
u_copy (v, r, err)
	  vertex v;
	  redex r;
	  int *err;

	  // This function is used as the ana function in a splitter to
	  // create a vertex numbered the same as the given redex.
{
  if ((! v) ? FAIL(5363) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5364) : 0)
	 return NULL;
  if ((! r) ? FAIL(5365) : (r->r_magic != REDEX_MAGIC) ? FAIL(5366) : 0)
	 return NULL;
  return vertex_of (r->r_value, err);
}








vertex
v_copy (v, r, err)
	  vertex v;
	  redex r;
	  int *err;

	  // This function is used as the cata function in a splitter to
	  // create a vertex numbered the same as the given redex.
{
  if ((! v) ? FAIL(5367) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5368) : 0)
	 return NULL;
  if ((! r) ? FAIL(5369) : (r->r_magic != REDEX_MAGIC) ? FAIL(5370) : 0)
	 return NULL;
  return vertex_of (r->r_value, err);
}










edge
e_copy (source, label, target, err)
	  redex source;
	  edge label;
	  redex target;
	  int *err;

	  // This function creates a copy of the given edge and ignores the
	  // source and target redexes. It's used as the mark function on
	  // both ctops of both ctopses in a splitter.
{
  if ((! source) ? FAIL(5371) : (source->r_magic != REDEX_MAGIC) ? FAIL(5372) : 0)
	 return NULL;
  if ((! target) ? FAIL(5373) : (target->r_magic != REDEX_MAGIC) ? FAIL(5374) : 0)
	 return NULL;
  if ((! label) ? FAIL(5375) : (label->e_magic != EDGE_MAGIC) ? FAIL(5376) : 0)
	 return NULL;
  return edge_of (label->e_value, err);
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
	 FAIL(5377);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(5378);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(5379) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(5380) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(5381) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(5382) : *err);
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

  struct cru_splitter_s s = {
	 .ana = (cru_bop) u_copy,
	 .cata = (cru_bop) v_copy,
	 .sp_ctops = {
		.inward = {
		  .ana_labeler = {
			 .top = (cru_top) e_copy},
		  .cata_labeler = {
			 .top = (cru_top) e_copy}},
		.outward = {
		  .ana_labeler = {
			 .top = (cru_top) e_copy},
		  .cata_labeler = {
			 .top = (cru_top) e_copy}}},
	 .sp_prop = {
		.incident = {
		  .vacuous_case = (cru_nop) zero,
		  .reduction = (cru_bop) serial_sum,
		  .map = (cru_top) serial_edge_checker,
		  .m_free = (cru_destructor) free_mapex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .map = (cru_top) edge_checker,
		  .reduction = (cru_bop) sum,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.vertex = {
		  .map = (cru_top) r_copy,
		  .m_free = (cru_destructor) free_redex}}};

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
  g = cru_split (g, &s, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(5383) : vertex_count ? THE_FAIL(5384) : redex_count ? THE_FAIL(5385) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(5386);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(5387);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
