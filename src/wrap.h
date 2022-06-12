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

#ifndef CRU_WRAP_H
#define CRU_WRAP_H 1

// This file declares a malloc wrapper to simulate constrained memory
// resources during testing and a free wrapper for accounting
// purposes. The malloc wrapper calls the actual memory allocator up
// to a limited number of times starting from initialization but then
// always returns NULL thereafter. The free wrapper keeps a count of
// the number of deallocations performed so that double free errors
// and memory leaks can be detected. Allocations in external library
// functions and user code that don't use these wrappers are
// approximated by a call to _cru_allocations_allowed near their call
// sites. In production builds, WRAP is undefined and the wrappers
// incur no cost. Refer to the programs under the test directory and
// the mtest script for usage examples.

#ifdef WRAP
#define _cru_malloc(s) _cru_malloc_wrapper (__LINE__, __FILE__, s)
#define _cru_free(p) _cru_free_wrapper (__LINE__, __FILE__, sizeof (*(p)), p)
#define MEMAVAIL _cru_allocation_allowed (__LINE__, __FILE__, err)
#define NOMEM (! MEMAVAIL)
#else
#define _cru_malloc(s) malloc (s)
#define _cru_free(p) free (p)
#define MEMAVAIL 1
#define NOMEM 0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization and teardown -------------------------------------------------------------

// initialize pthread resources
extern int
_cru_open_wrap (int *err);

// release pthread resources and report errors to stderr
extern void
_cru_close_wrap ();

// --------------- wrapper functions -----------------------------------------------------------------------

// used in place of malloc if memory testing is enabled
extern void *
_cru_malloc_wrapper (unsigned line, char *file, size_t s);

// used in place of free if memory testing is enabled
extern void
_cru_free_wrapper (unsigned line, char *file, size_t s, void *p);

// --------------- control functions -----------------------------------------------------------------------

// simulate a memory allocation and return non-zero if successful
extern int
_cru_allocation_allowed (unsigned line, char *file, int *err);

// set the limit of allowed successful allocations
extern void
_crudev_limit_allocations (uintptr_t limit, int *err);

// --------------- diagnostic functions --------------------------------------------------------------------

// report the number of allocations performed since the limit was set
extern uintptr_t
_crudev_allocations_performed (int *err);

// return non-zero if the deallocations balance the allocations and no other errors have been raised
extern int
_crudev_all_clear (int *err);

#ifdef __cplusplus
}
#endif
#endif
