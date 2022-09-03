// Create a hypercubic graph with endogenous vertices and labels and
// construct a partition that assigns the same class to vertices having
// the same number of 1 bits, but distinct classes otherwise. Then
// unite the classes of vertices with equal parity and check that
// every vertex is in the opposite class as its neighbors, which are
// all in the same class.

#include <stdio.h>
#include <stdlib.h>
#include <cru.h>
#include "readme.h"

static cru_partition h;



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
		cru_connect ((cru_edge) outgoing_edge, (cru_vertex) remote_vertex, err);
	 }
}








cru_class
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  uintptr_t local_vertex;
	  uintptr_t connecting_edge;
	  uintptr_t remote_vertex;
	  int *err;

	  // Return the class of the remote vertex if an edge is labeled
	  // by the index of the bit in which its endpoints differ, and
	  // NULL otherwise.
{
  if (((local_vertex ^ remote_vertex) == (1 << connecting_edge)) ? 1 : ! FAIL(2072))
	 return cru_class_of (h, (cru_vertex) remote_vertex, err);
  return NULL;
}








uintptr_t 
vertex_checker (edges_in, vertex, edges_out, err)
	  cru_class edges_in;
	  uintptr_t vertex;
	  cru_class edges_out;
	  int *err;

	  // Validate a vertex based on the incoming and outgoing
	  // edges being valid.
{
  return ! (*err ? 1 : (! edges_in) ? FAIL(2073) : (! edges_out) ? FAIL(2074) : (edges_in != edges_out) ? FAIL(2075) : 0);
}






uintptr_t
sum (l, r, err)
	  uintptr_t l;
	  uintptr_t r;
	  int *err;

	  // Compute the sum of two numbers, checking for overflow and
	  // other errors.
{
  uintptr_t s;

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(2076) : (s < r) ? FAIL(2077) : 0) ? 0 : s);
}







cru_class
intersection (l, r, err)
	  cru_class l;
	  cru_class r;
	  int *err;

	  // Return one of two equal classes.
{
  if (*err ? 1 : (! l) ? FAIL(2078) : (! r) ? FAIL(2079) : (l != r) ? FAIL(2080) : 0)
	 return NULL;
  return l;
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
		  .reduction = (cru_bop) intersection,
		  .map = (cru_top) edge_checker},
		.outgoing = {
		  .reduction = (cru_bop) intersection,
		  .map = (cru_top) edge_checker}}};

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(2081))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == DIMENSION * NUMBER_OF_VERTICES) ? 0 : FAIL(2082))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(2083))
	 return 0;
  return 1;
}








uintptr_t
common_property (edges_in, given_vertex, edges_out, err)
	  uintptr_t edges_in;
	  uintptr_t given_vertex;
	  uintptr_t edges_out;
	  int *err;

	  // Return the number of 1 bits of a vertex. This function cast as
	  // a cru_top is passed as c.cl_prop.vertex.map in the cru_classifier
	  // c.
{
  uintptr_t bits;

  if (*err)
	 return 0;
  if (edges_in ? FAIL(2084) : 0)
	 return 0;
  if ((edges_out != ((DIMENSION * (DIMENSION - 1)) >> 1)) ? FAIL(2085) : 0)
	 return 0;
  for (bits = 0; given_vertex; given_vertex >>= 1)
	 bits += (given_vertex & 1);
  return bits;
}








uintptr_t
outgoing_map (local_vertex, connecting_edge, remote_vertex, err)
	  uintptr_t local_vertex;
	  uintptr_t connecting_edge;
	  uintptr_t remote_vertex;
	  int *err;

	  // Return the edge label.
{
  return connecting_edge;
}







static cru_partition
united (c, err)
	  cru_partition c;
	  int *err;

	  // Unite the classes of similar parity by picking one from each
	  // class and relying on transitivity.
{
  void *x;
  void *y;
  uintptr_t i;
  cru_class xh, yh;

  for (i = 0; *err ? 0 : (i < DIMENSION - 1); i++)
	 {
		xh = cru_class_of (c, x = (void *) (((uintptr_t) 1 << i) - 1), err);
		yh = cru_class_of (c, y = (void *) (((uintptr_t) 1 << (i + 2)) - 1), err);
		if (*err != ENOMEM)
		  if (cru_united (c, xh, yh, err) ? 0 : FAIL(2086))
			 break;
		if (((xh = cru_class_of (c, x, err)) == (yh = cru_class_of (c, y, err))) ? 0 : FAIL(2087))
		  break;
	 }
  return c;
}







int
main (argc, argv)
	  int argc;
	  char **argv;
{
  uintptr_t limit;
  cru_graph g;
  int err;
  int v;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule};

  struct cru_classifier_s c = {
	 .cl_prop = {
		.outgoing = {
		  .map = (cru_top) outgoing_map,
		  .reduction = (cru_bop) sum},
		.vertex = {
		  .map = (cru_top) common_property}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  g = cru_built (&b, 0, UNKILLABLE, LANES, &err);
  h = united (cru_partition_of (g, &c, UNKILLABLE, LANES, &err), &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  cru_free_partition (h, NOW, &err);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
