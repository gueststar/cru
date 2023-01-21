/*
  cru -- co-recursion utilities

  copyright (c) 2022-2023 Dennis Furey

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

#ifndef CRU_GRAPH_H
#define CRU_GRAPH_H 1

#include "ptr.h"

// arbitrary magic number for consistency checks
#define GRAPH_MAGIC 1832393831

// opaque type representing a graph

struct cru_graph_s
{
  int glad;                       // holds GRAPH_MAGIC if the graph is valid
  node_list base_node;            // the node from which all other nodes are reachable through outgoing edges
  node_list nodes;                // a list whereby all nodes in the graph are accessible in an unspecified order
  struct cru_sig_s g_sig;         // description of the graph to be updated when built, merged, or mutated
  void *g_store;                  // user defined storage associated with a graph
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization and teardown -------------------------------------------------------------

// initialize pthread resources
extern int
_cru_open_graph (int *err);

// done when the process exits
extern void
_cru_close_graph (void);

// --------------- allocation ------------------------------------------------------------------------------

// create a new graph
extern cru_graph
_cru_graph_of (cru_sig s, node_list *base, node_list n, int *err);

// --------------- reclamation -----------------------------------------------------------------------------

// free a graph sequentially
extern void
_cru_free_now (cru_graph g, int *err);

// free a graph concurrently but in a single thread
extern void
_cru_free_later (cru_graph g, int *err);

// cru worker for freeing a graph concurrently
extern void *
_cru_freeing_task (port s, int *err);

// --------------- analysis --------------------------------------------------------------------------------

// return non-zero if a graph is invalid
extern int
_cru_bad (cru_graph g, int *err);

// return non-zero if the graph has no back edges
extern int
_cru_half_duplex (cru_graph g, int *err);

// return non-zero if a partial mutation to the graph would be compatible with its current destructors
extern int
_cru_compatible (cru_graph g, cru_kernel b, int *err);

// return non-zero both have all the same fields
extern int
_cru_identical (cru_sig l, cru_sig r, int *err);

// --------------- storage ---------------------------------------------------------------------------------

// associate arbitrary user-defined data with a graph
extern void
_cru_store (cru_graph g, void *s, int *err);

// retrieve user-defined data previously associated with a graph
extern void *
_cru_retrieval (cru_graph g, int *err);

#ifdef __cplusplus
}
#endif
#endif
