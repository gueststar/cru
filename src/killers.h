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

#ifndef CRU_KILLERS_H
#define CRU_KILLERS_H 1

#include <pthread.h>
#include "ptr.h"

// This file declares functions pertaining to interruption of graph
// operations at the request of user code using kill switches. The
// published API treats kill switches as an opaque type supporting
// only intialization, instigation, and reclamation. Kill switches are
// implemented mainly as pointers to a field in a router, which
// workers poll during their main event loops.

// bitmask specifying infrequency of kill switch sampling; bigger numbers mean less often
#define PERIOD 0x1f

// macro for polling the killed status of a router r using local variables in a task event loop
#define KILLED ((++sample & PERIOD) ? 0 : _cru_killed (&(r->killed), err))

// macro to simulate a kill request at the active site at the end of the first sampling period
#define KILL_SITE(n) if ((n != ACTIVE_SITE) ? 0 : (! sample) ? 0 : ! (sample & PERIOD)) _cru_kill (&(r->killed), err)

// the kill site number to be tested; zero means don't test at any site; see ../test/ktest.in
#define ACTIVE_SITE 0

// opaque type representing a kill switch

struct cru_kill_switch_s
{
  int deadly;                   // holds a magic number if the structure is valid
  int *killed;                  // address of the killed field in an associated router
  int killable;                 // requests killing when next enabled
  pthread_mutex_t safety;       // locks the killed field
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization and teardown -------------------------------------------------------------

// initialize static storage
extern int
_cru_open_killers (int *err);

// release static storage
extern void
_cru_close_killers (void);

// --------------- kill switch operations ------------------------------------------------------------------

// create a new kill switch
extern cru_kill_switch
_cru_new_kill_switch (int *err);

// safely connect a kill switch to a router
extern void
_cru_enable_killing (cru_kill_switch k, int *r, int *err);

// safely disconnect a kill switch from a router
extern void
_cru_disable_killing (cru_kill_switch k, int *err);

// safely poll the killed field in a router
extern int
_cru_killed (int *k, int *err);

// poll specifically for internal kill requests
extern int
_cru_killed_internally (int *k, int *err);

// safely set the killed field in a router
extern void
_cru_kill (int *k, int *err);

// safely set the killed field in a router specifying an internal kill request
extern void
_cru_kill_internally (int *k, int *err);

#ifdef __cplusplus
}
#endif
#endif
