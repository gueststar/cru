// Create an acyclic hypercubic graph with exogenous vertices and
// edges and use induction to compute the number of paths between
// opposite vertices. Do it the hard way using redexes for the math.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1833446073
#define EDGE_MAGIC 2029628876
#define REDEX_MAGIC 1397533929

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(3421))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3422) : ++redex_count ? 0 : FAIL(3423))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(3424) : 0))
	 return r;
 a: free (r);
  return NULL;
}







void
free_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex. This function cast to a cru_destructor is
	  // passed to the cru library as i.in_fold.r_free in the inducer
	  // i.
{
  if ((! r) ? FAIL(3425) : (r->r_magic != REDEX_MAGIC) ? FAIL(3426) : 0)
	 return;
  r->r_magic = MUGGLE(139);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3427) : redex_count-- ? 0 : FAIL(3428))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3429) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(3430))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3431) : ++edge_count ? 0 : FAIL(3432))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(3433) : 0))
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
  if ((! e) ? FAIL(3434) : (e->e_magic != EDGE_MAGIC) ? FAIL(3435) : 0)
	 return;
  e->e_magic = MUGGLE(140);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3436) : edge_count-- ? 0 : FAIL(3437))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(3438) : 0)
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
  if ((! a) ? FAIL(3439) : (a->e_magic != EDGE_MAGIC) ? FAIL(3440) : 0)
	 return 0;
  if ((! b) ? FAIL(3441) : (b->e_magic != EDGE_MAGIC) ? FAIL(3442) : 0)
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

  if ((! e) ? GLOBAL_FAIL(3443) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(3444) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(3445))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3446) : ++vertex_count ? 0 : FAIL(3447))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(3448) : 0))
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
  if ((! v) ? FAIL(3449) : (v->v_magic != VERTEX_MAGIC) ? FAIL(3450) : 0)
	 return;
  v->v_magic = MUGGLE(141);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3451) : vertex_count-- ? 0 : FAIL(3452))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3453) : 0)
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
  if ((! a) ? FAIL(3454) : (a->v_magic != VERTEX_MAGIC) ? FAIL(3455) : 0)
	 return 0;
  if ((! b) ? FAIL(3456) : (b->v_magic != VERTEX_MAGIC) ? FAIL(3457) : 0)
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

  if ((! v) ? GLOBAL_FAIL(3458) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(3459) : 0)
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
	  // from that of the given vertex in exactly one bit, but only if
	  // the remote vertex has more 1s in its encoding. Number the
	  // edges consecutively from zero. This function cast to a
	  // cru_connector is passed to the cru library as b.connector in
	  // the builder b.
{
  uintptr_t outgoing_edge, v, remote_value, local_bits, remote_bits;
  vertex r;
  edge e;

  if ((! given_vertex) ? FAIL(3460) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3461) : 0)
	 return;
  v = given_vertex->v_value;
  for (local_bits = 0; v; v >>= 1)
	 local_bits += (v & 1);
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		v = remote_value = given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge);
		for (remote_bits = 0; v; v >>= 1)
		  remote_bits += (v & 1);
		if (remote_bits <= local_bits)
		  continue;
		r = vertex_of (remote_value, err);
		cru_connect (edge_of (outgoing_edge, err), r, err);
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
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(3462) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3463) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3464) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3465) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3466) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3467) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(3468) : 0)
	 return NULL;
  r = redex_of (1, err);
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
  uintptr_t v, bits;
  redex r;

  if (*err ? 1 : (! given_vertex) ? FAIL(3469) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3470) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(3471) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(3472) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(3473) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(3474) : 0)
	 return NULL;
  v = given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if ((edges_in->r_value != bits) ? FAIL(3475) : (edges_out->r_value != (DIMENSION - bits)) ? FAIL(3476) : 0)
	 return NULL;
  r = redex_of (1, err);
  return r;
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(3477) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(3478) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3479) : (s < (r ? r->r_value : 0)) ? FAIL(3480) : 0)
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






uintptr_t
variations (n)
	  uintptr_t n;

	  // Compute the predecessor of the number of paths the easy way
	  // given the dimension n.
{
  return (n ? (n * (variations (n - 1) + 1)) : 0);
}







int
valid (g, path_count, err)
	  cru_graph g;
	  redex path_count;
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
		  .map = (cru_top) edge_checker,
		  .vacuous_case = (cru_nop) zero,
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

  v = 0;
  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(3481))
	 goto a;
  if ((cru_edge_count (g, LANES, err) == (((uintptr_t) DIMENSION) * NUMBER_OF_VERTICES) >> 1) ? 0 : FAIL(3482))
	 goto a;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(3483))
	 goto a;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(3484))
	 goto a;
  if ((r->r_value == NUMBER_OF_VERTICES) ? 0 : FAIL(3485))
	 goto b;
  if ((! path_count) ? FAIL(3486) : (path_count->r_magic == REDEX_MAGIC) ? 0 : FAIL(3487))
	 goto b;
  v = ((path_count->r_value == (1 + variations (DIMENSION))) ? 1 : ! FAIL(3488));
 b: free_redex (r, err); 
 a: free_redex (path_count, err);
  return v;
}









redex
one (err)
	  int *err;

	  // Return a redex with a value of 1, for use as the vacuous case in
	  // an induction over redexes.
{
  return redex_of (1, err);
}







static redex
path_counter (local_vertex, connecting_edge, remote_value, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  redex remote_value;
	  int *err;

	  // Compute a partial sum of the number of paths through a hypercube by
	  // returning the partial sum associated with the terminus of the edge
	  // in the course of an induction.
{
  redex r;

  if (*err ? 1 : (! local_vertex) ? FAIL(3489) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3490) : 0)
	 return NULL;
  if ((! connecting_edge) ? FAIL(3491) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3492) : 0)
	 return NULL;
  if ((! remote_value) ? FAIL(3493) : (remote_value->r_magic != REDEX_MAGIC) ? FAIL(3494) : 0)
	 return NULL;
  r = redex_of (remote_value->r_value, err);
  return r;
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
	 FAIL(3495);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(3496);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(3497) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(3498) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(3499) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(3500) : *err);
}







int
main (argc, argv)
	  int argc;
	  char **argv;
{
  int vertex_lock_created;
  int redex_lock_created;
  int edge_lock_created;
  int direction, v, err;
  redex path_count;
  uintptr_t limit;
  cru_graph g;

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

  struct cru_inducer_s i = {
	 .in_fold = {
		.reduction = (cru_bop) sum,
		.map = (cru_top) path_counter,
		.vacuous_case = (cru_nop) one,
		.m_free = (cru_destructor) free_redex,
		.r_free = (cru_destructor) free_redex}};

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
  i.in_zone.initial_vertex = vertex_of (NUMBER_OF_VERTICES - 1, &err);
  for (direction = ! (v = 1); v ? (direction < 2) : 0; direction++)   // try both directions
	 {
		i.in_zone.offset = i.in_zone.backwards = direction;
		v = valid (g, cru_induced (g, &i, UNKILLABLE, LANES, &err), &err);
	 }
  free_vertex (i.in_zone.initial_vertex, &err);
  cru_free_now (g, LANES, &err);
  pthread_mutex_lock (&edge_lock);
  pthread_mutex_lock (&vertex_lock);
  if (! err)
	 err = (edge_count ? THE_FAIL(3501) : vertex_count ? THE_FAIL(3502) : redex_count ? THE_FAIL(3503) : global_err);
  pthread_mutex_unlock (&vertex_lock);
  pthread_mutex_unlock (&edge_lock);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3504);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(3505);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
