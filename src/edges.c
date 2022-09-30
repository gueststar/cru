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
#include "brig.h"
#include "cthread.h"
#include "errs.h"
#include "edges.h"
#include "nodes.h"
#include "wrap.h"
#include "table.h"

#define BY_ACCUMULATOR 1
#define NOT_BY_ACCUMULATOR 0





// --------------- memory management -----------------------------------------------------------------------





edge_list
_cru_edge (r, label, remote_vertex, remote_node, next_edge, err)
	  cru_destructor_pair r;
	  void *label;
	  void *remote_vertex;
	  node_list remote_node;
	  edge_list next_edge;
 	  int *err;

	  // Create a new edge with the vertex value embedded.
{
  edge_list new_edge;
  struct cru_destructor_pair_s z;
  int ux;

  if (*err)
	 goto a;
  if ((new_edge = (edge_list) _cru_malloc (sizeof (*new_edge))) ? 0 : RAISE(ENOMEM))
	 goto a;
  memset (new_edge, 0, sizeof(*new_edge));
  new_edge->label = label;
  if (remote_node)
	 new_edge->remote.node = remote_node;
  else
	 new_edge->remote.vertex = remote_vertex;
  new_edge->next_edge = next_edge;
  return new_edge;
 a: memset (&z, 0, sizeof (z));
  z.e_free = (r ? r->e_free : NULL);
  _cru_free_edges_and_termini (remote_node ? &z : r, next_edge, err);
  if (! r)
	 return NULL;
  if (remote_vertex ? r->v_free : NULL)
	 APPLY(r->v_free, remote_vertex);
  if (label ? r->e_free : NULL)
	 APPLY(r->e_free, label);
  return NULL;
}







void
_cru_free_edges (e, err)
	  edge_list e;
	  int *err;

	  // Dispose of a list of edges.
{
  _cru_free_edges_and_termini (NO_DESTRUCTORS, e, err);
}







void
_cru_free_edges_and_labels (label_destructor, e, err)
	  cru_destructor label_destructor;
	  edge_list e;
	  int *err;

	  // Dispose of a list of edges and its labels.
{
  struct cru_destructor_pair_s r;

  memset (&r, 0, sizeof (r));
  r.e_free = label_destructor;
  _cru_free_edges_and_termini (&r, e, err);
}








void
_cru_free_labels (label_destructor, e, err)
	  cru_destructor label_destructor;
	  edge_list e;
	  int *err;

	  // Dispose of only the labels in a list of edges.
{
  int ux;

  if (! label_destructor)
	 for (; e; e = e->next_edge)
		e->label = NULL;
  else
	 for (; e; e = e->next_edge)
		{
		  if (e->label)
			 APPLY(label_destructor, e->label);
		  e->label = NULL;
		}
}







void
_cru_free_edges_and_termini (r, e, err)
	  cru_destructor_pair r;
	  edge_list e;
	  int *err;

	  // Dispose of a list of edges and their terminal vertices.
{
  edge_list t;
  int ux;

  if ((! r) ? 1 : r->e_free ? 0 : ! (r->v_free))
	 for (; (t = e); _cru_free (t))
		{
		  e->label = e->remote.vertex = NULL;
		  e = e->next_edge;
		}
  else if (r->v_free ? r->e_free : NULL)
	 for (; (t = e); _cru_free (t))
		{
		  if (e->label)
			 APPLY(r->e_free, e->label);
		  if (e->remote.vertex)
			 APPLY(r->v_free, e->remote.vertex);
		  e->label = e->remote.vertex = NULL;
		  e = e->next_edge;
		}
  else if (r->e_free)
	 for (; (t = e); _cru_free (t))
		{
		  if (e->label)
			 APPLY(r->e_free, e->label);
		  e->label = e->remote.vertex = NULL;
		  e = e->next_edge;
		}
  else
	 for (; (t = e); _cru_free (t))
		{
		  if (e->remote.vertex)
			 APPLY(r->v_free, e->remote.vertex);
		  e->label = e->remote.vertex = NULL;
		  e = e->next_edge;
		}
}






// --------------- editing ---------------------------------------------------------------------------------









void
_cru_push_edge (new_edge, old_edges, err)
	  edge_list new_edge;
	  edge_list *old_edges;
	  int *err;

	  // Insert a unit edge list at the beginning of an existing edge list.
{
  if ((! new_edge) ? 1 : new_edge->next_edge ? IER(853) : old_edges ? 0 : IER(854))
	 return;
  new_edge->next_edge = *old_edges;
  *old_edges = new_edge;
}








void
_cru_enqueue_edge (new_edge, front, back, err)
	  edge_list new_edge;
	  edge_list *front;
	  edge_list *back;
	  int *err;

	  // Append an edge list to an existing edge list.
{
  if ((! new_edge) ? 1 : (! front) ? IER(855) : (! back) ? IER(856) : ((! *front) != ! *back) ? IER(857) : 0)
	 return;
  if ((! *back) ? 0 : (*back)->next_edge ? IER(858) : 0)
	 return;
  if (*front)
	 (*back)->next_edge = new_edge;
  else
	 *front = *back = new_edge;
  while ((*back)->next_edge)
	 *back = (*back)->next_edge;
}







edge_list
_cru_cat_edges (l, r)
	  edge_list l;
	  edge_list r;

	  // Consume two edge lists and return their concatenation. The
	  // running time is proportional to the length of the left list.
	  // The lists must be disjoint or a cyclic structure will be
	  // created. In the interest of performance, this condition isn't
	  // checked.
{
  edge_list *n;

  if (! (l ? r : NULL))
	 return (l ? l : r);
  for (n = &(l->next_edge); *n; n = &((*n)->next_edge));
  *n = r;
  return l;
}







edge_list
_cru_repeating_labels (o, n, err)
	  cru_order o;
	  edge_list n;
	  int *err;

	  // Remove and return edges with repeating labels from an edge
	  // list. Leave the first of each edge whose label is repeated in
	  // the list.
{
  uintptr_t h;
  edge_list *e;
  multiset_table t;
  edge_list r;

  r = NULL;
  e = &n;
  if ((! o) ? IER(859) : (! (o->hash)) ? IER(860) : (! *e) ? 1 : ! ((*e)->next_edge))
	 return NULL;
  for (t = NULL; *err ? NULL : *e;)
	 if (_cru_already_recorded (h = (o->hash) ((*e)->label), o->equal, (*e)->label, t, err))
		_cru_push_edge (_cru_popped_edge (e, err), &r, err);
	 else
		{
		  _cru_record_edge (h, o->equal, (*e)->label, &t, err);
		  e = &((*e)->next_edge);
		}
  _cru_discount (t);
  return r;
}








edge_list
_cru_popped_edge (edges, err)
	  edge_list *edges;
	  int *err;

	  // Remove and return the first edge from a list.
{
  edge_list o;

  if ((! edges) ? IER(861) : *edges ? 0 : IER(862))
	 return NULL;
  *edges = (o = *edges)->next_edge;
  o->next_edge = NULL;
  return o;
}






edge_list
_cru_deleted_edge (target, edges, err)
	  edge_list target;
	  edge_list *edges;
	  int *err;

	  // Remove and return a specific edge from a list.
{
  edge_list *e;

  if ((! edges) ? IER(863) : 0)
	 return NULL;
  for (e = edges; *e; e = &((*e)->next_edge))
	 if (*e == target)
		return _cru_popped_edge (e, err);
  IER(864);
  return NULL;
}







edge_list
_cru_minimum_edge (t, edges, err)
	  cru_qpred t;
	  edge_list *edges;
	  int *err;

	  // Remove and return the minimum edge from a list based on a total
	  // order relational predicate.
{
  edge_list e, m;
  int ux, ut;

  m = NULL;
  if ((! t) ? IER(865) : (! edges) ? IER(866) : *edges ? 0 : IER(867))
	 return NULL;
  for (e = *edges; *err ? NULL : e; e = e->next_edge)
	 if (! (e->remote.node ? 0 : IER(868)))
		if (m ? PASSED(t, e->label, e->remote.node->vertex, m->label, m->remote.node->vertex) : 1)
		  m = e;
  return (*err ? NULL : _cru_deleted_edge (m, edges, err));
}









edge_list
_cru_disconnections (terminus, carrier, edges, err)
	  node_list terminus;
	  edge_list carrier;
	  edge_list *edges;
	  int *err;

	  // Remove edges matching the given terminus and carrier.
{
  edge_list *e;
  edge_list result;

  if (terminus ? (carrier ? (edges ? (carrier->next_edge ? IER(869) : 0) : IER(870)) : 1) : 1)
	 return NULL;
  result = NULL;
  for (e = edges; *e;)
	 if (((*e)->label == carrier->label) ? ((*e)->remote.node == terminus) : 0)
		result = _cru_cat_edges (_cru_popped_edge (e, err), result);
	 else
		e = &((*e)->next_edge);
  return result;
}






edge_list
_cru_deduplicated_edges (e, o, d, err)
	  edge_list e;
	  cru_order o;
	  cru_destructor d;
	  int *err;

	  // Consume and deduplicate an edge list.
{
  edge_list result;

  result = _cru_deduplicated_brigade (o, _cru_marshalled (&e, BY_TERMINUS, d, err), BY_TERMINUS, d, err);
  _cru_free_edges_and_labels (d, e, err);
  return result;
}









// --------------- reduction -------------------------------------------------------------------------------






uintptr_t
_cru_degree (e)
	  edge_list e;

	  // Return the number of edges in an edge list.
{
  uintptr_t d;

  for (d = 0; e; e = e->next_edge)
	 d++;
  return d;
}











static void *
reduced_edges (m, v, e, by_class, by_accumulator, with_locks, err)
	  cru_fold m;
	  void *v;               // source vertex from every edge
	  edge_list e;
	  int by_class;          // if non-zero, refer to the composite vertex of the remote vertex's class
	  int by_accumulator;    // if non-zero, use a partial result in place of the remote vertex
	  int with_locks;
	  int *err;

	  // Fold or reduce a list of edge labels into a single value. When
	  // folding, the partial result is the right operand. If tests are
	  // being run, allow an allocation error to be simulated at each
	  // call site to the test code.
{
#define DANGLING(l,x) (((x = (by_class ? CLASS_OF(l->remote.node) : l->remote.node))) ? 0 : IER(871))
#define VERTEX(x) (by_accumulator ? x->accumulator : with_locks ? _cru_read (&(x->vertex), err) : x->vertex)

  int ux;
  void *ua;
  node_list n;
  void *map_result;
  void *reduction_result;
  void *previous_reduction_result;

  if (m ? 0 : IER(872))
	 return NULL;
  if (! e)
	 return ((m->vacuous_case ? 0 : RAISE(CRU_UNDVAC)) ? NULL : CALLED(m->vacuous_case));
  if (m->map ? 0 : IER(873))
	 return NULL;
  if (m->vacuous_case)
	 reduction_result = CALLED(m->vacuous_case);
  else if ((m->m_free != m->r_free) ? IER(874) : DANGLING(e,n))
	 return NULL;
  else
	 {
		reduction_result = APPLIED(m->map, v, e->label, VERTEX(n));
		e = e->next_edge;
	 }
  if ((! e) ? 0 : m->reduction ? 1 : ! IER(875))
	 for (; *err ? 0 : e ? (! DANGLING(e,n)) : 0; e = e->next_edge)
		{
		  previous_reduction_result = reduction_result;
		  map_result = APPLIED(m->map, v, e->label, VERTEX(n));
		  reduction_result = (*err ? NULL : APPLIED(m->reduction, map_result, previous_reduction_result));
		  if (map_result ? m->m_free : NULL)
			 APPLY(m->m_free, map_result);
		  if (previous_reduction_result ? m->r_free : NULL)
			 APPLY(m->r_free, previous_reduction_result);
		}
  if (*err ? (reduction_result ? m->r_free : NULL) : NULL)
	 APPLY(m->r_free, reduction_result);
  return (*err ? NULL : reduction_result);
}









void *
_cru_reduced_edges (m, v, e, err)
	  cru_fold m;
	  void *v;             // source vertex from every edge
	  edge_list e;
	  int *err;

	  // Reduce a list of edges into a single value using the remote vertex.
{
  return reduced_edges (m, v, e, BY_TERMINUS, NOT_BY_ACCUMULATOR, WITHOUT_LOCKS, err);
}









void *
_cru_reduced_edges_by_accumulator (m, v, e, err)
	  cru_fold m;
	  void *v;             // source vertex from every edge
	  edge_list e;
	  int *err;

	  // Reduce a list of edges based on the remote accumulator.
{
  return reduced_edges (m, v, e, BY_TERMINUS, BY_ACCUMULATOR, WITHOUT_LOCKS, err);
}








void *
_cru_configurably_reduced_edges (m, v, e, by_class, with_locks, err)
	  cru_fold m;
	  void *v;               // source vertex from every edge
	  edge_list e;
	  int by_class;          // if non-zero, refer to the composite vertex of the remote vertex's class
	  int with_locks;
	  int *err;

	  // Reduce a list of edge labels using either the remote vertex or
	  // the remote composite vertex.
{
  return reduced_edges (m, v, e, by_class, NOT_BY_ACCUMULATOR, with_locks, err);
}
