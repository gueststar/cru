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

#include <nthm/nthm.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "errs.h"


// the maximum number of error codes worth recording
#define ERROR_LIMIT 16

// used for storing error codes not reportable any other way
static int global_error[ERROR_LIMIT];

// the number of errors reported
static unsigned error_count = 0;

// for exclusive access to the global error code
static pthread_mutex_t error_lock;

// unrecoverable pthread error
static int deadlocked = 0;


// --------------- initialization and teardown -------------------------------------------------------------








int
_cru_open_errs (err)
	  int *err;

	  // Initialize pthread resources.
{
  pthread_mutexattr_t a;
  unsigned i;

  for (i = 0; i < ERROR_LIMIT; i++)
	 global_error[i] = 0;
  if (! _cru_error_checking_mutex_type (&a, err))
	 return 0;
  if (pthread_mutex_init (&error_lock, &a) ? IER(903) : 0)
	 {
		pthread_mutexattr_destroy (&a);
		return 0;
	 }
  if (pthread_mutexattr_destroy (&a) ? (! IER(904)) : 1)
	 return 1;
  pthread_mutex_destroy (&error_lock);
  return 0;
}







void
_cru_close_errs ()

	  // Report global errors. This routine isn't thread safe and
	  // should be called once only at exit.
{
  int last_error;
  unsigned i;

  last_error = 0;
  nthm_sync (&last_error);
  if (deadlocked ? 0 : (last_error = (pthread_mutex_destroy (&error_lock) ? THE_IER(905) : 0)) ? (error_count + 1) : 0)
	 {
		if (error_count < ERROR_LIMIT)
		  global_error[error_count] = last_error;
		error_count++;
	 }
  for (i = 0; (i < error_count) ? (i < ERROR_LIMIT) : 0; i++)
	 fprintf (stderr, "%s\n", cru_strerror (global_error[i]));
  if (i == error_count)
	 return;
  fprintf (stderr, "cru: %u further error%s ", error_count - i, (error_count - i > 1) ? "s" : "");
  fprintf (stderr, "w%s detected\n", (error_count - i > 1) ? "ere" : "as");
}





// --------------- error handling functions ----------------------------------------------------------------



 





int
_cru_error_checking_mutex_type (a, err)
	  pthread_mutexattr_t *a;
	  int *err;

	  // Initialize the attributes for a mutex to use for error checking.
{
  if ((! a) ? IER(906) : pthread_mutexattr_init (a) ? IER(907) : 0)
	 return 0;
  if (! (pthread_mutexattr_settype (a, PTHREAD_MUTEX_ERRORCHECK) ? IER(908) : 0))
	 return 1;
  pthread_mutexattr_destroy (a);
  return 0;
}






void
_cru_globally_throw (err)
	  int err;

	  // Safely assign the global error code.
{
  if (deadlocked ? 1 : ! err)
	 return;
  if (pthread_mutex_lock (&error_lock) ? (deadlocked = 1) : 0)
	 return;
  if (error_count + 1)                      // stop counting errors if the counter is about to overflow
	 {
		if (error_count < ERROR_LIMIT)
		  global_error[error_count] = err;
		if (error_count ? (global_error[error_count - 1] != err) : 1)
		  error_count++;
	 }
  if (pthread_mutex_unlock (&error_lock))
	 deadlocked = 1;
}







// --------------- public facing API -----------------------------------------------------------------------







const char*
cru_strerror (err)
	  int err;

	  // Return a short explanation of the given error code. This
	  // function isn't thread safe.
{
#define POSIX_FMT "cru-%d.%d.%d: %s"
#define IER_FMT "cru-%d.%d.%d: internal error code %d"
#define FAIL_FMT "cru-%d.%d.%d: test failure code %d"
#define UNDIAGNOSED_FMT "cru-%d.%d.%d: undiagnosed error"

  static char error_buffer[80 + sizeof(IER_FMT)];

  if (err >= 0)
	 {
		errno = 0;
		sprintf (error_buffer, POSIX_FMT, CRU_VERSION_MAJOR, CRU_VERSION_MINOR, CRU_VERSION_PATCH, strerror (err));
		if (errno)
		  sprintf (error_buffer, UNDIAGNOSED_FMT, CRU_VERSION_MAJOR, CRU_VERSION_MINOR, CRU_VERSION_PATCH);
		return error_buffer;
	 }
  switch (err)
	 {
	 case CRU_BADCLS: return "cru: invalid or corrupted class or partition";
	 case CRU_BADGPH: return "cru: invalid or corrupted graph";
	 case CRU_BADKIL: return "cru: invalid or corrupted kill switch";
	 case CRU_BADPRT: return "cru: invalid or corrupted partition";
	 case CRU_DEVDBF: return "cru: double free";
	 case CRU_DEVIAL: return "cru: inconsistent allocation limits";
	 case CRU_DEVMLK: return "cru: memory leak";
	 case CRU_DEVPBL: return "cru: diagnostic feature unavailable in production";
	 case CRU_INCCON: return "cru: inconsistent connectors in build specification";
	 case CRU_INCFIL: return "cru: inconsistent filter specification";
	 case CRU_INCINV: return "cru: inconsistent initial vertex in zone";
	 case CRU_INCMRG: return "cru: inconsistent merge specification";
	 case CRU_INCMUT: return "cru: inconsistent mutations";
	 case CRU_INCSPB: return "cru: inconsistent spreading builder";
	 case CRU_INCTRP: return "cru: inconsistent traversal priorities";
	 case CRU_INCVEL: return "cru: inconsistent vertex equality relation";
	 case CRU_INTKIL: return "cru: user interrupt";
	 case CRU_INTOVF: return "cru: vertex limit exceeded during build or expansion";
	 case CRU_INTOOC: return "cru: operation attempted out of context";
	 case CRU_NULCLS: return "cru: null class";
	 case CRU_NULCSF: return "cru: null classifier specification";
	 case CRU_NULKIL: return "cru: null kill switch";
	 case CRU_NULPRT: return "cru: null partition";
	 case CRU_PARDPV: return "cru: duplicate vertices detected while partitioning";
	 case CRU_PARVNF: return "cru: vertex not found by partition";
	 case CRU_TPCMPR: return "cru: type conflict in map reduction destructors";
	 case CRU_TPCMUT: return "cru: type conflict in mutated destructors";
	 case CRU_UNDANA: return "cru: undefined ana function in splitter";
	 case CRU_UNDCAT: return "cru: undefined cata function in splitter";
	 case CRU_UNDCBO: return "cru: undefined conditional binary operator";
	 case CRU_UNDCON: return "cru: undefined connectors in build specification";
	 case CRU_UNDCQO: return "cru: undefined conditional quartenary operator";
	 case CRU_UNDCTO: return "cru: undefined conditional ternary operator";
	 case CRU_UNDEFB: return "cru: undefined edge fabricator";
	 case CRU_UNDEPR: return "cru: undefined edge product operator in crosser";
	 case CRU_UNDEQU: return "cru: undefined equality relation";
	 case CRU_UNDEXP: return "cru: undefined predicate in stretcher specification";
	 case CRU_UNDHSH: return "cru: undefined hash function";
	 case CRU_UNDMAP: return "cru: undefined map in fold specification";
	 case CRU_UNDPOP: return "cru: undefined postponable predicate in postponer";
	 case CRU_UNDRED: return "cru: undefined reduction in fold specification";
	 case CRU_UNDVAC: return "cru: undefined vacuous case in fold specification";
	 case CRU_UNDVFB: return "cru: undefined vertex fabricator";
	 case CRU_UNDVPR: return "cru: undefined vertex product operator in crosser";
	 default :
		if ((NTHM_MIN_ERR <= (-err)) ? ((-err) <= NTHM_MAX_ERR) : 0)
		  return nthm_strerror (err);
		if ((CRU_MIN_ERR <= (-err)) ? ((-err) <= CRU_MAX_ERR) : 0)
		  sprintf (error_buffer, IER_FMT, CRU_VERSION_MAJOR, CRU_VERSION_MINOR, CRU_VERSION_PATCH, -err);
		else if ((CRU_MIN_FAIL <= (-err)) ? ((-err) <= CRU_MAX_FAIL) : 0)
		  sprintf (error_buffer, FAIL_FMT, CRU_VERSION_MAJOR, CRU_VERSION_MINOR, CRU_VERSION_PATCH, -err);
		else
		  sprintf (error_buffer, UNDIAGNOSED_FMT, CRU_VERSION_MAJOR, CRU_VERSION_MINOR, CRU_VERSION_PATCH);
		return error_buffer;
	 }
}
