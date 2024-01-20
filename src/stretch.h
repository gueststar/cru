/*
  cru -- co-recursion utilities

  copyright (c) 2022-2024 Dennis Furey

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

#ifndef CRU_STRETCH_H
#define CRU_STRETCH_H 1

#include "ptr.h"

// routines pertaining to stretching a graph by interposing vertices
// along the edges

#ifdef __cplusplus
extern "C"
{
#endif

// return a queue of every received node
extern node_queue
_cru_pruning_task (port source, int *err);

// enqueue the rest of the nodes in a graph scattered by hashes for subsequent deletion
extern void
_cru_cull (packet_list *i, packet_list b, cru_hash h, node_list *q, packet_pod d, int by_class, int *err);

// launch an extension with a previously prepared router and block until finished
extern cru_graph
_cru_stretched (cru_graph g, cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
