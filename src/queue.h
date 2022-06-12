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

#ifndef CRU_QUEUE_H
#define CRU_QUEUE_H 1

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// default values of parameter to _cru_queue_of
#define NO_NODES NULL

// a queue of nodes enables merging in constant time

struct node_queue_s
{
  node_list front;                // first node in the queue
  node_list back;                 // last node in the queue
  int qu_status;                  // error code
  int basic;                      // non-zero means the first node in the queue is the base node of a graph
};

// --------------- memory management -----------------------------------------------------------------------

// make a new queue out of a list
extern node_queue
_cru_queue_of (node_list n, int *err);

// return the nodes in a queue and free it
extern node_list
_cru_nodes_of (node_queue q, int *err);

// reclaim all storage associated with a node queue
extern void
_cru_free_node_queue (node_queue q, cru_destructor_pair r, int *err);

// --------------- editing ---------------------------------------------------------------------------------

// append a unit list of nodes to the end of a queue of nodes
extern int
_cru_enqueued_node (node_list new_node, node_queue *q, int *err);

// append one node queue to another
extern int
_cru_append_nodes (node_queue *q, node_queue r, int *err);

#ifdef __cplusplus
}
#endif
#endif
