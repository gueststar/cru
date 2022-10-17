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

// replacement functions substituted as defaults where appropriate for
// unspecified functions in user-defined parameters to API calls

#ifndef CRU_REPL_H
#define CRU_REPL_H 1

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- replacement hash functions --------------------------------------------------------------

// always throw an error and return zero
extern uintptr_t
_cru_undefined_hash (void *v);

// quick hash function for mixing up pointer-sized values
extern uintptr_t
_cru_scalar_hash (void *n);

// --------------- replacement binary predicates -----------------------------------------------------------

// always raise an error and return zero
extern int
_cru_undefined_bpred (void *x, void *y, int *err);

// unconditionally return a zero
extern int
_cru_false_bpred (void *l, void *r, int *err);

// unconditionally return a non-zero value
extern int
_cru_true_bpred (void *l, void *r, int *err);

// compare two pointers and return non-zero if equal
extern int
_cru_equality_bpred (void *x, void *y, int *err);

// --------------- replacement ternary predicates ----------------------------------------------------------

// unconditionally return a non-zero value
extern int
_cru_true_tpred (void *l, void *m, void *r, int *err);

// --------------- replacement quarternary predicates ------------------------------------------------------

// unconditionally return a non-zero value
extern int
_cru_true_qpred (void *w, void *x, void *y, void *z, int *err);

// --------------- replacement nullary operators -----------------------------------------------------------

// unconditionally raise an error
extern void *
_cru_undefined_nop (int *err);

// --------------- replacement unary operators -------------------------------------------------------------

// unconditionally return the argument
extern void *
_cru_identity_uop (void *x, int *err);

// --------------- replacement binary operators ------------------------------------------------------------

// raise an error and return NULL
extern void *
_cru_undefined_bop (void *x, void *y, int *err);

// return the left argument
extern void *
_cru_identity_bop (void *l, void *r, int *err);

// return the lesser of two pointers cast to unsigned integers
extern void *
_cru_minimizing_bop (void *x, void *y, int *err);

// --------------- replacement ternary operators -----------------------------------------------------------

// unconditionally raise an error and return NULL
extern void *
_cru_undefined_top (void *l, void *m, void *r, int *err);

// return the middle argument
extern void *
_cru_identity_top (void *l, void *m, void *r, int *err);

// return a non-null value
extern void *
_cru_true_top (void *l, void *m, void *r, int *err);

// return a null value
extern void *
_cru_false_top (void *l, void *m, void *r, int *err);

// return the disjunction of the left and right operands
extern void *
_cru_disjoining_top (void *l, void *m, void *r, int *err);

// --------------- replacement quarternary operators -------------------------------------------------------

// unconditionally return the argument x
extern void *
_cru_identity_qop (void *w, void *x, void *y, void *z, int *err);

#ifdef __cplusplus
}
#endif
#endif
