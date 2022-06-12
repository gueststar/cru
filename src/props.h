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

#ifndef CRU_PROPS_H
#define CRU_PROPS_H 1

// functions for assiging and clearing vertex property fields used in
// splitting and stretching operations

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// compute and store the property associated with a single vertex
extern int
_cru_propped (node_list n, edge_list i, edge_list o, cru_prop p, int *err);

// free the vertex properties from a list of nodes and return it
extern node_list
_cru_unpropped (node_list n, cru_destructor d, int *err);

// assign the properties fields in a graph using a prepared router and leave the graph in half-duplex form
extern int
_cru_set_properties (cru_graph g, cru_kill_switch k, router r, int *err);

// clear the properties fields in a graph assuming it's still connected
extern void
_cru_unset_properties (cru_graph g, router r, int *err);

// clear the properties fields in a graph whose nodes are stored in a router's ports' survivors lists
extern void
_cru_clear_properties (router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
