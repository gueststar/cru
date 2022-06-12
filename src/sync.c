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


#include <stdlib.h>
#include <string.h>
#include "errs.h"
#include "killers.h"
#include "pack.h"
#include "ports.h"
#include "repl.h"
#include "route.h"
#include "sync.h"
#include "wrap.h"




// --------------- error handling --------------------------------------------------------------------------




static int
error_consumed (r, status, err)
	  router r;
	  int status;
	  int *err;

	  // Safely consume the status code into a router and return
	  // non-zero if it is consumed. The status field is used to store
	  // an error code raised by any worker associated with the
	  // router. An existing non-zero status shouldn't be overwritten.
{
  if (! status)
	 return 1;
  if ((! r) ? IER(1591) : (r->valid != ROUTER_MAGIC) ? IER(1592) : 0)
	 return 0;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1593) : 0) ? (r->valid = MUGGLE(57)) : 0)
	 return 0;
  if ((r->ro_status == status) ? 1 : r->ro_status ? 0 : (r->ro_status = status))
	 status = 0;
  if ((pthread_mutex_unlock (&(r->lock)) ? IER(1594) : 0) ? (r->valid = MUGGLE(58)) : 0)
	 return 0;
  return ! status;
}





void
_cru_throw (r, err)
	  router r;
	  int *err;

	  // Safely set the error status of a router. This is called by
	  // worker threads just before exiting in case the error has not
	  // been otherwise reported.
{
  int status;

  status = *err;
  if (error_consumed (r, status, err))
	 *err = 0;
}






int
_cru_catch (r, err)
	  router r;
	  int *err;

	  // Interrogate the status code of a router.
{
  int status;

  if ((! r) ? IER(1595) : (r->valid != ROUTER_MAGIC) ? IER(1596) : 0)
	 return *err;
  status = r->ro_status;
  r->ro_status = 0;
  RAISE(status);
  return status;
}









// --------------- logging ---------------------------------------------------------------------------------





void
_cru_swell (r, err)
	  router r;
	  int *err;

	  // Make a note of there being one more worker running.
{
  unsigned u;

  if ((! r) ? IER(1597) : (r->valid != ROUTER_MAGIC) ? IER(1598) : 0)
	 return;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1599) : 0) ? (r->valid = MUGGLE(59)) : 0)
	 return;
  if (! (((u = r->running + 1) ? 0 : IER(1600)) ? (r->valid = MUGGLE(60)) : 0))
	 r->running = u;
  if (pthread_mutex_unlock (&(r->lock)) ? IER(1601) : 0)
	 r->valid = MUGGLE(61);
}






int
_cru_dwindled (r, err)
	  router r;
	  int *err;

	  // Make a note of there being one less worker running due to its
	  // suspension, and if there are no workers left running then
	  // signal a transition indicating quiescence.
{
  if ((! r) ? IER(1602) : (r->valid != ROUTER_MAGIC) ? IER(1603) : 0)
	 return 0;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1604) : 0) ? (r->valid = MUGGLE(62)) : 0)
	 return 0;
  if ((r->running ? 0 : IER(1605)) ? (pthread_mutex_unlock (&(r->lock)) ? (r->valid = MUGGLE(63)) : 1) : 0)
	 return 0;
  if (--(r->running) ? 0 : (r->quiescent = 1))
	 if ((pthread_cond_signal (&(r->transition)) ? IER(1606) : 0) ? (r->valid = MUGGLE(64)) : 0)
		{
		  pthread_mutex_unlock (&(r->lock));
		  return 0;
		}
  if ((pthread_mutex_unlock (&(r->lock)) ? IER(1607) : 0) ? (r->valid = MUGGLE(65)) : 0)
	 return 0;
  return 1;
}








// --------------- deadlock detection ----------------------------------------------------------------------



#ifdef DEADLOCK_DETECTION

static int
deadlocked (b, h, r, err)
	  unsigned b;              // minimum backoff setting of any active worker
	  uintptr_t h;             // current router state hash
	  router r;
	  int *err;

	  // Detect deadlock based on previous state hashes or save one for
	  // future reference.
{
  int dead;
  unsigned i;

  dead = 0;
  if ((! b) ? 1 : (b > BACKOFF_LIMIT) ? 1 : (! h) ? 1 : (! r) ? IER(1608) : (r->valid != ROUTER_MAGIC) ? IER(1609) : 0)
	 return 0;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1610) : 0) ? (r->valid = MUGGLE(66)) : 0)
	 return 0;
  if (b != BACKOFF_LIMIT)                                         // progress detected
	 memset (&(r->state_hash), 0, sizeof (r->state_hash));         // flush the state hash cache
  else
	 for (i = 0; i < DEAD_POOL; i++)
		{
		  if (r->state_hash[i] ? 0 : ((r->state_hash[i] = h)))      // first detection of this hash since stalled progress
			 break;
		  if ((r->state_hash[i] == h) ? ((dead = 1)) : 0)           // second detection, probable deadlock
			 break;
		}
  if (i == DEAD_POOL)                // use a sliding window in case oscillation starts after an initial transient
	 {
		memmove (&(r->state_hash[0]), &(r->state_hash[1]), (DEAD_POOL - 1) * sizeof (h));
		r->state_hash[DEAD_POOL - 1] = 0;
	 }
  if (pthread_mutex_unlock (&(r->lock)) ? IER(1611) : 0)
	 r->valid = MUGGLE(67);
  return dead;
}









static void
unslice (r, err)
	  router r;
	  int *err;

	  // Safely clear the sliced field in each port so that the job
	  // won't be checked for deadlock again until all workers have had
	  // a slice.
{
  port p;
  unsigned i;

  if ((! r) ? IER(1612) : (r->valid != ROUTER_MAGIC) ? IER(1613) : r->ports ? 0 : IER(1614))
	 return;
  for (i = 0; i < r->lanes; i++)
	 {
		if ((!(p = r->ports[i])) ? IER(1615) : (p->gruntled != PORT_MAGIC) ? IER(1616) : 0)
		  continue;
		if ((pthread_mutex_lock (&(p->suspension)) ? IER(1617) : 0) ? (p->gruntled = MUGGLE(68)) : 0)
		  continue;
		p->sliced = 0;
		if (pthread_mutex_unlock (&(p->suspension)) ? IER(1618) : 0)
		  p->gruntled = MUGGLE(69);
	 }
}

#endif // DEADLOCK_DETECTION




// --------------- supervision -----------------------------------------------------------------------------




void
_cru_undefer (r, err)
	  router r;
	  int *err;

	  // Resume workers where possible by recirculating their deferred
	  // packets. Also take the opportunity to detect deadlock, which
	  // occurs when all workers have cycled through their deferred
	  // packets repeatedly with no progress.
{
#define NEW_ASSIGNMENT(x) (x->assigned ? 0 : ((x->assigned = x->deferred)) ? (!(x->deferred = NULL)) : 0)

  port p;
  unsigned i;
#ifdef DEADLOCK_DETECTION
  unsigned b;              // the minimum backoff setting of any worker in receipt of a packet
  uintptr_t h;             // hash of the current router state
  int all_sliced;          // non-zero means all workers in receipt of a packet have had a time slice

  h = 0;
  all_sliced = 1;
  b = BACKOFF_LIMIT + 1;
#endif
  if ((! r) ? IER(1619) : (r->valid != ROUTER_MAGIC) ? IER(1620) : r->ports ? 0 : IER(1621))
	 return;
  for (i = 0; i < r->lanes; i++)
	 {
		if ((!(p = r->ports[i])) ? IER(1622) : (p->gruntled != PORT_MAGIC) ? IER(1623) : 0)
		  continue;
		if ((pthread_mutex_lock (&(p->suspension)) ? IER(1624) : 0) ? (p->gruntled = MUGGLE(70)) : 0)
		  continue;
#ifdef DEADLOCK_DETECTION
		if (p->assigned ? 1 : ! ! (p->deferred))
		  {
			 all_sliced = (all_sliced ? p->sliced : 0);
			 b = ((b < p->backoff) ? b : p->backoff);
		  }
		h ^= _cru_scalar_hash (p->deferred);
#endif
		if (NEW_ASSIGNMENT(p) ? p->waiting : 0)
		  {
			 _cru_swell (r, err);
			 if (pthread_cond_signal (&(p->resumable)) ? IER(1625) : 0)
				p->gruntled = MUGGLE(71);
		  }
		if (pthread_mutex_unlock (&(p->suspension)) ? IER(1626) : 0)
		  p->gruntled = MUGGLE(72);
	 }
#ifdef DEADLOCK_DETECTION
  if (*err ? 1 : ! all_sliced)
	 return;
  if (deadlocked (b, h, r, err))
	 RAISE(CRU_INTDLK);
  if (b ? (b <= BACKOFF_LIMIT) : 0)
	 unslice (r, err);
#endif
}












void
_cru_wait_for_quiescence (k, r, err)
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Wait until all workers are suspended.
{
  if ((! r) ? IER(1627) : (r->valid != ROUTER_MAGIC) ? IER(1628) : 0)
	 return;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1629) : 0) ? (r->valid = MUGGLE(73)) : 0)
	 return;
  _cru_enable_killing (k, &(r->killed), err);
  if (r->quiescent ? 0 : pthread_cond_wait (&(r->transition), &(r->lock)) ? IER(1630) : 0)
	 r->valid = MUGGLE(74);
  else if (pthread_mutex_unlock (&(r->lock)) ? IER(1631) : 0)
	 r->valid = MUGGLE(75);
  _cru_disable_killing (k, err);
}









void
_cru_dismiss (r, err)
	  router r;
	  int *err;

	  // Signal all workers in a router to exit. The router normally
	  // should be quiescent at this point.
{
  unsigned i;
  port source;

  if ((! r) ? IER(1632) : (r->valid != ROUTER_MAGIC) ? IER(1633) : 0)
	 return;
  if ((!(r->lanes)) ? IER(1634) : (!(r->ports)) ? IER(1635) : 0)
	 return;
  for (i = 0; i < r->lanes; i++)
	 if (!((!(source = r->ports[i])) ? IER(1636) : (source->gruntled == PORT_MAGIC) ? 0 : IER(1637)))
		{
		  if ((pthread_mutex_lock (&(source->suspension)) ? IER(1638) : 0) ? (source->gruntled = MUGGLE(76)) : 0)
			 continue;
		  if (source->assigned ? IER(1639) : 0)
			 _cru_nack (source->assigned, err);
		  source->assigned = NULL;
		  if (source->waiting ? (pthread_cond_signal (&(source->resumable)) ? IER(1640) : 0) : ! (source->dismissed = 1))
			 source->gruntled = MUGGLE(77);
		  if (pthread_mutex_unlock (&(source->suspension)) ? IER(1641) : 0)
			 source->gruntled = MUGGLE(78);
		}
}
