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

#ifndef CRU_INFER_H
#define CRU_INFER_H 1

// Inferrers validate and return a copy of the argument with
// replacement functions assigned to unspecified fields if possible.
// The _cru_deduplicator function constructs a merger for
// deduplicating a graph based on the sig used to build it.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern cru_builder
_cru_inferred_builder (cru_builder b, void *v, int *err);

extern cru_crosser
_cru_inferred_crosser (cru_crosser c, int *err);

extern cru_mapreducer
_cru_inferred_mapreducer (cru_mapreducer m, int *err);

extern cru_inducer
_cru_inferred_inducer (cru_inducer i, int *err);

extern cru_composer
_cru_inferred_composer (cru_composer c, cru_sig s, int *err);

extern cru_stretcher
_cru_inferred_stretcher (cru_stretcher x, cru_sig s, int *err);

extern cru_splitter
_cru_inferred_splitter (cru_splitter x, cru_sig s, int *err);

extern cru_classifier
_cru_inferred_classifier (cru_classifier c, int *err);

extern cru_merger
_cru_inferred_merger (cru_merger c, cru_sig s, int *err);

extern cru_mutator
_cru_inferred_mutator (cru_mutator m, cru_sig s, int *err);

extern cru_filter
_cru_inferred_filter (cru_filter f, cru_sig s, int *err);

extern cru_merger
_cru_deduplicator (cru_sig s, int *err);

extern cru_postponer
_cru_inferred_postponer (cru_postponer p, cru_sig s, int *err);

extern cru_fabricator
_cru_inferred_fabricator (cru_fabricator a, cru_sig s, int *err);

#ifdef __cplusplus
}
#endif
#endif
