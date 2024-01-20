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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "brig.h"
#include "errs.h"
#include "edges.h"
#include "nodes.h"
#include "wrap.h"



void
_cru_push_bucket (bucket, b, d, err)
	  edge_list bucket;
	  brigade *b;
	  cru_destructor d;
	  int *err;

	  // Push a new bucket into a brigade.
{
  brigade n;

  if ((! b) ? IER(557) : (n = (brigade) _cru_malloc (sizeof (*n))) ? 0 : RAISE(ENOMEM))
	 {
		_cru_free_edges_and_labels (d, bucket, err);
		return;
	 }
  memset (n, 0, sizeof (*n));
  n->bucket = bucket;
  n->other_buckets = *b;
  *b = n;
}









brigade
_cru_popped_bucket (b, err)
	  brigade *b;
	  int *err;

	  // Return the first bucket from a brigade.
{
  brigade t;

  if ((! b) ? IER(558) : ! *b)
	 return NULL;
  t = *b;
  *b = (*b)->other_buckets;
  t->other_buckets = NULL;
  return t;
}





static brigade
deleted_bucket (target, b, err)
	  brigade target;
	  brigade *b;
	  int *err;

	  // Remove and return a specific bucket from a
	  // brigade. Cf. _cru_deleted_edge in edges.c.
{
  brigade *e;

  if ((! b) ? IER(559) : 0)
	 return NULL;
  for (e = b; *e; e = &((*e)->other_buckets))
	 if (*e == target)
		return _cru_popped_bucket (e, err);
  IER(560);
  return NULL;
}







brigade
_cru_minimum_bucket (t, b, err)
	  cru_qpred t;
	  brigade *b;
	  int *err;

	  // Remove and return the minimum bucket from a brigade based on a
	  // relational predicate. Assume each member of each bucket is
	  // equivalent to all other members of the same bucket with
	  // respect to the predicate. Cf. _cru_minimum_edge in edges.c.
{
  brigade c, m;
  edge_list e;
  int ux, ut;

  if ((! t) ? IER(561) : (! b) ? IER(562) : (! *b) ? IER(563) : 0)
	 return NULL;
  m = NULL;
  for (c = *b; *err ? NULL : c; c = c->other_buckets)
	 if ((e = c->bucket) ? 1 : ! IER(564))
		if (e->remote.node ? 1 : ! IER(565))
		  if (m ? PASSED(t, e->label, e->remote.node->vertex, m->bucket->label, m->bucket->remote.node->vertex) : 1)
			 m = c;
  return (*err ? NULL : deleted_bucket (m, b, err));
}










void
_cru_free_brigade (b, err)
	  brigade b;
	  int *err;

	  // Free a brigade ignoring the buckets.
{
  brigade o;

  for (; (o = b); _cru_free (o))
	 b = b->other_buckets;
}








brigade
_cru_bucketed (e, by_class, err)
	  edge_list *e;
	  int by_class;
	  int *err;

	  // Turn an edge list into a brigade classified either by the
	  // termini or the classes thereof. This algorithm is used as a
	  // replacement for _cru_marshalled in table.c if Judy arrays are
	  // unavailable.
{
  brigade t;
  brigade *f;
  node_list n, o;

  t = NULL;
  if (e ? 0 : IER(566))
	 return NULL;
  o = NULL;
  for (f = &t; *e;)
	 {
		if (((o = (by_class ? CLASS_OF((*e)->remote.node) : (*e)->remote.node))) ? 0 : IER(567))
		  break;
		if (! *f)
		  _cru_push_bucket (NULL, f, NO_LABEL_DESTRUCTOR, err);
		if (! *f)
		  break;
		if ((*f)->bucket ? ((n = (*f)->bucket->remote.node) ? (by_class ? (CLASS_OF(n) != o) : (n != o)) : IER(568)) : 0)
		  {
			 f = &((*f)->other_buckets);
			 continue;
		  }
		_cru_push_edge (_cru_popped_edge (e, err), &((*f)->bucket), err);
		o = NULL;
		f = &t;
	 }
  return t;
}








brigade
_cru_bundled (r, e, err)
	  cru_bpred r;
	  edge_list *e;
	  int *err;

	  // Turn an edge list into a brigade classified by an arbitrary
	  // relation r and consume the edge list if successful but leave
	  // it otherwise. Treat a null relation r as one that's always
	  // false. This function is a less performant alternative to
	  // _cru_rallied in table.c that doesn't depend on Judy arrays.
{
#define EMPTY_BUCKET NULL

  brigade t, o;
  brigade *f;
  int ux, ut;

  t = NULL;
  if ((! e) ? IER(569) : ! *e)
	 return NULL;
  if (! r)
	 for (; *err ? NULL : *e; t->bucket = _cru_popped_edge (e, err))
		{
		  _cru_push_bucket (EMPTY_BUCKET, &t, NO_LABEL_DESTRUCTOR, err);
		  if (*err ? 1 : t ? 0 : IER(570))
			 break;
		}
  else
	 for (f = &t; *err ? NULL : *e;)
		{
		  if (! *f)
			 _cru_push_bucket (EMPTY_BUCKET, f, NO_LABEL_DESTRUCTOR, err);
		  if (*err ? 1 : *f ? 0 : IER(571))
			 break;
		  if ((*f)->bucket ? PASSED(r, (*f)->bucket->label, (*e)->label) : 1)
			 {
				_cru_push_edge (_cru_popped_edge (e, err), &((*f)->bucket), err);
				f = &t;
			 }
		  else
			 f = &((*f)->other_buckets);
		}
  return (((*e ? 1 : *err) ? (*e = _cru_cat_edges (_cru_unbundled (t), *e)) : NULL) ? NULL : t);
}









edge_list
_cru_unbundled (b)
	  brigade b;

	  // Flatten a brigade into an edge list.
{
  edge_list result;
  brigade o;

  for (result = NULL; (o = b); _cru_free (o))
	 {
		result = _cru_cat_edges (b->bucket, result);
		b = b->other_buckets;
	 }
  return result;
}








brigade
_cru_cat_brigades (l, r)
	  brigade l;
	  brigade r;

	  // Consume and concatenate two brigades. Cf. _cru_cat_edges in
	  // edges.c.
{
  brigade *n;

  if (! (l ? r : NULL))
	 return (l ? l : r);
  for (n = &(l->other_buckets); *n; n = &((*n)->other_buckets));
  *n = r;
  return l;
}








edge_list
_cru_deduplicated_brigade (r, t, by_class, d, err)
	  cru_order r;
	  brigade t;
	  int by_class;
	  cru_destructor d;             // edge label destructor
	  int *err;

	  // Return a list containing exactly one edge for each combination
	  // of label and remote node in a brigade.
{
  edge_list e, u, result;
  brigade o;

  for (result = NULL; (o = t); _cru_free (o))
	 {
		_cru_free_edges_and_labels (d, _cru_repeating_labels (r, t->bucket, err), err);
		if (by_class)
		  for (u = t->bucket; u; u = u->next_edge)
			 u->remote.node = CLASS_OF(u->remote.node);
		result = _cru_cat_edges (t->bucket, result);
		t = t->other_buckets;
	 }
  if (*err)
	 _cru_free_edges_and_labels (d, result, err);
  return (*err ? NULL : result);
}









edge_list
_cru_reduced_brigade (m, v, t, by_class, d, err)
	  cru_fold m;
	  void *v;
	  brigade t;
	  int by_class;
	  cru_destructor d;    // input label destructor
	  int *err;

	  // Convert a brigade to an edge list with one edge for each
	  // bucket and the edge's label derived from the edges in the
	  // bucket by user-supplied map and reduction functions. Consume
	  // the brigade.
{
  struct cru_destructor_pair_s r;
  int deduplicating;                 // non-zero means assume deduplication
  edge_list result;
  node_list n;
  brigade o;
  void **s;
  void *l;

  memset (&r, 0, sizeof (r));
  r.e_free = (m ? m->r_free : NULL);
  for (result = NULL; (o = t); _cru_free (o))
	 {
		if (t->bucket ? 1 : ! IER(572))
		  if ((n = (by_class ? CLASS_OF(t->bucket->remote.node) : t->bucket->remote.node)) ? 1 : ! IER(573))
			 {
				l = _cru_configurably_reduced_edges (m, v, t->bucket, by_class, WITHOUT_LOCKS, err);
				_cru_push_edge (_cru_edge (&r, l, NO_VERTEX, n, NO_NEXT_EDGE, err), &result, err);
			 }
		_cru_free_edges_and_labels (d, t->bucket, err);
		t = t->other_buckets;
	 }
  if (*err)
	 _cru_free_edges_and_labels (m ? m->r_free : NULL, result, err);
  return (*err ? NULL : result);
}
