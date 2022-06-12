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

#ifndef CRU_FILTERS_H
#define CRU_FILTERS_H 1

// declarations of functions used for filtering a graph

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// co-operatively initialize survivors lists in all ports associated with a router
extern void *
_cru_populating_task (port source, int *err);

// first pass to delete explicitly deletable nodes and edges
extern router
_cru_filtered (cru_graph *g, router r, cru_kill_switch k, int *err);

// second pass to delete nodes and edges made unreachable by the first pass
extern int
_cru_pruned (cru_graph g, router r, cru_kill_switch k, int *err);

#ifdef __cplusplus
}
#endif
#endif
