/*
  cru -- co-recursion utilities

  copyright (c) 2022-2023 Dennis Furey

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

#ifndef CRU_CROSS_H
#define CRU_CROSS_H 1

// functions used for graph products

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// cross a graph co-operatively with other workers
extern void *
_cru_crossing_task (port source, int *err);

// concurrently cross the graphs
extern cru_graph
_cru_cross (cru_graph g, cru_graph h, cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
