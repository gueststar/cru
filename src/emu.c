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

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "classes.h"
#include "edges.h"
#include "emu.h"
#include "errs.h"
#include "wrap.h"



// --------------- multiset table bins ---------------------------------------------------------------------



void
_cru_multirecord (e, l, t, err)
	  cru_bpred e;                // equality of edge labels
	  void *l;                       // an edge label
	  multiset_table_bin *t;
	  int *err;

	  // Include an edge in the multiset but don't bump the multiplicity.
{
  multiset_table_bin *p;
  int ux, ut;

  if ((! t) ? IER(875) : e ? 0 : IER(876))
	 return;
  for (p = t; (! *p) ? 0 : FAILED(e, l, (*p)->edge_label); p = &((*p)->other_edges));
  if (*err ? 1 : *p ? 1 : (*p = (multiset_table_bin) _cru_malloc (sizeof (**p))) ? 0 : RAISE(ENOMEM))
	 return;
  memset (*p, 0, sizeof (**p));
  (*p)->edge_label = l;
}






void
_cru_multimerge (t, q, e, err)
	  multiset_table_bin *t;
	  multiset_table_bin *q;
	  cru_bpred e;            // user-defined equality relation
	  int *err;

	  // Merge two edge multisets and accumulate the multiplicities.
	  // This algorithm normally would take quadratic time but the
	  // arguments are always empty or unit lists unless there were
	  // hash collisions or no Judy arrays. Multiplicities are numbered
	  // from zero so their combination is the successor of their
	  // sum. Arithmetic overflow shouldn't be possible but is checked
	  // anyway.
{
  multiset_table_bin *p;
  multiset_table_bin o;
  uintptr_t m;
  int ux, ut;

  if ((! t) ? 1 : (! q) ? IER(877) : e ? 0 : IER(878))
	 return;
  while (*q ? (! *err) : 0)
	 {
		for (p = t; (! *p) ? 0 : FAILED(e, (*p)->edge_label, (*q)->edge_label); p = &((*p)->other_edges));
		if (*err)
		  break;
		if (*p)
		  {
			 m = (*p)->multiplicity + (*q)->multiplicity + 1;
			 if ((m <= (*p)->multiplicity) ? IER(879) : (m <= (*q)->multiplicity) ? IER(880) : 0)
				break;
			 (*p)->multiplicity = m;
			 *q = (o = *q)->other_edges;
			 _cru_free (o);
			 continue;
		  }
		*p = *q;
		*q = (*q)->other_edges;
		(*p)->other_edges = NULL;
	 }
  for (; (o = *q); _cru_free (o))
	 *q = (*q)->other_edges;      // reached only if there was an error
}








void
_cru_multipoint (t, n, err)
	  multiset_table_bin t;
	  node_list n;
	  int *err;

	  // Point the edges in a multiset table bin to a node n.
{
  for (; t; t = t->other_edges)
	 if (t->edge ? 1 : ! IER(881))
		t->edge->remote.node = n;
}







void
_cru_multifree (t)
	  multiset_table_bin t;

	  // Free a multiset.
{
  multiset_table_bin o;

  for (; (o = t); _cru_free (o))
	 t = t->other_edges;
}





// --------------- node sets -------------------------------------------------------------------------------





int
_cru_member_nj (node, seen)
	  node_list node;
	  node_set_nj seen;

	  // Return non-zero if the given node is in the set.
{
  for (; seen; seen = seen->subset)
	 if (seen->element == node)
		return 1;
  return 0;
}







int
_cru_set_membership_nj (node, seen, err)
	  node_list node;
	  node_set_nj *seen;
	  int *err;

	  // Return non-zero if the input node is successfully stored in
	  // the set.
{
  node_set_nj p;

  if ((! seen) ? IER(882) : (p = (node_set_nj) _cru_malloc (sizeof (*p))) ? 0 : RAISE(ENOMEM))
	 return 0;
  memset (p, 0, sizeof (*p));
  p->element = node;
  p->subset = *seen;
  *seen = p;
  return 1;
}







void
_cru_clear_membership_nj (node, seen)
	  node_list node;
	  node_set_nj *seen;

	  // Remove the node from the set.
{
  node_set_nj *p;
  node_set_nj o;

  if (! seen)
	 return;
  for (p = seen; (o = *p); p = &(o->subset))
	 if (o->element == node)
		{
		  *p = o->subset;
		  _cru_free (o);
		  return;
		}
}








int
_cru_test_and_set_membership_nj (node, seen, err)
	  node_list node;
	  node_set_nj *seen;
	  int *err;

	  // Return non-zero if the given node has been previously stored
	  // in the set. Otherwise, store it and return 0.
{
  node_set_nj p;

  if (seen ? 0 : IER(883))
	 return 0;
  for (p = *seen; p; p = p->subset)
	 if (p->element == node)
		return 1;
  if ((p = (node_set_nj) _cru_malloc (sizeof (*p))) ? 0 : RAISE(ENOMEM))
	 return 0;
  memset (p, 0, sizeof (*p));
  p->element = node;
  p->subset = *seen;
  *seen = p;
  return 0;
}








void
_cru_forget_members_nj (seen)
	  node_set_nj seen;

	  // Free the set of previously stored nodes.
{
  node_set_nj t;

  for (; (t = seen); _cru_free (t))
	 seen = seen->subset;
}





// --------------- vertex maps -----------------------------------------------------------------------------







cru_class
_cru_image_nj (m, v, err)
	  vertex_map_nj m;
	  void *v;
	  int *err;

	  // Return the class associated with a vertex.
{
  for (; m; m = m->antirefinement)
	 if (m->preimage == v)
		return m->image;
  return NULL;
}








void
_cru_free_vertex_map_nj (m, err)
	  vertex_map_nj m;
	  int *err;

	  // Free a vertex map.
{
  vertex_map_nj o;

  for (; (o = m); _cru_free (o))
	 {
		_cru_free_class (m->image, err);
		m = m->antirefinement;
	 }
}









void
_cru_associate_nj (m, v, h, err)
	  vertex_map_nj *m;
	  void *v;
	  cru_class h;
	  int *err;

	  // Store an association of a class with a vertex in the map if
	  // there isn't one already and consume the class.
{
  vertex_map_nj *o;

  if ((! h) ? 1 : (h->classed != CLASS_MAGIC) ? IER(884) : m ? 0 : IER(885))
	 goto a;
  for (o = m; *o; o = &((*o)->antirefinement))
	 if ((*o)->preimage == v)
		goto a;
  if ((*o = (vertex_map_nj) _cru_malloc (sizeof (**o))) ? 0 : RAISE(ENOMEM))
	 goto a;
  memset (*o, 0, sizeof (**o));
  (*o)->preimage = v;
  (*o)->image = h;
  return;
 a: _cru_free_class (h, err);
}
