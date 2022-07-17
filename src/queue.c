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
#include "nodes.h"
#include "queue.h"
#include "route.h"
#include "wrap.h"


// --------------- memory management -----------------------------------------------------------------------






node_queue
_cru_queue_of (n, err)
	  node_list n;
	  int *err;

	  // Make a queue out of a list and consume the error.
{
  node_queue q;

  if ((q = (node_queue) _cru_malloc (sizeof (*q))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (q, 0, sizeof (*q));
  q->qu_status = *err;
  *err = 0;
  if ((q->back = q->front = n))
	 for (q->front->previous = &(q->front); q->back->next_node;)
		q->back = q->back->next_node;
  return q;
}








node_list
_cru_nodes_of (q, err)
	  node_queue q;
	  int *err;

	  // Return the nodes in a queue and free it.
{
  node_list result;

  if (! q)
	 return NULL;
  if ((result = q->front))
	 result->previous = NULL;
  RAISE(q->qu_status);
  q->qu_status = THE_IER(1387);
  _cru_free (q);
  return result;
}








void
_cru_free_node_queue (q, r, err)
	  node_queue q;
	  cru_destructor_pair r;
	  int *err;

	  // Reclaim all storage associated with a node queue.
{
  _cru_free_nodes (_cru_nodes_of (q, err), r, err);
}







// --------------- editing ---------------------------------------------------------------------------------








int
_cru_enqueued_node (new_node, q, err)
	  node_list new_node;
	  node_queue *q;
	  int *err;

	  // Put a new node into a node queue at the end.
{
  if ((! q) ? IER(1388) : 0)
	 return 0;
  if ((! new_node) ? 1 : new_node->next_node ? IER(1389) : new_node->previous ? IER(1390) : 0)
	 return 0;
  if (*q ? NULL : (*q = (node_queue) _cru_malloc (sizeof (**q))))
	 memset (*q, 0, sizeof (**q));
  else if (*q ? 0 : RAISE(ENOMEM))
	 return 0;
  if (((!((*q)->front)) != !((*q)->back)) ? IER(1391) : (!((*q)->back)) ? 0 : (*q)->back->next_node ? IER(1392) : 0)
	 return 0;
  if ((*q)->back)
	 {
		new_node->previous = &((*q)->back->next_node);
		(*q)->back = (*q)->back->next_node = new_node;
	 }
  else
	 {
		(*q)->front = (*q)->back = new_node;
		new_node->previous = &((*q)->front);
	 }
  return 1;
}









int
_cru_append_nodes (q, r, err)
	  node_queue *q;
	  node_queue r;
	  int *err;

	  // Append one node queue to another.
{
  if (q ? 0 : IER(1393))
	 return 0;
  if ((! r) ? 1 : *q ? 0 : ! ! (*q = r))
	 return 1;
  if (((!(r->front)) != ! (r->back)) ? IER(1394) : ((!((*q)->front)) != !((*q)->back)) ? IER(1395) : 0)
	 return 0;
  (*q)->qu_status = ((*q)->qu_status ? (*q)->qu_status : r->qu_status);
  if ((! (r->front)) ? (! ((*q)->front)) : 0)
	 goto a;
  if (!(r->front ? (*q)->front : NULL))
	 {
		(*q)->front = ((*q)->front ? (*q)->front : r->front);
		(*q)->back = ((*q)->back ? (*q)->back : r->back);
		(*q)->front->previous = &((*q)->front);
		goto a;
	 }
  if (r->back->next_node ? IER(1396) : (*q)->back->next_node ? IER(1397) : (r->front->previous != &(r->front)) ? IER(1398) : 0)
	 return 0;
  (*q)->back->next_node = r->front;
  r->front->previous = &((*q)->back->next_node);
  (*q)->back = r->back;
 a: _cru_free (r);
  return 1;
}
