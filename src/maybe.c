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
#include "maybe.h"
#include "wrap.h"




// --------------- memory management -----------------------------------------------------------------------





maybe
_cru_new_maybe (x, v, err)
	  int x;
	  void *v;
	  int *err;

	  // Allocate a new maybe and consume the error.
{
  maybe m;

  if ((m = (maybe) _cru_malloc (sizeof (*m))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (m, 0, sizeof (*m));
  m->ma_status = *err;
  m->extant = x;
  m->value = v;
  *err = 0;
  return m;
}







void
_cru_free_maybe (x, d, err)
	  maybe x;
	  cru_destructor d;
	  int *err;

	  // Deallocate the storage used by a maybe.
{
  int ux;
  void *ua;

  if (! x)
	 return;
  if ((! d) ? NULL : x->extant ? x->value : NULL)
	 APPLY(d, x->value);
  x->value = NULL;
  RAISE(x->ma_status);
  x->ma_status = THE_IER(1110);
  _cru_free (x);
}








void
_cru_free_maybe_pair (p, err)
	  maybe_pair p;
	  int *err;

	  // Reclaim the storage associated with a maybe pair.
{
  if (! p)
	 return;
  _cru_free_maybe (p->x, p->d, err);
  _cru_free_maybe (p->y, p->d, err);
  _cru_free (p);
}





// --------------- combination -----------------------------------------------------------------------------







maybe_pair
_cru_maybe_paired (x, y, r, d, err)
	  maybe *x;
	  maybe y;
	  cru_bop r;
	  cru_destructor d;
	  int *err;

	  // Make a pair of maybe types. Discard either operand if it's
	  // null or not extant but observe its status code if
	  // possible. Return null if both are discarded.
{
  maybe_pair p;

  if (x ? 0 : IER(1111))
	 goto a;
  RAISE(*x ? (*x)->ma_status : y ? y->ma_status : 0);
  if ((! r) ? IER(1112) : (*x ? (*x)->extant : 0) ? 0 : y ? y->extant : 0)
	 goto b;
  if ((p = (maybe_pair) _cru_malloc (sizeof (*p))) ? 0 : RAISE(ENOMEM))
	 goto b;
  memset (p, 0, sizeof (*p));
  if (!((*x ? (*x)->extant : 0) ? (p->x = *x) : NULL))
	 _cru_free_maybe (*x, d, err);
  if (!((y ? y->extant : 0) ? (p->y = y) : NULL))
	 _cru_free_maybe (y, d, err);
  *x = NULL;
  p->d = d;
  p->r = r;
  return p;
 b: _cru_free_maybe (*x, d, err);
  *x = NULL;
 a: _cru_free_maybe (y, d, err);
  return NULL;
}
 








maybe
_cru_maybe_fused (p, err)
	  maybe_pair p;
	  int *err;

	  // Reduce a pair of maybe types to a single one. Assume based on
	  // the invariants in _cru_maybe_paired that being non-null
	  // implies existence.
{
  maybe m;
  void *v;
  void *ua;
  int ux;

  if (! p)
	 return NULL;
  if ((m = (p->x ? p->x : p->y)) ? (!(p->x ? p->y : NULL)) : 1)
	 goto a;
  if ((p->r ? 0 : IER(1113)) ? (! (m = NULL)) : 0)
	 goto b;
  v = (*err ? NULL : APPLIED(p->r, p->x->value, p->y->value));
  if (! (p->d))
	 goto c;
  if (p->x->value)
	 APPLY(p->d, p->x->value);
  if (p->y->value)
	 APPLY(p->d, p->y->value);
  p->x->value = p->y->value = NULL;
 c: m->value = v;
  m->ma_status = (p->x->ma_status ? p->x->ma_status : p->y->ma_status ? p->y->ma_status : *err);
  *((m == p->x) ? &(p->y->value) : &(p->x->value)) = NULL;
 a: *((m == p->x) ? &(p->x) : &(p->y)) = NULL;
 b: _cru_free_maybe_pair (p, err);
  return m;
}
