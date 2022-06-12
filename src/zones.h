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

#ifndef CRU_ZONES_H
#define CRU_ZONES_H 1

// routines pertaining to reachability analysis for constrained
// traversal orders

#ifdef __cplusplus
extern "C"
{
#endif

#include "ptr.h"

// find the initial node in the router's zone and perform reachability analysis if necessary
extern node_list
_cru_initial_node (cru_graph g, cru_kill_switch k, router r, int *err);

// return non-zero if a node's reachable prerequisites have been visited
extern int
_cru_visitable (node_list n, port s, int *unvisitable, int *err);

// return non-zero if all nodes adjacent to a node via outgoing edges have been visited.
extern int
_cru_retirable (node_list n, port s, int *unvisitable, int *err);

#ifdef __cplusplus
}
#endif
#endif
