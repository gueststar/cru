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
#include <stdlib.h>
#include <string.h>
#include <nthm/nthm.h>
#include "crew.h"
#include "cthread.h"
#include "errs.h"
#include "maybe.h"
#include "queue.h"
#include "route.h"
#include "wrap.h"
#include "sync.h"

// If the reserve crews are replenished at each public API entry
// point, then the minimum limit needed to guarantee adequate reserves
// can be determined statically.
#define RESERVE_CREW_LIMIT 0x8

// an unused variable whose address is outside the heap
static uintptr_t null_crew_target;

// the pointer used to mark the end of the reserved crew list
#define NULL_CREW (crew) &null_crew_target

// storage for extra crews if the heap overflows
static crew reserve_crews = NULL_CREW;

// the number of crews in reserve
static uintptr_t crew_count = 0;

// for exclusive access to the reserve crews
static pthread_mutex_t crew_lock;


// --------------- initialization and teardown -------------------------------------------------------------




static crew
crew_of (err)
	  int *err;

	  // Create a crew on the heap if possible.
{
  crew c;

  if ((c = (crew) _cru_malloc (sizeof (*c))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (c, 0, sizeof (*c));
  return c;
}








int
_cru_open_crew (err)
	  int *err;

	  // Initialize pthread resources.
{
  pthread_mutexattr_t a;

  if (! _cru_error_checking_mutex_type (&a, err))
	 goto a;
  if (pthread_mutex_init (&crew_lock, &a) ? IER(697) : 0)
	 {
		pthread_mutexattr_destroy (&a);
		goto a;
	 }
  if (! (pthread_mutexattr_destroy (&a) ? IER(698) : 0))
	 return 1;
  pthread_mutex_destroy (&crew_lock);
 a: return 0;
}








void
_cru_close_crew ()

	  // Release pthread resources.
{
  int err;
  crew c, t;

  err = 0;
  for (c = reserve_crews; (t = ((c == NULL_CREW) ? NULL : c)); _cru_free (t))
	 {
		if (crew_count-- ? 0 : (err = THE_IER(699)))
		  break;
		c = c->c_held;
	 }
  if (err ? 0 : crew_count)
	 err = THE_IER(700);
  if (pthread_mutex_destroy (&crew_lock) ? (! err) : 0)
	 err = THE_IER(701);
  _cru_globally_throw (err);
}






// --------------- memory management -----------------------------------------------------------------------




crew
_cru_crew_of (err)
	  int *err;

	  // Create a crew or borrow it from the reserves if possible.
{
  crew c;

  if ((c = crew_of (err)) ? 1 : pthread_mutex_lock (&(crew_lock)) ? IER(702) : 0)
	 goto a;
  if ((reserve_crews == NULL_CREW) ? 1 : crew_count ? 0 : IER(703))
	 goto b;
  c = reserve_crews;
  reserve_crews = reserve_crews->c_held;
  c->c_held = NULL_CREW;
  c->created = 0;
  crew_count--;
 b: if (pthread_mutex_unlock (&crew_lock))
	 IER(704);
 a: return c;
}







static void
free_crew (c, err)
	  crew c;
	  int *err;

	  // Release the storage of a crew or hold it in
	  // reserve.
{
  crew *r;

  if (! c)
	 return;
  if ((! (c->c_held)) ? 1 : pthread_mutex_lock (&crew_lock) ? IER(705) : 0)
	 goto a;
  if ((crew_count < RESERVE_CREW_LIMIT) ? 0 : pthread_mutex_unlock (&crew_lock) ? IER(706) : 1)
	 goto a;
  for (r = &reserve_crews; *r != NULL_CREW; r = &((*r)->c_held))
	 if ((c == *r) ? IER(707) : *r ? 0 : IER(708))                  // check for a double-free
		goto b;
  crew_count++;
  *r = c;
 b: if (pthread_mutex_unlock (&crew_lock))
	 IER(709);
  return;
 a: _cru_free (c);
}








uintptr_t
_cru_reserved_crews (err)
	  int *err;

	  // Return the current number of crews held in reserve.
{
  uintptr_t result;

  if (pthread_mutex_lock (&crew_lock) ? IER(710) : 0)
	 return 0;
  result = crew_count;
  if (pthread_mutex_unlock (&crew_lock))
	 IER(711);
  return result;
}







void
_cru_replenish_crews (err)
	  int *err;

	  // Create more reserve crews if necessary.
{
  crew c;
  int dblx;

  if (pthread_mutex_lock (&crew_lock) ? IER(712) : 0)
	 return;
  for (dblx = 0; (crew_count < RESERVE_CREW_LIMIT) ? ((c = crew_of (&dblx))) : NULL; ++crew_count)
	 {
		c->c_held = reserve_crews;
		reserve_crews = c;
	 }
  RAISE(dblx);
  if (pthread_mutex_unlock (&crew_lock))
	 IER(713);
}






// --------------- recruitment -----------------------------------------------------------------------------




int
_cru_crewed (c, m, r, err)
	  crew c;
	  runner m;
	  router r;
	  int *err;

	  // Attempt to create a full crew of worker threads. If
	  // successful, the crew will be left waiting for its first
	  // packet. If unsuccessful, any partial crew will have been
	  // instructed to exit and wait only for synchronization.
{
  int dblx;

  dblx = 0;
  if ((! r) ? IER(714) : (! c) ? IER(715) : (r->valid != ROUTER_MAGIC) ? IER(716) : (! (r->threads)) ? IER(717) : 0)
	 goto a;
  if ((r->ports ? 0 : IER(718)) ? (r->valid = MUGGLE(10)) : (r->threads ? 0 : IER(719)) ? (r->valid = MUGGLE(11)) : 0)
	 goto a;
  if ((pthread_mutex_lock (&(r->lock)) ? IER(720) : 0) ? (r->valid = MUGGLE(12)) : 0)
	 goto a;
  c->bays = r->lanes;
  c->ids = r->threads;
  for (c->created = 0; dblx ? 0 : (c->created < c->bays); (c->created)++)
	 if (_cru_create (&(c->ids[c->created]), m, r->ports[c->created], &dblx))
		break;
  r->running = c->created;
  if ((pthread_mutex_unlock (&(r->lock)) ? IER(721) : 0) ? (r->valid = MUGGLE(13)) : ! (c->created))
	 goto a;
  _cru_wait_for_quiescence (UNKILLABLE, r, &dblx);  // let all workers block waiting for their first packet
  if (c->created == r->lanes)                       // if there's a full crew, don't finish the job before it starts
	 {
		r->quiescent = 0;
		RAISE(dblx);
		return 1;
	 }
  _cru_dismiss (r, &dblx);                           // the job is off so unblock all workers without sending them a packet
  _cru_wait_for_quiescence (UNKILLABLE, r, &dblx);   // let all workers terminate
 a: RAISE(dblx);
  return 0;
}





// --------------- synchronization -------------------------------------------------------------------------





int
_cru_status_disjunction (c, err)
	  crew c;
	  int *err;

	  // Join with worker threads whose identifiers are given and
	  // assign at most one result, which will be an error code.
{
  unsigned i;
  uintptr_t status;
  int disjunction;

  disjunction = 0;
  if ((! c) ? 1 : (c->bays < c->created) ? IER(722) : (! (c->ids)) ? IER(723) : 0)
	 return *err;
  for (i = 0; i < c->created; i++)
	 if (pthread_join (c->ids[i], (void **) &status) ? (! IER(724)) : 1)
		{
		  disjunction = (status ? 1 : disjunction);
		  RAISE((int) status);
		}
  free_crew (c, err);
  return disjunction;
}










uintptr_t
_cru_summation (c, err)
	  crew c;
	  int *err;

	  // Join with the crew and return the sum of their return values.
{
  uintptr_t sum, n, i;

  if ((! c) ? 1 : (c->bays < c->created) ? IER(725) : (! (c->ids)) ? IER(726) : 0)
	 return 0;
  for (sum = i = 0; i < c->created; i++)
	 if (pthread_join (c->ids[i], (void **) &n) ? (! IER(727)) : 1)
		sum += n;
  free_crew (c, err);
  return sum;
}








node_list
_cru_node_union (c, base_bay, base, err)
	  crew c;
	  unsigned base_bay;    // hash of the base vertex mod the number of workers
	  node_list *base;      // address of the base node
	  int *err;

	  // Join with all builder threads and assemble the subgraphs
	  // they've created into a single graph. Point the base node to
	  // the node with the base vertex in it, which will be at the
	  // front of the queue built by the builder indexed by the
	  // base_bay.
{
  int hit;
  unsigned i;
  node_queue q, r;

  q = NULL;
  if (base)
	 *base = NULL;
  if ((! c) ? 1 : (c->bays < c->created) ? IER(728) : c->ids ? 0 : IER(729))
	 goto a;
  hit = 0;
  for (i = 0; i < c->created; i++)
	 {
		if (pthread_join (c->ids[i], (void **) &r) ? IER(730) : ! r)
		  continue;
		if (base ? (r->basic ? 1 : (i == base_bay)) : 0)
		  if (hit ? IER(731) : ++hit)
			 *base = r->front;
		_cru_append_nodes (&q, r, err);
 	 }
 a: free_crew (c, err);
  return _cru_nodes_of (q, err);
}








void
_cru_maybe_disjunction (c, result, err)
	  crew c;
	  void **result;
	  int *err;

	  // Join with all worker threads and assign at most one result.
{
#define NO_DESTRUCTOR NULL

  int any;
  maybe x;
  uintptr_t i;

  if (result)
	 *result = NULL;
  if ((! c) ? 1 : (c->bays < c->created) ? IER(732) : (! (c->ids)) ? IER(733) : (any = 0))
	 return;
  x = NULL;
  for (i = 0; i < c->created; i++)
	 if (pthread_join (c->ids[i], (void **) &x) ? (! IER(734)) : ! ! x)
		{
		  if (x->extant ? (result ? (any ? (! IER(735)) : ++any) : ! IER(736)) : 0)
			 *result = x->value;
		  RAISE(x->ma_status);
		  _cru_free_maybe (x, NO_DESTRUCTOR, err);
		  x = NULL;
		}
  free_crew (c, err);
}










void
_cru_maybe_reduction (c, m, result, err)
	  crew c;
	  cru_fold m;
	  void **result;
	  int *err;

	  // Join with all worker threads whose identifiers are given and
	  // reduce their results.
{
  maybe x;
  nthm_pipe p;
  maybe_pair z;
  cru_destructor d;
  uintptr_t next_worker;

  next_worker = 0;
  if (result)
	 *result = NULL;
  if ((! m) ? IER(737) : (! c) ? 1 : (c->bays < c->created) ? IER(738) : c->ids ? 0 : IER(739))
	 return;
  if (((d = m->m_free) != m->r_free) ? IER(740) : 0)
	 return;
  if ((! (c->created)) ? 1 : NOMEM ? 1 : ! nthm_enter_scope (err))
	 goto a;
  for (; (next_worker < c->created); next_worker++)
	 if (NOMEM ? 1 : ! nthm_open ((nthm_worker) _cru_maybe_joiner, &(c->ids[next_worker]), err))
		break;
  for (x = NULL; *err ? NULL : NOMEM ? NULL : (p = nthm_select (err));)
	 {
		if (! (x = (maybe) nthm_read (p, err)))
		  continue;
		if (! (p = (NOMEM ? NULL : nthm_select (err))))
		  break;
		if ((z = _cru_maybe_paired (&x, (maybe) nthm_read (p, err), m->reduction, m->r_free, err)))
		  if (NOMEM ? 1 : ! nthm_open ((nthm_worker) _cru_maybe_fused, z, err))
			 _cru_free_maybe_pair (z, err);
	 }
  if (*err ? 1 : (! (x ? x->extant : 0)) ? 1 : result ? 0 : IER(741))
	 goto b;
  *err = x->ma_status;
  *result = x->value;
  x->value = NULL;
 b: _cru_free_maybe (x, m->r_free, err);
  while ((p = nthm_select (err)))
	 _cru_free_maybe ((maybe) nthm_read (p, err), d, err);          // should never happen unless there was an error
  nthm_exit_scope (err);
 a: for (; next_worker < c->created; next_worker++)
	 if (! pthread_join (c->ids[next_worker], (void **) &x))
		_cru_free_maybe (x, d, err);                                 // should never happen unless there was a worse error
  free_crew (c, err);
}
