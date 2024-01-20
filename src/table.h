/*
  cru -- co-recursion utilities

  copyright (c) 2022-2024 Dennis Furey

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

#ifndef CRU_TABLE_H
#define CRU_TABLE_H 1

// This file declares performance-critical routines for data storage
// and retrieval. They rely on Judy arrays for acceptable performance.
// If Judy arrays are unavailable, the code will still work but not
// fast enough for production.
//
// Packet tables are used by workers building a graph to store
// incoming packets carrying vertices and to detect when they receive
// the same vertex more than once from other workers.
//
// Coterminal tables are used when graphs are merged or
// deduplicated to detect when edges pointing to the same vertex
// should be merged.
//
// Multiset tables are used during graph merge operations in support
// of the edge filter predicate in the cru_merge data structure
// defined in include/cru/data_types.h and explained in
// include/cru/function_types.h. They are also used when a graph is
// built with subconnectors to check whether a vertex is reached more
// than once by an edge with the same label.
//
// Node sets are used in various contexts by workers to keep a binary
// record of whether or not any given node has been visited or seen.
//
// Vertex maps are used to build the partition data structure that is
// used to associate vertices in a graph with user defined equivalence
// classes by cru_partition_of as defined in include/cru/cru.h.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- packet tables ---------------------------------------------------------------------------

// return the list of previously received packets whose hashes clash with the given one
extern packet_list *
_cru_collision (uintptr_t index, packet_table *seen, int *err);

// consume a packet table and return the concatenation of its entries
extern packet_list
_cru_unpacked (packet_table seen);

// dispose of the previously recorded collisions
extern void
_cru_forget_collisions (packet_table seen, int *err);

// --------------- coterminal tables -----------------------------------------------------------------------

// make a brigade of edge lists such that coterminal edges share a bucket
extern brigade
_cru_marshalled (edge_list *e, int by_class, cru_destructor d, int *err);

// make a brigade of edge lists such that edges related by user-defined criteria share a bucket
extern brigade
_cru_rallied (cru_hash h, cru_bpred r, edge_list *e, int *err);

// --------------- multiset tables -------------------------------------------------------------------------

// include an edge in the table but don't bump the multiplicity if it's already present
extern void
_cru_record_edge (uintptr_t h, cru_bpred e, void *l, multiset_table *t, int *err);

// return non-zero if an edge is in the table with any multiplicity
extern int
_cru_already_recorded (uintptr_t h, cru_bpred e, void *l, multiset_table t, int *err);

// merge q into t with multiplicites accumulated, and free q
extern void
_cru_merge (multiset_table *t, multiset_table q, cru_bpred e, int *err);

// return the multiplicity of a multiset table entry
extern uintptr_t
_cru_multiplicity (uintptr_t h, cru_bpred e, void *l, multiset_table t, int *err);

// point the edges in a multiset table to a node n.
extern void
_cru_redirect (multiset_table t, node_list n, int *err);

// free a multiset table
extern void
_cru_discount (multiset_table t);

// --------------- node sets -------------------------------------------------------------------------------

// return non-zero if the given node is in the set
extern int
_cru_member (node_list node, node_set seen);

// return non-zero if the input node is successfully stored in the set
extern int
_cru_set_membership (node_list node, node_set *seen, int *err);

// remove the node from the set if it's present
extern void
_cru_clear_membership (node_list node, node_set *seen);

// return non-zero if the given node has been previously stored in the set, and store it
extern int
_cru_test_and_set_membership (node_list node, node_set *seen, int *err);

// free the set of previously stored nodes
extern void
_cru_forget_members (node_set seen);

// --------------- vertex maps -----------------------------------------------------------------------------

// return the class associated with a vertex
extern cru_class
_cru_image (vertex_map m, void *v, int *err);

// free a vertex map and unconditionally return NULL
extern void
_cru_free_vertex_map (vertex_map m, int *err);

// store an association of a class with a vertex in the map
extern void
_cru_associate (vertex_map *m, void *v, cru_class h, int *err);

#ifdef __cplusplus
}
#endif
#endif
