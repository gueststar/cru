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

#include <strings.h>
#include <stdlib.h>
#include "errs.h"
#include "killers.h"
#include "mapreduce.h"
#include "maybe.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "route.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"







static void
visit (n, p, result, err)
	  node_list n;
	  cru_prop p;
	  maybe *result;
	  int *err;

	  // Operate on a single node in a graph. If there are no previous
	  // results, prop the node to compute the first one. If there are
	  // previous results, prop the node to compute the next one and
	  // combine it with the previous ones according to the prop's
	  // vertex fold's reduction operator. The unary operator result
	  // type has to be the same as the binary operator result type.
{
  int ux;
  void *ua;
  void *left;
  void *right;
  cru_destructor d;

  if (*err ? 1 : (! p) ? IER(1105) : (! result) ? IER(1106) : p->vertex.reduction ? 0 : IER(1107))
	 return;
  if (((d = p->vertex.m_free) != p->vertex.r_free) ? IER(1108) : 0)
	 return;
  if (*result)
	 goto a;
  if (((*result = _cru_new_maybe (PRESENT, right = _cru_mapped_node (p, n, err), err))) ? (! *err) : 0)
	 return;
  _cru_free_maybe (*result, NO_DESTRUCTOR, err);
  *result = NULL;
  goto b;
 a: left = _cru_mapped_node (p, n, err);
  right = (*result)->value;
  (*result)->value = APPLIED(p->vertex.reduction, left, right);
  if (d ? left : NULL)
	 APPLY(d, left);
 b: if (d ? right : NULL)
	 APPLY(d, right);
}








maybe
_cru_mapreducing_task (source, err)
	  port source;
	  int *err;

	  // Apply the map to incoming vertices and return their reduction
	  // when quiescent.
{
  packet_pod destinations;    // outgoing packets
  packet_list incoming;       // incoming packets
  unsigned sample;
  node_set seen;              // previously received packet payloads
  maybe result;
  node_list n;
  int killed;
  router r;

  sample = 0;
  killed = 0;
  seen = NULL;
  result = NULL;
  if ((! source) ? IER(1109) : (source->gruntled != PORT_MAGIC) ? IER(1110) : 0)
	 return NULL;
  if ((! (r = source->local)) ? IER(1111) : (r->valid != ROUTER_MAGIC) ? IER(1112) : 0)
	 return NULL;
  if ((!(destinations = source->peers)) ? IER(1113) : (r->tag != MAP) ? IER(1114) : 0)
	 {
		_cru_abort (source, destinations, err);
		goto a;
	 }
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 {
		KILL_SITE(19);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed)
		  goto b;
		if ((n = (node_list) incoming->payload) ? (_cru_test_and_set_membership (n, &seen, err) ? 1 : *err) : IER(1115))
		  goto b;
		_cru_scattered (r->mapreducer.ma_zone.backwards ? n->edges_in : n->edges_out, destinations, err);
		visit (n, &(r->mapreducer.ma_prop), &result, err);
	 b: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  if ((*err ? 1 : killed) ? (! result) : 1)
	 goto a;
  if (r->mapreducer.ma_prop.vertex.r_free == r->mapreducer.ma_prop.vertex.m_free)
	 _cru_free_maybe (result, r->mapreducer.ma_prop.vertex.r_free, err);
  else if (IER(1116))
	 _cru_free_maybe (result, NO_DESTRUCTOR, err);
  result = NULL;
 a: if (*err)
	 result = _cru_new_maybe (ABSENT, NULL, err);
  _cru_forget_members (seen);
  return result;
}










void *
_cru_mapreduce (k, i, r, err)
	  cru_kill_switch k;
	  node_list i;
	  router r;
	  int *err;

	  // Launch a map-reduction, block until finished, and consume the
	  // router.
{
  void *result;

  result = NULL;
  _cru_disable_killing (k, err);
  _cru_maybe_reduction_launched (k, i, r, (void **) &result, err);
  _cru_free_router (r, err);
  return result;
}
