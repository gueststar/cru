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

#ifndef CRU_SYNC_H
#define CRU_SYNC_H 1

// routines for maintaining counts and other invariants about routers
// and worker pools

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- error handling --------------------------------------------------------------------------

// safely set the error status of a router
extern void
_cru_throw (router r, int *err);

// interrogate and clear the status code of a router
extern int
_cru_catch (router r, int *err);

// --------------- logging ---------------------------------------------------------------------------------

// make a note of there being one more worker running
extern void
_cru_swell (router r, int *err);

// make a note of there being one less worker running
extern int
_cru_dwindled (router r, int *err);

// --------------- supervision -----------------------------------------------------------------------------

// resume workers where possible by recirculating their deferred packets
extern void
_cru_undefer (router r, int *err);

// called from a supervising thread, instruct all worker threads to exit
extern void
_cru_dismiss (router r, int *err);

// block until all workers stop running either because they've exited or are waiting for packets
extern void
_cru_wait_for_quiescence (cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
