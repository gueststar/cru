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

#ifndef CRU_ERRS_H
#define CRU_ERRS_H 1

#include <errno.h>
#include <pthread.h>
#include "ptr.h"

// the n-th non-magic value
#define MUGGLE(n) (n)

// Ordinarily the macro to raise an error reports only the first in a
// cascade of errors, but if memory testing is underway, it gives all
// other errors priority over a memory overflow so that memory
// overflow will be reported only if no other errors occur before or
// afterwards.

#ifndef WRAP
#define RAISE(e) (*err = (*err ? *err : (e)))
#else
#define RAISE(e) (*err = (*err ? (*err != ENOMEM) : 0) ? *err : (e) ? (e) : *err)
#endif

// the n-th internal error code
#define THE_IER(n) (-n)

// macro for raising an internal error
#define IER(n) RAISE(THE_IER(n))

// These macros call user functions allowing them to raise an error
// but not to reference the real error flag. Local variables ux (user
// exception), ua (user application), and ut (user test) must be
// declared where the macros are used. The macros also simulate heap
// overflow and other errors reportable by user code if memory testing
// is underway with NOMEM defined in wrap.h.

#define CALLED(v) (NOMEM ? NULL : *err ? NULL : ( ux = 0, ua = (v) (&ux), RAISE(ux), ua ))
#define PASSED(p, ...) (NOMEM ? 0 : *err ? 0 : ( ux = 0, ut = (p) (__VA_ARGS__, &ux), RAISE(ux), *err ? 0 : ut ))
#define FAILED(p, ...) (NOMEM ? 0 : *err ? 0 : ( ux = 0, ut = (p) (__VA_ARGS__, &ux), RAISE(ux), *err ? 0 : ! ut ))
#define APPLIED(f, ...) (NOMEM ? NULL : *err ? NULL : ( ux = 0, ua = (f) (__VA_ARGS__, &ux), RAISE(ux), ua ))
#define APPLY(d, ...) do { ux = 0; if (! (d)) break; (d) (__VA_ARGS__, &ux); RAISE(ux); } while (0)

#ifdef __cplusplus
extern "C"
{
#endif

// initialize pthread resources
extern int
_cru_open_errs (int *err);

// initialize the attributes for a mutex to use error checking
extern int
_cru_error_checking_mutex_type (pthread_mutexattr_t *a, int *err);

// safely assign the global error code
extern void
_cru_globally_throw (int err);

// release pthread resources and report errors to stderr
extern void
_cru_close_errs ();

#ifdef __cplusplus
}
#endif
#endif
