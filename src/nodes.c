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
#include "cthread.h"
#include "defo.h"
#include "edges.h"
#include "errs.h"
#include "nodes.h"
#include "wrap.h"




// --------------- memory management -----------------------------------------------------------------------






node_list
_cru_node_of (z, vertex, edges_in, edges_out, err)
	  cru_destructor_pair z;
	  void *vertex;
	  edge_list edges_in;
	  edge_list edges_out;
	  int *err;

	  // Create a new graph node.
{
  node_list new_node;
  int ux;

  if (*err ? 1 : (new_node = (node_list) _cru_malloc (sizeof (*new_node))) ? 0 : RAISE(ENOMEM))
	 {
		if ((! z) ? NULL : z->v_free ? vertex : NULL)
		  APPLY(z->v_free, vertex);
		_cru_free_edges (edges_in, err);
		_cru_free_edges_and_termini (z, edges_out, err);
		return NULL;
	 }
  memset (new_node, 0, sizeof (*new_node));
  new_node->vertex = vertex;
  new_node->edges_in = edges_in;
  new_node->edges_out = edges_out;
  return new_node;
}










void
_cru_free_nodes (nodes, r, err)
	  node_list nodes;
	  cru_destructor_pair r;
	  int *err;

	  // Free a node list along with its edges and vertices.
{
  node_list t;
  int ux;

  for (; (t = nodes); _cru_free (t))
	 {
		_cru_free_edges (nodes->edges_in, err);
		_cru_free_edges_and_labels (r ? r->e_free : NO_LABEL_DESTRUCTOR, nodes->edges_out, err);
		if (nodes->vertex ? (r ? r->v_free : NULL) : NULL)
		  APPLY(r->v_free, nodes->vertex);
		nodes->vertex = NULL;
		nodes = nodes->next_node;
	 }
}







void
_cru_free_adjacencies (nodes, r, err)
	  node_list *nodes;
	  cru_destructor_pair r;
	  int *err;

	  // Free a node list along with its edges, vertices, and adjacent vertices.
{
  node_list t, n;
  int ux;

  if (nodes ? 0 : IER(1222))
	 return;
  for (n = *nodes; (t = n); _cru_free (t))
	 {
		_cru_free_edges (n->edges_in, err);
		_cru_free_edges_and_termini (r, n->edges_out, err);
		if (n->vertex ? (r ? r->v_free : NULL) : NULL)
		  APPLY(r->v_free, n->vertex);
		n->vertex = NULL;
		n = n->next_node;
	 }
  *nodes = NULL;
}






void
_cru_free_vertices (nodes, d, err)
	  node_list nodes;
	  cru_destructor d;
	  int *err;

	  // Free just the vertices in a node list.
{
  int ux;

  if (! d)
	 for (; nodes; nodes = nodes->next_node)
		nodes->vertex = NULL;
  else
	 for (; nodes; nodes = nodes->next_node)
		if (nodes->vertex)
		  {
			 APPLY(d, nodes->vertex);
			 nodes->vertex = NULL;
		  }
}








void
_cru_free_outgoing_edges_and_labels (nodes, d, err)
	  node_list nodes;
	  cru_destructor d;
	  int *err;

	  // Free just the outgoing edges in a node list.
{
  for (; nodes; nodes = nodes->next_node)
	 {
		_cru_free_edges_and_labels (d, nodes->edges_out, err);
		nodes->edges_out = NULL;
	 }
}








void
_cru_free_incoming_edges_and_labels (nodes, d, err)
	  node_list nodes;
	  cru_destructor d;
	  int *err;

	  // Free just the incoming edges in a node list and unmark the
	  // nodes.
{
  for (; nodes; nodes = nodes->next_node)
	 {
		nodes->marked = NULL;
		_cru_free_edges_and_labels (d, nodes->edges_in, err);
		nodes->edges_in = NULL;
	 }
}



// --------------- editing ---------------------------------------------------------------------------------






int
_cru_pushed_node (n, nodes, err)
	  node_list n;
	  node_list *nodes;
	  int *err;

	  // Insert a unit node list into the front of an existing node list.
{
  if ((! n) ? IER(1223) : (! nodes) ? IER(1224) : n->previous ? IER(1225) : n->next_node ? IER(1226) : 0)
	 return 0;
  if (*(n->previous = nodes))
	 (*nodes)->previous = &(n->next_node);
  n->next_node = *nodes;
  *nodes = n;
  return 1;
}








node_list
_cru_severed (n, err)
	  node_list n;
	  int *err;

	  // Separate a node from its neighboring nodes in a list. If the
	  // neighbors are already separated, leave them that way.
{
  if ((! n) ? IER(1227) : (!(n->previous)) ? IER(1228) : 0)
	 return NULL;
  if (*(n->previous) == n)
	 *(n->previous) = n->next_node;
  if (n->next_node ? (n->next_node->previous == &(n->next_node)) : 0)
	 n->next_node->previous = n->previous;
  n->next_node = NULL;
  n->previous = NULL;
  return n;
}








node_list
_cru_half_severed (n)
	  node_list n;

	  // Sever a node from a list. This operation leaves dangling
	  // pointers in its successor and predecessor nodes in the list,
	  // so it should be done either to every node in the list or to
	  // none of them. However, because this operation doesn't read or
	  // write to the successor or predecessor, it can be done
	  // concurrently without locking.
{
  if (! n)
	 return NULL;
  n->previous = NULL;
  n->next_node = NULL;
  return n;
}






node_list
_cru_cat_nodes (l, r)
	  node_list l;
	  node_list r;

	  // Consume two node lists and return their concatenation. The
	  // running time is proportional to the length of the left list.
{
  node_list *n;

  if (! (l ? r : NULL))
	 return (l ? l : r);
  for (n = &(l->next_node); *n; n = &((*n)->next_node));
  *n = r;
  r->previous = n;
  return l;
}








int
_cru_transplanted_in (l, r, err)
	  node_list l;
	  node_list r;
	  int *err;

	  // Return non-zero of the edges into the right node can be
	  // transplanted to the left node, and if so, transplant them.
{
  if ((! l) ? IER(1229) : (! r) ? IER(1230) : (l == r))
	 return 0;
  l->edges_in = _cru_cat_edges (r->edges_in, l->edges_in);
  r->edges_in = NULL;
  return 1;
}








int
_cru_transplanted_out (l, r, err)
	  node_list l;
	  node_list r;
	  int *err;

	  // Return non-zero of the edges from the right node can be
	  // transplanted to the left node, and if so, transplant them.
{
  if ((! l) ? IER(1231) : (! r) ? IER(1232) : (l == r))
	 return 0;
  l->edges_out = _cru_cat_edges (r->edges_out, l->edges_out);
  r->edges_out = NULL;
  return 1;
}





// --------------- analysis --------------------------------------------------------------------------------





int
_cru_listed (n, l)
	  node_list n;
	  node_list l;

	  // Return non-zero if n appears in l.
{
  while (l ? (n != l) : 0)
	 l = l->next_node;
  return ! ! l;
}






static void *
mapped_node (p, n, with_locks, err)
	  cru_prop p;
	  node_list n;
	  int with_locks;
	  int *err;

	  // If there are edge reductions, reduce the edges and feed the
	  // results along with the vertex to the map. If there are no edge
	  // reductions but there is a map, feed just the vertex to it. If
	  // tests are being run, allow an allocation error to be simulated
	  // at each call site to the test code.
{
  int ux;
  void *v;
  void *i;
  void *o;
  void *ua;
  void *map_result;

  if ((! n) ? IER(1233) : (! p) ? IER(1234) : (! (p->vertex.map)) ? IER(1235) : *err)
	 return NULL;
  v = (with_locks ? _cru_read (&(n->vertex), err) : n->vertex);
  if (_cru_empty_fold (&(p->incident)) ? (i = NULL) : NON_NULL)
	 i = _cru_configurably_reduced_edges (&(p->incident), v, n->edges_in, BY_TERMINUS, with_locks, err);
  if (_cru_empty_fold (&(p->outgoing)) ? (o = NULL) : NON_NULL)
	 o = _cru_configurably_reduced_edges (&(p->outgoing), v, n->edges_out, BY_TERMINUS, with_locks, err);
  map_result = APPLIED(p->vertex.map, i, v, o);
  if (i ? p->incident.r_free : NULL)
	 APPLY(p->incident.r_free, i);
  if (o ? p->outgoing.r_free : NULL)
	 APPLY(p->outgoing.r_free, o);
  if ((! *err) ? NULL : map_result ? p->vertex.m_free : NULL)
	 APPLY(p->vertex.m_free, map_result);
  return (*err ? NULL : map_result);
}






void *
_cru_mapped_node (p, n, err)
	  cru_prop p;
	  node_list n;
	  int *err;

	  // Probe a node without locks.
{
  return mapped_node (p, n, WITHOUT_LOCKS, err);
}







void *
_cru_mapped_node_with_locks (p, n, err)
	  cru_prop p;
	  node_list n;
	  int *err;

	  // Probe a node with locks.
{
  return mapped_node (p, n, WITH_LOCKS, err);
}








void *
_cru_reduced_nodes (p, n, err)
	  cru_prop p;
	  node_list n;
	  int *err;

	  // Fold or reduce a prop over all vertices and adjacent edges in
	  // a list. When folding, the right operand is the result from the
	  // previous iteration. If tests are being run, allow an
	  // allocation error to be simulated at each call site to the test
	  // code.
{
  int ux;
  void *ua;
  void *map_result;
  void *reduction_result;
  void *previous_reduction_result;

  if (p ? 0 : IER(1236))
	 return NULL;
  if (! n)
	 return ((p->vertex.vacuous_case ? 0 : RAISE(CRU_UNDVAC)) ? NULL : CALLED(p->vertex.vacuous_case));
  if (p->vertex.vacuous_case)
	 reduction_result = CALLED(p->vertex.vacuous_case);
  else if ((p->vertex.m_free != p->vertex.r_free) ? IER(1237) : 0)
	 return NULL;
  else
	 {
		reduction_result = mapped_node (p, n, WITHOUT_LOCKS, err);
		n = n->next_node;
	 }
  if ((! n) ? 0 : p->vertex.reduction ? 1 : ! IER(1238))
	 for (; *err ? NULL : n; n = n->next_node)
		{
		  previous_reduction_result = reduction_result;
		  map_result = mapped_node (p, n, WITHOUT_LOCKS, err);
		  reduction_result = (*err ? NULL : APPLIED(p->vertex.reduction, map_result, previous_reduction_result));
		  if (p->vertex.m_free ? map_result : NULL)
			 APPLY(p->vertex.m_free, map_result);
		  if (p->vertex.r_free ? previous_reduction_result : NULL)
			 APPLY(p->vertex.r_free, previous_reduction_result);
		}
  if (*err ? (reduction_result ? p->vertex.r_free : NULL) : NULL)
	 APPLY(p->vertex.r_free, reduction_result);
  return (*err ? NULL : reduction_result);
}
