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

#ifndef CRU_MAYBE_H
#define CRU_MAYBE_H 1

#include "ptr.h"

// x parameter values to _cru_new_maybe
#define ABSENT 0
#define PRESENT 1

// default parameter to _cru_free_maybe
#define NO_DESTRUCTOR NULL

// representation for a possible value

struct maybe_s
{
  void *value;
  int ma_status;          // an error code caused by attempting to compute the value
  int extant;             // if non-zero, the value has been assigned
};

// representation for a pair of possible values and a way to combine them

struct maybe_pair_s
{
  maybe x;
  maybe y;
  void *s;                // user defined storage
  cru_bop r;              // the way to combine them
  cru_destructor d;       // the way to reclaim them
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- memory management -----------------------------------------------------------------------

// allocate a new maybe type and consume the error
extern maybe
_cru_new_maybe (int x, void *v, int *err);

// deallocate the storage used by a 
extern void
_cru_free_maybe (maybe x, cru_destructor d, int *err);

// reclaim the storage associated with a maybe pair
extern void
_cru_free_maybe_pair (maybe_pair p, int *err);

// --------------- combination -----------------------------------------------------------------------------

// make a pair of maybe types
extern maybe_pair
_cru_maybe_paired (maybe *x, maybe y, cru_bop r, cru_destructor d, void *s, int *err);

// reduce a pair of maybe types to a single one
extern maybe
_cru_maybe_fused (maybe_pair p, int *err);

#ifdef __cplusplus
}
#endif
#endif
