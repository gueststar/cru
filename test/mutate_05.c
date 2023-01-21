// Create an acyclic hypercubic graph with exogenous vertices and
// edges, bitwise invert the vertices and reverse the order of the
// edge labels and check the result. Do it in depth first order going
// backwards from the opposite of the initial vertex.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1833446073
#define FLIPPED_VERTEX_MAGIC 1462475299
#define EDGE_MAGIC 2029628876
#define FLIPPED_EDGE_MAGIC 2063057840
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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(4563))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(4564) : ++redex_count ? 0 : FAIL(4565))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(4566) : 0))
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
  if ((! r) ? FAIL(4567) : (r->r_magic != REDEX_MAGIC) ? FAIL(4568) : 0)
	 return;
  r->r_magic = MUGGLE(188);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(4569) : redex_count-- ? 0 : FAIL(4570))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(4571) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(4572))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4573) : ++edge_count ? 0 : FAIL(4574))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(4575) : 0))
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
  if ((! e) ? FAIL(4576) : (e->e_magic != EDGE_MAGIC) ? FAIL(4577) : 0)
	 return;
  e->e_magic = MUGGLE(189);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4578) : edge_count-- ? 0 : FAIL(4579))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4580) : 0)
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
  if ((! a) ? FAIL(4581) : (a->e_magic != EDGE_MAGIC) ? FAIL(4582) : 0)
	 return 0;
  if ((! b) ? FAIL(4583) : (b->e_magic != EDGE_MAGIC) ? FAIL(4584) : 0)
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

  if ((! e) ? GLOBAL_FAIL(4585) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(4586) : 0)
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
  if ((! e) ? FAIL(4587) : (e->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4588) : 0)
	 return;
  e->e_magic = MUGGLE(190);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4589) : edge_count-- ? 0 : FAIL(4590))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4591) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(4592))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4593) : ++vertex_count ? 0 : FAIL(4594))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(4595) : 0))
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
  if ((! v) ? FAIL(4596) : (v->v_magic != VERTEX_MAGIC) ? FAIL(4597) : 0)
	 return;
  v->v_magic = MUGGLE(191);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4598) : vertex_count-- ? 0 : FAIL(4599))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(4600) : 0)
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
  if ((! v) ? FAIL(4601) : (v->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4602) : 0)
	 return;
  v->v_magic = MUGGLE(192);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4603) : vertex_count-- ? 0 : FAIL(4604))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(4605) : 0)
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
  if ((! a) ? FAIL(4606) : (a->v_magic != VERTEX_MAGIC) ? FAIL(4607) : 0)
	 return 0;
  if ((! b) ? FAIL(4608) : (b->v_magic != VERTEX_MAGIC) ? FAIL(4609) : 0)
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
  if ((! a) ? FAIL(4610) : (a->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4611) : 0)
	 return 0;
  if ((! b) ? FAIL(4612) : (b->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4613) : 0)
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

  if ((! v) ? GLOBAL_FAIL(4614) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(4615) : 0)
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

  if ((! v) ? GLOBAL_FAIL(4616) : (v->v_magic != FLIPPED_VERTEX_MAGIC) ? GLOBAL_FAIL(4617) : 0)
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

  if ((! given_vertex) ? FAIL(4618) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4619) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(4620) : (connecting_edge->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4621) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(4622) : (local_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4623) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(4624) : (remote_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4625) : 0)
	 return NULL;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << (DIMENSION - connecting_edge->e_value))) ? FAIL(4626) : 0)
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

  if (*err ? 1 : (! given_vertex) ? FAIL(4627) : (given_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4628) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(4629) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(4630) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(4631) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(4632) : 0)
	 return NULL;
  v = given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if ((edges_in->r_value != (DIMENSION - bits)) ? FAIL(4633) : (edges_out->r_value != bits) ? FAIL(4634) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(4635) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(4636) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(4637) : (s < (r ? r->r_value : 0)) ? FAIL(4638) : 0)
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(4639))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((DIMENSION * NUMBER_OF_VERTICES) >> 1)) ? 0 : FAIL(4640))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(4641))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(4642))
	 return 0;
  v = (r->r_value == NUMBER_OF_VERTICES);
  free_redex (r, err);
  return v;
}







edge
eflip (local_vertex, label, remote_vertex, err)
	  vertex local_vertex;
	  edge label;
	  vertex remote_vertex;
	  int *err;

	  // Renumber an edge so as to invert the ordering. This function
	  // cast to a cru_top is passed as m.mu_kernel.e_op.map in the
	  // mutator m.
{
  edge e;

  if (*err ? 1 : (! local_vertex) ? FAIL(4643) : (local_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4644) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(4645) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4646) : 0)
	 return NULL;
  if ((! label) ? FAIL(4647) : (label->e_magic != EDGE_MAGIC) ? FAIL(4648) : 0)
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

  if (*err ? 1 : (! given_vertex) ? FAIL(4649) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4650) : 0)
	 return NULL;
  if ((edges_in != (void *) 1) ? FAIL(4651) : (edges_out != (void *) 1) ? FAIL(4652) : 0)
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
	  // right types during a backwards local first mutation.
{
  if (*err ? 1 : (! local_vertex) ? FAIL(4653) : (! edge_label) ? FAIL(4654) : (! adjacent_vertex) ? FAIL(4655) : 0)
	 return 0;
  if ((local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4656) : 0)
	 return 0;
  if ((edge_label->e_magic != EDGE_MAGIC) ? FAIL(4657) : 0)
	 return 0;
  if ((adjacent_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4658) : 0)
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
	  // right types during a backwards local first mutation.
{
  if (*err ? 1 : (! local_vertex) ? FAIL(4659) : (! edge_label) ? FAIL(4660) : (! adjacent_vertex) ? FAIL(4661) : 0)
	 return 0;
  if ((local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4662) : 0)
	 return 0;
  if ((edge_label->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4663) : 0)
	 return 0;
  if ((adjacent_vertex->v_magic != FLIPPED_VERTEX_MAGIC) ? FAIL(4664) : 0)
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
	 FAIL(4665);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(4666);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(4667) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(4668) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(4669) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(4670) : *err);
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

  struct cru_mutator_s m = {
	 .mu_plan = {
		.local_first = 1,
		.zone = {
		  .offset = 1,
		  .backwards = 1}},
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
  m.mu_plan.zone.initial_vertex = vertex_of (NUMBER_OF_VERTICES - 1, &err);
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  g = cru_mutated (g, &m, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  free_vertex (m.mu_plan.zone.initial_vertex, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(4671) : vertex_count ? THE_FAIL(4672) : redex_count ? THE_FAIL(4673) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(4674);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(4675);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
