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

#include <errno.h>
#include "cthread.h"
#include "errs.h"
#include "repl.h"
#include "wrap.h"

// the number of locks available for atomic read and write operations;
// increasing it reduces contention but uses more memory
#define LOCK_POOL_SIZE 0x100

// for atomic access to vertices
static pthread_rwlock_t lock_pool[LOCK_POOL_SIZE];

// created threads have these attributes
static pthread_attr_t thread_attribute;

// created mutexes have these attributes
static pthread_mutexattr_t mutex_attribute;



// --------------- initialization --------------------------------------------------------------------------








static int
thread_type (a, err)
	  pthread_attr_t *a;
	  int *err;

	  // Initialize the attributes for all created threads.
{
  return ! ((! a) ? IER(772) : pthread_attr_init (a) ? IER(773) : 0);
}









int
_cru_open_cthread (err)
	  int *err;

	  // Initialize all static storage or bust.
{
  unsigned i;

  if (! _cru_error_checking_mutex_type (&mutex_attribute, err))
	 return 0;
  for (i = 0; i < LOCK_POOL_SIZE; i++)
	 if (pthread_rwlock_init (&(lock_pool[i]), NULL) ? IER(774) : 0)
		goto a;
  if (thread_type (&thread_attribute, err))
	 return 1;
 a: pthread_mutexattr_destroy (&mutex_attribute);
  while (i--)
	 pthread_rwlock_destroy (&(lock_pool[i]));
  return 0;
}






// --------------- teardown --------------------------------------------------------------------------------








static void
release_pthread_resources (err)
	  int *err;

	  // Release pthread related resources.
{
  unsigned i;

  if (pthread_mutexattr_destroy (&mutex_attribute))
	 IER(775);
  if (pthread_attr_destroy (&thread_attribute))
	 IER(776);
  for (i = 0; i < LOCK_POOL_SIZE; i++)
	 if (pthread_rwlock_destroy (&(lock_pool[i])))
		IER(777);
}






void
_cru_close_cthread ()

	  // Do this when the process exits.
{
  int err;

  err = 0;
  release_pthread_resources (&err);
  _cru_globally_throw (err);
}






// --------------- pthread wrappers with specialized attributes and error codes ----------------------------






int
_cru_create (id, start_routine, arg, err)
	  pthread_t *id;
	  runner start_routine;
	  port arg;
	  int *err;

	  // Create a thread with attributes defined in
	  // thread_attribute. Allow simulation of memory errors during
	  // testing.
{
  int e;

  if ((! id) ? IER(778) : (! start_routine) ? IER(779) : NOMEM)
	 return *err;
  if (! (e = pthread_create (id, &thread_attribute, (void *(*) (void *)) start_routine, (void *) arg)))
	 return 0;
  RAISE((e == ENOMEM) ? e : (e == EAGAIN) ? e : (e == EPERM) ? e : THE_IER(780));
  return e;
}








int
_cru_mutex_init (m, err)
	  pthread_mutex_t *m;
	  int *err;

	  // Initialize a mutex with attributes defined in
	  // mutex_attribute. Allow simulation of memory errors during
	  // testing.
{
  int e;

  if (NOMEM)
	 return *err;
  if (! (e = pthread_mutex_init (m, &mutex_attribute)))
	 return 0;
  RAISE((e == ENOMEM) ? e : (e == EAGAIN) ? e : (e == EPERM) ? e : THE_IER(781));
  return e;
}











int
_cru_cond_init (c, err)
	  pthread_cond_t *c;
	  int *err;

	  // Initialize a condition with default attributes. Allow
	  // simulation of memory errors during testing.
{
  int e;

  if (NOMEM)
	 return *err;
  if (! (e = pthread_cond_init (c, NULL)))
	 return 0;
  RAISE((e == ENOMEM) ? e : (e == EAGAIN) ? e : THE_IER(782));
  return e;
}





// --------------- atomic operations on pointers -----------------------------------------------------------





void *
_cru_read (v, err)
	  void **v;
	  int *err;

	  // Read the value of v in a way that prevents concurrent writes
	  // to it. If another thread is attempting to write to the same
	  // location, allow the result to depend non-deterministically on
	  // whichever one goes first. On some machines, maybe most,
	  // assignment to a pointer is atomic and the locks therefore are
	  // unnecessary, but there's no portable way to detect that
	  // characteristic and in any case valgrind treats the race as an
	  // error.
{
  void *x;
  unsigned i;

  if ((! v) ? IER(783) : pthread_rwlock_rdlock (&(lock_pool[i = ((_cru_scalar_hash (v)) % LOCK_POOL_SIZE)])) ? IER(784) : 0)
	 return NULL;
  x = *v;
  return ((pthread_rwlock_unlock (&(lock_pool[i])) ? IER(785) : 0) ? NULL : x);
}





void
_cru_write (v, x, err)
	  void **v;
	  void *x;
	  int *err;

	  // Write a value to v in a way that prevents concurrent reads
	  // from it or writes to it.
{
  unsigned i;

  if ((! v) ? IER(786) : pthread_rwlock_wrlock (&(lock_pool[i = ((_cru_scalar_hash (v)) % LOCK_POOL_SIZE)])) ? IER(787) : 0)
	 return;
  *v = x;
  if (pthread_rwlock_unlock (&(lock_pool[i])))
	 IER(788);
}





void
_cru_lock_for_writing (v, err)
	  void **v;
	  int *err;

	  // Begin a critical section of write operations that will end with a write to v.
{
  if ((! v) ? IER(789) : 0)
	 return;
  if (pthread_rwlock_wrlock (&(lock_pool[((_cru_scalar_hash (v)) % LOCK_POOL_SIZE)])))
	 IER(790);
}






void
_cru_unlock_for_reading (v, x, err)
	  void **v;
	  void *x;
	  int *err;

	  // End a critical section of write operations with a write to v.
{
  if ((! v) ? IER(791) : 0)
	 return;
  *v = x;
  if (pthread_rwlock_unlock (&(lock_pool[((_cru_scalar_hash (v)) % LOCK_POOL_SIZE)])))
	 IER(792);
}









void
_cru_set (lock, f, err)
	  pthread_rwlock_t *lock;
	  int *f;
	  int *err;

	  // Safely set a flag.
{
  if ((! f) ? IER(793) : *f ? IER(794) : pthread_rwlock_wrlock (lock) ? IER(795) : 0)
	 return;
  *f = 1;
  if (pthread_rwlock_unlock (lock))
	 IER(796);
}

