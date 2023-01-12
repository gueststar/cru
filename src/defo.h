/*
  cru -- co-recursion utilities

  copyright (c) 2022-2023 Dennis Furey

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

// This file declares initialization helper functions and diagnostics
// enabling sensible defaults for unspecified fields in user-defined
// parameters to API calls. See infer.c for the call sites.

#ifndef CRU_DEFO_H
#define CRU_DEFO_H 1

#include "ptr.h"

// allow y as the default value of x if x is NULL
#define ALLOW(x,y) x = (x ? x : y)

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- predicates ------------------------------------------------------------------------------

// return non-zero if an ordering specifies no order
extern int
_cru_empty_order (cru_order o);

// raise an error unless all fields in an order are already defined
extern int
_cru_full_order (cru_order o, int *err);

// return non-zero if a fold specifies no operations
extern int
_cru_empty_fold (cru_fold m);

// return non-zero if a prop specifies no folds
extern int
_cru_empty_prop (cru_prop p);

// return non-zero if a classifier specifies no prop or ordering
extern int
_cru_empty_classifier (cru_classifier c);

// --------------- allowances ------------------------------------------------------------------------------

// assign functions to a fold not intended to be used
extern void
_cru_allow_undefined_fold (cru_fold f, int *err);

// assign order fields not intended to be used
extern void
_cru_allow_undefined_order (cru_order o, int *err);

// assign order fields suitable for endogenous operands
extern void
_cru_allow_scalar_order (cru_order o, int *err);

// overwrite empty fields in o with corresponding fields in n
extern void
_cru_allow_order (cru_order o, cru_order n, int *err);

// --------------- intiializers ----------------------------------------------------------------------------

// confirm that a fold has been initialized with default fields consistent with its destructors
extern int
_cru_filled_fold (cru_fold m, int *err);

// confirm that a ctops has been initialized with default fields
extern int
_cru_filled_ctops (cru_ctop_pair f, int *err);

// infer a fold as above where only the map is required
extern int
_cru_filled_map (cru_fold f, int *err);

// return non-zero if replacement functions are reasonably assigned to a partially specified prop
extern int
_cru_filled_prop (cru_prop p, int *err);

// assign default functions to a sig where they can be reasonably inferred and return non-zero if successful
extern int
_cru_filled_sig (cru_sig s, int *err);

// return non-zero if replacement functions are successfully assigned to a partially specified classifer
extern int
_cru_filled_classifier (cru_classifier c, int *err);

#ifdef __cplusplus
}
#endif
#endif
