// Test usage of the bmap field in a vertex mutator dependent on
// adjacent edge labels in a cyclic graph.

#include <stdio.h>
#include <stdlib.h>
#include <cru.h>
#include "readme.h"



void
building_rule (given_vertex, err)
	  uintptr_t given_vertex;
	  int *err;
{
	 cru_connect ((cru_edge) (given_vertex + 8), (cru_vertex) ((given_vertex + 1) & 0x7), err);
}




uintptr_t
bump (edges_in, given_vertex, edges_out, err)
	  void *edges_in;
	  uintptr_t given_vertex;
	  void *edges_out;
	  int *err;

	  // Return the given vertex + 16.
{
  return given_vertex + 16;
}




uintptr_t
vchk (l, m, r, err)
	  uintptr_t l;
 	  uintptr_t m;
 	  uintptr_t r;
	  int *err;
{
  return 0;
}



uintptr_t
echk (l, m, err)
	  uintptr_t l;
 	  uintptr_t m;
	  int *err;
{
  return 0;
}




uintptr_t
none (err)
	  int *err;
{
  return 0;
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
  uintptr_t u;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule};

  struct cru_mutator_s m = {
	 .mu_kernel = {
		.v_op = {
		  .outgoing = {
			 .bmap = (cru_bop) echk,
			 .vacuous_case = (cru_nop) none},
		  .vertex = {
			 .map = (cru_top) bump}}}};

  err = 0;
  v = 1;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  g = cru_built (&b, 0, UNKILLABLE, LANES, &err);
  g = cru_mutated (g, &m, UNKILLABLE, LANES, &err);
  cru_free_now (g, LANES, &err);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
