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

#ifndef CRU_COPY_H
#define CRU_COPY_H 1

// This file declares functions that copy and free user-facing
// structures, whose associated malloc call sites are collected into
// the same source copy.c for diagnostic purposes.

#include "ptr.h"

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization --------------------------------------------------------------------------

extern int
_cru_open_copy (int *err);

extern void
_cru_close_copy (void);

// --------------- memory management -----------------------------------------------------------------------

extern cru_builder
_cru_builder_copy (cru_builder i, int *err);

extern void
_cru_free_builder (cru_builder o);

extern cru_crosser
_cru_crosser_copy (cru_crosser i, int *err);

extern void
_cru_free_crosser (cru_crosser o);

extern cru_mapreducer
_cru_mapreducer_copy (cru_mapreducer i, int *err);

extern void
_cru_free_mapreducer (cru_mapreducer o);

extern cru_inducer
_cru_inducer_copy (cru_inducer i, int *err);

extern void
_cru_free_inducer (cru_inducer o);

extern cru_composer
_cru_composer_copy (cru_composer i, int *err);

extern void
_cru_free_composer (cru_composer o);

extern cru_stretcher
_cru_stretcher_copy (cru_stretcher i, int *err);

extern void
_cru_free_stretcher (cru_stretcher o);

extern cru_splitter
_cru_splitter_copy (cru_splitter i, int *err);

extern void
_cru_free_splitter (cru_splitter o);

extern cru_classifier
_cru_classifier_copy (cru_classifier i, int *err);

extern void
_cru_free_classifier (cru_classifier o);

extern cru_merger
_cru_merger_copy (cru_merger i, int *err);

extern void
_cru_free_merger (cru_merger o);

extern cru_mutator
_cru_mutator_copy (cru_mutator i, int *err);

extern void
_cru_free_mutator (cru_mutator o);

extern cru_filter
_cru_filter_copy (cru_filter i, int *err);

extern void
_cru_free_filter (cru_filter o);

extern cru_postponer
_cru_postponer_copy (cru_postponer i, int *err);

extern void
_cru_free_postponer (cru_postponer o);

extern cru_fabricator
_cru_fabricator_copy (cru_fabricator i, int *err);

extern void
_cru_free_fabricator (cru_fabricator o);

#ifdef __cplusplus
}
#endif
#endif
