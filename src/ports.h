/*
  cru -- co-recursion utilities

  copyright (c) 2022-2024 Dennis Furey

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

#ifndef CRU_PORTS_H
#define CRU_PORTS_H 1

// This file declares functions relevent to sending and receiving
// packets by worker threads.

#include "ptr.h"

// arbitrary magic number for consistency checks
#define PORT_MAGIC 664161206

// a prime number exceeding the THREAD_LIMIT constant defined in route.c
#define LARGE_PRIME 1031

// a modified mod operator for better load balancing
#define MOD(a,b) (((a) % LARGE_PRIME) % (b))

// a kill switch parameter to _cru_exchanged for workers that are killed but have to run anyway to avoid deadlock
#define IGNORE_KILL_SWITCH 0

// log of the maximum microsleep parameter used in an exponential backoff protocol
#define BACKOFF_LIMIT 18

// default value for the destinations parameter to abort functions
#define NO_POD NULL

// persistent data associated with an individual worker thread

struct port_s
{
  int gruntled;                 // holds PORT_MAGIC if no pthread operation has failed
  int waiting;                  // set to non-zero when a worker is waiting to be resumed
  int dismissed;                // set to non-zero when a worker needn't wait to be resumed
  unsigned backoff;             // log of the number of microseconds to wait before retrying if non-zero
  unsigned own_index;           // the position of this port in the array of the local router's ports
  packet_list assigned;         // incoming packets waiting to be processed on this port
  packet_list deferred;         // incoming packets whose prerequisites are not yet available
  union
  {
	 edge_list disconnections;   // edges that don't survive filtering
	 int disabled;               // non-zero when the worker on this port can't continue due to an error
  };
  pthread_mutex_t suspension;   // locks access to the port during non-atomic operations
  pthread_cond_t resumable;     // signaled to wake up an idle port when more incoming packets are available
  pthread_rwlock_t p_lock;      // for other threads to read and this one to write the visited set
  node_list survivors;          // temporary storage for nodes that survive being filtered
  node_list deletions;          // temporary storage for nodes that don't survive being filtered
  node_set reachable;           // set of nodes assigned to this port that are reachable by the current traversal
  packet_pod peers;
  union
  {
	 packet_table collided;
	 node_set visited;           // set of nodes assigned to this port for which results have been computed
	 node_set deleted;
  };
  router local;                 // context in which this port co-operates with others
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- allocation ------------------------------------------------------------------------------

// create a packet port with index o and local router l
extern port
_cru_new_port (unsigned o, router l, int *err);

// --------------- reclamation -----------------------------------------------------------------------------

// tear down a port
extern void
_cru_free_port (port *p, int *err);

// discard all further packets, and unconditionally return null
extern void *
_cru_abort (port source, packet_pod destinations, int *err);

// discard all further packets and return the error code cast to a pointer
extern void *
_cru_abort_status (port source, packet_pod destinations, int *err);

// discard all further packets and their payloads, and return the error code
extern void *
_cru_abort_packets (port source, packet_pod destinations, cru_destructor_pair z, int *err);

// discard all packets and selectively free their payloads
extern void *
_cru_abort_new_packets (port source, packet_pod destinations, cru_destructor_pair z, int *err);

// --------------- dataflow --------------------------------------------------------------------------------

// send an empty packet to the given port
extern int
_cru_ping (port destination, int *err);

// receive an empty packet and send one to the next port
extern int
_cru_pingback (port source, int *err);

// queue more packets on a port
extern int
_cru_assigned (port destination, packet_list *more_packets, int *err);

// receive some packets from the source, blocking if necessary, after sending others to the destinations
extern packet_list
_cru_exchanged (port source, packet_pod destinations, int *err);

#ifdef __cplusplus
}
#endif
#endif
