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

#ifndef CRU_SCATTER_H
#define CRU_SCATTER_H 1

// This file declares functions implementing the scatter side of a
// scatter/gather protocol. The parameter t is a packet pod local to
// the caller's thread whose associated array of packet lists has one
// entry for each worker. Packets are queued for their respective
// workers by being loaded into the pod. Scattering requires memory
// allocation and therefore might not always succeed. Unscatterable
// packets are scattered in various ways. In best effort scattering
// operations, no action is taken beyond raising an error.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- best effort scattering ------------------------------------------------------------------

// scatter and consume a list of packets prepared by the caller
extern void
_cru_scatter (packet_list p, packet_pod t, int *err);

// send an empty packet to the worker responsible for the terminus of each edge and return non-zero if successful
extern int
_cru_scattered (edge_list e, packet_pod t, int *err);

// send a packet with a sender field to another worker
extern void
_cru_send_from (void *v, edge_list c, node_list sender, packet_pod t, int *err);

// send a packet with no sender but a specified payload, carrier, and receiver
extern int
_cru_received_by (void *v, edge_list c, node_list recipient, packet_pod t, int *err);

// send a node-addressed edge-labeled packet to a worker for each outgoing edge from n
extern edge_list
_cru_scatter_out (node_list n, packet_pod t, int *err);

// send a node-addressed edge-labeled packet to a worker for each incoming edge to n
extern void
_cru_scatter_in (node_list n, packet_pod t, int *err);

// send an initial packet to each adjacent node
extern void
_cru_scattered_initially (node_list n, cru_hash h, packet_pod t, int *err);

// send a vertex-addressed edge-labeled packet to a worker for each outgoing edge from n
extern int
_cru_scattered_by_hashes (node_list n, cru_hash h, packet_pod t, int by_class, int *err);

// --------------- consumptive scattering ------------------------------------------------------------------

// send a vertex-addressed edge-labeled packet to a worker for each outgoing edge from n and delete the edges
extern int
_cru_unscatterable_by_hashes (node_list n, cru_hash h, cru_destructor d, packet_pod t, int is_initial, int *err);

// remove scattered edges with no carriers from the list
extern int
_cru_unscatterable (edge_list *e, node_list n, cru_destructor d, packet_pod t, int *err);

// scatter outgoing edges from n routed by vertex hashes and consume unscatterable edges 
extern void
_cru_scatter_out_or_consume (node_list n, cru_hash h, cru_destructor_pair z, packet_pod t, int *err);

// scatter and consume the edges in a list of edge maps
extern edge_list
_cru_scatter_and_stretch (cru_destructor_pair z, edge_map e, node_list m, cru_hash h, packet_pod t, int *err);

#ifdef __cplusplus
}
#endif
#endif
