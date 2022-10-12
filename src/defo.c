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

#include "defo.h"
#include "errs.h"
#include "repl.h"
#include "wrap.h"

// --------------- predicates ------------------------------------------------------------------------------



int
_cru_empty_order (o)
	  cru_order o;

	  // Return non-zero if an ordering specifies no order.
{
  if (! o)
	 return 1;
  if (o->hash ? (o->hash != (cru_hash) _cru_undefined_hash) : 0)
	 return 0;
  if (o->equal ? (o->equal != (cru_bpred) _cru_undefined_bpred) : 0)
	 return 0;
  return 1;
}





int
_cru_full_order (o, err)
	  cru_order o;
	  int *err;

	  // Raise an error unless all fields in an order are already
	  // defined.
{
  if (o ? 0 : IER(805))
	 return 0;
  if ((o->hash ? (o->hash == (cru_hash) _cru_undefined_hash) : 1) ? RAISE(CRU_UNDHSH) : 0)
	 return 0;
  if ((o->equal ? (o->equal == (cru_bpred) _cru_undefined_bpred) : 1) ? RAISE(CRU_UNDEQU) : 0)
	 return 0;
  return 1;
}






int
_cru_empty_fold (m)
	  cru_fold m;

	  // Return non-zero if a map-reduction specifies no operations.
{
  if (! m)
	 return 1;
  if (m->map ? ((m->map != (cru_top) _cru_undefined_top) ? (m->map != (cru_top) _cru_disjoining_top) : 0) : 0)
	 return 0;
  if (m->reduction ? (m->reduction != (cru_bop) _cru_undefined_bop) : 0)
	 return 0;
  if (m->vacuous_case ? (m->vacuous_case != (cru_nop) _cru_undefined_nop) : 0)
	 return 0;
  return (m->r_free ? 0 : ! (m->m_free));
}








int
_cru_empty_prop (p)
	  cru_prop p;

	  // Return non-zero if a prop specifies no operation.
{
  if (! p)
	 return 1;
  if (! _cru_empty_fold (&(p->vertex)))
	 return 0;
  if (! _cru_empty_fold (&(p->incident)))
	 return 0;
  if (! _cru_empty_fold (&(p->outgoing)))
	 return 0;
  return 1;
}




int
_cru_empty_classifier (c)
	  cru_classifier c;

	  // Return non-zero if a classifier specifies no prop or ordering.
{
  return ((! c) ? 1 : (! _cru_empty_prop (&(c->cl_prop))) ? 0 : _cru_empty_order (&(c->cl_order)));
}




// --------------- allowances ------------------------------------------------------------------------------




void
_cru_allow_undefined_fold (f, err)
	  cru_fold f;
	  int *err;

	  // Assign functions to a fold not intended to be used.
{
  if (f ? 0 : IER(806))
	 return;
  if (! (f->m_free))
	 ALLOW (f->map, (cru_top) _cru_undefined_top);
  if (! (f->r_free))
	 ALLOW (f->reduction, (cru_bop) _cru_undefined_bop);
}









void
_cru_allow_undefined_order (o, err)
	  cru_order o;
	  int *err;

	  // Assign order fields not intended to be used.
{
  if (o ? 0 : IER(807))
	 return;
  ALLOW (o->hash, (cru_hash) _cru_undefined_hash);
  ALLOW (o->equal, (cru_bpred) _cru_undefined_bpred);
}







void
_cru_allow_scalar_order (o, err)
	  cru_order o;
	  int *err;

	  // Assign order fields suitable for endogenous operands.
{
  if (o ? 0 : IER(808))
	 return;
  ALLOW (o->hash, (cru_hash) _cru_scalar_hash);
  ALLOW (o->equal, (cru_bpred) _cru_equality_bpred);
}






void
_cru_allow_order (o, n, err)
	  cru_order o;
	  cru_order n;
	  int *err;

	  // Overwrite empty fields in o with corresponding fields in n.
{
  if ((! n) ? IER(809) : o ? 0 : IER(810))
	 return;
  ALLOW (o->hash, n->hash);
  ALLOW (o->equal, n->equal);
}





// --------------- initializers ----------------------------------------------------------------------------






int
_cru_filled_fold (m, err)
	  cru_fold m;
	  int *err;

	  // Use default functions in a fold where possible. If there are
	  // no respective destructors, then the map and reduction
	  // operators are assumed not to operate on pointers. The default
	  // map in that case is the identity function, which returns its
	  // middle operand, and the default reduction picks the lesser one
	  // by unsigned integer comparison. Where there is a destructor,
	  // there must be a corresponding user-defined map or reduction
	  // producing results of a type consumed by that destructor. If
	  // there's no vacuous case, then the map and the reduction must
	  // have compatible result types, as indicated by having identical
	  // destructors.
{
  if (m ? 0 : IER(811))
	 return 0;
  if (m->m_free ? 0 : ! (m->r_free))
	 ALLOW (m->map, (cru_top) _cru_identity_top);
  else if (m->map ? 0 : RAISE(CRU_UNDMAP))
	 return 0;
  if (m->r_free ? 0 : ! (m->m_free))
	 ALLOW (m->reduction, (cru_bop) _cru_minimizing_bop);
  else if (m->reduction ? 0 : RAISE(CRU_UNDRED))
	 return 0;
  if (m->map == (cru_top) _cru_undefined_top)
	 m->m_free = m->r_free;
  if (m->reduction == (cru_bop) _cru_undefined_bop)
	 m->r_free = m->m_free;
  if (m->vacuous_case ? (m->vacuous_case != (cru_nop) _cru_undefined_nop) : 0)
	 return 1;
  if ((m->m_free == m->r_free) ? 0 : RAISE(CRU_TPCMPR))
	 return 0;
  return 1;
}







static int
filled_ctop (p, err)
	  cru_ctop p;
	  int *err;

	  // Check for a defined mark and allow a default always true
	  // condition in a ctop.
{
  if (p ? 0 : IER(812))
	 return 0;
  if (p->top ? 0 : RAISE(CRU_UNDCTO))
	 return 0;
  ALLOW (p->tpred, (cru_tpred) _cru_true_tpred);
  return 1;
}






int
_cru_filled_ctops (f, err)
	  cru_ctop_pair f;
	  int *err;

	  // Confirm that a ctops can be initialized with default fields.
{
  if (f ? 0 : IER(813))
	 return 0;
  return (filled_ctop (&(f->ana_labeler), err) ? filled_ctop (&(f->cata_labeler), err) : 0);
}






int
_cru_filled_map (f, err)
	  cru_fold f;
	  int *err;

	  // Infer a fold as above where only the map is required.
{
  if (f ? 0 : IER(814))
	 return 0;
  ALLOW (f->reduction, (cru_bop) _cru_undefined_bop);
  return _cru_filled_fold (f, err);
}







int
_cru_filled_prop (p, err)
	  cru_prop p;
	  int *err;

	  // Return non-zero if replacement functions are assigned to a
	  // partially specified prop. Either of the incident or outgoing
	  // edge map reductions is allowed to be empty, in which case it
	  // is ignored.
{
  if (p ? 0 : IER(815))
	 return 0;
  if (_cru_empty_fold (&(p->incident)) ? 0 : ! _cru_filled_fold (&(p->incident), err))
	 return 0;
  if (_cru_empty_fold (&(p->outgoing)) ? 0 : ! _cru_filled_fold (&(p->outgoing), err))
	 return 0;
  return _cru_filled_fold (&(p->vertex), err);
}








int
_cru_filled_sig (s, err)
	  cru_sig s;
	  int *err;

	  // Use default functions in a sig where possible. If there is no
	  // user-defined vertex destructor, then default hashes and
	  // relations can be used for vertices because the graph
	  // presumably uses an endogenous representation for vertices. The
	  // same goes for edges. If there is a user-defined destructor in
	  // either case, then the corresponding hashes and relations must
	  // also be defined by the user.
{
  if (s ? 0 : IER(816))
	 return 0;
  if (! (s->destructors.e_free))                                                   // if edge labels are endogenous
	 _cru_allow_scalar_order (&(s->orders.e_order), err);
  else if (! _cru_full_order (&(s->orders.e_order), err))
	 return 0;
  if (s->destructors.v_free)                                                       // if vertices are endogenous
	 return _cru_full_order (&(s->orders.v_order), err);
  _cru_allow_scalar_order (&(s->orders.v_order), err);
  return 1;
}






int
_cru_filled_classifier (c, err)
	  cru_classifier c;
	  int *err;

	  // Return non-zero if replacement functions are assigned to a
	  // partially specified classifer.
{
  if ((! c) ? IER(817) : 0)
	 return 0;
  ALLOW (c->cl_prop.vertex.reduction, (cru_bop) _cru_undefined_bop);
  if (! _cru_filled_prop (&(c->cl_prop), err))
	 return 0;
  if (! (c->cl_prop.vertex.m_free))
 	 _cru_allow_scalar_order (&(c->cl_order), err);
  if (! _cru_full_order (&(c->cl_order), err))
	 return 0;
  return 1;
}
