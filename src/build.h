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

#ifndef CRU_BUILD_H
#define CRU_BUILD_H 1

// functions used for building a graph

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// build a graph co-operatively with other workers
extern void *
_cru_building_task (port source, int *err);

// concurrently build the graph of all nodes reachable from the initial vertex v
extern cru_graph
_cru_built (void *v, cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
