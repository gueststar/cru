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

#ifndef CRU_NODES_H
#define CRU_NODES_H 1

#include "ptr.h"

// default values of base and base_bay parameters to _cru_node_union
#define NO_BASE NULL
#define NO_BASE_BAY UINT32_MAX

// default values of edges_in and edges_out parameters to _cru_node_of
#define NO_EDGES_IN NULL
#define NO_EDGES_OUT NULL

// mark field values; can be arbitrary but must not clash with heap pointers
#define SEEN ((void *) 1)
#define SCATTERED ((void *) 2)
#define RETIRED ((void *) 3)
#define STRETCHED ((void *) 4)

// A single node in a list of nodes in a graph is best kept small
// because there could be millions of them, so there are lots of
// unions. There's a dedicated field for the partial result computed
// during induction operations in every node, but then this field can
// also be used for the vertex property needed during expansion
// operations, which is never needed at the same time.

struct node_list_s
{
  void *vertex;                   // content of a node in a graph
  union
  {
	 void *marked;                 // used during mutation
	 void *accumulator;            // used during induction
	 void *vertex_property;        // used during expansion
	 mapex_pair props;             // used during composition
  };
  edge_list edges_out;            // connections to successor adjacent nodes
  union
  {
	 void *class_mark;             // used during merging
	 edge_list edges_in;           // from predecessor adjacent nodes
	 edge_list edges_by;           // outgoing edges created by composition that bypass adjacent nodes
	 edge_list edges_postponed;    // brought forward from predecessor nodes by postponement
	 node_list class;              // the node to subsume this one if the graph is merged
	 node_list doppleganger;       // the node created along with this one during a split operation
  };
  node_list *previous;            // points to the next_node field of another arbitrarily situated node in the graph
  node_list next_node;            // other nodes in the graph in an unspecified order
};

// to discriminate edges in from class fields
#define CLASSIFIED(n) (n ? (! ! (n->class)) : 0)

// every node is at least in a class of its own
#define CLASS_OF(n) (CLASSIFIED(n) ? n->class : n)

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- memory management -----------------------------------------------------------------------

// allocate a new node
extern node_list
_cru_node_of (cru_destructor_pair z, void *vertex, edge_list edges_in, edge_list edges_out, int *err);

// free a node list along with its edges and vertices
extern void
_cru_free_nodes (node_list nodes, cru_destructor_pair r, int *err);

// free a node list along with its edges, vertices, and adjacent vertices
extern void
_cru_free_adjacencies (node_list *nodes, cru_destructor_pair r, int *err);

// free just the vertices in a node list
extern void
_cru_free_vertices (node_list nodes, cru_destructor d, int *err);

// free just the outgoing edges and labels in a node list
extern void
_cru_free_outgoing_edges_and_labels (node_list nodes, cru_destructor d, int *err);

// free just the incoming edges in a node list
extern void
_cru_free_incoming_edges_and_labels (node_list nodes, cru_destructor d, int *err);

// --------------- editing ---------------------------------------------------------------------------------

// insert a unit node list into the front of an existing node list
extern int
_cru_pushed_node (node_list n, node_list *nodes, int *err);

// separate a node from its neighboring nodes in the list and return it
extern node_list
_cru_severed (node_list n, int *err);

// separate a node from its neighboring nodes in the list without updating the neighbors and return it
extern node_list
_cru_half_severed (node_list n);

// consume two node lists and return their concatenation
extern node_list
_cru_cat_nodes (node_list l, node_list r);

// return non-zero of the edges into the right node can be transplanted to the left node, and if so, transplant them
extern int
_cru_transplanted_in (node_list l, node_list r, int *err);

// return non-zero of the edges from the right node can be transplanted to the left node, and if so, transplant them
extern int
_cru_transplanted_out (node_list l, node_list r, int *err);

// --------------- analysis --------------------------------------------------------------------------------

// return non-zero if n appears in l
extern int
_cru_listed (node_list n, node_list l);

// apply functions in a prop to a vertex and its adjacent edges
extern void *
_cru_mapped_node (cru_prop p, node_list n, int *err);

// as above, but allowing for concurrently changeable adjacent vertices
extern void *
_cru_mapped_node_with_locks (cru_prop p, node_list n, int *err);

// apply functions in a prop to all vertices and adjacent edges in a list and combine them
extern void *
_cru_reduced_nodes (cru_prop p, node_list n, int *err);

#ifdef __cplusplus
}
#endif
#endif
