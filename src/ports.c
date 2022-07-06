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
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "route.h"
#include "sync.h"
#include "table.h"
#include "wrap.h"



// --------------- allocation ------------------------------------------------------------------------------






port
_cru_new_port (o, l, err)
	  unsigned o;               // the port's index relative to the local router's array of ports
	  router l;                 // the local router to which the port reports
	  int *err;

	  // Create a port with the given local router and an empty remote
	  // router.
{
  port p;
  packet_pod d;

  if ((! l) ? IER(1268) : (l->valid != ROUTER_MAGIC) ? IER(1269) : 0)
	 return NULL;
  if ((o >= l->lanes) ? IER(1270) : 0)
	 return NULL;
  if ((p = (port) _cru_malloc (sizeof (*p))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (p, 0, sizeof (*p));
  if (_cru_mutex_init (&(p->suspension), err))
	 goto a;
  if (_cru_cond_init (&(p->resumable), err))
	 goto b;
  if (pthread_rwlock_init (&(p->p_lock), NULL) ? IER(1271) : 0)
	 goto c;
  if (! (p->peers = _cru_pod_of (l->lanes, err)))
	 goto d;
  p->local = l;
  p->own_index = o;
  p->gruntled = PORT_MAGIC;
  return p;
 d: pthread_rwlock_destroy (&(p->p_lock));
 c: pthread_cond_destroy (&(p->resumable));
 b: pthread_mutex_destroy (&(p->suspension));
 a: _cru_free (p);
  return NULL;
}




// --------------- reclamation -----------------------------------------------------------------------------




void
_cru_free_port (p, err)
	  port *p;
	  int *err;

	  // Tear down a port.
{
  if ((! p) ? IER(1272) : ! *p)
	 return;
  if (pthread_mutex_destroy (&((*p)->suspension)))
	 IER(1273);
  if (pthread_cond_destroy (&((*p)->resumable)))
	 IER(1274);
  if (pthread_rwlock_destroy (&((*p)->p_lock)))
	 IER(1275);
  _cru_free_pod ((*p)->peers, err);
  _cru_forget_members ((*p)->reachable);
  _cru_forget_members ((*p)->visited);
  _cru_nack ((*p)->assigned, err);
  _cru_nack ((*p)->deferred, err);
  (*p)->gruntled = MUGGLE(35);
  _cru_free (*p);
  *p = NULL;
}






void *
_cru_abort (source, destinations, err)
	  port source;
	  packet_pod destinations;
	  int *err;

	  // Discard all further packets.
{
  packet_list incoming;

  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 _cru_nack (_cru_popped_packet (&incoming, err), err);
  return NULL;
}









void *
_cru_abort_status (source, destinations, err)
	  port source;
	  packet_pod destinations;
	  int *err;

	  // Discard all further packets and return the error code.
{
  intptr_t status;
  packet_list incoming;

  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 _cru_nack (_cru_popped_packet (&incoming, err), err);
  status = *err;
  return (void *) status;
}









void *
_cru_abort_packets (source, destinations, z, err)
	  port source;
	  packet_pod destinations;
	  cru_destructor_pair z;
	  int *err;

	  // Kill the job and discard all further packets and return the error code.
{
  intptr_t status;
  packet_list incoming;

  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, destinations, err));)
	 if (z)
		_cru_free_packets (_cru_popped_packet (&incoming, err), z->v_free, err);
	 else
		_cru_nack (_cru_popped_packet (&incoming, err), err);
  status = *err;
  return NULL;
}









// --------------- dataflow --------------------------------------------------------------------------------








static packet_list
assignment (source, err)
	  port source;
	  int *err;

	  // Retrieve the incoming packets assigned to a port and not yet
	  // processed. This function runs in the context of the thread
	  // that receives the assigned packets. Experimental use of
	  // concurrency kit functions hasn't demonstrated any performance
	  // benefit and is disabled by default.
{
  packet_list incoming;

  if ((! source) ? IER(1276) : (source->gruntled != PORT_MAGIC) ? IER(1277) : 0)
	 return NULL;
#ifdef CK_F_PR_FAS_PTR
  incoming = ck_pr_fas_ptr (&(source->assigned), NULL);
  return incoming;
#endif
  if ((pthread_mutex_lock (&(source->suspension)) ? IER(1278) : 0) ? (source->gruntled = MUGGLE(36)) : 0)
	 return NULL;
  incoming = source->assigned;
  source->assigned = NULL;
  if (pthread_mutex_unlock (&(source->suspension)) ? IER(1279) : 0)
	 source->gruntled = MUGGLE(37);
  return incoming;
}









static int
assigned (destination, more_packets, err)
	  port destination;
	  packet_list *more_packets;
	  int *err;

	  // Put more packets into the queue of packets assigned to a
	  // destination port. If the destination port's worker was
	  // waiting, send a signal to resume it. If packets are assigned
	  // to a running worker, return non-zero.
{
  packet_list last_packet;
  int unassigned, running;

  if ((! more_packets) ? IER(1280) : ! (last_packet = *more_packets))
	 return 0;
  if ((! destination) ? IER(1281) : (destination->gruntled != PORT_MAGIC) ? IER(1282) : 0)
	 goto a;
  while (last_packet->next_packet)
	 last_packet = last_packet->next_packet;
  if ((pthread_mutex_lock (&(destination->suspension)) ? IER(1283) : 0) ? (destination->gruntled = MUGGLE(38)) : 0)
	 goto a;
  unassigned = ! (last_packet->next_packet = destination->assigned);
  destination->assigned = *more_packets;
  running = ! (destination->waiting);
  if (destination->waiting ? unassigned : 0)
	 {
		_cru_swell (destination->local, err);
		if (pthread_cond_signal (&(destination->resumable)) ? IER(1284) : 0)
		  destination->gruntled = MUGGLE(39);
	 }
  if (pthread_mutex_unlock (&(destination->suspension)) ? IER(1285) : 0)
	 destination->gruntled = MUGGLE(40);
  *more_packets = NULL;
  return running;
 a: _cru_free_packets (*more_packets, NO_VERTEX_DESTRUCTOR, err);
  *more_packets = NULL;
  return 0;
}







int
_cru_ping (destination, err)
	  port destination;
	  int *err;

	  // Send an empty packet to the given port.
{
  packet_list p;
  int running, dblx, tplx, done;

  dblx = tplx = 0;
  if ((done = ! ! (p = _cru_initial_packet_of (NO_PAYLOAD, NO_HASH, &dblx))))
	 running = assigned (destination, &p, &tplx);
  RAISE(dblx);
  RAISE(tplx);
  return done;
}






int
_cru_pingback (source, err)
	  port source;
	  int *err;

	  // Receive an empty packet and send one to the next port.
{
  packet_list incoming;
  router r;

  if ((! source) ? IER(1286) : (source->gruntled != PORT_MAGIC) ? IER(1287) : 0)
	 goto a;
  if ((! (r = source->local)) ? IER(1288) : (r->valid != ROUTER_MAGIC) ? IER(1289) : (! (r->ports)) ? IER(1290) : 0)
	 goto a;
  if ((! (r->lanes)) ? IER(1291) : (source->own_index >= r->lanes) ? IER(1292) : 0)
	 goto a;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, NO_POD, err));)
	 {
		_cru_nack (_cru_popped_packet (&incoming, err), err);
		if ((source->own_index + 1) < r->lanes)
		  _cru_ping (r->ports[source->own_index + 1], err);
	 }
  return 1;
 a: return ! ! _cru_abort (source, NO_POD, err);
}







int
_cru_assigned (destination, more_packets, err)
	  port destination;
	  packet_list *more_packets;
	  int *err;

	  // Assign as above with no vertex destructor.
{
  int running;   // ignored
  int dblx;

  dblx = 0;
  running = assigned (destination, more_packets, &dblx);
  RAISE(dblx);
  return ! dblx;
}










static void
expose (source, deferrals, err)
	  port source;
	  packet_list *deferrals;
	  int *err;

	  // Expose the deferred packets to all other workers by moving
	  // them from the current worker's local destinations pod to its
	  // port's deferred field, which is accessed by convention only
	  // when secured by a lock. This action enables any worker thread
	  // to resume any other thread safely when the only alternative
	  // would have been deadlock.
{
  packet_list s;

  if (deferrals ? (!(s = *deferrals)) : IER(1293))
	 return;
  if ((!source) ? IER(1294) : (source->gruntled != PORT_MAGIC) ? IER(1295) : 0)
	 return;
  while (s->next_packet)
	 s = s->next_packet;
  if ((pthread_mutex_lock (&(source->suspension)) ? IER(1296) : 0) ? (source->gruntled = MUGGLE(41)) : 0)
	 return;
  s->next_packet = source->deferred;
  source->deferred = *deferrals;
  if (pthread_mutex_unlock (&(source->suspension)) ? IER(1297) : 0)
	 source->gruntled = MUGGLE(42);
  *deferrals = NULL;
}










static void
circulate (source, destinations, err)
	  port source;
	  packet_pod destinations;
	  int *err;

	  // Dispatch all packet lists buffered in the pod to the input
	  // ports of other workers. If the job shows no other progress,
	  // recycle the deferred packets for all workers. Back off
	  // exponentially if the worker is spinning too much.
{
#define MAX_PORTABLE_USLEEP_PARAMETER 999999
#define MIN(a,b) (a < b ? a : (unsigned) b)

  uintptr_t i, enabled, runners;

  if ((! destinations) ? 1 : (! (destinations->pod)) ? IER(1298) : 0)
	 return;
  if ((! source) ? IER(1299) : (source->gruntled != PORT_MAGIC) ? IER(1300) : 0)
	 return;
  if ((!(source->local)) ? IER(1301) : (source->local->valid != ROUTER_MAGIC) ? IER(1302) : 0)
	 {
		source->gruntled = MUGGLE(43);
		return;
	 }
  if ((source->local->ports ? 0 : IER(1303)) ? (source->local->valid = MUGGLE(44)) : 0)
	 return;
  expose (source, &(destinations->deferrals), err);
  for (runners = enabled = i = 0; i < destinations->arity; i++)
	 if (destinations->pod[i])
		if ((i < source->local->lanes) ? (enabled = 1) : ! IER(1304))
		  runners += ! ! assigned (source->local->ports[i], &(destinations->pod[i]), err);
  if (enabled ? 0 : (runners <= 1))
	 _cru_undefer (source->local, err);
  if (source->backoff ? *err : 1)
	 return;
  usleep (MIN(MAX_PORTABLE_USLEEP_PARAMETER, 1 << source->backoff));
  if (source->backoff == BACKOFF_LIMIT)
	 return;
  if ((pthread_mutex_lock (&(source->suspension)) ? IER(1305) : 0) ? (source->gruntled = MUGGLE(45)) : 0)
	 return;
  if (++(source->backoff) ? 0 : IER(1306))
	 source->gruntled = MUGGLE(46);
  if (pthread_mutex_unlock (&(source->suspension)) ? IER(1307) : 0)
	 source->gruntled = MUGGLE(47);
}









static packet_list
blocking_assignment (source, err)
	  port source;
	  int *err;

	  // Wait until at least one new packet arrives and is signaled
	  // from some other thread unless the worker has been dismissed or
	  // there are packets available without waiting. Then return the
	  // new packets if any.
{
  packet_list s;

  if ((! source) ? IER(1308) : (source->gruntled != PORT_MAGIC) ? IER(1309) : 0)
	 return NULL;
  if ((pthread_mutex_lock (&(source->suspension)) ? IER(1310) : 0) ? (source->gruntled = MUGGLE(48)) : 0)
	 return NULL;
  if ((s = source->assigned) ? (!(source->assigned = NULL)) : source->dismissed)
	 {
		if (pthread_mutex_unlock (&(source->suspension)) ? IER(1311) : 0)
 		  source->gruntled = MUGGLE(49);
		return s;
	 }
  if (! _cru_dwindled (source->local, err))
	 goto a;
  source->waiting = 1;
  if (pthread_cond_wait (&(source->resumable), &(source->suspension)) ? IER(1312) : 0)
	 source->gruntled = MUGGLE(50);
  s = source->assigned;
  source->assigned = NULL;
  source->waiting = 0;
 a: if (pthread_mutex_unlock (&(source->suspension)) ? IER(1313) : 0)
	 source->gruntled = MUGGLE(51);
  return s;
}










packet_list
_cru_exchanged (source, destinations, err)
	  port source;
	  packet_pod destinations;
	  int *err;

	  // This function acquires more packets on behalf of the worker
	  // that calls it. The quickest way is by getting them from the
	  // destination pod entry associated with the worker's own source
	  // port, which can be recycled without any locking. The second
	  // quickest way is to get packets that have already been queued
	  // for this worker, and the third quickest is to wait for more to
	  // arrive. To avoid deadlock, the last alternative requires first
	  // sending whatever packets are ready to go.
{
  packet_list s;

  if ((! source) ? IER(1314) : (source->gruntled != PORT_MAGIC) ? IER(1315) : 0)
	 return NULL;
  if ((s = _cru_recycled (destinations, source->own_index, err)) ? s : (s = assignment (source, err)))
	 return s;
  circulate (source, destinations, err);
  return blocking_assignment (source, err);
}
