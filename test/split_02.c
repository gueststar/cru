// Create a hypercubic graph with exogenous vertices and labels, split
// it with a splitter that makes copies only of the odd numbered
// vertices and all edges touching them, and check that the result is
// a graph with one and a half times the number of vertices and a
// complicated number of edges.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1621983008
#define EDGE_MAGIC 1793655370
#define REDEX_MAGIC 1516565629
#define MAPEX_MAGIC 1404090392

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(5187))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5188) : ++redex_count ? 0 : FAIL(5189))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(5190) : 0))
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
  if ((! r) ? FAIL(5191) : (r->r_magic != REDEX_MAGIC) ? FAIL(5192) : 0)
	 return;
  r->r_magic = MUGGLE(217);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5193) : redex_count-- ? 0 : FAIL(5194))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(5195) : 0)
	 return;
  free (r);
}









void
free_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex.
{
  if ((! r) ? FAIL(5196) : (r->r_magic != MAPEX_MAGIC) ? FAIL(5197) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(5198))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5199) : ++edge_count ? 0 : FAIL(5200))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(5201) : 0))
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
  if ((! e) ? FAIL(5202) : (e->e_magic != EDGE_MAGIC) ? FAIL(5203) : 0)
	 return;
  e->e_magic = MUGGLE(218);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5204) : edge_count-- ? 0 : FAIL(5205))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(5206) : 0)
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

  if ((! e) ? GLOBAL_FAIL(5207) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(5208) : 0)
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
  if ((! a) ? FAIL(5209) : (a->e_magic != EDGE_MAGIC) ? FAIL(5210) : 0)
	 return 0;
  if ((! b) ? FAIL(5211) : (b->e_magic != EDGE_MAGIC) ? FAIL(5212) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(5213))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5214) : ++vertex_count ? 0 : FAIL(5215))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(5216) : 0))
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
  if ((! v) ? FAIL(5217) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5218) : 0)
	 return;
  v->v_magic = MUGGLE(219);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5219) : vertex_count-- ? 0 : FAIL(5220))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(5221) : 0)
	 return;
  free (v);
}








void
free_fused_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex resulting from a fusion operation.
{
  if ((! v) ? FAIL(5222) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5223) : 0)
	 return;
  v->v_magic = MUGGLE(220);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5224) : vertex_count-- ? 0 : FAIL(5225))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(5226) : 0)
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
  if ((! a) ? FAIL(5227) : (a->v_magic != VERTEX_MAGIC) ? FAIL(5228) : 0)
	 return 0;
  if ((! b) ? FAIL(5229) : (b->v_magic != VERTEX_MAGIC) ? FAIL(5230) : 0)
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

  if ((! v) ? GLOBAL_FAIL(5231) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(5232) : 0)
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

  if ((! given_vertex) ? FAIL(5233) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5234) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(5235) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(5236) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5237) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5238) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5239) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5240) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(5241) : 0)
	 return NULL;
  return redex_of (1, err);
}










redex
serial_edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // as above but sequential
{
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(5242) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(5243) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5244) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5245) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5246) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5247) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(5248) : 0)
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
  if (*err ? 1 : (! given_vertex) ? FAIL(5249) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5250) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(5251) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(5252) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5253) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5254) : 0)
	 return NULL;
  if ((given_vertex->v_value & 1))
	 {
		if ((edges_out->r_value != ((DIMENSION << 1) - 1)) ? FAIL(5255) : 0)
		  return NULL;
		if ((edges_in->r_value != ((DIMENSION << 1) - 1)) ? FAIL(5256) : 0)
		  return NULL;
	 }
  else
	 {
		if ((edges_out->r_value != (DIMENSION + 1)) ? FAIL(5257) : 0)
		  return NULL;
		if ((edges_in->r_value != (DIMENSION + 1)) ? FAIL(5258) : 0)
		  return NULL;
	 }
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(5259) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5260) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5261) : (s < (r ? r->r_value : 0)) ? FAIL(5262) : 0)
	 return NULL;
  return redex_of (s, err);
}







redex
serial_sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // as above but for folds with a vacuous case
{
  uintptr_t s;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != MAPEX_MAGIC) ? FAIL(5263) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5264) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5265) : (s < (r ? r->r_value : 0)) ? FAIL(5266) : 0)
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
#define V (NUMBER_OF_VERTICES + (NUMBER_OF_VERTICES >> 1))
#define E (V * (DIMENSION + 1)) + (NUMBER_OF_VERTICES * (DIMENSION - 2))

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

  if (*err ? 1 : (cru_vertex_count (g, LANES, err) == V) ? 0 : FAIL(5267))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == E) ? 0 : FAIL(5268))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(5269))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(5270))
	 return 0;
  if (!(v = (r->r_value == V)))
	 FAIL(5271);
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
  if ((! edges_in) ? FAIL(5272) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(5273) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5274) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5275) : 0)
	 return NULL;
  if ((! (edges_in->r_value)) ? 0 : (edges_in->r_value != DIMENSION) ? FAIL(5276) : 0)
	 return NULL;
  if ((edges_out->r_value != DIMENSION) ? FAIL(5277) : 0)
	 return NULL;
  if ((! v) ? FAIL(5278) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5279) : 0)
	 return NULL;
  return redex_of (v->v_value, err);
}







vertex
v_copy (v, r, err)
	  vertex v;
	  redex r;
	  int *err;

	  // This function is used as the ana and cata functions in a
	  // splitter to create a vertex numbered the same as the given
	  // redex.
{
  if ((! v) ? FAIL(5280) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5281) : 0)
	 return NULL;
  if ((! r) ? FAIL(5282) : (r->r_magic != REDEX_MAGIC) ? FAIL(5283) : 0)
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
  if ((! source) ? FAIL(5284) : (source->r_magic != REDEX_MAGIC) ? FAIL(5285) : 0)
	 return NULL;
  if ((! target) ? FAIL(5286) : (target->r_magic != REDEX_MAGIC) ? FAIL(5287) : 0)
	 return NULL;
  if ((! label) ? FAIL(5288) : (label->e_magic != EDGE_MAGIC) ? FAIL(5289) : 0)
	 return NULL;
  return edge_of (label->e_value, err);
}







int
splittable (v, r, err)
	  vertex v;
	  redex r;
	  int *err;

	  // Return non-zero if the given redex has an odd number. This
	  // function cast to a cru_bpred assigned to the fissile field of
	  // a splitter calls for only the odd propertied vertices to be
	  // split.
{
  if ((! v) ? FAIL(5290) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5291) : 0)
	 return 0;
  if ((! r) ? FAIL(5292) : (r->r_magic != REDEX_MAGIC) ? FAIL(5293) : 0)
	 return 0;
  return (r->r_value & 1);
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
	 FAIL(5294);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(5295);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(5296) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(5297) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(5298) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(5299) : *err);
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
	 .ana = (cru_bop) v_copy,
	 .cata = (cru_bop) v_copy,
	 .fissile = (cru_bpred) splittable,
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
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
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
	 err = (edge_count ? THE_FAIL(5300) : vertex_count ? THE_FAIL(5301) : redex_count ? THE_FAIL(5302) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(5303);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(5304);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
