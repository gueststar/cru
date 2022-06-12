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

#ifndef CRU_CREW_H
#define CRU_CREW_H 1

// routines for management of worker thread identifiers

// result parameter to _cru_maybe_disjunction when synchronization is expected but not a result
#define NO_RESULT NULL

#include "ptr.h"
#include "runners.h"

struct crew_s
{
  unsigned bays;         // the maximum number of threads that can be on this crew
  unsigned created;      // the number of threads actually on it
  crew c_held;           // non-null means this crew is among those normally held in reserve for heap overflows
  pthread_t *ids;        // the thread identifiers
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization and teardown -------------------------------------------------------------

// initialize pthread resources
extern int
_cru_open_crew (int *err);

// release pthread resources
extern void
_cru_close_crew ();

// --------------- memory management -----------------------------------------------------------------------

// create a crew with the given number of bays
extern crew
_cru_crew_of (int *err);

// return the current number of crews held in reserve
extern uintptr_t
_cru_reserved_crews (int *err);

// create more reserve crews if necessary
extern void
_cru_replenish_crews (int *err);

// --------------- recruitment -----------------------------------------------------------------------------

// start a full crew if successful or dismiss a partial crew if unsuccessful
extern int
_cru_crewed (crew c, runner m, router r, int *err);

// --------------- synchronization -------------------------------------------------------------------------

// join with the crew and assign the first non-zero return value to the error code
extern int
_cru_status_disjunction (crew c, int *err);

// join with the crew and return the sum of their return values
extern uintptr_t
_cru_summation (crew c, int *err);

// join with threads returning node queues and concatenate their results
extern node_list
_cru_node_union (crew c, unsigned base_bay, node_list *base, int *err);

// join with worker threads returning a maybe and assign at most one result
extern void
_cru_maybe_disjunction (crew c, void **result, int *err);

// join with all worker threads returning a maybe and reduce their results
extern void
_cru_maybe_reduction (crew c, cru_fold m, void **result, int *err);

#ifdef __cplusplus
}
#endif
#endif
