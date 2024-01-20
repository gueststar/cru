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

#ifndef CRU_CLASSES_H
#define CRU_CLASSES_H 1

// This file declares functions for keeping track of equivalence
// classes of vertices. An equivalence class is associated with a
// class such that the classes of any two vertices are equal if and
// only if the vertices are related. The class is implemented as a
// pointer so that the memory allocator can ensure that distinct
// classes are always unequal to each other even if they're created in
// different threads. It points to a reference counter and to a fixed
// integer value used only for consistency checks. A partition uses an
// array of vertex maps instead of just one so that the vertex maps
// can be built concurrently.

#include "ptr.h"

// arbitrary magic number stored in partitions for consistency checks
#define PARTITION_MAGIC 1518114127

// arbitrary magic number stored in classes for consistency checks
#define CLASS_MAGIC 1520126179

struct cru_partition_s
{
  int parted;                     // always equal to PARTITION_MAGIC
  unsigned plurality;             // the number of vertex maps
  pthread_rwlock_t loch;          // for thread-safe a union-find API
  vertex_map *maps;               // an array of vertex maps
};

struct cru_class_s
{
  int classed;                    // always equal to CLASS_MAGIC
  uintptr_t references;           // a reference count used to avoid double-free errors during reclamation
  cru_class superclass;           // class of a class containing this one if applicable
};

#ifdef __cplusplus
extern "C"
{
#endif

// allocate a partition
extern cru_partition
_cru_partition (unsigned p, int *err);

// free a class
extern void
_cru_free_class (cru_class h, int *err);

// concurrently derive a partition from the given router and consume the router
extern cru_partition
_cru_partition_of (cru_graph g, cru_kill_switch k, router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
