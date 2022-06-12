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
#include "errs.h"
#include "edges.h"
#include "emap.h"
#include "wrap.h"




// --------------- memory management -----------------------------------------------------------------------







edge_map
_cru_edge_map (z, a, p, n, err)
	  cru_destructor_pair z;
	  edge_list a;
	  edge_list p;
	  edge_map *n;
	  int *err;

	  // Consume an edge map and create a new one.
{
  edge_map e;

  if (n ? 0 : IER(872))
	 goto a;
  if ((e = (edge_map) _cru_malloc (sizeof (*e))) ? 0 : RAISE(ENOMEM))
	 goto b;
  memset (e, 0, sizeof (*e));
  e->ante = a;
  e->post = p;
  e->next_map = *n;
  *n = NULL;
  return e;
 b: _cru_free_map (z, *n, err);
  *n = NULL;
 a: _cru_free_edges_and_termini (z, a, err);
  _cru_free_edges_and_labels (z ? z->e_free : NULL, p, err);
  return NULL;
}






void
_cru_free_map (z, e, err)
	  cru_destructor_pair z;
	  edge_map e;
	  int *err;

	  // Free a list of edge_maps.
{
  edge_map o;
  struct cru_destructor_pair_s r;

  memset (&r, 0, sizeof (r));
  for (r.e_free = (z ? z->e_free : NULL); (o = e);)
	 {
		_cru_free_edges_and_termini (z, e->ante, err);
		_cru_free_edges_and_termini (&r, e->post, err);
		e = e->next_map;
		_cru_free (o);
	 }
}






// --------------- editing ---------------------------------------------------------------------------------





void
_cru_pop_map (p, err)
	  edge_map *p;
	  int *err;

	  // Remove first edge map from a list.
{
  edge_map o;

  if (*err ? 1 : (! p) ? IER(873) : ((o = *p)) ? 0 : IER(874))
	 return;
  *p = (*p)->next_map;
  o->next_map = NULL;
  _cru_free_map (NO_DESTRUCTORS, o, err);
}
