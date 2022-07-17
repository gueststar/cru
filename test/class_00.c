// Create a hypercubic graph with exogenous vertices and edges and
// construct a partition that assigns the same class to vertices having
// the same number of 1 bits, but distinct classes otherwise. Cluster
// the graph doing all the math and comparisons the hard way, using
// redexes to hold the numbers. Exhaustively verify the partition.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1623007406
#define EDGE_MAGIC 1302980600
#define REDEX_MAGIC 1584807269
#define PROPERTY_MAGIC 1493118918
#define OUTGOING_REDEX_MAGIC 1334084452

// has to be declared here because it's needed by the edge and vertex checkers
static cru_partition h;

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(1908))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(1909) : ++redex_count ? 0 : FAIL(1910))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(1911) : 0))
	 return r;
 a: free (r);
  return NULL;
}









void
free_outgoing_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a result of traversing the incident
	  // edges on a vertex during a merge operation. This
	  // function cast to a cru_destructor is passed to the cru library
	  // as c.cl_prop.outgoing.r_free in the classifier c.
{
  if ((! r) ? FAIL(1912) : (r->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(1913) : 0)
	 return;
  r->r_magic = MUGGLE(87);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(1914) : redex_count-- ? 0 : FAIL(1915))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(1916) : 0)
	 return;
  free (r);
}








void
free_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex.
{
  if ((! r) ? FAIL(1917) : (r->r_magic != REDEX_MAGIC) ? FAIL(1918) : 0)
	 return;
  r->r_magic = MUGGLE(88);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(1919) : redex_count-- ? 0 : FAIL(1920))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(1921) : 0)
	 return;
  free (r);
}








void
free_property (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a property for graph merging
	  // This function cast to a cru_destructor is passed to
	  // the cru library as c.cl_prop.vertex.m_free in the
	  // classifier c.
{
  if ((! r) ? FAIL(1922) : (r->r_magic != PROPERTY_MAGIC) ? FAIL(1923) : 0)
	 return;
  r->r_magic = MUGGLE(89);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(1924) : redex_count-- ? 0 : FAIL(1925))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(1926) : 0)
	 return;
  free (r);
}
 







int
equal_properties (a, b, err)
	  redex a;
	  redex b;
	  int *err;

	  // Compare two redexes and return non-zero if they are
	  // equal.
{
  if ((! a) ? FAIL(1927) : (a->r_magic != PROPERTY_MAGIC) ? FAIL(1928) : 0)
	 return 0;
  if ((! b) ? FAIL(1929) : (b->r_magic != PROPERTY_MAGIC) ? FAIL(1930) : 0)
	 return 0;
  return a->r_value == b->r_value;
}








uintptr_t
property_hash (r)
	  redex r;

	  // Mix up low-entropy numeric values.
{
  size_t i;
  uintptr_t u;

  if ((! r) ? GLOBAL_FAIL(1931) : (r->r_magic != PROPERTY_MAGIC) ? GLOBAL_FAIL(1932) : 0)
	 return 0;
  u = r->r_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}








edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(1933))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(1934) : ++edge_count ? 0 : FAIL(1935))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(1936) : 0))
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
  if ((! e) ? FAIL(1937) : (e->e_magic != EDGE_MAGIC) ? FAIL(1938) : 0)
	 return;
  e->e_magic = MUGGLE(90);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(1939) : edge_count-- ? 0 : FAIL(1940))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(1941) : 0)
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

  if ((! e) ? GLOBAL_FAIL(1942) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(1943) : 0)
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
  if ((! a) ? FAIL(1944) : (a->e_magic != EDGE_MAGIC) ? FAIL(1945) : 0)
	 return 0;
  if ((! b) ? FAIL(1946) : (b->e_magic != EDGE_MAGIC) ? FAIL(1947) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(1948))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(1949) : ++vertex_count ? 0 : FAIL(1950))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(1951) : 0))
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
  if ((! v) ? FAIL(1952) : (v->v_magic != VERTEX_MAGIC) ? FAIL(1953) : 0)
	 return;
  v->v_magic = MUGGLE(91);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(1954) : vertex_count-- ? 0 : FAIL(1955))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(1956) : 0)
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
  if ((! a) ? FAIL(1957) : (a->v_magic != VERTEX_MAGIC) ? FAIL(1958) : 0)
	 return 0;
  if ((! b) ? FAIL(1959) : (b->v_magic != VERTEX_MAGIC) ? FAIL(1960) : 0)
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

  if ((! v) ? GLOBAL_FAIL(1961) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(1962) : 0)
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

  if ((! given_vertex) ? FAIL(1963) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(1964) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge), err);
		cru_connect (edge_of (outgoing_edge, err), remote_vertex, err);
	 }
}









cru_class
incident_edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return the class assigned to a remote vertex if its value
	  // doesn't exceed that of the local vertex.
{
  if (*err ? 1 : (! connecting_edge) ? FAIL(1965) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(1966) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(1967) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(1968) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(1969) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(1970) : 0)
	 return NULL;
  if ((((local_vertex->v_value ^ remote_vertex->v_value)) != (1 << connecting_edge->e_value)) ? FAIL(1971) : 0)
	 return NULL;
  return ((remote_vertex->v_value < local_vertex->v_value) ? cru_class_of (h, remote_vertex, err) : NULL);
}








cru_class
outgoing_edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return the class assigned to a remote vertex if its value
	  // exceeds that of the local vertex.
{
  if (*err ? 1 : (! connecting_edge) ? FAIL(1972) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(1973) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(1974) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(1975) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(1976) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(1977) : 0)
	 return NULL;
  if ((((local_vertex->v_value ^ remote_vertex->v_value)) != (1 << connecting_edge->e_value)) ? FAIL(1978) : 0)
	 return NULL;
  return ((remote_vertex->v_value > local_vertex->v_value) ? cru_class_of (h, remote_vertex, err) : NULL);
}










cru_class
common_class (l, r, err)
	  cru_class l;
	  cru_class r;
	  int *err;

	  // Return the defined class if the other is undefined or one of
	  // either of the pair if both are defined and equal.
{
  if (*err)
	 return NULL;
  if (! (l ? r : NULL))
	 return (l ? l : r);
  return (((l == r) ? 0 : FAIL(1979)) ? NULL : l);
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







redex
vertex_checker (edges_in, given_vertex, edges_out, err)
	  cru_class edges_in;
	  vertex given_vertex;
	  cru_class edges_out;
	  int *err;

	  // Confirm that a vertex's class differs from those of its
	  // adjacent vertices, that all lower numbered adjacent vertices
	  // have a common_class, that all higher numbered adjacent
	  // vertices share a common_class, and the the classes of the
	  // lower numbered adjacent vertices differ from the those of the
	  // higher numbered adjacent vertices.
{
  cru_class c;
  uintptr_t v, bits;

  if (*err ? 1 : (! given_vertex) ? FAIL(1980) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(1981) : 0)
	 return NULL;
  if ((edges_in == edges_out) ? FAIL(1982) : 0)
	 return NULL;
  if (((c = cru_class_of (h, given_vertex, err)) == edges_in) ? FAIL(1983) : (c == edges_out) ? FAIL(1984) : 0)
	 return NULL;
  v = given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if (*err != ENOMEM)
	 if ((cru_class_size (c, err) != choose (DIMENSION, bits)) ? FAIL(1985) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(1986) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(1987) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(1988) : (s < (r ? r->r_value : 0)) ? FAIL(1989) : 0)
	 return NULL;
  return redex_of (s, err);
}








redex
outgoing_sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // Compute the sum of two numbers checking for overflow and other
	  // errors.
{
  uintptr_t s;
  redex t;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(1990) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(1991) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(1992) : (s < (r ? r->r_value : 0)) ? FAIL(1993) : 0)
	 return NULL;
  if ((t = redex_of (s, err)))
	 t->r_magic = OUTGOING_REDEX_MAGIC;
  return t;
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
		  .reduction = (cru_bop) common_class,
		  .map = (cru_top) incident_edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) common_class,
		  .map = (cru_top) outgoing_edge_checker}}};

  redex r;
  int v;

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(1994))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == (DIMENSION * NUMBER_OF_VERTICES)) ? 0 : FAIL(1995))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(1996))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(1997))
	 return 0;
  if (!(v = (r->r_value == NUMBER_OF_VERTICES)))
	 FAIL(1998);
  free_redex (r, err);
  return v;
}









redex
common_property (edges_in, given_vertex, edges_out, err)
	  redex edges_in;
	  vertex given_vertex;
	  redex edges_out;
	  int *err;

	  // Return the number of 1 bits of a vertex. This function cast as
	  // a cru_top is passed as c.cl_prop.vertex.map in the cru_classifier
	  // c.
{
#define D ((DIMENSION * (DIMENSION - 1)) >> 1)

  redex r;
  uintptr_t v, bits;

  if (*err ? 1 : (! given_vertex) ? FAIL(1999) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2000) : 0)
	 return NULL;
  if (edges_in ? FAIL(2001) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(2002) : (edges_out->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(2003) : 0)
	 return NULL;
  if ((edges_out->r_value != D) ? FAIL(2004) : 0)
	 return NULL;
  v = given_vertex->v_value;
  for (bits = 0; v; v >>= 1)
	 bits += (v & 1);
  if ((r = redex_of (bits, err)))
	 r->r_magic = PROPERTY_MAGIC;
  return r;
}











redex
outgoing_map (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return an outgoing redex of the edge label.
{
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(2005) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(2006) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(2007) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2008) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(2009) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(2010) : 0)
	 return NULL;
  if ((r = redex_of (connecting_edge->e_value, err)))
	 r->r_magic = OUTGOING_REDEX_MAGIC;
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
	 FAIL(2011);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(2012);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(2013) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(2014) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(2015) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(2016) : *err);
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

  struct cru_classifier_s c = {
	 .cl_prop = {
		.outgoing = {
		  .map = (cru_top) outgoing_map,
		  .reduction = (cru_bop) outgoing_sum,
		  .m_free = (cru_destructor) free_outgoing_redex,
		  .r_free = (cru_destructor) free_outgoing_redex},
		.vertex = {
		  .map = (cru_top) common_property,
		  .m_free = (cru_destructor) free_property}},
	 .cl_order = {
		.hash = (cru_hash) property_hash,
		.equal = (cru_bpred) equal_properties}};

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
  h = cru_partition_of (g, &c, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  cru_free_partition (h, NOW, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(2017) : vertex_count ? THE_FAIL(2018) : redex_count ? THE_FAIL(2019) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(2020);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(2021);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
