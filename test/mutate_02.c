// Create a hypercubic graph with exogenous vertices and edges,
// reverse the order of the edge labels and check the result.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1215179225
#define EDGE_MAGIC 1243008628
#define FLIPPED_EDGE_MAGIC 1926698486

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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(4127))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4128) : ++edge_count ? 0 : FAIL(4129))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(4130) : 0))
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
  if ((! e) ? FAIL(4131) : (e->e_magic != EDGE_MAGIC) ? FAIL(4132) : 0)
	 return;
  e->e_magic = MUGGLE(179);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4133) : edge_count-- ? 0 : FAIL(4134))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4135) : 0)
	 return;
  free (e);
}







void
free_flipped_edge (e, err)
	  edge e;
	  int *err;

	  // Reclaim an edge as above assuming it has been flipped. This
	  // function cast to a cru_destructor is passed to the cru library
	  // as m.mu_kernel.e_op.m_free in the mutator m.
{
  if ((! e) ? FAIL(4136) : (e->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4137) : 0)
	 return;
  e->e_magic = MUGGLE(180);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(4138) : edge_count-- ? 0 : FAIL(4139))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(4140) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(4141))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4142) : ++vertex_count ? 0 : FAIL(4143))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(4144) : 0))
	 return v;
 a: free (v);
  return NULL;
}








void
free_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex. This function cast to a cru_destructor is
	  // passed to the cru library as b.bu_sig.destructors.v_free in
	  // the builder b.
{
  if ((! v) ? FAIL(4145) : (v->v_magic != VERTEX_MAGIC) ? FAIL(4146) : 0)
	 return;
  v->v_magic = MUGGLE(181);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(4147) : vertex_count-- ? 0 : FAIL(4148))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(4149) : 0)
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
  if ((! a) ? FAIL(4150) : (a->v_magic != VERTEX_MAGIC) ? FAIL(4151) : 0)
	 return 0;
  if ((! b) ? FAIL(4152) : (b->v_magic != VERTEX_MAGIC) ? FAIL(4153) : 0)
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

  if ((! v) ? GLOBAL_FAIL(4154) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(4155) : 0)
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

  if ((! given_vertex) ? FAIL(4156) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4157) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(4158) : (connecting_edge->e_magic != FLIPPED_EDGE_MAGIC) ? FAIL(4159) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(4160) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4161) : 0)
	 return 0;
  if ((! remote_vertex) ? FAIL(4162) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4163) : 0)
	 return 0;
  if (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << (DIMENSION - connecting_edge->e_value))) ? FAIL(4164) : 0)
	 return 0;
  return 1;
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
  if (*err ? 1 : (! given_vertex) ? FAIL(4165) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4166) : 0)
	 return 0;
  return ! ((edges_in != DIMENSION) ? FAIL(4167) : (edges_out != DIMENSION) ? FAIL(4168) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(4169) : (s < r) ? FAIL(4170) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(4171))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == DIMENSION * NUMBER_OF_VERTICES) ? 0 : FAIL(4172))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(4173))
	 return 0;
  return 1;
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

  if (*err ? 1 : (! local_vertex) ? FAIL(4174) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4175) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(4176) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(4177) : 0)
	 return NULL;
  if ((! label) ? FAIL(4178) : (label->e_magic != EDGE_MAGIC) ? FAIL(4179) : 0)
	 return NULL;
  if ((e = edge_of (DIMENSION - label->e_value, err)))
	 e->e_magic = FLIPPED_EDGE_MAGIC;
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
	 FAIL(4180);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(4181);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(4182) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(4183) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(4184) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(4185) : *err);
}







int
main (argc, argv)
	  int argc;
	  char **argv;
{
  int vertex_lock_created;
  int edge_lock_created;
  uintptr_t limit;
  cru_graph g;
  int err;
  int v;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule,
	 .bu_sig = {
		.orders = {
		  .v_order = {
			 .hash = (cru_hash) vertex_hash,
			 .equal = (cru_bpred) equal_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge,
		  .v_free = (cru_destructor) free_vertex}}};

  struct cru_mutator_s m = {
	 .mu_kernel = {
		.e_op = {
		  .map = (cru_top) eflip,
		  .m_free = (cru_destructor) free_flipped_edge}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  g = cru_mutated (g, &m, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(4186) : vertex_count ? THE_FAIL(4187) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(4188);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(4189);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
