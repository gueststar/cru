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

#ifndef CRU_BRIG_H
#define CRU_BRIG_H 1

// This file declares routines pertaining to equivalence relations on
// edges. A brigade is a list of buckets such that the edges in each
// bucket have something in common with other edges in the same
// bucket.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// set of edge lists

struct brigade_s
{
  edge_list bucket;                    // all edges in the same bucket have something in common
  brigade other_buckets;
};

// default relation parameter for _cru_bundled
#define UNRELATED NULL

// push a new bucket into a brigade
extern void
_cru_push_bucket (edge_list bucket, brigade *b, cru_destructor d, int *err);

// return the first bucket from a brigade
extern brigade
_cru_popped_bucket (brigade *b, int *err);

// remove and return the minimum bucket from a brigade based on a relational predicate
extern brigade
_cru_minimum_bucket (cru_qpred t, brigade *b, int *err);

// free a brigade ignoring the buckets
extern void
_cru_free_brigade (brigade b, int *err);

// turn an edge list into a brigade classified either by the termini or the classes thereof
extern brigade
_cru_bucketed (edge_list *e, int by_class, int *err);

// turn an edge list into a brigade classified by a user-defined relation f and consume the edge list
extern brigade
_cru_bundled (cru_bpred r, edge_list *e, int *err);

// flatten a brigade into an edge list
extern edge_list
_cru_unbundled (brigade b);

// consume and concatenate two brigades
extern brigade
_cru_cat_brigades (brigade l, brigade r);

// return the union of edges in a brigade by deduplicating the buckets
extern edge_list
_cru_deduplicated_brigade (cru_order r, brigade t, int by_class, cru_destructor d, int *err);

// convert a brigade to an edge list with one edge for each bucket
extern edge_list
_cru_reduced_brigade (cru_fold m, void *v, brigade t, int by_class, cru_destructor d, int *err);

#ifdef __cplusplus
}
#endif
#endif
