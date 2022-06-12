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

#ifndef CRU_CTHREAD_H
#define CRU_CTHREAD_H 1

#include <pthread.h>
#include "ptr.h"
#include "runners.h"

// This file declares wrappers around pthread functions with
// default attributes common to all cru library functions.

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization and teardown -------------------------------------------------------------

// initialize pthread resources
extern int
_cru_open_cthread (int *err);

// release pthread resources
extern void
_cru_close_cthread ();

// --------------- pthread wrappers with specialized attributes and error codes ----------------------------

// create a thread with default attributes
extern int
_cru_create (pthread_t *id, runner start_routine, port arg, int *err);

// initialize a mutex with default attributes
extern int
_cru_mutex_init (pthread_mutex_t *m, int *err);

// initialize a condition with default attributes
extern int
_cru_cond_init (pthread_cond_t *c, int *err);

// --------------- atomic operations on pointers -----------------------------------------------------------

// read the value of v in a way that prevents concurrent writes to it
extern void *
_cru_read (void **v, int *err);

// write a value to v in a way that prevents concurrent reads from it
extern void
_cru_write (void **v, void *x, int *err);

// begin a critical section of non-atomic write operations that will end with a write to v
extern void
_cru_lock_for_writing (void **v, int *err);

// end a critical section of non-atomic write operations with a write to v
extern void
_cru_unlock_for_reading (void **v, void *x, int *err);

// safely set a flag
extern void
_cru_set (pthread_rwlock_t *lock, int *f, int *err);

#ifdef __cplusplus
}
#endif
#endif
