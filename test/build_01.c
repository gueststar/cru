// Create a hypercubic graph with endogenous vertices and exogenous
// edges and then check that it's all there.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define EDGE_MAGIC 1701449498

// a count of allocated edges to detect memory leaks
static uintptr_t edge_count;

// needed to lock the count
static pthread_mutex_t edge_lock;

typedef struct edge_s
{
  int e_magic;               // arbitrary constant used to detect double-free errors
  uintptr_t e_value;
} *edge;





edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(1800))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(1801) : ++edge_count ? 0 : FAIL(1802))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(1803) : 0))
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
  if ((! e) ? FAIL(1804) : (e->e_magic != EDGE_MAGIC) ? FAIL(1805) : 0)
	 return;
  e->e_magic = MUGGLE(81);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(1806) : edge_count-- ? 0 : FAIL(1807))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(1808) : 0)
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
  if ((! a) ? FAIL(1809) : (a->e_magic != EDGE_MAGIC) ? FAIL(1810) : 0)
	 return 0;
  if ((! b) ? FAIL(1811) : (b->e_magic != EDGE_MAGIC) ? FAIL(1812) : 0)
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

  if ((! e) ? GLOBAL_FAIL(1813) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(1814) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}









void
building_rule (given_vertex, err)
	  uintptr_t given_vertex;
	  int *err;

	  // Declare one outgoing edge from the given vertex along each
	  // dimensional axis to a vertex whose binary encoding differs
	  // from that of the given vertex in exactly one bit. Number the
	  // edges consecutively from zero. This function cast to a
	  // cru_connector is passed to the cru library as b.connector in
	  // the builder b.
{
  uintptr_t outgoing_edge;
  uintptr_t remote_vertex;

  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = (given_vertex ^ (uintptr_t) (1 << outgoing_edge));
		cru_connect (edge_of (outgoing_edge, err), (cru_vertex) remote_vertex, err);
	 }
}








uintptr_t
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  uintptr_t local_vertex;
	  edge connecting_edge;
	  uintptr_t remote_vertex;
	  int *err;

	  // Return 1 if an edge is labeled by the index of the bit in
	  // which its endpoints differ, and 0 otherwise.
{
  if (*err ? 1 : (! connecting_edge) ? FAIL(1815) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(1816) : 0)
	 return 0;
  return ! (((local_vertex ^ remote_vertex) != (1 << connecting_edge->e_value)) ? FAIL(1817) : 0);
}








uintptr_t
vertex_checker (edges_in, vertex, edges_out, err)
	  uintptr_t edges_in;
	  void *vertex;
	  uintptr_t edges_out;
	  int *err;

	  // Validate a vertex based on the incoming and outgoing
	  // edges being valid.
{
  return ! (*err ? 1 : (edges_in != DIMENSION) ? FAIL(1818) : (edges_out != DIMENSION) ? FAIL(1819) : 0);
}







uintptr_t
sum (l, r, err)
	  uintptr_t l;
	  uintptr_t r;
	  int *err;

	  // Compute the sum of two pointers cast to unsigned, checking for
	  // overflow and other errors.
{
  uintptr_t s;

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(1820) : (s < r) ? FAIL(1821) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(1822))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((uintptr_t) DIMENSION) * NUMBER_OF_VERTICES) ? 0 : FAIL(1823))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(1824))
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
	 FAIL(1825);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(1826);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(1827) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(1828) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(1829) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(1830) : *err);
}






int
main (argc, argv)
	  int argc;
	  char **argv;
{
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
			 .equal = equal_edges,
			 .hash = edge_hash}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  g = cru_built (&b, 0, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  err = (edge_count ? THE_FAIL(1831) : err);
 a: if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(1832);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
