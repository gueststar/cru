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

#ifndef CRU_EMU_H
#define CRU_EMU_H 1

#include "ptr.h"

// This file declares functions operating on multiset table bins,
// which are used in table.c to build tables of edge multisets, and
// some reference implementations of operations described in table.h
// used only if Judy arrays are unavailable.

struct multiset_table_bin_s
{
  union
  {
	 void *edge_label;   // just the label
	 edge_list edge;     // the label through edge->label and the remote node through edge->remote.node
  };
  uintptr_t multiplicity;
  multiset_table_bin other_edges;
};

struct node_set_nj_s
{
  node_list element;
  node_set_nj subset;
};

struct vertex_map_nj_s
{
  void *preimage;
  cru_class image;
  vertex_map_nj antirefinement;
#ifdef WRAP
  int pad;          // require data structures to have distinct sizes for memory management testing
#endif
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- edge multisets --------------------------------------------------------------------------

// include an edge in the multiset but don't bump the multiplicity
extern void
_cru_multirecord (cru_bpred e, void *l, multiset_table_bin *t, int *err);

// merge two edge multisets and accumulate the multiplicities
extern void
_cru_multimerge (multiset_table_bin *t, multiset_table_bin *q, cru_bpred e, int *err);

// point the edges in a multiset table bin to a node n
extern void
_cru_multipoint (multiset_table_bin t, node_list n, int *err);

// free a multiset
extern void
_cru_multifree (multiset_table_bin t);

// --------------- node sets -------------------------------------------------------------------------------

// return non-zero if the given node is in the set
extern int
_cru_member_nj (node_list node, node_set_nj seen);

// return non-zero if the input node is successfully stored in the set
extern int
_cru_set_membership_nj (node_list node, node_set_nj *seen, int *err);

// remove the node from the set
extern void
_cru_clear_membership_nj (node_list node, node_set_nj *seen);

// return non-zero if the given node has been previously stored in the set, and store it
extern int
_cru_test_and_set_membership_nj (node_list node, node_set_nj *seen, int *err);

// free the set of previously stored nodes
extern void
_cru_forget_members_nj (node_set_nj seen);

// --------------- vertex maps -----------------------------------------------------------------------------

// return the class associated with a vertex
extern cru_class
_cru_image_nj (vertex_map_nj m, void *v, int *err);

// free a vertex map
extern void
_cru_free_vertex_map_nj (vertex_map_nj m, int *err);

// store an association of a class with a vertex in the map
extern void
_cru_associate_nj (vertex_map_nj *m, void *v, cru_class h, int *err);

#ifdef __cplusplus
}
#endif
#endif
