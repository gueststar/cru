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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "errs.h"
#include "crew.h"
#include "cthread.h"
#include "pack.h"
#include "pods.h"
#include "wrap.h"

// the number of error locations worth recording
#define ERR_LIMIT 0x40

// the number failed allocation attempts worth recording
#define TEST_LIMIT 0x40

// the successor of the largest anticipated argument to _cru_malloc measured in memory quanta
#define SIZE_LIMIT 0x100

typedef struct heap_tracker_s
{
  char* caller;            // name of a source file in which memory has been allocated
  uintptr_t count;         // a dynamically updated count of currently active storage areas allocated in the source file
} *heap_tracker;

#ifdef WRAP

// the number of memory allocations allowed
static uintptr_t malloc_calls_allowed = UINTPTR_MAX;

// the number of memory allocations performed
static uintptr_t malloc_call_count = 0;

// the number of memory deallocations performed
static uintptr_t free_call_count = 0;

// non-zero means there have been more deallocations than allocations
static int double_free = 0;

// the number of times an error such as a double free is detected
static unsigned err_count = 0;

// the number of attempts to allocate memory when a failure is simulated
static unsigned test_count = 0;

// file names wherein failed allocations are simulated during test runs
static char *test_file[TEST_LIMIT];

// corresponding line numbers where failed allocations are simulated during test runs
static unsigned test_line[TEST_LIMIT];

// file names wherein double free errors are detected
static char *err_file[ERR_LIMIT];

// corresponding line numbers where double free errors are detected
static unsigned err_line[ERR_LIMIT];

// a record of live allocations by size and caller
static struct heap_tracker_s tracker[SIZE_LIMIT][MALLOX];

// for exclusive access to the above
static pthread_mutex_t wrap_lock;

#endif

// non-zero means _cru_open_wrap has been called and counting has started
static int initialized = 0;


// --------------- initialization and teardown -------------------------------------------------------------




int
_cru_open_wrap (err)
	  int *err;

	  // Initialize pthread resources. Setting the intialized flag
	  // isn't thread-safe, but this function is called only once.
{
#ifdef WRAP
  unsigned i, j;

  for (i = 0; i < SIZE_LIMIT; i++)
	 for (j = 0; j < MALLOX; j++)
		memset (&(tracker[i][j]), 0, sizeof (tracker[i][j]));
  for (test_count = i = 0; i < TEST_LIMIT; i++)
	 {
		test_file[i] = NULL;
		test_line[i] = 0;
	 }
  for (err_count = i = 0; i < ERR_LIMIT; i++)
	 {
		err_file[i] = NULL;
		err_line[i] = 0;
	 }
  return (initialized = ! _cru_mutex_init (&wrap_lock, err));
#else
  return initialized = 1;
#endif
}





#ifdef WRAP

static char *
file_name (p, t, err)
	  char *p;
	  char **t;
	  int *err;

	  // Return a pointer to the file name portion of the given path
	  // name, which is either the substring following the last path
	  // separator if the string contains path separators but doesn't
	  // end with one, or the whole string otherwise.
{
#define PATH_SEPARATOR '/'
#define TAB_LENGTH 4

  char *f;

  if ((! t) ? IER(1730) : p ? 0 : IER(1731))
	 return NULL;
  for (f = p; *p; f = ((*(p++) == PATH_SEPARATOR) ? p : f));
  *t = ((strlen (f = (*f ? f : p))) < TAB_LENGTH) ? "\t" : "";
  return f;
}

#endif






void
_cru_close_wrap ()

	  //  Release pthread resources and print the message locations and
	  //  error locations if any on exit. Report multiple consecutive
	  //  messages only once each assuming equal strings in the arrays
	  //  are shared copies.
{
#ifdef WRAP
#define REPEATED(m,f,l,n,t) (((m = ((n == t) ? NULL : f[n])) == (n ? f[n-1] : NULL)) ? (n ? (l[n] == l[n-1]) : 0) : 0)

  int err;
  char *p;
  char *t;
  unsigned i, j;

  t = "";
  err = 0;
  for (i = j = 0; initialized ? (i <= TEST_LIMIT) : 0; i++)
	 if (! (REPEATED(p, test_file, test_line, i, TEST_LIMIT) ? ++j : 0))
		{
		  fprintf (stderr, j ? "%s\t(%u times)\n" : i ? "\n" : "", t, j + 1);
		  if ((j = ! (p ? *p : 0)))
			 break;
		  fprintf (stderr, "cru: memory denied at %s:%-4u", file_name (test_file[i], &t, &err), test_line[i]);
		}
  if (test_count > i)
	 fprintf (stderr, "cru: %u further allocation attempt%s\n", test_count - i, ((test_count - i) == 1) ? "" : "s");
  for (i = j = 0; initialized ? (i <= ERR_LIMIT) : 0; i++)
	 if (! (REPEATED(p, err_file, err_line, i, ERR_LIMIT) ? ++j : 0))
		{
		  fprintf (stderr, j ? "%s\t\t(%u times)\n" : i ? "\n" : "", t, j + 1);
		  if ((j = ! (p ? *p : 0)))
			 break;
		  fprintf (stderr, "cru: double free at %s:%u", file_name (err_file[i], &t, &err), err_line[i]);
		}
  if (err_count > i)
	 fprintf (stderr, "cru: %u further double free attempt%s\n", err_count - i, ((err_count - i) == 1) ? "" : "s");
  if (initialized ? (! double_free) : 0)
	 for (i = 0; i < SIZE_LIMIT; i++)
		for (j = 0; (j < MALLOX) ? (tracker[i][j]).caller : NULL; j++)
		  {
			 if (! (tracker[i][j]).count)
				continue;
			 fprintf (stderr, "cru: memory leaked in %s", file_name ((tracker[i][j]).caller, &t, &err));
			 fprintf (stderr, ((tracker[i][j]).count > 1) ? "%s\t\t(%lu times)\n" : "\n", t, (tracker[i][j]).count);
			 err = (err ? err : CRU_DEVMLK);
		  }
  _cru_globally_throw ((! initialized) ? THE_IER(1732) : pthread_mutex_destroy (&wrap_lock) ? THE_IER(1733) : 0);
#endif
}






// --------------- wrapper functions -----------------------------------------------------------------------







void *
_cru_malloc_wrapper (line, file, s)
	  unsigned line;
	  char *file;
	  size_t s;

	  // This function is used in place of malloc if memory testing is
	  // enabled. It keeps a count of the number of allocations
	  // performed and allocates memory only if the count is under the
	  // limit, or if soft limits are in effect, then only if the count
	  // is not at the limit. The file names and line numbers of the
	  // first denied allocations up to TEST_LIMIT are saved for
	  // reporting on exit. A record of memory in use indexed by size
	  // and source file is kept in the heap tracker.
{
  int err;
  void *p;
  unsigned i;
  heap_tracker t;

  p = NULL;
#ifndef WRAP
  err = CRU_DEVPBL;
#else
  if (! initialized)
	 return malloc (s);
  if ((err = (pthread_mutex_lock (&wrap_lock) ? THE_IER(1734) : 0)))
	 goto a;
  if (s ? 0 : (err = THE_IER(1735)))
	 goto b;
#ifdef SOFT_LIMIT
  if (test_count ? (malloc_call_count += ! ! (p = malloc (s))) : 0)
	 goto c;
#else
  if ((malloc_call_count > malloc_calls_allowed) ? (err = THE_IER(1736)) : 0)
	 goto b;
#endif
  if (! ((malloc_call_count < malloc_calls_allowed) ? (malloc_call_count += ! ! (p = malloc (s))) : 0))
	 goto d;
 c: if ((s >>= QUANTUM) ? 0 : (err = THE_IER(1737)))
	 goto b;
  if ((--s >= SIZE_LIMIT) ? 1 : (t = tracker[s]) ? 0 : (err = THE_IER(1738)))
	 goto b;
  for (i = 0; i < MALLOX; i++)
	 if ((((t[i]).caller == file) ? 1 : (t[i]).caller ? 0 : ! ! ((t[i]).caller = file)) ? ++((t[i]).count) : 0)
		break;
  err = ((i >= MALLOX) ? THE_IER(1739) : (t[i]).count ? 0 : THE_IER(1740));
  goto b;
 d: if ((test_count >= TEST_LIMIT) ? test_count++ : 0)
	 goto b;
  test_file[test_count] = file;
  test_line[test_count++] = line;
 b: if (pthread_mutex_unlock (&wrap_lock) ? (! err) : 0)
	 err = THE_IER(1741);
#endif
 a: _cru_globally_throw (err);
  return p;
}






void
_cru_free_wrapper (line, file, s, p)
	  unsigned line;
	  char *file;
	  size_t s;
	  void *p;

	  // In place of free () if memory testing is enabled, keep a count
	  // of the number of deallocations performed, and deallocate
	  // memory only if the count is less than the number of
	  // allocations. Otherwise, globally throw a double free error.
	  // Also check that the deallocations correspond to allocations of
	  // the same size and in the same source file using the heap
	  // tracker array, so that the specific site of any double free
	  // can be reported on exit.
{
  int err;
  unsigned i;
  heap_tracker t;

#ifndef WRAP
  err = CRU_DEVPBL;
#else
  if (initialized)
	 goto a;
  free (p);
  return;
 a: if ((err = (pthread_mutex_lock (&wrap_lock) ? THE_IER(1742) : 0)))
	 goto b;
  if ((s >>= QUANTUM) ? 0 : (err = THE_IER(1743)))
	 goto d;
  if ((--s >= SIZE_LIMIT) ? 1 : (t = tracker[s]) ? 0 : (err = THE_IER(1744)))
	 goto d;
  for (i = 0; (i < MALLOX) ? ((t[i]).caller ? ((t[i]).caller != file) : 0) : 0; i++);
  if (! (((i < MALLOX) ? (t[i]).count : 0) ? (! ((t[i]).count)--) : (double_free = err = CRU_DEVDBF)))
	 goto d;
  if ((err_count >= ERR_LIMIT) ? err_count++ : 0)
	 goto d;
  err_file[err_count] = file;
  err_line[err_count++] = line;
  goto c;
 d: free_call_count++;
 c: free (p);
  if (pthread_mutex_unlock (&wrap_lock) ? (! err) : 0)
	 err = THE_IER(1745);
#endif
 b: _cru_globally_throw ((err == CRU_DEVDBF) ? 0 : err);
}







// --------------- control functions -----------------------------------------------------------------------








int
_cru_allocation_allowed (line, file, err)
	  unsigned line;
	  char *file;
	  int *err;

	  // Simulate a memory allocation and return non-zero if
	  // successful. This function is used during testing to confirm
	  // that cru deals appropriately with failed memory allocations or
	  // other errors detected at the call sites of external library
	  // functions or user code. Similarly to _cru_malloc_wrapper,
	  // record the file and line number associated with each simulated
	  // allocation failure detected up to TEST_LIMIT, and record the
	  // test count thereafter.
{
  int result;

  result = 0;
#ifndef WRAP
  RAISE(CRU_DEVPBL);
#else
  if (! initialized)
	 return 1;
  if (pthread_mutex_lock (&wrap_lock) ? IER(1746) : 0)
	 goto a;
  if (double_free ? RAISE(CRU_DEVDBF) : (free_call_count > malloc_call_count) ? IER(1747) : 0)
	 goto b;
#ifdef SOFT_LIMIT
  if (test_count ? (malloc_call_count += ! ! (free_call_count += ! ! (result = 1))) : 0)
	 goto b;
#else
  if ((malloc_call_count > malloc_calls_allowed) ? IER(1748) : 0)
	 goto b;
#endif
  if ((malloc_call_count < malloc_calls_allowed) ? (malloc_call_count += ! ! (free_call_count += ! ! (result = 1))) : 0)
	 goto b;
  if ((test_count >= TEST_LIMIT) ? test_count++ : 0)
	 goto c;
  test_file[test_count] = file;
  test_line[test_count++] = line;
 c: RAISE(ENOMEM);
 b: if (pthread_mutex_unlock (&wrap_lock))
	 IER(1749);
#endif
 a: return result;
}






void
_crudev_limit_allocations (limit, err)
	  uintptr_t limit;
	  int *err;

	  // Set the limit of allowed successful allocations. The limit can
	  // be set more than once provided it's not raised above the
	  // number of malloc calls already performed or simulated. The
	  // test programs currently set the limit only once in each run.
{
#ifndef WRAP
  RAISE(CRU_DEVPBL);
#else
  if ((! initialized) ? IER(1750) : pthread_mutex_lock (&wrap_lock) ? IER(1751) : 0)
	 return;
  if (! ((limit < malloc_call_count) ? RAISE(CRU_DEVIAL) : 0))
	 malloc_calls_allowed = limit;
  if (pthread_mutex_unlock (&wrap_lock))
	 IER(1752);
#endif
}






// --------------- diagnostic functions --------------------------------------------------------------------






uintptr_t
_crudev_allocations_performed (err)
	  int *err;

	  // Report the number of allocations performed.
{
  uintptr_t result;

#ifndef WRAP
  result = 0;
  RAISE(CRU_DEVPBL);
#else
  if ((! initialized) ? IER(1753) : *err ? 1 : pthread_mutex_lock (&wrap_lock) ? IER(1754) : 0)
	 return 0;
  result = malloc_call_count;
 a: if (pthread_mutex_unlock (&wrap_lock))
	 IER(1755);
#endif
  return result;
}










int
_crudev_all_clear (err)
	  int *err;

	  // Return non-zero if the deallocations and reserved structures
	  // balance the allocations and there are no other errors. This
	  // test isn't failsafe because a memory leak and a double free
	  // could hide each other if the memory leak happens first.
	  // However, detection of double free errors via the heap tracker
	  // is more reliable if each type of allocated structure is always
	  // allocated and freed within the same source file, and different
	  // types local to the same source file have distinct sizes. In
	  // any case, leaked memory allocated by the test driver code
	  // isn't detected even if cru is at fault by neglecting to call
	  // the corresponding destructor, so the test programs have to
	  // check for it themselves. This function is called at the
	  // conclusion of each test program but before the exit routines
	  // set by atexit() so reserved structures might still exist.
{
#ifdef WRAP
  uintptr_t reserved;

  if (initialized ? 0 : IER(1756))
	 goto a;
  reserved = _cru_reserved_packets (err) + _cru_reserved_crews (err);
  if (*err ? 1 : pthread_mutex_lock (&wrap_lock) ? IER(1757) : 0)
	 goto a;
  if (double_free ? RAISE(CRU_DEVDBF) : (free_call_count > malloc_call_count) ? IER(1758) : 0)
	 goto b;
  if (free_call_count + reserved < malloc_call_count)
	 RAISE(CRU_DEVMLK);
 b: if (pthread_mutex_unlock (&wrap_lock))
	 IER(1759);
#endif
 a: return ! *err;
}
