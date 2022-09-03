// Create an acyclic hypercubic graph with exogenous vertices and
// edges, bitwise invert the vertices and reverse the order of the
// edge labels and check the result. Do it in depth first order, which
// should be faster than breadth first.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1479421605
#define FLIPPED_VERTEX_MAGIC 1543285964
#define EDGE_MAGIC 1917623662
#define FLIPPED_EDGE_MAGIC 1943140965
#define REDEX_MAGIC 1168861873

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(4415))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(4416) : ++redex_count ? 0 : FAIL(4417))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(4418) : 0))
	 return r;
 a: free (r);
  return NULL;
}







void
free_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex. This function cast to a cru_destructor is
	  // passed to the cru library as m.ma_prop.vertex.r_free in the
	  // mapreducer m.
{
  if ((! r) ? FAIL(4419) : (r->r_magic != REDEX_MAGIC) ? FAIL(4420) : 0)
	 return;
  r->r_magic = MUGGLE(183);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(4421) : redex_count-- ? 0 : FAIL(4422))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(4423) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(4424))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4425) : ++edge_count ? 0 : FAIL(4426))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(4427) : 0))
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
  if ((! e) ? FAIL(4428) : (e->e_magic != EDGE_MAGIC) ? FAIL(4429) : 0)
	 return;
  e->e_magic = MUGGLE(184);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4430) : edge_count-- ? 0 : FAIL(4431))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4432) : 0)
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
  if ((! a) ? FAIL(4433) : (a->e_magic != EDGE_MAGIC) ? FAIL(4434) : 0)
	 return 0;
  if ((! b) ? FAIL(4435) : (b->e_magic != EDGE_MAGIC) ? FAIL(4436) : 0)
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

  if ((! e) ? GLOBAL_FAIL(4437) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(4438) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







void
free_flipped_edge (e, err)
	  edge e;
	  int *err;

	  // Reclaim an edge as above assuming it has been flipped. This
	  // function cast to a cru_destructor is passed to the cru library
	  // as m.mu_kernel.e_op.m_free in the mutator m.
{
  if ((! e) ? FAIL(4439) : (e->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4440) : 0)
	 return;
  e->e_magic = MUGGLE(185);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4441) : edge_count-- ? 0 : FAIL(4442))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4443) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(4444))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4445) : ++vertex_count ? 0 : FAIL(4446))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(4447) : 0))
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
  if ((! v) ? FAIL(4448) : (v->v_magic != VERTEX_MAGIC) ? FAIL(4449) : 0)
	 return;
  v->v_magic = MUGGLE(186);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4450) : vertex_count-- ? 0 : FAIL(4451))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(4452) : 0)
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
  if ((! v) ? FAIL(4453) : (v->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4454) : 0)
	 return;
  v->v_magic = MUGGLE(187);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4455) : vertex_count-- ? 0 : FAIL(4456))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(4457) : 0)
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
  if ((! a) ? FAIL(4458) : (a->v_magic != VERTEX_MAGIC) ? FAIL(4459) : 0)
	 return 0;
  if ((! b) ? FAIL(4460) : (b->v_magic != VERTEX_MAGIC) ? FAIL(4461) : 0)
	 return 0;
  return a->v_value == b->v_value;
}









int
flipped_equal_vertices (a, b, err)
	  vertex a;
	  vertex b;
	  int *err;

	  // Compare two flipped vertices and return non-zero if they are
	  // equal.
{
  if ((! a) ? FAIL(4462) : (a->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4463) : 0)
	 return 0;
  if ((! b) ? FAIL(4464) : (b->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4465) : 0)
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

  if ((! v) ? GLOBAL_FAIL(4466) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(4467) : 0)
	 return 0;
  u = v->v_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







uintptr_t
flipped_vertex_hash (v)
	  vertex v;

	  // Mix up low-entropy numeric values.
{
  size_t i;
  uintptr_t u;

  if ((! v) ? GLOBAL_FAIL(4468) : (v->v_magic != FLIPPED_VERTEX_MAGIC) ? GLOBAL_FAIL(4469) : 0)
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
  uintptr_t outgoing_edge, v, local_bits, remote_bits, remote_value;

  if ((! given_vertex) ? FAIL(4470) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4471) : 0)
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
		cru_connect (edge_of (outgoing_edge, err), vertex_of (remote_value, err), err);
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(4472) : (connecting_edge->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4473) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(4474) : (local_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4475) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(4476) : (remote_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4477) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << (DIMENSION - connecting_edge->e_value))) ? FAIL(4478) : 0)
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
  uintptr_t v, bits;

  if (*err ? 1 : (! given_vertex) ? FAIL(4479) : (given_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4480) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(4481) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(4482) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(4483) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(4484) : 0)
	 return NULL;
  v = given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if ((edges_in->r_value != (DIMENSION - bits)) ? FAIL(4485) : (edges_out->r_value != bits) ? FAIL(4486) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(4487) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(4488) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(4489) : (s < (r ? r->r_value : 0)) ? FAIL(4490) : 0)
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(4491))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((DIMENSION * NUMBER_OF_VERTICES) >> 1)) ? 0 : FAIL(4492))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(4493))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(4494))
	 return 0;
  v = (r->r_value == NUMBER_OF_VERTICES);
  free_redex (r, err);
  return v;
}







edge
eflip (local_vertex, label, adjacent_vertex, err)
	  vertex local_vertex;
	  edge label;
	  vertex adjacent_vertex;
	  int *err;

	  // Renumber an edge so as to invert the ordering. This function
	  // cast to a cru_top is passed as m.mu_kernel.e_op.map in the
	  // mutator m.
{
  edge e;

  if (*err ? 1 : (! local_vertex) ? FAIL(4495) : (local_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4496) : 0)
	 return NULL;
  if ((! adjacent_vertex) ? FAIL(4497) : (adjacent_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4498) : 0)
	 return NULL;
  if ((! label) ? FAIL(4499) : (label->e_magic != EDGE_MAGIC) ? FAIL(4500) : 0)
	 return NULL;
  if ((e = edge_of (DIMENSION - label->e_value, err)))
	 e->e_magic = FLIPPED_EDGE_MAGIC;
  return e;
}







vertex
vflip (edges_in, given_vertex, edges_out, err)
	  void *edges_in;
	  vertex given_vertex;
	  void *edges_out;
	  int *err;

	  // Return the bitwise complement of the given vertex.
{
  vertex v;

  if (*err ? 1 : (! given_vertex) ? FAIL(4501) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4502) : 0)
	 return NULL;
  if ((edges_in != (void *) 1) ? FAIL(4503) : (edges_out != (void *) 1) ? FAIL(4504) : 0)
	 return NULL;
  if ((v = vertex_of (given_vertex->v_value ^ (uintptr_t) (NUMBER_OF_VERTICES - 1), err)))
	 v->v_magic = FLIPPED_VERTEX_MAGIC;
  return v;
}







uintptr_t
conjunction (l, r, err)
	  uintptr_t l;
	  uintptr_t r;
	  int *err;

	  // edge reduction binary operator
{
  return (*err ? 0 : l ? r : 0);
}






uintptr_t
truism (err)
	  int *err;

	  // edge reduction vacuous case
{
  return ! *err;
}




uintptr_t
mutation_check_in (local_vertex, edge_label, adjacent_vertex, err)
	  vertex local_vertex;
	  edge edge_label;
	  vertex adjacent_vertex;
	  int *err;

	  // Check that the incoming edge origins and termini have the
	  // right types during a forward remote first mutation.
{
  if (*err ? 1 : (! local_vertex) ? FAIL(4505) : (! edge_label) ? FAIL(4506) : (! adjacent_vertex) ? FAIL(4507) : 0)
	 return 0;
  if ((local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4508) : 0)
	 return 0;
  if ((edge_label->e_magic != EDGE_MAGIC) ? FAIL(4509) : 0)
	 return 0;
  if ((adjacent_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4510) : 0)
	 return 0;
  return 1;
}






uintptr_t
mutation_check_out (local_vertex, edge_label, adjacent_vertex, err)
	  vertex local_vertex;
	  edge edge_label;
	  vertex adjacent_vertex;
	  int *err;

	  // Check that the outgoing edge origins and termini have the
	  // right types during a forward remote first mutation.
{
  if (*err ? 1 : (! local_vertex) ? FAIL(4511) : (! edge_label) ? FAIL(4512) : (! adjacent_vertex) ? FAIL(4513) : 0)
	 return 0;
  if ((local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4514) : 0)
	 return 0;
  if ((edge_label->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4515) : 0)
	 return 0;
  if ((adjacent_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4516) : 0)
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
	 FAIL(4517);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(4518);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(4519) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(4520) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(4521) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(4522) : *err);
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

  struct cru_mutator_s m = {
	 .mu_plan = {
		.remote_first = 1},
	 .mu_kernel = {
		.v_op = {
		  .vertex = {
			 .map = (cru_top) vflip,
			 .m_free = (cru_destructor) free_flipped_vertex},
		  .incident = {
			 .reduction = (cru_bop) conjunction,
			 .vacuous_case = (cru_nop) truism,
			 .map = (cru_top) mutation_check_in},
		  .outgoing = {
			 .reduction = (cru_bop) conjunction,
			 .vacuous_case = (cru_nop) truism,
			 .map = (cru_top) mutation_check_out}},
		.e_op = {
		  .map = (cru_top) eflip,
		  .m_free = (cru_destructor) free_flipped_edge}},
	 .mu_orders = {
		.v_order = {
		  .hash = (cru_hash) flipped_vertex_hash,
		  .equal = (cru_bpred) flipped_equal_vertices}}};

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
  g = cru_mutated (g, &m, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(4523) : vertex_count ? THE_FAIL(4524) : redex_count ? THE_FAIL(4525) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(4526);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(4527);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
