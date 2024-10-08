// Create a hypercubic graph with endogenous vertices and labels
// and transfer a copy of each even numbered edge to the terminus
// of each odd numbered sibling edge.

#include <stdio.h>
#include <stdlib.h>
#include <cru.h>
#include "readme.h"

// the number of adjacent vertices to each vertex after postponement
#define DEGREE ((uintptr_t) (((DIMENSION & 1) ? ((DIMENSION - 1) * (DIMENSION + 3)) : (DIMENSION * (DIMENSION + 2))) >> 2))





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








uintptr_t
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  uintptr_t local_vertex;
	  uintptr_t connecting_edge;
	  uintptr_t remote_vertex;
	  int *err;

	  // Return 1 iff an odd numbered edge is labeled by the index of
	  // the bit in which its endpoints differ, and vice versa for even
	  // numbered edges.
{
  if ((connecting_edge & 1) ? ((local_vertex ^ remote_vertex) == (1 << connecting_edge)) : 0)
	 return 1;
  if ((! (connecting_edge & 1)) ? ((local_vertex ^ remote_vertex) != (1 << connecting_edge)) : 0)
	 return 1;
  return 0;
}








uintptr_t 
vertex_checker (edges_in, vertex, edges_out, err)
	  uintptr_t edges_in;
	  uintptr_t vertex;
	  uintptr_t edges_out;
	  int *err;

	  // Validate a vertex based on the incoming and outgoing
	  // edges being valid.
{
  return ! (*err ? 1 : (edges_in != DEGREE) ? FAIL(4801) : (edges_out != DEGREE) ? FAIL(4802) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(4803) : (s < r) ? FAIL(4804) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(4805))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == (DEGREE * NUMBER_OF_VERTICES)) ? 0 : FAIL(4806))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(4807))
	 return 0;
  return 1;
}






int
even_label (o, l, t, err)
	  uintptr_t o;
	  uintptr_t l;
	  uintptr_t t;
	  int *err;

	  // Treat all edges with an even numbered label as postponable.
{
  return ! (l & 1);
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

  struct cru_postponer_s p = {
	 .postponable = (cru_tpred) even_label};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  g = cru_built (&b, 0, UNKILLABLE, LANES, &err);
  g = cru_postponed (g, &p, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
