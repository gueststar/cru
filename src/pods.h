/*
  cru -- co-recursion utilities

  copyright (c) 2022-2023 Dennis Furey

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

#ifndef CRU_PODS_H
#define CRU_PODS_H 1

#include "ptr.h"

// A packet pod is used locally within each worker thread to queue the
// outgoing packets for the other workers.

struct packet_pod_s
{
  unsigned arity;               // the number of packet lists in the pod
  packet_list deferrals;        // previously received packets that should be retried after something changes
  packet_list *pod;             // an array with one packet list for each worker associated with the current router
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- memory management -----------------------------------------------------------------------

// create a packet pod with the given number of locations
extern packet_pod
_cru_pod_of (unsigned a, int *err);

// deallocate a packet pod, all queued packets to it, and their payloads
extern void
_cru_free_pod (packet_pod p, int *err);

// --------------- dataflow --------------------------------------------------------------------------------

// remove and return the packet list from the i-th location in the pod, which can be empty or non-empty
extern packet_list
_cru_recycled (packet_pod s, unsigned i, int *err);

#ifdef __cplusplus
}
#endif
#endif
