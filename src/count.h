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

#ifndef CRU_COUNT_H
#define CRU_COUNT_H 1

#include "ptr.h"
#include "route.h"

#ifdef __cplusplus
extern "C"
{
#endif

// count the vertices in a graph co-operatively with other workers
extern void *
_cru_node_counting_task (port s, int *err);

// count the terminal vertices in a graph co-operatively with other workers
extern void *
_cru_terminus_counting_task (port s, int *err);

// count the edges in a graph co-operatively with other workers
extern void *
_cru_edge_counting_task (port s, int *err);

// concurrently count either edges or vertices depending on the task, returning non-zero if successful
extern int
_cru_counted (uintptr_t *count, node_list i, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
