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

#ifndef CRU_LAUNCH_H
#define CRU_LAUNCH_H 1

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// launch a traversal with a previously prepared router, block until finished, and return the status disjunction
extern int
_cru_launched (cru_kill_switch k, node_list i, router r, int *err);

// traverse with a specified hash for the initial packet and return the status disjunction
extern int
_cru_status_launched (cru_kill_switch k, node_list i, uintptr_t h, router r, int *err);

// traverse with a specified payload and hash for the initial packet and set the count
extern int
_cru_count_launched (cru_kill_switch k, node_list i, void *a, uintptr_t q, router r, uintptr_t *count, int *err);

// traverse with a specified hash for the initial packet and set the node list
extern int
_cru_queue_launched (cru_kill_switch k, node_list i, uintptr_t q, router r, node_list *u, int *err);

// traverse and return at most one result from a router whose workers return maybe types
extern int
_cru_maybe_disjunction_launched (cru_kill_switch k, node_list i, router r, void **result, int *err);

// return a combination of the results from a router whose workers return maybe types
extern int
_cru_maybe_reduction_launched (cru_kill_switch k, node_list i, router r, void **result, int *err);

// launch a traversal to build a graph from the base v
extern int
_cru_graph_launched (cru_kill_switch k, void *v, uintptr_t q, router r, cru_graph *g, int *err);

#ifdef __cplusplus
}
#endif
#endif
