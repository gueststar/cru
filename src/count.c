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
#include "count.h"
#include "edges.h"
#include "errs.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "table.h"








void *
_cru_node_counting_task (s, err)
	  port s;
	  int *err;

	  // Count the vertices in a graph co-operatively with other workers.
{
  packet_list incoming;       // incoming packets
  uintptr_t count;
  unsigned sample;
  node_set seen;              // previously received packet payloads
  packet_pod d;               // outgoing packets
  node_list n;
  int killed;
  router r;

  killed = 0;
  count = 0;
  sample = 0;
  seen = NULL;
  if ((! s) ? IER(690) : (s->gruntled != PORT_MAGIC) ? IER(691) : 0)
	 return NULL;
  if ((! (r = s->local)) ? IER(692) : (r->valid != ROUTER_MAGIC) ? IER(693) : 0)
	 return NULL;
  if (((d = s->peers)) ? 0 : IER(694))
	 return _cru_abort (s, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, d, err));)
	 {
		KILL_SITE(6);
		killed = (killed ? 1 : KILLED);
		if ((n = (node_list) incoming->payload) ? (_cru_test_and_set_membership (n, &seen, err) ? 1 : *err) : IER(695))
		  goto a;
		if (killed ? 1 : *err)
		  goto a;
		_cru_scatter_out (n, d, err);
		if (! ++count)
		  IER(696);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  return (*err ? NULL : (void *) count);
}











void *
_cru_edge_counting_task (s, err)
	  port s;
	  int *err;

	  // Count the edges in a graph co-operatively with other workers.
{
  packet_list incoming;       // incoming packets
  uintptr_t count;
  unsigned sample;
  node_set seen;              // previously received packet payloads
  packet_pod d;               // outgoing packets
  node_list n;
  edge_list e;
  int killed;
  router r;

  count = 0;
  sample = 0;
  killed = 0;
  seen = NULL;
  if ((! s) ? IER(697) : (s->gruntled != PORT_MAGIC) ? IER(698) : 0)
	 return NULL;
  if ((! (r = s->local)) ? IER(699) : (r->valid != ROUTER_MAGIC) ? IER(700) : 0)
	 return NULL;
  if (((d = s->peers)) ? 0 : IER(701))
	 return _cru_abort (s, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, d, err));)
	 {
		KILL_SITE(7);
		killed = (killed ? 1 : KILLED);
		if ((n = (node_list) incoming->payload) ? (_cru_test_and_set_membership (n, &seen, err) ? 1 : *err) : IER(702))
		  goto a;
		if (killed ? 1 : *err)
		  goto a;
		_cru_scatter_out (n, d, err);
		for (e = n->edges_out; e; e = e->next_edge)
		  if (! ++count)
			 IER(703);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  return (*err ? NULL : (void *) count);
}













int
_cru_counted (count, i, r, err)
	  uintptr_t *count;
	  node_list i;
	  router r;
	  int *err;

	  // Concurrently compute the summation of something starting from
	  // an initial node i. The task r->work in the router r is
	  // probably initialized before entry as either
	  // _cru_node_counting_task or _cru_edge_counting_task.
{
  return _cru_count_launched (UNKILLABLE, i, i, _cru_scalar_hash (i), r, count, err);
}
