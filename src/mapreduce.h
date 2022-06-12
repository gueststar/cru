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

#ifndef CRU_MAPREDUCE_H
#define CRU_MAPREDUCE_H 1

// operations in support of constrained reachability

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// apply the map to incoming vertices and return their reduction when quiescent
extern maybe
_cru_mapreducing_task (port source, int *err);

// launch a concurrent mapreduce operation allowing for constrained order traversal and block until finished
extern void *
_cru_mapreduce (cru_kill_switch k, node_list i, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
