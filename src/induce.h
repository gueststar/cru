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

#ifndef CRU_INDUCE_H
#define CRU_INDUCE_H 1

// This file declares functions for traversing a graph depth first to
// compute a cumulative result by backward induction with partial
// results stored temporarily in the graph nodes.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// launch an induction with a previously prepared router and block until finished
extern void *
_cru_induce (cru_kill_switch k, node_list i, node_list b, router r, int *err);

// co-operatively visit each reachable vertex in a graph and compute the associated value
extern void *
_cru_inducing_task (port s, int *err);

#ifdef __cplusplus
}
#endif
#endif
