// Create a hypercubic graph with exogenous vertices and edges and
// stretch it by creating an additional vertex along every edge with
// the vertex numbered zero and its incoming and outgoing edges
// labeled zero while deleting the original edge. With deduplication,
// the result should be a star shaped graph with vertex zero connected
// once to every vertex and every other vertex connected DIMENSION
// times to vertex 0.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1277378416
#define EDGE_MAGIC 1983632472
#define REDEX_MAGIC 1364441810
#define VREDEX_MAGIC 1298140490
#define IREDEX_MAGIC 1556656963
#define OREDEX_MAGIC 1185000649
#define IMAPEX_MAGIC 1654953190

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(5745))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5746) : ++redex_count ? 0 : FAIL(5747))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(5748) : 0))
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
  if ((! r) ? FAIL(5749) : (r->r_magic != REDEX_MAGIC) ? FAIL(5750) : 0)
	 return;
  r->r_magic = MUGGLE(235);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(5751) : redex_count-- ? 0 : FAIL(5752))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(5753) : 0)
	 return;
  free (r);
}




void
free_vredex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex as above but expect a different magic number.
{
  if ((! r) ? FAIL(5754) : (r->r_magic != VREDEX_MAGIC) ? FAIL(5755) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
}




void
free_iredex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex as above but expect a different magic number.
{
  if ((! r) ? FAIL(5756) : (r->r_magic != IREDEX_MAGIC) ? FAIL(5757) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
}




void
free_imapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex as above but expect a different magic number.
{
  if ((! r) ? FAIL(5758) : (r->r_magic != IMAPEX_MAGIC) ? FAIL(5759) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
}





void
free_oredex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex as above but expect a different magic number.
{
  if ((! r) ? FAIL(5760) : (r->r_magic != OREDEX_MAGIC) ? FAIL(5761) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(5762))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5763) : ++edge_count ? 0 : FAIL(5764))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(5765) : 0))
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
  if ((! e) ? FAIL(5766) : (e->e_magic != EDGE_MAGIC) ? FAIL(5767) : 0)
	 return;
  e->e_magic = MUGGLE(236);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(5768) : edge_count-- ? 0 : FAIL(5769))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(5770) : 0)
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
  if ((! a) ? FAIL(5771) : (a->e_magic != EDGE_MAGIC) ? FAIL(5772) : 0)
	 return 0;
  if ((! b) ? FAIL(5773) : (b->e_magic != EDGE_MAGIC) ? FAIL(5774) : 0)
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

  if ((! e) ? GLOBAL_FAIL(5775) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(5776) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(5777))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5778) : ++vertex_count ? 0 : FAIL(5779))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(5780) : 0))
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
  if ((! v) ? FAIL(5781) : (v->v_magic != VERTEX_MAGIC) ? FAIL(5782) : 0)
	 return;
  v->v_magic = MUGGLE(237);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(5783) : vertex_count-- ? 0 : FAIL(5784))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(5785) : 0)
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
  if ((! a) ? FAIL(5786) : (a->v_magic != VERTEX_MAGIC) ? FAIL(5787) : 0)
	 return 0;
  if ((! b) ? FAIL(5788) : (b->v_magic != VERTEX_MAGIC) ? FAIL(5789) : 0)
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

  if ((! v) ? GLOBAL_FAIL(5790) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(5791) : 0)
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

  if ((! given_vertex) ? FAIL(5792) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5793) : 0)
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

  if (*err ? 1 : (! connecting_edge) ? FAIL(5794) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(5795) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(5796) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5797) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(5798) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5799) : 0)
	 return NULL;
  if (! (connecting_edge->e_value))
	 goto a;
  if (connecting_edge->e_value ? FAIL(5800) : 0)
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
  if (*err ? 1 : (! given_vertex) ? FAIL(5801) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5802) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(5803) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(5804) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(5805) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(5806) : 0)
	 return NULL;
  if (given_vertex->v_value ? ((edges_in->r_value != 1) ? FAIL(5807) : 0) : 0)
	 return NULL;
  if (given_vertex->v_value ? ((edges_out->r_value != DIMENSION) ? FAIL(5808) : 0) : 0)
	 return NULL;
  if (given_vertex->v_value ? 0 : (edges_in->r_value != ((NUMBER_OF_VERTICES - 1) * DIMENSION) + 1) ? FAIL(5809) : 0)
	 return NULL;
  if (given_vertex->v_value ? 0 : (edges_out->r_value != NUMBER_OF_VERTICES) ? FAIL(5810) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(5811) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(5812) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(5813) : (s < (r ? r->r_value : 0)) ? FAIL(5814) : 0)
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
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if ((cru_vertex_count (g, LANES, err) == (NUMBER_OF_VERTICES)) ? 0 : FAIL(5815))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((DIMENSION + 1) * NUMBER_OF_VERTICES - DIMENSION)) ? 0 : FAIL(5816))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(5817))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(5818))
	 return 0;
  if (!(v = (r->r_value == NUMBER_OF_VERTICES)))
	 FAIL(5819);
  free_redex (r, err);
  return v;
}







int
cubex (source_redex, edge_label, target_redex, err)
	  redex source_redex;
	  edge edge_label;
	  redex target_redex;
	  int *err;

	  // If the number of outgoing edges from a vertex is equal to the
	  // dimension, then interpose a new vertex for each edge and
	  // delete the edge.
{
  if ((! source_redex) ? FAIL(5820) : (source_redex->r_magic != VREDEX_MAGIC) ? FAIL(5821) : 0)
	 return 0;
  if ((! target_redex) ? FAIL(5822) : (target_redex->r_magic != VREDEX_MAGIC) ? FAIL(5823) : 0)
	 return 0;
  if ((! edge_label) ? FAIL(5824) : (edge_label->e_magic != EDGE_MAGIC) ? FAIL(5825) : 0)
	 return 0;
  if (((target_redex->r_value) & MASK) != DIMENSION)
	 return 0;
  cru_stretch (edge_of (0, err), vertex_of (0, err), edge_of (0, err), err);
  return 1;
}







redex
iredex_map (source_vertex, edge_label, target_vertex, err)
	  vertex source_vertex;
	  edge edge_label;
	  vertex target_vertex;
	  int *err;

	  // Create a redex derived from two vertices and an incoming edge
	  // in a graph. This function cast to a cru_top is passed as
	  // x.prop.incident.map in the stretcher x.
{
  redex r;

  if ((! source_vertex) ? FAIL(5826) : (source_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5827) : 0)
	 return NULL;
  if ((! target_vertex) ? FAIL(5828) : (target_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5829) : 0)
	 return NULL;
  if ((! edge_label) ? FAIL(5830) : (edge_label->e_magic != EDGE_MAGIC) ? FAIL(5831) : 0)
	 return NULL;
  if ((r = redex_of (edge_label->e_value, err)))
	 r->r_magic = IMAPEX_MAGIC;
  return r;
}





redex
vacuous_iredex (err)
	  int *err;

	  // Create a redex associated with an empty set of incoming
	  // vertices in a graph. This function cast as a cru_nop is passed
	  // as x.prop.incident.vacuous_case in the stretcher x.
{
  redex r;

  if ((r = redex_of (0, err)))
	 r->r_magic = IREDEX_MAGIC;
  return r;
}






redex
oredex_map (source_vertex, edge_label, target_vertex, err)
	  vertex source_vertex;
	  edge edge_label;
	  vertex target_vertex;
	  int *err;

	  // Create a redex derived from two vertices and an outgoing edge
	  // in a graph. Return 1 unconditionally. This function cast to a
	  // cru_top is passed as x.prop.outgoing.map in the stretcher x.
{
  redex r;

  if ((! source_vertex) ? FAIL(5832) : (source_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5833) : 0)
	 return NULL;
  if ((! target_vertex) ? FAIL(5834) : (target_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5835) : 0)
	 return NULL;
  if ((! edge_label) ? FAIL(5836) : (edge_label->e_magic != EDGE_MAGIC) ? FAIL(5837) : 0)
	 return NULL;
  if ((r = redex_of (1, err)))
	 r->r_magic = OREDEX_MAGIC;
  return r;
}





redex
vredex_map (redex_in, given_vertex, redex_out, err)
	  redex redex_in;
	  vertex given_vertex;
	  redex redex_out;
	  int *err;

	  // Create a redex representing the vertex property in a graph as
	  // derived from the incoming and outgoing edge redexes and the
	  // vertex. Return the vertex value and the outgoing edge redex.
	  // This function cast to a cru_top is passed as x.prop.vertex.map
	  // in the stretcher x.
{
  redex r;

  if ((! redex_in) ? FAIL(5838) : (redex_in->r_magic != IREDEX_MAGIC) ? FAIL(5839) : 0)
	 return NULL;
  if ((! redex_out) ? FAIL(5840) : (redex_out->r_magic != OREDEX_MAGIC) ? FAIL(5841) : 0)
	 return NULL;
  if ((! given_vertex) ? FAIL(5842) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(5843) : 0)
	 return NULL;
  if ((r = redex_of (((given_vertex->v_value) << HALF_POINTER_WIDTH) | (redex_out->r_value), err)))
	 r->r_magic = VREDEX_MAGIC;
  return r;
}





redex
either_iredex (a, b, err)
	  redex a;
	  redex b;
	  int *err;

	  // Perform an arbitrary binary operation on input edge
	  // redexes. This function cast as a cru_bop is passed as
	  // x.prop.incident.reduction in the stretcher x.
{
  redex r;

  if ((! a) ? FAIL(5844) : (a->r_magic != IMAPEX_MAGIC) ? FAIL(5845) : 0)
	 return NULL;
  if ((! b) ? FAIL(5846) : (b->r_magic != IREDEX_MAGIC) ? FAIL(5847) : 0)
	 return NULL;
  if ((r = redex_of (a->r_value, err)))
	 r->r_magic = IREDEX_MAGIC;
  return r;
}





redex
oredex_sum (a, b, err)
	  redex a;
	  redex b;
	  int *err;

	  // Perform addition on output edge redexes and return the
	  // sum. This function cast as a cru_bop is passed as
	  // x.prop.incident.reduction in the stretcher x.
{
  redex r;

  if ((! a) ? FAIL(5848) : (a->r_magic != OREDEX_MAGIC) ? FAIL(5849) : 0)
	 return NULL;
  if ((! b) ? FAIL(5850) : (b->r_magic != OREDEX_MAGIC) ? FAIL(5851) : 0)
	 return NULL;
  if ((r = redex_of (a->r_value + b->r_value, err)))
	 r->r_magic = OREDEX_MAGIC;
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
	 FAIL(5852);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(5853);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(5854) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(5855) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(5856) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(5857) : *err);
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

  struct cru_stretcher_s x = {
	 .st_prop = {
		.vertex = {
		  .map = (cru_top) vredex_map,
		  .m_free = (cru_destructor) free_vredex},
		.incident = {
		  .map = (cru_top) iredex_map,
		  .reduction = (cru_bop) either_iredex,
		  .vacuous_case = (cru_nop) vacuous_iredex,
		  .m_free = (cru_destructor) free_imapex,
		  .r_free = (cru_destructor) free_iredex},
		.outgoing = {
		  .map = (cru_top) oredex_map,
		  .reduction = (cru_bop) oredex_sum,
		  .m_free = (cru_destructor) free_oredex,
		  .r_free = (cru_destructor) free_oredex}},
	 .expander = (cru_tpred) cubex};
 
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
  g = cru_stretched (g, &x, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(5858) : vertex_count ? THE_FAIL(5859) : redex_count ? THE_FAIL(5860) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(5861);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(5862);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
