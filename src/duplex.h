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

#ifndef CRU_DUPLEX_H
#define CRU_DUPLEX_H 1

// This file declared functions to create or remove back edges, which
// can be created or removed at will because they're a redundant
// feature of the graph representation whereby any predecessor to a
// node is addressable therefrom. It also declares a function that
// concurrently combines similar edges and vertices in a graph
// of which there should always be only one copy.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// co-operatively insert back edges
extern void *
_cru_full_duplexing_task (port source, int *err);

// co-operatively delete back edges
extern void *
_cru_half_duplexing_task (port source, int *err);

// combine similar vertices and edges in a graph
extern cru_graph
_cru_deduplicated (cru_graph g, cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
