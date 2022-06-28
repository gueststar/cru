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
  if ((! r) ? IER(1590) : (r->valid != ROUTER_MAGIC) ? IER(1591) : 0)
	 return 0;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1592) : 0) ? (r->valid = MUGGLE(57)) : 0)
	 return 0;
  if ((r->ro_status == status) ? 1 : r->ro_status ? 0 : (r->ro_status = status))
	 status = 0;
  if ((pthread_mutex_unlock (&(r->lock)) ? IER(1593) : 0) ? (r->valid = MUGGLE(58)) : 0)
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

  if ((! r) ? IER(1594) : (r->valid != ROUTER_MAGIC) ? IER(1595) : 0)
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

  if ((! r) ? IER(1596) : (r->valid != ROUTER_MAGIC) ? IER(1597) : 0)
	 return;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1598) : 0) ? (r->valid = MUGGLE(59)) : 0)
	 return;
  if (! (((u = r->running + 1) ? 0 : IER(1599)) ? (r->valid = MUGGLE(60)) : 0))
	 r->running = u;
  if (pthread_mutex_unlock (&(r->lock)) ? IER(1600) : 0)
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
  if ((! r) ? IER(1601) : (r->valid != ROUTER_MAGIC) ? IER(1602) : 0)
	 return 0;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1603) : 0) ? (r->valid = MUGGLE(62)) : 0)
	 return 0;
  if ((r->running ? 0 : IER(1604)) ? (pthread_mutex_unlock (&(r->lock)) ? (r->valid = MUGGLE(63)) : 1) : 0)
	 return 0;
  if (--(r->running) ? 0 : (r->quiescent = 1))
	 if ((pthread_cond_signal (&(r->transition)) ? IER(1605) : 0) ? (r->valid = MUGGLE(64)) : 0)
		{
		  pthread_mutex_unlock (&(r->lock));
		  return 0;
		}
  if ((pthread_mutex_unlock (&(r->lock)) ? IER(1606) : 0) ? (r->valid = MUGGLE(65)) : 0)
	 return 0;
  return 1;
}





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
  if ((! r) ? IER(1607) : (r->valid != ROUTER_MAGIC) ? IER(1608) : r->ports ? 0 : IER(1609))
	 return;
  for (i = 0; i < r->lanes; i++)
	 {
		if ((!(p = r->ports[i])) ? IER(1610) : (p->gruntled != PORT_MAGIC) ? IER(1611) : 0)
		  continue;
		if ((pthread_mutex_lock (&(p->suspension)) ? IER(1612) : 0) ? (p->gruntled = MUGGLE(66)) : 0)
		  continue;
		if (NEW_ASSIGNMENT(p) ? p->waiting : 0)
		  {
			 _cru_swell (r, err);
			 if (pthread_cond_signal (&(p->resumable)) ? IER(1613) : 0)
				p->gruntled = MUGGLE(67);
		  }
		if (pthread_mutex_unlock (&(p->suspension)) ? IER(1614) : 0)
		  p->gruntled = MUGGLE(68);
	 }
}












void
_cru_wait_for_quiescence (k, r, err)
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Wait until all workers are suspended.
{
  if ((! r) ? IER(1615) : (r->valid != ROUTER_MAGIC) ? IER(1616) : 0)
	 return;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(1617) : 0) ? (r->valid = MUGGLE(69)) : 0)
	 return;
  _cru_enable_killing (k, &(r->killed), err);
  if (r->quiescent ? 0 : pthread_cond_wait (&(r->transition), &(r->lock)) ? IER(1618) : 0)
	 r->valid = MUGGLE(70);
  else if (pthread_mutex_unlock (&(r->lock)) ? IER(1619) : 0)
	 r->valid = MUGGLE(71);
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

  if ((! r) ? IER(1620) : (r->valid != ROUTER_MAGIC) ? IER(1621) : 0)
	 return;
  if ((!(r->lanes)) ? IER(1622) : (!(r->ports)) ? IER(1623) : 0)
	 return;
  for (i = 0; i < r->lanes; i++)
	 if (!((!(source = r->ports[i])) ? IER(1624) : (source->gruntled == PORT_MAGIC) ? 0 : IER(1625)))
		{
		  if ((pthread_mutex_lock (&(source->suspension)) ? IER(1626) : 0) ? (source->gruntled = MUGGLE(72)) : 0)
			 continue;
		  if (source->assigned ? IER(1627) : 0)
			 _cru_nack (source->assigned, err);
		  source->assigned = NULL;
		  if (source->waiting ? (pthread_cond_signal (&(source->resumable)) ? IER(1628) : 0) : ! (source->dismissed = 1))
			 source->gruntled = MUGGLE(73);
		  if (pthread_mutex_unlock (&(source->suspension)) ? IER(1629) : 0)
			 source->gruntled = MUGGLE(74);
		}
}
