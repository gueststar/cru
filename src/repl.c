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

#include "errs.h"
#include "repl.h"




// --------------- replacement hash functions --------------------------------------------------------------
 



uintptr_t
_cru_undefined_hash (v)
	  void *v;

	  // Complain and return no hash at all.
{
  _cru_globally_throw (THE_IER(1435));
  return 0;
}




uintptr_t
_cru_scalar_hash (n)
	  void *n;

	  // Mix up a pointer based on murmur3.
{
#define BIG_CONSTANT(x) (x##LLU)

  uintptr_t k;

  k = ((uintptr_t) n) + 1;
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;
  return k;
}




// --------------- replacement binary predicates -----------------------------------------------------------



int
_cru_undefined_bpred (x, y, err)
	  void *x;
	  void *y;
	  int *err;

	  // Always raise an error and return zero.
{
  IER(1436);
  return 0;
}





int
_cru_false_bpred (l, r, err)
	  void *l;
	  void *r;
	  int *err;

	  // Unconditionally return zero.
{
  return 0;
}





int
_cru_true_bpred (l, r, err)
	  void *l;
	  void *r;
	  int *err;

	  // Unconditionally return a non-zero value.
{
  return 1;
}






int
_cru_equality_bpred (x, y, err)
	  void *x;
	  void *y;
	  int *err;

	  // Compare two pointers and return non-zero if equal.
{
  return (x == y);
}






// --------------- replacement ternary predicates ----------------------------------------------------------




int
_cru_true_tpred (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Unconditionally return a non-zero value.
{
  return 1;
}





// --------------- replacement quarternary predicates ------------------------------------------------------




int
_cru_true_qpred (w, x, y, z, err)
	  void *w;
	  void *x;
	  void *y;
	  void *z;
	  int *err;

	  // Unconditionally return a non-zero value.
{
  return 1;
}




// --------------- replacement nullary operators -----------------------------------------------------------





void *
_cru_undefined_nop (err)
	  int *err;

	  // Unconditionally raise an error.
{
  IER(1437);
  return NULL;
}




 

// --------------- replacement unary operators -------------------------------------------------------------




void *
_cru_identity_uop (x, err)
	  void *x;
	  int *err;

	  // Unconditionally return the argument. Because this function
	  // consumes its argument, it's appropriate only as a replacement
	  // for an operator on scalar properties or endogenous vertices or
	  // edges.
{
  return x;
}




// --------------- replacement binary operators ------------------------------------------------------------






void *
_cru_undefined_bop (x, y, err)
	  void *x;
	  void *y;
	  int *err;

	  // Raise an error, consume both operands and return NULL.
{
  cru_destructor d;

  IER(1438);
  return NULL;
}





void *
_cru_identity_bop (l, r, err)
	  void *l;
	  void *r;
	  int *err;

	  // Return the left argument. Because this function consumes its
	  // left argument, it's appropriate only as a replacement for an
	  // operator on scalar properties or endogenous vertices or edges.
{
  return l;
}








void *
_cru_minimizing_bop (x, y, err)
	  void *x;
	  void *y;
	  int *err;

	  // Return the lesser of two pointers cast to unsigned integers.
{
  return (((uintptr_t) x < (uintptr_t) y) ? x : y);
}







// --------------- replacement ternary operators -----------------------------------------------------------






void *
_cru_identity_top (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Return the middle argument. Because this function consumes
	  // one of its arguments, it's appropriate only as a replacement
	  // for an operator on scalar properties or endogenous vertices or
	  // edges.
{
  return m;
}





void *
_cru_undefined_top (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Unconditionally raise an internal error and return NULL.
{
  IER(1439);
  return NULL;
}





void *
_cru_undefined_map (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Unconditionally raise an undefined map error and return NULL.
{
  RAISE(CRU_UNDMAP);
  return NULL;
}







void *
_cru_true_top (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Return a non-null value.
{
  return NON_NULL;
}





void *
_cru_false_top (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Return a null value.
{
  return NULL;
}






void *
_cru_disjoining_top (l, m, r, err)
	  void *l;
	  void *m;
	  void *r;
	  int *err;

	  // Return the disjunction of the left and right operands.
{
  return (l ? l : r);
}



// --------------- replacement quarternary operators -------------------------------------------------------



void *
_cru_identity_qop (w, x, y, z, err)
	  void *w;
	  void *x;
	  void *y;
	  void *z;
	  int *err;

	  // Unconditionally return the second argument x. Because this
	  // function consumes one of its arguments, it's appropriate only
	  // as a replacement for an operator on scalar properties or
	  // endogenous vertices or edges.
{
  return x;
}

