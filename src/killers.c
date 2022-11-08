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
#include <string.h>
#include <stdlib.h>
#include "cthread.h"
#include "errs.h"
#include "killers.h"
#include "wrap.h"

#define KILLED_BY_USER 1
#define KILLED_INTERNALLY 2

// arbitrary magic number for sanity checks
#define KILL_MAGIC 1551557419

// for exclusive access to routers' killed fields; the same one locks all of them
static pthread_mutex_t killer_lock;

#define CONTROL_ENTRY                  \
  if (err ? NULL : (err = &ignored))    \
    ignored = 0;                         \


// --------------- initialization and teardown -------------------------------------------------------------




int
_cru_open_killers (err)
	  int *err;

	  // Initialize pthread resources.
{
  return ! _cru_mutex_init (&killer_lock, err);
}







void
_cru_close_killers ()

	  // Do this when the process exits.
{
  _cru_globally_throw (pthread_mutex_destroy (&killer_lock) ? THE_IER(1055) : 0);
}





// --------------- kill switch operations ------------------------------------------------------------------







cru_kill_switch
_cru_new_kill_switch (err)
	  int *err;

	  // Create a new kill switch.
{
  cru_kill_switch k;

  k = NULL;
  if ((k = (cru_kill_switch) _cru_malloc (sizeof (*k))) ? 0 : RAISE(ENOMEM))
	 goto a;
  memset (k, 0, sizeof (*k));
  k->deadly = KILL_MAGIC;
  if (! (_cru_mutex_init (&(k->safety), err) ? IER(1056) : *err))
	 goto a;
  k->deadly = MUGGLE(19);
  _cru_free (k);
 a: return (*err ? NULL : k);
}








void
_cru_enable_killing (k, r, err)
	  cru_kill_switch k;
	  int *r;
	  int *err;

	  // Safely connect a kill switch to a router. If killing was
	  // requested while the kill switch was disabled, do it now.
{
  int t;

  if ((! k) ? 1 : (k->deadly != KILL_MAGIC) ? RAISE(CRU_BADKIL) : 0)
	 return;
  if ((pthread_mutex_lock (&(k->safety)) ? IER(1057) : 0) ? (k->deadly = MUGGLE(20)) : 0)
	 return;
  k->killed = r;
  t = k->killable;
  k->killable = 0;
  if (pthread_mutex_unlock (&(k->safety)) ? IER(1058) : 0)
	 k->deadly = MUGGLE(21);
  if (t)
	 _cru_kill (r, err);
}








void
_cru_disable_killing (k, err)
	  cru_kill_switch k;
	  int *err;

	  // Safely disconnect a kill switch from a router.
{
  if ((! k) ? 1 : (k->deadly != KILL_MAGIC) ? RAISE(CRU_BADKIL) : 0)
	 return;
  if ((! k) ? 1 : (pthread_mutex_lock (&(k->safety)) ? IER(1059) : 0) ? (k->deadly = MUGGLE(22)) : 0)
	 return;
  k->killed = NULL;
  if (pthread_mutex_unlock (&(k->safety)) ? IER(1060) : 0)
	 k->deadly = MUGGLE(23);
}








void
_cru_kill (k, err)
	  int *k;
	  int *err;

	  // Safely set the killed field in a router.
{
  if ((! k) ? 1 : pthread_mutex_lock (&killer_lock) ? IER(1061) : 0)
	 return;
  *k = KILLED_BY_USER;
  if (pthread_mutex_unlock (&killer_lock))
	 IER(1062);
  RAISE(CRU_INTKIL);
}







void
_cru_kill_internally (k, err)
	  int *k;
	  int *err;

	  // Safely set the killed field in a router without raising a
	  // user-facing error.
{
  if ((! k) ? 1 : pthread_mutex_lock (&killer_lock) ? IER(1063) : 0)
	 return;
  *k = KILLED_INTERNALLY;
  if (pthread_mutex_unlock (&killer_lock))
	 IER(1064);
}







int
_cru_killed (k, err)
	  int *k;
	  int *err;

	  // Safely poll the killed field in a router.
{
  int t;

  t = 0;
  if ((! k) ? 1 : pthread_mutex_lock (&killer_lock) ? IER(1065) : 0)
	 return 0;
  t = *k;
  if (pthread_mutex_unlock (&killer_lock))
	 IER(1066);
  return t;
}








int
_cru_killed_internally (k, err)
	  int *k;
	  int *err;

	  // Safely poll the killed field in a router specifically for
	  // internal kill events.
{
  int t;

  t = 0;
  if ((! k) ? 1 : pthread_mutex_lock (&killer_lock) ? IER(1067) : 0)
	 return 0;
  t = *k;
  if (pthread_mutex_unlock (&killer_lock))
	 IER(1068);
  return (t == KILLED_INTERNALLY);
}






// --------------- public facing API -----------------------------------------------------------------------






void
cru_kill (k, err)
	  cru_kill_switch k;
	  int *err;

	  // Safely set the killed field in a router. If killing was
	  // requested previously, do it now.
{
  int ignored;
  int dblx;

  CONTROL_ENTRY;
  if (k ? 0 : RAISE(CRU_NULKIL))
	 return;
  if (((k->deadly != KILL_MAGIC)) ? RAISE(CRU_BADKIL) : 0)
	 return;
  if ((pthread_mutex_lock (&(k->safety)) ? IER(1069) : 0) ? (k->deadly = MUGGLE(24)) : 0)
	 return;
  dblx = k->killable = ! (k->killed);
  if (pthread_mutex_unlock (&(k->safety)) ? IER(1070) : 0)
	 k->deadly = MUGGLE(25);
  if (dblx)
	 return;
  _cru_kill (k->killed, &dblx);
  if (dblx != CRU_INTKIL)
	 RAISE(dblx);
}









void
cru_free_kill_switch (k, err)
	  cru_kill_switch k;
	  int *err;

	  // Deallocate a kill switch.
{
  int ignored;

  CONTROL_ENTRY;
  if (k ? 0 : RAISE(CRU_NULKIL))
	 return;
  if (((k->deadly != KILL_MAGIC)) ? RAISE(CRU_BADKIL) : 0)
	 return;
  if (pthread_mutex_destroy (&(k->safety)))
	 IER(1071);
  k->deadly = MUGGLE(26);
  _cru_free (k);
}
