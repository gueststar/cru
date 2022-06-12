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

#ifndef CRU_EDGES_H
#define CRU_EDGES_H 1

#include "ptr.h"

// default values for unspecified edge fields
#define NO_SIG NULL
#define NO_NODE NULL
#define NO_LABEL NULL
#define NO_VERTEX NULL
#define NO_NEXT_EDGE NULL

// default values for a destructors argument
#define NO_DESTRUCTORS NULL

// default values for and unspecified label destructor
#define NO_LABEL_DESTRUCTOR NULL

// use the vertex in the remote node when reducing edges
#define BY_TERMINUS 0

// use the vertex in the remote node's class when reducing edges
#define BY_CLASS 1

// do or do not lock remote vertices when reducing edges; see _cru_read in cthread.c
#define WITH_LOCKS 1
#define WITHOUT_LOCKS 0

// a list of labeled edges connected to a particular node in a graph

struct edge_list_s
{
  void *label;                    // optional label on an edge in a graph
  union
  {
	 void *vertex;                 // initialized to a vertex before the node at the other end is built
	 node_list node;               // changed to a node after one with the corresponding vertex is built
  } remote;
  edge_list next_edge;            // other edges connected to this end
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- memory management -----------------------------------------------------------------------

// create a new edge with the vertex value embedded
extern edge_list
_cru_edge (cru_destructor_pair r, void *label, void *remote_vertex, node_list remote_node, edge_list next_edge, int *err);

// dispose of a list of edges
extern void
_cru_free_edges (edge_list e, int *err);

// dispose of a list of edges and their labels
extern void
_cru_free_edges_and_labels (cru_destructor label_destructor, edge_list e, int *err);

// dispose of only the labels in a list of edges
extern void
_cru_free_labels (cru_destructor label_destructor, edge_list e, int *err);

// dispose of a list of edges, their labels, and their terminal vertices
extern void
_cru_free_edges_and_termini (cru_destructor_pair r, edge_list e, int *err);

// --------------- editing ---------------------------------------------------------------------------------

// insert a unit edge list at the beginning of an existing edge list
extern void
_cru_push_edge (edge_list new_edge, edge_list *old_edges, int *err);

// append an edge list to an existing edge list
extern void
_cru_enqueue_edge (edge_list new_edge, edge_list *front, edge_list *back, int *err);

// remove and return the first edge from a list
extern edge_list
_cru_popped_edge (edge_list *edges, int *err);

// remove and return a specific edge from a list
extern edge_list
_cru_deleted_edge (edge_list target, edge_list *edges, int *err);

// consume two edge lists of arbitrary length and return their concatenation
extern edge_list
_cru_cat_edges (edge_list l, edge_list r);

// remove and return edges with repeating labels from an edge list
extern edge_list
_cru_repeating_labels (cru_order o, edge_list n, int *err);

// remove edges matching the carrier and terminus
extern edge_list
_cru_disconnections (node_list terminus, edge_list carrier, edge_list *edges, int *err);

// consume and deduplicate an edge list
extern edge_list
_cru_deduplicated_edges (edge_list e, cru_order o, cru_destructor d, int *err);

// --------------- reduction -------------------------------------------------------------------------------

// return the number of edges in an edge list
extern uintptr_t
_cru_degree (edge_list e);

// reduce a list of edge labels into a single value
extern void *
_cru_reduced_edges (cru_fold m, void *v, edge_list e, int *err);

// reduce a list of edge labels based on the remote accumulator
extern void *
_cru_reduced_edges_by_accumulator (cru_fold m, void *v, edge_list e, int *err);

// reduce a list of edge labels into a single value based either the remote vertex or its class
extern void *
_cru_configurably_reduced_edges (cru_fold m, void *v, edge_list e, int by_class, int with_locks, int *err);

#ifdef __cplusplus
}
#endif
#endif
