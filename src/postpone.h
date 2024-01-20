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

#ifndef CRU_POSTPONE_H
#define CRU_POSTPONE_H 1

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// launch a postponement operation with a previously prepared router and block until finished
extern void
_cru_postpone (cru_graph *g, cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
