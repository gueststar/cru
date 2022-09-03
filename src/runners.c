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
#include <strings.h>
#include "errs.h"
#include "maybe.h"
#include "ports.h"
#include "queue.h"
#include "repl.h"
#include "route.h"
#include "runners.h"
#include "sync.h"
#include "wrap.h"




// --------------- runners ---------------------------------------------------------------------------------




void *
_cru_status_runner (s)
	  port s;

	  // Run a worker in a newly created thread and return its error status on exit.
{
  uintptr_t status;
  router r;
  int err;

  if ((err = ((s ? (s->gruntled == PORT_MAGIC) : 0) ? 0 : THE_IER(1449))))
	 goto a;
  if ((err = (((r = s->local) ? ((r->valid == ROUTER_MAGIC) ? r->work : NULL) : 0) ? 0 : THE_IER(1450))))
	 goto a;
  status = (uintptr_t) (r->work) (s, &err);
  _cru_throw (r, &err);
 a: _cru_globally_throw (err);
  pthread_exit ((void *) status);
}








void *
_cru_maybe_runner (s)
	  port s;

	  // Run a worker returning a maybe and exit the thread.
{
  router r;
  maybe m;
  int err;

  m = NULL;
  if ((err = ((s ? (s->gruntled == PORT_MAGIC) : 0) ? 0 : THE_IER(1451))))
	 goto a;
  if ((err = (((r = s->local) ? ((r->valid == ROUTER_MAGIC) ? r->work : NULL) : 0) ? 0 : THE_IER(1452))))
	 goto a;
  if ((m = (maybe) (r->work) (s, &err)) ? 0 : err)
	 m = _cru_new_maybe (ABSENT, NULL, &err);             // report the error through the result if possible
  _cru_throw (r, &err);
 a: _cru_globally_throw (err);                           // report the error globally otherwise
  pthread_exit ((void *) m);
}










void *
_cru_queue_runner (s)
	  port s;

	  // Run a worker returning a queue and return it by exiting the thread.
{
  int err;
  router r;
  node_queue q;

  q = NULL;
  if ((err = ((s ? (s->gruntled == PORT_MAGIC) : 0) ? 0 : THE_IER(1453))))
	 goto a;
  if ((err = (((r = s->local) ? ((r->valid == ROUTER_MAGIC) ? r->work : NULL) : 0) ? 0 : THE_IER(1454))))
	 goto a;
  if ((q = (r->work) (s, &err)) ? 0 : err)
	 q = _cru_queue_of (NO_NODES, &err);            // report the error through the result if possible
  _cru_throw (r, &err);
 a: _cru_globally_throw (err);
  pthread_exit ((void *) q);
}










void *
_cru_count_runner (s)
	  port s;

	  // Run a worker returning a count and return it by exiting the thread.
{
  router r;
  uintptr_t c;
  int err;

  if ((err = ((s ? (s->gruntled == PORT_MAGIC) : 0) ? 0 : THE_IER(1455))))
	 goto a;
  if ((err = (((r = s->local) ? ((r->valid == ROUTER_MAGIC) ? r->work : NULL) : 0) ? 0 : THE_IER(1456))))
	 goto a;
  c = (uintptr_t) (r->work) (s, &err);
  _cru_throw (r, &err);
 a: _cru_globally_throw (err);
  pthread_exit ((void *) c);
}







// --------------- joiners ---------------------------------------------------------------------------------






maybe
_cru_maybe_joiner (id, err)
	  pthread_t *id;
	  int *err;

	  // Join with a thread returning a maybe.
{
  maybe result;

  result = NULL;
  if ((! id) ? IER(1457) : pthread_join (*id, (void **) &result) ? IER(1458) : 0)
	 return NULL;
  RAISE(result ? result->ma_status : 0);
  return result;
}
