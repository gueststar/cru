// Create a hypercubic graph with endogenous vertices and labels and
// use the cross product operation to create a deep copy of it.

#include <stdio.h>
#include <stdlib.h>
#include <cru.h>
#include "readme.h"





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

	  // Return 1 if an edge is labeled by the index of the bit in
	  // which its endpoints differ, and 0 otherwise.
{
  return ! (*err ? 1 : ((local_vertex ^ remote_vertex) != (1 << connecting_edge)) ? FAIL(5028) : 0);
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
  return ! (*err ? 1 : (edges_in != DIMENSION) ? FAIL(5029) : (edges_out != DIMENSION) ? FAIL(5030) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(5031) : (s < r) ? FAIL(5032) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(5033))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == DIMENSION * NUMBER_OF_VERTICES) ? 0 : FAIL(5034))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(5035))
	 return 0;
  return 1;
}






uintptr_t
vertex_product (left_vertex, right_vertex, err)
	  uintptr_t left_vertex;
	  uintptr_t right_vertex;
	  int *err;

	  // Return one of two equal vertices.
{
  if ((left_vertex == right_vertex) ? 0 : FAIL(5036))
	 return 0;
  return left_vertex;
}





uintptr_t
edge_product (left_edge, right_edge, err)
	  uintptr_t left_edge;
	  uintptr_t right_edge;
	  int *err;

	  // Return one of two equal edges.
{
  if ((left_edge == right_edge) ? 0 : FAIL(5037))
	 return 0;
  return left_edge;
}





int
edge_equality (left_edge, right_edge, err)
	  uintptr_t left_edge;
	  uintptr_t right_edge;
	  int *err;

	  // Return non-zero iff two given edges are equal.
{
  return (left_edge == right_edge);
}





int
main (argc, argv)
	  int argc;
	  char **argv;
{
  uintptr_t limit;
  cru_graph g, h;
  int err;
  int v;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule};

  struct cru_crosser_s c = {
	 .e_prod = {
		.bpred = (cru_bpred) edge_equality,
		.bop = (cru_bop) edge_product},
	 .v_prod = (cru_bop) vertex_product};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  g = cru_built (&b, 0, UNKILLABLE, LANES, &err);
  h = cru_crossed (g, g, &c, UNKILLABLE, LANES, &err);
  v = valid (h, &err);
  cru_free_now (g, LANES, &err);
  cru_free_now (h, LANES, &err);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
