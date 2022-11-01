/*
  Cru -- co-recursion utilities

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
#include "copy.h"
#include "defo.h"
#include "errs.h"
#include "infer.h"
#include "repl.h"
#include "wrap.h"





cru_builder
_cru_inferred_builder (b, v, err)
	  cru_builder b;
	  void *v;
	  int *err;

	  // Return a copy of the builder with replacement functions
	  // assigned. Vertex comparison is always needed for building, and
	  // edge comparison is needed for building with subconnectors.
{
  int ux, ut;
  cru_builder b_copy;

  if (! (b_copy = _cru_builder_copy (b, err)))
	 return NULL;
  if (b_copy->subconnector ? 0 : b_copy->connector ? 0 : RAISE(CRU_UNDCON))
	 goto a;
  if ((b_copy->connector ? b_copy->subconnector : NULL) ? RAISE(CRU_INCCON) : 0)
	 goto a;
  if (! (b_copy->bu_sig.destructors.v_free))
	 _cru_allow_scalar_order (&(b_copy->bu_sig.orders.v_order), err);
  if (! (b_copy->bu_sig.destructors.e_free))
	 _cru_allow_scalar_order (&(b_copy->bu_sig.orders.e_order), err);
  if (b_copy->connector)
	 ALLOW (b_copy->bu_sig.orders.e_order.hash, (cru_hash) _cru_undefined_hash);
  if (! _cru_full_order (&(b_copy->bu_sig.orders.v_order), err))
	 goto a;
  if (! _cru_full_order (&(b_copy->bu_sig.orders.e_order), err))
	 goto a;
  _cru_allow_undefined_order (&(b_copy->bu_sig.orders.e_order), err);
  if (b_copy->bu_sig.orders.v_order.equal ? 0 : IER(1036))
	 goto a;
  if (FAILED(b_copy->bu_sig.orders.v_order.equal, v, v) ? RAISE(CRU_INCVEL) : 0)
	 goto a;
  return b_copy;
 a: _cru_free_builder (b_copy);
  return NULL;
}






cru_crosser
_cru_inferred_crosser (c, err)
	  cru_crosser c;
	  int *err;

	  // Return a copy of the crosser with replacement functions
	  // assigned.
{
  cru_crosser c_copy;

  if (! (c_copy = _cru_crosser_copy (c, err)))
	 return NULL;
  ALLOW (c_copy->e_prod.bpred, (cru_bpred) _cru_true_bpred);
  if (c_copy->v_prod ? 0 : RAISE(CRU_UNDVPR))
	 goto a;
  if (c_copy->e_prod.bop ? 0 : RAISE(CRU_UNDEPR))
	 goto a;
  if (! (c_copy->cr_sig.destructors.e_free))
	 _cru_allow_scalar_order (&(c_copy->cr_sig.orders.e_order), err);
  _cru_allow_undefined_order (&(c_copy->cr_sig.orders.e_order), err);
  if (! (c_copy->cr_sig.destructors.v_free))
	 _cru_allow_scalar_order (&(c_copy->cr_sig.orders.v_order), err);
  if (_cru_full_order (&(c_copy->cr_sig.orders.v_order), err))
	 return c_copy;
 a: _cru_free_crosser (c_copy);
  return NULL;
}










cru_mapreducer
_cru_inferred_mapreducer (m, err)
	  cru_mapreducer m;
	  int *err;

	  // Return a copy of the mapreducer with replacement functions assigned.
{
  cru_mapreducer m_copy;

  if (! (m_copy = _cru_mapreducer_copy (m, err)))
	 return NULL;
  if (_cru_filled_prop (&(m_copy->ma_prop), err))
	 if ((m_copy->ma_prop.vertex.m_free == m_copy->ma_prop.vertex.r_free) ? 1 : ! RAISE(CRU_TPCMPR))
		return m_copy;
  _cru_free_mapreducer (m_copy);
  return NULL;
}









cru_inducer
_cru_inferred_inducer (i, err)
	  cru_inducer i;
	  int *err;

	  // Return a copy of the inducer with replacement functions assigned.
{
  cru_inducer i_copy;

  if (! (i_copy = _cru_inducer_copy (i, err)))
	 return NULL;
  if (i_copy->boundary_value ? 0 : ! (i_copy->in_fold.vacuous_case))
	 RAISE(CRU_UNDVAC);
  else
	 {
		if (! (i_copy->in_fold.map))
		  ALLOW (i_copy->in_fold.reduction, (cru_bop) _cru_undefined_bop);
		ALLOW (i_copy->in_fold.map, (cru_top) _cru_undefined_map);
		if (_cru_filled_fold (&(i_copy->in_fold), err))
		  return i_copy;
	 }
  _cru_free_inducer (i_copy);
  return NULL;
}









cru_composer
_cru_inferred_composer (c, s, err)
	  cru_composer c;
	  cru_sig s;
	  int *err;

	  // Return a copy of the composer with replacement functions
	  // assigned. If the edge labels are endogenous, as indicated by a
	  // NULL destructor, then allow a default composition operation
	  // that unconditionally returns the local label.
{
  cru_composer c_copy;

  if ((! _cru_full_order (&(s->orders.e_order), err)) ? 1 : ! (c_copy = _cru_composer_copy (c, err)))
	 return NULL;
  ALLOW (c_copy->labeler.qpred, (cru_qpred) _cru_true_qpred);
  if (! (s->destructors.e_free))
	 ALLOW (c_copy->labeler.qop, (cru_qop) _cru_identity_qop);
  if (c_copy->labeler.qop ? 0 : RAISE(CRU_UNDCQO))
	 goto a;
  if (! _cru_filled_map (&(c_copy->co_props.adjacent.vertex), err))
	 goto a;
  if (! _cru_filled_prop (&(c_copy->co_props.adjacent), err))
	 goto a;
  if (_cru_filled_map (&(c_copy->co_props.local.vertex), err) ? _cru_filled_prop (&(c_copy->co_props.local), err) : 0)
	 return c_copy;
 a: _cru_free_composer (c_copy);
  return NULL;
}








cru_stretcher
_cru_inferred_stretcher (x, s, err)
	  cru_stretcher x;
	  cru_sig s;
	  int *err;

	  // Return a copy of the stretcher with replacement functions
	  // assigned.
{
  cru_stretcher x_copy;

  if ((! _cru_filled_sig (s, err)) ? 1 : ! (x_copy = _cru_stretcher_copy (x, err)))
	 return NULL;
  if (! (_cru_filled_map (&(x_copy->st_prop.vertex), err) ? _cru_filled_prop (&(x_copy->st_prop), err) : 0))
	 goto a;
  if (x_copy->expander ? 1 : ! RAISE(CRU_UNDEXP))
	 return x_copy;
 a: _cru_free_stretcher (x_copy);
  return NULL;
}








cru_splitter
_cru_inferred_splitter (x, s, err)
	  cru_splitter x;
	  cru_sig s;
	  int *err;

	  // Return a copy of the splitter with replacement functions
	  // assigned.
{
  cru_splitter x_copy;

  if ((! _cru_filled_sig (s, err)) ? 1 : (! s) ? IER(1037) : ! (x_copy = _cru_splitter_copy (x, err)))
	 return NULL;
  ALLOW (x_copy->fissile, (cru_bpred) _cru_true_bpred);
  if (! (_cru_filled_map (&(x_copy->sp_prop.vertex), err) ? _cru_filled_prop (&(x_copy->sp_prop), err) : 0))
	 goto a;
  if (! (s->destructors.v_free))
	 {
		ALLOW (x_copy->ana, (cru_bop) _cru_identity_bop);
		ALLOW (x_copy->cata, (cru_bop) _cru_identity_bop);
	 }
  if ((! (x_copy->ana)) ? RAISE(CRU_UNDANA) : (! (x_copy->cata)) ? RAISE(CRU_UNDCAT) : 0)
	 goto a;
  if (! (s->destructors.e_free))
	 {
		ALLOW (x_copy->sp_ctops.inward.ana_labeler.top, (cru_top) _cru_identity_top);
		ALLOW (x_copy->sp_ctops.inward.cata_labeler.top, (cru_top) _cru_identity_top);
		ALLOW (x_copy->sp_ctops.outward.ana_labeler.top, (cru_top) _cru_identity_top);
		ALLOW (x_copy->sp_ctops.outward.cata_labeler.top, (cru_top) _cru_identity_top);
	 }
  if (_cru_filled_ctops (&(x_copy->sp_ctops.inward), err) ? _cru_filled_ctops (&(x_copy->sp_ctops.outward), err) : 0)
	 return x_copy;
 a: _cru_free_splitter (x_copy);
  return NULL;
}






cru_classifier
_cru_inferred_classifier (c, err)
	  cru_classifier c;
	  int *err;

	  // Return a copy of the classifier with replacement functions
	  // assigned.
{
  cru_classifier c_copy;

  if ((! c) ? RAISE(CRU_NULCSF) : ! (c_copy = _cru_classifier_copy (c, err)))
	 return NULL;
  if (_cru_filled_classifier (c_copy, err))
	 return c_copy;
  _cru_free_classifier (c_copy);
  return NULL;
}








static cru_merger
inferred_merger (c, s, err)
	  cru_merger c;
	  cru_sig s;       // original graph sig to be updated
	  int *err;

	  // Return a copy of the merger with replacement functions
	  // assigned based on a partially specified merger. If the given
	  // merger has no vertex kernel prop and no class order, then all
	  // vertices are considered to be in a class of their own. If
	  // there is an edge predicate, then there must also be an edge
	  // ordering.
{
  cru_merger c_copy;

  if ((! s) ? IER(1038) : ! (c_copy = _cru_merger_copy (c, err)))
	 return NULL;
  if (_cru_empty_prop (&(c_copy->me_kernel.v_op)))                                // if vertices are not to be fused
	 {
		_cru_allow_order (&(c_copy->me_orders.v_order), &(s->orders.v_order), err); // keep the original vertex ordering
		ALLOW (c_copy->me_kernel.v_op.vertex.map, (cru_top) _cru_undefined_top);    // keep each vertex the way it is
		ALLOW (c_copy->me_kernel.v_op.vertex.r_free, s->destructors.v_free);
	 }
  else if (! _cru_filled_prop (&(c_copy->me_kernel.v_op), err))
	 goto a;
  else
	 {                                                                             // otherwise update it
		s->destructors.v_free = c_copy->me_kernel.v_op.vertex.r_free;
		memcpy (&(s->orders.v_order), &(c_copy->me_orders.v_order), sizeof (s->orders.v_order));
	 }
  if (_cru_empty_fold (&(c_copy->me_kernel.e_op)))                                // if edges are not to be fused
	 _cru_allow_order (&(c_copy->me_orders.e_order), &(s->orders.e_order), err);   // keep the original edge ordering
  else if (! _cru_filled_fold (&(c_copy->me_kernel.e_op), err))
	 goto a;
  else
	 {                                                                             // otherwise update it
		s->destructors.e_free = c_copy->me_kernel.e_op.r_free;
		memcpy (&(s->orders.e_order), &(c->me_orders.e_order), sizeof (s->orders.e_order));
	 }
  if (! (c_copy->pruner))                                                         // if edges are not to be filtered
	 _cru_allow_undefined_order (&(c_copy->me_orders.e_order), err);               // require no edge ordering
  else if (! _cru_full_order (&(c_copy->me_orders.e_order), err))                 // otherwise require one
	 goto a;
  if (_cru_empty_classifier (&(c_copy->me_classifier)))                           // if no classification is defined
	 {                                                                             // make all vertices mutually unrelated
		ALLOW (c_copy->me_classifier.cl_order.hash, (cru_hash) _cru_scalar_hash);   // make each vertex its own class property
		ALLOW (c_copy->me_classifier.cl_order.equal, (cru_bpred) _cru_false_bpred);
		if (! _cru_filled_map (&(c_copy->me_classifier.cl_prop.vertex), err))
		  goto a;
		if (! _cru_filled_prop (&(c_copy->me_classifier.cl_prop), err))
		  goto a;
	 }
  if (_cru_filled_classifier (&(c_copy->me_classifier), err))
	 return c_copy;
 a: _cru_free_merger (c_copy);
  return NULL;
}









cru_merger
_cru_inferred_merger (c, s, err)
	  cru_merger c;
	  cru_sig s;
	  int *err;

	  // as above but with extra validation not applicable to
	  // deduplicators
{
  if ((! c) ? IER(1039) : 0)
	 return NULL;
  if ((_cru_empty_prop (&(c->me_kernel.v_op)) == _cru_empty_classifier (&(c->me_classifier))) ? 0 : RAISE(CRU_INCMRG))
	 return NULL;
  return inferred_merger (c, s, err);
}









static int
inferred_sig (s, o, k, err)
	  cru_sig s;
	  cru_order_pair o;
	  cru_kernel k;
	  int *err;

	  // Modify the sig s to correspond to the mutated form of the
	  // graph described by s.
{
  struct cru_sig_s n;

  if ((! s) ? IER(1040) : (! o) ? IER(1041) : (! k) ? IER(1042) : 0)
	 return 0;
  memset (&n, 0, sizeof (n));
  memcpy (&(n.destructors), &(s->destructors), sizeof (n.destructors));
  _cru_allow_order (&(n.orders.e_order), &(o->e_order), err);
  _cru_allow_order (&(n.orders.v_order), &(o->v_order), err);
  if (_cru_empty_fold (&(k->e_op)) ? 1 : (k->e_op.m_free == s->destructors.e_free))
	 _cru_allow_order (&(n.orders.e_order), &(s->orders.e_order), err);
  else
	 n.destructors.e_free = k->e_op.m_free;
  if (_cru_empty_prop (&(k->v_op)) ? 1 : (k->v_op.vertex.m_free == s->destructors.v_free))
	 _cru_allow_order (&(n.orders.v_order), &(s->orders.v_order), err);
  else
	 n.destructors.v_free = k->v_op.vertex.m_free;
  if (! (n.destructors.e_free))
	 _cru_allow_scalar_order (&(n.orders.e_order), err);
  if (*err)
	 return 0;
  memcpy (s, &n, sizeof (*s));
  return 1;
}







cru_mutator
_cru_inferred_mutator (m, s, err)
	  cru_mutator m;
	  cru_sig s;
	  int *err;

	  // Return a copy of the mutator with replacement functions
	  // assigned based on a partially specified mutator. The traversal
	  // order can be local first, remote first, or unconstrained, but
	  // not both. If the traversal order is unconstrained, then at
	  // most one of a vertex mutation or an edge mutation is allowed,
	  // and the vertex mutation is not allowed any incident or
	  // outgoing edge folds.
	  //
	  // These rules are necessary because mutating edges and vertices
	  // on the same pass in an unconstrained order would otherwise
	  // non-deterministically expose an inconsistent view of their
	  // adjacent vertices or edges to the kernels.
{
  cru_mutator m_copy;

  if (! (m_copy = _cru_mutator_copy (m, err)))
	 goto a;
  if (! inferred_sig (s, &(m_copy->mu_orders), &(m_copy->mu_kernel), err))
	 goto b;
  if (m_copy->mu_plan.local_first ? m_copy->mu_plan.remote_first : 0)   // contradictory orders
	 goto c;
  if (_cru_empty_prop (&(m_copy->mu_kernel.v_op)))
	 goto d;
  if (m_copy->mu_plan.remote_first)
	 goto e;
  if (m_copy->mu_plan.local_first)
	 goto e;
  if (! _cru_empty_fold (&(m_copy->mu_kernel.e_op)))
	 goto c;
  if (! _cru_empty_fold (&(m_copy->mu_kernel.v_op.incident)))
	 goto c;
  if (! _cru_empty_fold (&(m_copy->mu_kernel.v_op.outgoing)))
	 goto c;
 e: if (! (_cru_filled_map (&(m_copy->mu_kernel.v_op.vertex), err) ? _cru_filled_prop (&(m_copy->mu_kernel.v_op), err) : 0))
	 goto b;
 d: ALLOW (m_copy->mu_kernel.e_op.reduction, (cru_bop) _cru_undefined_bop);
  if (_cru_empty_fold (&(m_copy->mu_kernel.e_op)) ? 0 : ! _cru_filled_map (&(m_copy->mu_kernel.e_op), err))
	 goto b;
  if (_cru_empty_fold (&(m_copy->mu_kernel.v_op.incident)) ? 0 : ! (m_copy->mu_kernel.v_op.incident.vacuous_case))
	 goto f;
  if (! (_cru_empty_fold (&(m_copy->mu_kernel.v_op.outgoing)) ? 0 : ! m_copy->mu_kernel.v_op.outgoing.vacuous_case))
	 goto g;
 f: RAISE(CRU_UNDVAC);
  goto b;
 g: return m_copy;
 c: RAISE(CRU_INCMUT);
 b: _cru_free_mutator (m_copy);
 a: return NULL;
}











cru_filter
_cru_inferred_filter (f, s, err)
	  cru_filter f;
	  cru_sig s;
	  int *err;

	  // Return a copy of a partially specified filter with replacement
	  // functions are assigned. A filter uses a prop to test the
	  // vertices, a fold to test the edges, and optionally an edge
	  // comparison order if an edge test is specified.
	  //
	  // If the prop is empty, then the filter calls for all vertices
	  // in the graph to be retained. If the prop is not empty, then
	  // it must specify at least one of a vertex, incident, or
	  // outgoing edge fold. The reduction in the vertex fold is never
	  // needed.
	  //
	  // If the edge test fold is not specified and no thinner is
	  // specified, then the filter calls for all edges in the graph to
	  // be retained. If the edge test is specified, then its reduction
	  // is never needed.
	  //
	  // If an edge test and an order are specified, only the order's
	  // equality relation and hash are needed, which are used for
	  // detecting equivalence classes of edges tested jointly. If no
	  // edge test is specified, then no ordering is needed.
	  //
	  // If a thinner is specified with no edge test, an edge test
	  // whose map always returns non-null is inferred, meaning that
	  // edges are removed base only on the thinner. If a thinner is is
	  // specified with no vertex test, a vertex test whose map always
	  // return non-null is inferred, meaning that edges from all
	  // vertices in the zone are subject to removal.
{
  cru_filter f_copy;

  if ((! s) ? IER(1043) : ! (f_copy = _cru_filter_copy (f, err)))
	 return NULL;
  if (f_copy->thinner ? _cru_empty_prop (&(f_copy->fi_kernel.v_op)) : 0)
	 ALLOW (f_copy->fi_kernel.v_op.vertex.map, (cru_top) _cru_false_top);
  ALLOW (f_copy->fi_kernel.v_op.vertex.map, (cru_top) _cru_disjoining_top);
  ALLOW (f_copy->fi_kernel.v_op.vertex.reduction, (cru_bop) _cru_undefined_bop);
  if (_cru_empty_prop (&(f_copy->fi_kernel.v_op)) ? 0 : ! _cru_filled_prop (&(f_copy->fi_kernel.v_op), err))
	 goto a;
  if (f_copy->thinner ? _cru_empty_fold (&(f_copy->fi_kernel.e_op)) : 0)
	 ALLOW (f_copy->fi_kernel.e_op.map, (cru_top) _cru_true_top);
  if (! _cru_empty_order (&(f_copy->fi_order)))
	 {
		_cru_allow_order (&(f_copy->fi_order), &(s->orders.e_order), err);
		if (! (s->destructors.e_free))
		  _cru_allow_scalar_order (&(f_copy->fi_order), err);
		if (! _cru_full_order (&(f_copy->fi_order), err))
		  goto a;		
		if (_cru_empty_fold (&(f_copy->fi_kernel.e_op)) ? RAISE(CRU_INCFIL) : 0)
		  goto a;
		if (! _cru_filled_fold (&(f_copy->fi_kernel.e_op), err))
		  goto a;
	 }
  if (_cru_empty_fold (&(f_copy->fi_kernel.e_op)) ? 1 : _cru_filled_fold (&(f_copy->fi_kernel.e_op), err))
	 return f_copy;
 a: _cru_free_filter (f_copy);
  return NULL;
}













cru_merger
_cru_deduplicator (s, err)
	  cru_sig s;
	  int *err;

	  // Construct a merger for deduplicating a graph based on the
	  // sig used to build it.
{
  struct cru_merger_s c;

  if (*err ? 1 : s ? 0 : IER(1044))
	 goto a;
  memset (&c, 0, sizeof (c));
  memcpy (&(c.me_orders), &(s->orders), sizeof (c.me_orders));
  c.me_classifier.cl_prop.vertex.map = (cru_top) _cru_identity_top;
  c.me_kernel.v_op.vertex.map = (cru_top) _cru_undefined_top;
  memcpy (&(c.me_classifier.cl_order), &(s->orders.v_order), sizeof (c.me_classifier.cl_order));
  if (_cru_full_order (&(s->orders.e_order), err) ? _cru_full_order (&(s->orders.v_order), err) : 0)
	 return inferred_merger (&c, s, err);
 a: return NULL;
}












cru_postponer
_cru_inferred_postponer (p, s, err)
	  cru_postponer p;
	  cru_sig s;
	  int *err;

	  // Create and return a copy of the postponer p with defaults
	  // initialized. The destructor d is that of the edge labels in
	  // the graph, indicating endogenous edges if it's null and
	  // exogenous otherwise. For endogenous edges, there's no need for
	  // a user-supplied edge label creating function in the mark field
	  // because one that copies the postponable edge can be used by
	  // default. For either kind of edges, unconditional relocation is
	  // allowed by default.
{
  cru_postponer p_copy;

  if ((! s) ? IER(1045) : ! (p_copy = _cru_postponer_copy (p, err)))
	 return NULL;
  if (! (s->destructors.e_free))
	 ALLOW (p_copy->postponement.bop, (cru_bop) _cru_identity_bop);
  ALLOW (p_copy->postponement.bpred, (cru_bpred) _cru_true_bpred);
  if (! (s->destructors.e_free))
	 _cru_allow_scalar_order (&(s->orders.e_order), err);
  if (! _cru_full_order (&(s->orders.e_order), err))
	 goto a;
  if (! ((! (p_copy->postponable)) ? RAISE(CRU_UNDPOP) : p_copy->postponement.bop ? 0 : RAISE(CRU_UNDCBO)))
	 return p_copy;
 a: _cru_free_postponer (p_copy);
  return NULL;
}









cru_fabricator
_cru_inferred_fabricator (a, s, err)
	  cru_fabricator a;
	  cru_sig s;
	  int *err;

	  // Create and return a copy of the given fabricator with default
	  // fields. If the a fabricator function is specified without a
	  // corresponding destructor, then allow the original graph's
	  // destructor to correspond to it. If a fabricator function isn't
	  // specified but nor is any corresponding destructor, then allow
	  // the identity function as a fabricator. In the latter case, the
	  // fabricated graph shares vertices or edge labels with the
	  // original, which won't be reclaimed when the fabricated graph
	  // is freed, but will be dangling if the original is freed first.
{
  cru_fabricator a_copy;

  if ((! s) ? IER(1046) : (! a) ? IER(1047) : ! (a_copy = _cru_fabricator_copy (a, err)))
	 return NULL;
  if (a_copy->e_fab ? (! (a_copy->fa_sig.destructors.e_free)) : 0)
	 {
		ALLOW (a_copy->fa_sig.destructors.e_free, s->destructors.e_free);
		_cru_allow_order (&(a_copy->fa_sig.orders.e_order), &(s->orders.e_order), err);
	 }
  if (a_copy->e_fab ? 0 : ! (a_copy->fa_sig.destructors.e_free))
	 ALLOW (a_copy->e_fab, (cru_uop) _cru_identity_uop);
  if (a_copy->e_fab ? 0 : RAISE(CRU_UNDEFB))
	 goto a;
  if (a_copy->v_fab ? (! (a_copy->fa_sig.destructors.v_free)) : 0)
	 {
		ALLOW (a_copy->fa_sig.destructors.v_free, s->destructors.v_free);
		_cru_allow_order (&(a_copy->fa_sig.orders.v_order), &(s->orders.v_order), err);
	 }
  if (a_copy->v_fab ? 0 : ! (a->fa_sig.destructors.v_free))
	 ALLOW (a_copy->v_fab, (cru_uop) _cru_identity_uop);
  if (a_copy->v_fab ? 0 : RAISE(CRU_UNDVFB))
	 goto a;
  return a_copy;
 a: _cru_free_fabricator (a_copy);
  return NULL;
}
