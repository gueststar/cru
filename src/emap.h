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

#ifndef CRU_EMAP_H
#define CRU_EMAP_H 1

#include "ptr.h"

// a list of pairs of unit lists of edges used during graph expansion

struct edge_map_s
{
  edge_list ante;                 // an edge leading to an interposed vertex
  edge_list post;                 // an edge intended to be emitted from the same vertex
  edge_map next_map;
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- memory management -----------------------------------------------------------------------

// consume an edge map and create a new one
extern edge_map
_cru_edge_map (cru_destructor_pair z, edge_list a, edge_list p, edge_map *n, int *err);

// free an edge_map
extern void
_cru_free_map (cru_destructor_pair z, edge_map e, int *err);

// --------------- editing ---------------------------------------------------------------------------------

// remove the first edge map from a list
extern void
_cru_pop_map (edge_map *p, int *err);

#ifdef __cplusplus
}
#endif
#endif
