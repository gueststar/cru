// Create a hypercubic graph with exogenous vertices and edges,
// traverse backwards from the maximum vertex and filter out any edge
// originating from a vertex whose bit count is half the dimension,
// which retains the major diagonal hyperplane but eliminates anything
// on the other side of it. Filter the edges the hard way by.fi_kerneling
// equivalence classes of edges even though each class contains only a
// single edge.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define EDGE_MAGIC 1846325907
#define REDEX_MAGIC 1301876190
#define MAPEX_MAGIC 1531328452
#define VERTEX_MAGIC 1707091892

// a count of allocated edges to detect memory leaks
static uintptr_t edge_count;

// needed to lock the count
static pthread_mutex_t edge_lock;

// a count of allocated vertices to detect memory leaks
static uintptr_t redex_count;

// needed to lock the count
static pthread_mutex_t redex_lock;

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

typedef struct redex_s
{
  int r_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t r_value;
} *redex;





edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(3017))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3018) : ++edge_count ? 0 : FAIL(3019))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(3020) : 0))
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
  if ((! e) ? FAIL(3021) : (e->e_magic != EDGE_MAGIC) ? FAIL(3022) : 0)
	 return;
  e->e_magic = MUGGLE(129);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3023) : edge_count-- ? 0 : FAIL(3024))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(3025) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(3026))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3027) : ++vertex_count ? 0 : FAIL(3028))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(3029) : 0))
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
  if ((! v) ? FAIL(3030) : (v->v_magic != VERTEX_MAGIC) ? FAIL(3031) : 0)
	 return;
  v->v_magic = MUGGLE(130);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3032) : vertex_count-- ? 0 : FAIL(3033))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3034) : 0)
	 return;
  free (v);
}








redex
redex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a redex.
{
  redex r;

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(3035))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3036) : ++redex_count ? 0 : FAIL(3037))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(3038) : 0))
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
  if ((! r) ? FAIL(3039) : (r->r_magic != REDEX_MAGIC) ? FAIL(3040) : 0)
	 return;
  r->r_magic = MUGGLE(131);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3041) : redex_count-- ? 0 : FAIL(3042))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3043) : 0)
	 return;
  free (r);
}









void
free_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex.
{
  if ((! r) ? FAIL(3044) : (r->r_magic != MAPEX_MAGIC) ? FAIL(3045) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
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
  if ((! a) ? FAIL(3046) : (a->e_magic != EDGE_MAGIC) ? FAIL(3047) : 0)
	 return 0;
  if ((! b) ? FAIL(3048) : (b->e_magic != EDGE_MAGIC) ? FAIL(3049) : 0)
	 return 0;
  return a->e_value == b->e_value;
}







uintptr_t
edge_hash (e)
	  edge e;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed to the cru library as f.fi_order.hash in the
	  // filter f.
{
  size_t i;
  uintptr_t u;

  if ((! e) ? GLOBAL_FAIL(3050) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(3051) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
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
  if ((! a) ? FAIL(3052) : (a->v_magic != VERTEX_MAGIC) ? FAIL(3053) : 0)
	 return 0;
  if ((! b) ? FAIL(3054) : (b->v_magic != VERTEX_MAGIC) ? FAIL(3055) : 0)
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

  if ((! v) ? GLOBAL_FAIL(3056) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(3057) : 0)
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

  if ((! given_vertex) ? FAIL(3058) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3059) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(3060) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3061) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(3062) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3063) : 0)
	 return 0;
  if ((! remote_vertex) ? FAIL(3064) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3065) : 0)
	 return 0;
  return ! (((local_vertex->v_value ^ remote_vertex->v_value) != (1 << connecting_edge->e_value)) ? FAIL(3066) : 0);
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
  uintptr_t v, bits;

  if (*err ? 1 : (! given_vertex) ? FAIL(3067) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3068) : 0)
	 return 0;
  v = given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if (bits < ((DIMENSION >> 1) - 1))
	 return ! ((edges_in != DIMENSION) ? FAIL(3069) : (edges_out != DIMENSION) ? FAIL(3070) : 0);
  if (bits == ((DIMENSION >> 1) - 1))
	 return ! ((edges_in != bits) ? FAIL(3071) : (edges_out != DIMENSION) ? FAIL(3072) : 0);
  return ! ((edges_in != bits) ? FAIL(3073) : edges_out ? FAIL(3074) : 0);
}







uintptr_t
null (err)
	  int *err;

	  // Unconditionally return zero. This function is used as as the
	  // vacuous case in an edge map reduction.
{
  return 0;
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(3075) : (s < r) ? FAIL(3076) : 0) ? 0 : s);
}






uintptr_t
choose (a, b)
	  uintptr_t a;
	  uintptr_t b;

	  // Return a!/b!(a-b)! ignoring overflow.
{
  uintptr_t c, d;

  for (d = c = 1; b; b--)
	 c = (c * a--) / d++;
  return c;
}






int
valid (g, err)
	  cru_graph g;
	  int *err;

	  // Check for correct vertex counts, edge counts, and edge labels.
{

#define REMAINING_VERTICES ((NUMBER_OF_VERTICES >> 1) + ((! (DIMENSION & 1)) * (choose (DIMENSION, DIMENSION >> 1) >> 1)))
#define REMAINING_EDGES ((REMAINING_VERTICES - choose (DIMENSION, DIMENSION >> 1)) * DIMENSION)

  struct cru_mapreducer_s m = {
	 .ma_prop = {
		.vertex = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) vertex_checker},
		.incident = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) null,
		  .map = (cru_top) edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) null,
		  .map = (cru_top) edge_checker}}};

  if ((cru_vertex_count (g, LANES, err) == REMAINING_VERTICES) ? 0 : FAIL(3077))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == REMAINING_EDGES) ? 0 : FAIL(3078))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == REMAINING_VERTICES) ? 0 : FAIL(3079))
	 return 0;
  return 1;
}








redex
edge_map (local_vertex, connecting_edge, adjacent_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex adjacent_vertex;
	  int *err;

	  // Map an edge to a redex indicating the number of 1 bits in the
	  // local vertex. This function cast to a cru_top is used in the
	  // f.fi_kernel.e_op.map field of the filter f.
{
  uintptr_t v, bits;
  redex r;

  if ((! connecting_edge) ? FAIL(3080) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3081) : 0)
	 return 0;
  if ((! local_vertex) ? FAIL(3082) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3083) : 0)
	 return 0;
  if ((! adjacent_vertex) ? FAIL(3084) : (adjacent_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3085) : 0)
	 return 0;
  v = local_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if ((r = redex_of (bits, err)))
	 r->r_magic = MAPEX_MAGIC;
  return r;
}







redex
edge_reduction (a, b, err)
	  redex a;
	  redex b;
	  int *err;

	  // Return NULL when the number of bits in either operand is half
	  // the dimension, indicating the the set of edges reduced by this
	  // operation should be filtered out. This function cast to a
	  // cru_bop is used in the f.fi_kernel.e_op.reduction field of the
	  // filter f.
{
  if ((! a) ? FAIL(3086) : (a->r_magic != MAPEX_MAGIC) ? FAIL(3087) : 0)
	 return NULL;
  if ((! b) ? FAIL(3088) : (b->r_magic != REDEX_MAGIC) ? FAIL(3089) : 0)
	 return NULL;
  if ((a->r_value == (DIMENSION >> 1)) ? 1 : (b->r_value == (DIMENSION >> 1)))
	 return NULL;
  return redex_of (0, err);
}






redex
edge_vacuous_case (err)
	  int *err;

	  // Return a non-null redex to be used as the vacuous case in the
	  // fold determined by the map and reduction operations
	  // above. This function cast to a cru_nop is ised in the
	  // f.fi_kernel.e_op.vacuous_case field in the filter f.
{
  return redex_of (0, err);
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
	 FAIL(3090);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(3091);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(3092) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(3093) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(3094) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(3095) : *err);
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
		  .v_order = {
			 .hash = (cru_hash) vertex_hash,
			 .equal = (cru_bpred) equal_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge,
		  .v_free = (cru_destructor) free_vertex}}};

  struct cru_filter_s f = {
	 .fi_zone = {
		.offset = 1,
		.backwards = 1},
	 .fi_order = {
		.hash = (cru_hash) edge_hash,
		.equal = (cru_bpred) equal_edges},
	 .fi_kernel = {
		.e_op = {
		  .map = (cru_top) edge_map,
		  .reduction = (cru_bop) edge_reduction,
		  .vacuous_case = (cru_nop) edge_vacuous_case,
		  .m_free = (cru_destructor) free_mapex,
		  .r_free = (cru_destructor) free_redex}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&redex_count, &redex_lock, &redex_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  f.fi_zone.initial_vertex = vertex_of (NUMBER_OF_VERTICES - 1, &err);
  g = cru_filtered (cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err), &f, UNKILLABLE, LANES, &err);
  free_vertex (f.fi_zone.initial_vertex, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(3096) : vertex_count ? THE_FAIL(3097) : redex_count ? THE_FAIL(3098) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3099);
  if ((! redex_lock_created) ? 0 : pthread_mutex_destroy (&(redex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3100);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(3101);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
