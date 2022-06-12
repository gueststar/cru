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

#ifndef CRU_RUNNERS_H
#define CRU_RUNNERS_H 1

#include <pthread.h>
#include "ptr.h"

// This file declares runner functions to run a worker task in a
// thread and to exit the thread when the worker returns. Different
// runners depend on the result types and other variations. All of
// them are meant to be cast to a runner and passed to _cru_crewed
// along with a router whose task has been initialized to the worker
// task as declared in crew.h and route.h. There is also a
// declaration of a joiner, which is meant to be cast as an nthm
// worker for reduction over maybes during the gathering phase of a
// job whose tasks return maybe types.

#ifdef __cplusplus
extern "C"
{
#endif

// the type of function passed to _cru_create
typedef void *(*runner) (port);

// --------------- runners ---------------------------------------------------------------------------------

// run a worker in a newly created thread and return its error status on exit
extern void *
_cru_status_runner (port s);

// run a worker returning a maybe and exit the thread
extern void *
_cru_maybe_runner (port s);

// run a worker returning a queue and return it by exiting the thread
extern void *
_cru_queue_runner (port s);

// run a worker returning a count and return it by exiting the thread
extern void *
_cru_count_runner (port s);

// --------------- joiners ---------------------------------------------------------------------------------

// join with a thread returning a maybe
extern maybe
_cru_maybe_joiner (pthread_t *id, int *err);

#ifdef __cplusplus
}
#endif
#endif
