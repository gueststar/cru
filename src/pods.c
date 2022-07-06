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
#include "errs.h"
#include "pack.h"
#include "pods.h"
#include "wrap.h"


// --------------- memory management -----------------------------------------------------------------------




packet_pod
_cru_pod_of (a, err)
	  unsigned a;
	  int *err;

	  // Create a new packet pod with the given arity on the heap if
	  // possible.
{
  packet_pod p;
  packet_list *d;
  size_t pod_size;

  if (a ? 0 : IER(1263))
	 return NULL;
  pod_size = a * sizeof (*d);
  if ((pod_size < a) ? IER(1264) : (pod_size < sizeof (*d)) ? IER(1265) : 0)
	 return NULL;
  if ((d = (packet_list *) _cru_malloc (pod_size)) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (d, 0, pod_size);
  if ((p = (packet_pod) _cru_malloc (sizeof (*p))) ? 0 : RAISE(ENOMEM))
	 {
		_cru_free (d);
		return NULL;
	 }
  memset (p, 0, sizeof (*p));
  p->arity = a;
  p->pod = d;
  return p;
}






void
_cru_free_pod (p, err)
	  packet_pod p;
	  int *err;

	  // Deallocate a packet pod, all queued packets to it, and their payloads.
{
  unsigned i;

  if ((! p) ? 1 : (! (p->pod)) ? IER(1266) : 0)
	 return;
  for (i = 0; i < p->arity; i++)
	 _cru_free_packets (p->pod[i], NO_VERTEX_DESTRUCTOR, err);
  _cru_free_packets (p->deferrals, NO_VERTEX_DESTRUCTOR, err);
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, p->arity * sizeof (p->pod[0]), p->pod);
#else
  free (p->pod);
#endif
  _cru_free (p);
}








// --------------- dataflow --------------------------------------------------------------------------------






packet_list
_cru_recycled (s, i, err)
	  packet_pod s;
	  unsigned i;
	  int *err;

	  // Harvest the packets from a particular list in a pod.
{
  packet_list r;

  if ((! s) ? 1 : (! (s->pod)) ? IER(1267) : (i >= s->arity))
	 return NULL;
  r = s->pod[i];
  s->pod[i] = NULL;
  return r;
}
