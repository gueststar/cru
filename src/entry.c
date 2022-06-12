/*
  cru -- co-recursion utilities

  copyright (c) 2022 Dennis Furey

  Cru is free software: you can redistribute it and/or modify it under
  the terms of version 3 of the GNU General Public License as
  published by the Free Software Foundation.

  Cru is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with cru. If not, see <https://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "build.h"
#include "classes.h"
#include "compose.h"
#include "copy.h"
#include "count.h"
#include "crew.h"
#include "cross.h"
#include "cthread.h"
#include "defo.h"
#include "duplex.h"
#include "edges.h"
#include "emap.h"
#include "errs.h"
#include "fab.h"
#include "filters.h"
#include "getset.h"
#include "graph.h"
#include "induce.h"
#include "infer.h"
#include "killers.h"
#include "launch.h"
#include "mapreduce.h"
#include "merge.h"
#include "mutate.h"
#include "nodes.h"
#include "pack.h"
#include "postpone.h"
#include "ptr.h"
#include "route.h"
#include "split.h"
#include "stretch.h"
#include "wrap.h"
#include "zones.h"

// This file defines entry points to the public API that require prior
// initialization of static storage across multiple sources.

// used to initialize static storage
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

// non-zero if initialization is successful
static int initialized = 0;

// an error returned by the initialization routine
static int initial_error = 0;

// done at the beginning of routines called by user code to create or alter edges in a graph

#define CONTROL_ENTRY                \
  if (err ? NULL : (err = &ignored))  \
    ignored = 0

// done at the beginning of diagnostic routines called by test programs

#define TEST_ENTRY                                                         \
  CONTROL_ENTRY;                                                            \
  pthread_once (&once_control, initialization);                              \
  if (initialized ? 0 : RAISE(initial_error ? initial_error : THE_IER(886)))  \
	 goto x;

// done when any user code calls a published API routine

#define API_ENTRY            \
  TEST_ENTRY;                 \
  _cru_replenish_crews (err);  \
  _cru_replenish_packets (err); \
  if (*err)                      \
	 goto x


// --------------- initialization and teardown -------------------------------------------------------------



static void
teardown ()

	  // Free all static storage. This function is installed by
	  // atexit () in the initialization routine.
{
  _cru_close_killers ();
  _cru_close_getset ();
  _cru_close_cthread ();
  _cru_close_packets ();
  _cru_close_crew ();
  _cru_close_wrap ();
  _cru_close_copy ();
  _cru_close_errs ();         // errs should always be closed last
}




static void
initialization ()

	  // Attempt initialization. Errs must be opened first and wrap last.
{
  if (! _cru_open_errs (&initial_error))
	 return;
  if (! _cru_open_getset (&initial_error))
	 goto a;
  if (! _cru_open_cthread (&initial_error))
	 goto b;
  if (! _cru_open_killers (&initial_error))
	 goto c;
  if (! _cru_open_crew (&initial_error))
	 goto d;
  if (! _cru_open_packets (&initial_error))
	 goto e;
  if (! _cru_open_copy (&initial_error))
	 goto f;
  if (! _cru_open_wrap (&initial_error))
	 goto g;
  if (atexit (teardown) ? (initial_error = (initial_error ? initial_error : THE_IER(887))) : 0)
	 goto h;
  initialized = 1;
  return;
 h: _cru_close_wrap ();
 g: _cru_close_copy ();
 f: _cru_close_packets ();
 e: _cru_close_crew ();
 d: _cru_close_killers ();
 c: _cru_close_cthread ();
 b: _cru_close_getset ();
 a: _cru_close_errs ();
}




// --------------- building --------------------------------------------------------------------------------





cru_kill_switch
cru_new_kill_switch (err)
	  int *err;

	  // Create a new kill switch.
{
  int ignored;
  cru_kill_switch k;

  API_ENTRY;
  k = _cru_new_kill_switch (err);
 x: return (*err ? NULL : k);
}







cru_graph
cru_built (b, v, k, lanes, err)
	  cru_builder b;
	  cru_vertex v;
	  cru_kill_switch k;
 	  unsigned lanes;
	  int *err;

	  // Concurrently build the graph of all nodes reachable from the
	  // initial vertex.
{
  int ux;
  router r;
  cru_graph g;
  int ignored;
  cru_destructor d;

  g = NULL;
  d = (b ? b->bu_sig.destructors.v_free : NULL);
  API_ENTRY;
  if (! (b ? (b = _cru_inferred_builder (b, v, err)) : NULL))
	 goto x;
  if ((r = _cru_building_router (b, (task) _cru_building_task, lanes ? lanes : NPROC ? NPROC : 1, err)))
	 {
		g = _cru_built (v, k, r, err);
		v = NULL;
	 }
  _cru_free_builder (b);
 x: if (v ? d : NULL)
	 APPLY(d, v);
  return g;
}







cru_graph
cru_crossed (g, h, c, k, lanes, err)
	  cru_graph g;
	  cru_graph h;
	  cru_crosser c;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Form a product of two graphs.
{
  cru_graph x;
  int ignored;

  x = NULL;
  API_ENTRY;
  if (! (c ? (c = _cru_inferred_crosser (c, err)) : NULL))
	 goto x;
  x = _cru_cross (g, h, k, _cru_crossing_router (c, (task) _cru_crossing_task, lanes ? lanes : NPROC ? NPROC : 1, err), err);
  _cru_free_crosser (c);
 x: return x;
}







cru_graph
cru_fabricated (g, f, k, lanes, err)
	  cru_graph g;
	  cru_fabricator f;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Form an isomorphic graph to a given graph.
{
  cru_graph h;
  int ignored;

  h = NULL;
  API_ENTRY;
  if ((! f) ? 1 : (! g) ? 1 : _cru_bad (g, err) ? 1 : ! (f = _cru_inferred_fabricator (f, &(g->g_sig), err)))
	 goto x;
  h = _cru_fabricated (g, k, _cru_fabricating_router (f, lanes ? lanes : NPROC ? NPROC : 1, err), err);
  _cru_free_fabricator (f);
 x: return h;
}






// --------------- control ---------------------------------------------------------------------------------





void
cru_connect (label, terminus, err)
	  cru_edge label;
	  cru_vertex terminus;
	  int *err;

	  // Assert an edge by pushing a newly created edge onto the list
	  // obtained from thread-specific storage for the current
	  // context. This function is called by user code when building a
	  // graph. The address of the user-supplied routine that calls
	  // this one is passed to cru in a builder struct as the connector
	  // or subconnector.
{
  edge_list *e;
  int ignored;
  context *x;

  CONTROL_ENTRY;
  if (((! initialized) ? 1 : (! (x = _cru_get_context ())) ? 1 : (*x != BUILDING)) ? RAISE(CRU_INTOOC) : 0)
	 return;
  if ((e = _cru_get_edges ()) ? 0 : IER(888))
	 return;
  *e = _cru_edge (_cru_get_destructors (), label, terminus, NO_NODE, *e, err);
}







void
cru_stretch (label_in, new_vertex, label_out, err)
	  cru_edge label_in;
	  cru_vertex new_vertex;
	  cru_edge label_out;
	  int *err;

	  // Create a new intermediate vertex with a single edge in and a
	  // single edge out, whose origin and termini are determined by
	  // the arguments to the poser that called this function.
{
  int ux;
  context *x;
  int ignored;
  edge_map t;
  edge_map *e;
  cru_destructor_pair z;
  edge_list a, p;

  CONTROL_ENTRY;
  z = _cru_get_destructors ();
  if (((! initialized) ? 1 : (! (x = _cru_get_context ())) ? 1 : (*x != STRETCHING)) ? RAISE(CRU_INTOOC) : 0)
	 goto a;
  if ((! z) ? IER(889) : (e = _cru_get_edge_maps ()) ? 0 : IER(890))
	 goto a;
  if (! (a = _cru_edge (z, label_in, new_vertex, NO_NODE, NO_NEXT_EDGE, err)))
	 goto b;
  if (! (p = _cru_edge (z, label_out, NO_VERTEX, NO_NODE, NO_NEXT_EDGE, err)))
	 _cru_free_edges_and_termini (z, a, err);
  else if ((t = _cru_edge_map (z, a, p, e, err)))
	 *e = t;
  return;
 a: if ((! z) ? NULL : label_in ? z->e_free : NULL)
	 APPLY(z->e_free, label_in);
  if ((! z) ? NULL : new_vertex ? z->v_free : NULL)
	 APPLY(z->v_free, new_vertex);
 b: if ((! z) ? NULL : label_out ? z->e_free : NULL)
	 APPLY(z->e_free, label_out);
}






// --------------- analysis --------------------------------------------------------------------------------






uintptr_t
cru_vertex_count (g, lanes, err)
	  cru_graph g;
	  unsigned lanes;
	  int *err;

	  // Concurrently compute and return the number of nodes in a graph.
{
  uintptr_t count;
  int ignored;
  node_list n;

  count = 0;
  API_ENTRY;
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  if ((! g) ? 1 : _cru_bad (g, err))
	 goto x;
  if ((lanes > 1) ? _cru_counted (&count, g->base_node, _cru_router ((task) _cru_node_counting_task, lanes, err), err) : 0)
	 goto x;
  for (n = g->nodes; n; n = n->next_node)
	 count++;
 x: return count;
}






uintptr_t
cru_edge_count (g, lanes, err)
	  cru_graph g;
	  unsigned lanes;
	  int *err;

	  // Concurrently compute and return the number of edges in a graph.
{
  uintptr_t count;
  int ignored;
  node_list n;
  edge_list e;

  count = 0;
  API_ENTRY;
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  if ((! g) ? 1 : _cru_bad (g, err))
	 goto x;
  if ((lanes > 1) ? _cru_counted (&count, g->base_node, _cru_router ((task) _cru_edge_counting_task, lanes, err), err) : 0)
	 goto x;
  for (n = g->nodes; n; n = n->next_node)
	 for (e = n->edges_out; e; e = e->next_edge)
		count++;
 x: return count;
}







void *
cru_mapreduced (g, m, k, lanes, err)
	  cru_graph g;
	  cru_mapreducer m;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Compute the result of walking over a graph when the traversal
	  // order doesn't matter.
{
  router r;
  int ignored;
  node_list initial;
  void *result;
  void *ua;
  int ux;

  result = NULL;
  API_ENTRY;
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  if (_cru_bad (g, err) ? 1 : (! m) ? 1 : ! (m = _cru_inferred_mapreducer (m, err)))
	 goto x;
  if (! g)
	 result = ((m->ma_prop.vertex.vacuous_case ? 0 : RAISE(CRU_UNDVAC)) ? NULL : CALLED(m->ma_prop.vertex.vacuous_case));
  else if ((initial = _cru_initial_node (g, k, r = _cru_mapreducing_router (m, &(g->g_sig), lanes, err), err)))
	 result = _cru_mapreduce (k, initial, _cru_reset (r, (task) _cru_mapreducing_task, err), err);
  else if (*err ? 1 : IER(891))
	 _cru_free_router (r, err);
  if (*err ? (m->ma_prop.vertex.r_free ? result : NULL) : NULL)
	 APPLY(m->ma_prop.vertex.r_free, result);
  _cru_free_mapreducer (m);
 x: return (*err ? NULL : result);
}






void *
cru_induced (g, i, k, lanes, err)
	  cru_graph g;
	  cru_inducer i;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Compute the result of walking over a graph when the traversal
	  // order matters.
{
  node_list initial;
  cru_inducer new_i;
  void *result;
  int ignored;
  void *ua;
  router r;
  int ux;

  new_i = NULL;
  result = NULL;
  API_ENTRY;
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  if (_cru_bad (g, err) ? 1 : (! i) ? 1 : ! (new_i = _cru_inferred_inducer (i, err)))
	 goto x;
  if (! g)
	 result = ((new_i->in_fold.vacuous_case ? 0 : RAISE(CRU_UNDVAC)) ? NULL : CALLED(new_i->in_fold.vacuous_case));
  else if ((initial = _cru_initial_node (g, k, r = _cru_inducing_router (new_i, &(g->g_sig), lanes, err), err)))
	 result = _cru_induce (k, initial, g->nodes, _cru_reset (r, (task) _cru_inducing_task, err), err);
  else
	 _cru_free_router (r, err);
  if (! *err)
	 goto x;
  if (new_i->in_fold.r_free ? result : NULL)
	 APPLY(new_i->in_fold.r_free, result);
 x: if (new_i)
	 _cru_free_inducer (new_i);
  return (*err ? NULL : result);
}





// --------------- classification --------------------------------------------------------------------------





cru_partition
cru_partition_of (g, c, k, lanes, err)
	  cru_graph g;
	  cru_classifier c;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Construct a map that assigns the same class to related
	  // vertices.
{
  cru_partition h;
  int ignored;

  h = NULL;
  API_ENTRY;
  if (*err ? 1  : (! g) ? 1 : ! (c = _cru_inferred_classifier (c, err)))
	 goto x;
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  h = _cru_partition_of (g, k, _cru_classifying_router (c, g->base_node, &(g->g_sig), lanes, err), err);
  _cru_free_classifier (c);
 x: return h;
}




// --------------- graph expansion -------------------------------------------------------------------------






cru_graph
cru_stretched (g, s, k, lanes, err)
	  cru_graph g;
	  cru_stretcher s;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Consume and stretch a graph by calling an stretcher for each
	  // edge. If the stretcher returns non-zero, delete the edge.
{
  router r;
  int ignored;

  API_ENTRY;
  _cru_disable_killing (k, err);
  if (_cru_bad (g, err))
	 goto x;
  if (*err ? 1 : (! g) ? 1 : (! s) ? 1 : ((s = _cru_inferred_stretcher (s, &(g->g_sig), err))) ? *err : 1)
	 goto x;
  if (((r = _cru_stretching_router (s, &(g->g_sig), lanes ? lanes : NPROC ? NPROC : 1, err))) ? (! *err) : 0)
	 g = _cru_stretched (g, k, r, err);
  _cru_free_stretcher (s);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}








cru_graph
cru_split (g, s, k, lanes, err)
	  cru_graph g;
	  cru_splitter s;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Consume and expand a graph by fissioning vertices.
{
  router r;
  int ignored;

  API_ENTRY;
  _cru_disable_killing (k, err);
  if (*err ? 1 : _cru_bad (g, err) ? 1 : (! g) ? 1 : (! s) ? 1 : ! (s = _cru_inferred_splitter (s, &(g->g_sig), err)))
	 goto x;
  if ((r = _cru_splitting_router (s, &(g->g_sig), lanes ? lanes : NPROC ? NPROC : 1, err)))
	 _cru_split (&g, k, r, err);
  _cru_free_splitter (s);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}







cru_graph
cru_composed (g, c, k, lanes, err)
	  cru_graph g;
	  cru_composer c;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Create additional edges in a graph based on compositions of consecutive edges.
{
  router r;
  int ignored;
  struct cru_sig_s new_sig;  // the given graph sig with additional fields inferred where possible

  API_ENTRY;
  _cru_disable_killing (k, err);
  if (_cru_bad (g, err) ? 1 : *err ? 1 : (! g) ? 1 : ! c)
	 goto x;
  memcpy (&new_sig, &(g->g_sig), sizeof (new_sig));
  if (! (c = _cru_inferred_composer (c, &new_sig, err)))
	 goto x;
  if ((r = _cru_composing_router (c, &new_sig, lanes ? lanes : NPROC ? NPROC : 1, err)))
	 g = _cru_composed (g, k, r, err);
  _cru_free_composer (c);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}






// --------------- graph contraction -----------------------------------------------------------------------






cru_graph
cru_merged (g, c, k, lanes, err)
	  cru_graph g;
	  cru_merger c;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Reduce a graph to one vertex for each vertex equivalence class
	  // and one edge for each edge equivalence class, possibly also
	  // filtering edges according to a predicate. If no vertex me_kernel
	  // operation is requested and the edge reclamation function is
	  // unchanged, operate just on the edges.
{
  struct cru_sig_s new_sig;
  int ignored;
  router r;

  API_ENTRY;
  _cru_disable_killing (k, err);
  if (_cru_bad (g, err) ? 1 : (! g) ? 1 : _cru_bad (g, err))
	 goto x;
  memcpy (&new_sig, &(g->g_sig), sizeof (new_sig));
  if ((! c) ? 1 : ! (c = _cru_inferred_merger (c, &new_sig, err)))
	 goto x;
  if (! (r = _cru_merging_router (c, &(g->g_sig), lanes ? lanes : NPROC ? NPROC : 1, err)))
	 goto x;
  g = _cru_merged (g, k, r, &new_sig, err);
  _cru_free_merger (c);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}







cru_graph
cru_filtered (g, f, k, lanes, err)
	  cru_graph g;
	  cru_filter f;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Remove vertices or edges from the graph according to the given
	  // filter, and also remove any vertices and edges that become
	  // consequently unreachable from the base node.
{
  int ignored;
  router r;

  API_ENTRY;
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  if (*err ? 1 : (! g) ? 1 : _cru_bad (g, err) ? 1 : (! f) ? 1 : ! (f = _cru_inferred_filter (f, &(g->g_sig), err)))
	 goto x;
  if (! (_cru_empty_prop (&(f->fi_kernel.v_op)) ? _cru_empty_fold (&(f->fi_kernel.e_op)) : 0))
	 if ((r = _cru_filtered (&g, _cru_filtering_router (f, &(g->g_sig), lanes, err), k, err)))
		_cru_pruned (g, r, k, err);
  _cru_free_filter (f);
  if (*err != CRU_INTKIL)
	 goto x;
  _cru_free_later (g, err);
  return NULL;
  x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}










cru_graph
cru_deduplicated (g, k, lanes, err)
	  cru_graph g;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Fuse identical edges and vertices.
{
  cru_merger c;
  int ignored;
  router r;

  API_ENTRY;
  _cru_disable_killing (k, err);
  lanes = (lanes ? lanes : NPROC ? NPROC : 1);
  if (_cru_bad (g, err) ? 1 : (! g) ? 1 : ! (c = _cru_deduplicator (&(g->g_sig), err)))
	 goto x;
  if (! (r = _cru_merging_router (c, &(g->g_sig), lanes, err)))
	 goto a;
  r->tag = DED;
  g = _cru_deduplicated (g, k, r, err);
 a: _cru_free_merger (c);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}







// --------------- graph surgery ---------------------------------------------------------------------------







cru_graph
cru_mutated (g, m, k, lanes, err)
	  cru_graph g;
	  cru_mutator m;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Overwrite vertices and edges in place.
{
  struct cru_sig_s new_sig;
  int ignored;
  router r;

  API_ENTRY;
  _cru_disable_killing (k, err);
  if ((! g) ? 1 : _cru_bad (g, err) ? 1 : ! m)
	 goto x;
  memcpy (&new_sig, &(g->g_sig), sizeof (new_sig));
  if (! (m = _cru_inferred_mutator (m, &new_sig, err)))
	 goto x;
  if ((r = _cru_mutating_router (m, &(g->g_sig), lanes = (lanes ? lanes: NPROC ? NPROC : 1), err)))
	 g = _cru_mutated (g, k, r, &new_sig, err);
  _cru_free_mutator (m);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}








cru_graph
cru_postponed (g, p, k, lanes, err)
	  cru_graph g;
	  cru_postponer p;
	  cru_kill_switch k;
	  unsigned lanes;
	  int *err;

	  // Selectively relocate edges to the termini of their siblings.
{
  router r;
  int ignored;
  struct cru_sig_s new_sig;

  API_ENTRY;
  _cru_disable_killing (k, err);
  if (_cru_bad (g, err) ? 1 : *err ? 1 : (! g) ? 1 : ! p)
	 goto x;
  memset (&new_sig, 0, sizeof (new_sig));
  memcpy (&new_sig, &(g->g_sig), sizeof (new_sig));
  if (! (p = _cru_inferred_postponer (p, &new_sig, err)))
	 goto x;
  if ((r = _cru_postponing_router (p, &new_sig, lanes ? lanes : NPROC ? NPROC : 1, err)))
	 _cru_postpone (&g, k, r, err);
  _cru_free_postponer (p);
 x: if (*err == CRU_INTKIL)
	 cru_free_later (g, err);
  else if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}






// --------------- reclamation -----------------------------------------------------------------------------




void
cru_free_now (g, lanes, err)
	  cru_graph g;
	  unsigned lanes;
	  int *err;

	  // Reclaim all storage associated with a graph concurrently and
	  // block until it's done.
{
  int ignored;
  router r;

  API_ENTRY;
  if (g ? (! _cru_bad (g, err)) : 0)
	 if ((lanes = (lanes ? lanes : NPROC ? NPROC : 1)) > 1)
		if ((r = _cru_razing_router (&(g->g_sig.destructors), (task) _cru_freeing_task, lanes, err)))
		  if (_cru_launched (UNKILLABLE, g->base_node, r, err))
			 g->nodes = g->base_node = NULL;
 x: _cru_free_now (g, err);
}






// --------------- testing utilities for development use only ----------------------------------------------




void
crudev_limit_allocations (limit, err)
	  uintptr_t limit;
	  int *err;

	  // Globally limit the number of memory allocations allowed to cru
	  // during the run of a test program. In a production build, this
	  // function does nothing but raise an error.
{
  int ignored;

  TEST_ENTRY;
 x: _crudev_limit_allocations (limit, err);
}






uintptr_t
crudev_allocations_performed (err)
	  int *err;

	  // Report the number of allocations performed by cru since the
	  // test program started. In a production build, this function
	  // raises an error and returns zero. Test programs use this
	  // function to evaluate the minimum allocation limit needed for a
	  // successful run.
{
  int ignored;

  TEST_ENTRY;
 x: return _crudev_allocations_performed (err);
}






int
crudev_all_clear (err)
	  int *err;

	  // Return non-zero if there are no other errors and the number of
	  // allocations performed by cru since the test program started
	  // equals the sum of the number of deallocations and the number
	  // of structures held in reserve to be freed on exit. In a
	  // production build, this function only returns ! *err.
{
  int ignored;

  TEST_ENTRY;
 x: return _crudev_all_clear (err);
}
