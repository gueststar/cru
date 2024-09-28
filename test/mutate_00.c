// Create a hypercubic graph with endogenous vertices and labels and
// complement all the vertices in place and check the result.

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
  return (*err ? 0 : ((local_vertex ^ remote_vertex) != (1 << connecting_edge)) ? (! FAIL(4199)) : 1);
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
  return ! (*err ? 1 : (edges_in != DIMENSION) ? FAIL(4200) : (edges_out != DIMENSION) ? FAIL(4201) : 0);
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

  return ((*err ? 1 : ((s = l + r) < l) ? FAIL(4202) : (s < r) ? FAIL(4203) : 0) ? 0 : s);
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

  if ((cru_vertex_count (g, LANES, err) == NUMBER_OF_VERTICES) ? 0 : FAIL(4204))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((uintptr_t) DIMENSION) * NUMBER_OF_VERTICES) ? 0 : FAIL(4205))
	 return 0;
  if ((((uintptr_t) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) == NUMBER_OF_VERTICES) ? 0 : FAIL(4206))
	 return 0;
  return 1;
}








uintptr_t
vflip (edges_in, given_vertex, edges_out, err)
	  void *edges_in;
	  uintptr_t given_vertex;
	  void *edges_out;
	  int *err;

	  // Return the bitwise complement of the given vertex.
{
  return given_vertex ^ (uintptr_t) (NUMBER_OF_VERTICES - 1);
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

  struct cru_mutator_s m = {
	 .mu_kernel = {
		.v_op = {
		  .vertex = {
			 .map = (cru_top) vflip}}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  g = cru_built (&b, 0, UNKILLABLE, LANES, &err);
  g = cru_mutated (g, &m, UNKILLABLE, LANES, &err);
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
