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

#ifndef CRU_GETSET_H
#define CRU_GETSET_H 1

// This file declares setters and getters for thread specific storage.
// Setters return zero when successful.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// library functions can set or clear a context and raise an error if
// user code calls a control function before or afterwards

typedef enum {IDLE, BUILDING, STRETCHING, GENERAL} context;

extern int
_cru_open_getset (int *err);

extern int
_cru_set_context (context *c, int *err);

extern context *
_cru_get_context (void);

extern int
_cru_set_edges (edge_list *edge, int *err);

extern edge_list *
_cru_get_edges (void);

extern int
_cru_set_edge_maps (edge_map *pair, int *err);

extern edge_map *
_cru_get_edge_maps (void);

extern int
_cru_set_destructors (cru_destructor_pair destructors, int *err);

extern cru_destructor_pair
_cru_get_destructors (void);

extern int
_cru_set_kill_switch (int *killed, int *err);

extern int *
_cru_get_kill_switch (void);

extern int
_cru_set_storage (void *t, int *err);

extern void *
_cru_get_storage (void);

extern void
_cru_close_getset (void);

#ifdef __cplusplus
}
#endif
#endif
