#ifndef CRU_H
#define CRU_H 1

#include <stdint.h>
#include "error_codes.h"
#include "function_types.h"
#include "data_types.h"

// valid values for the now argument to cru_free_partition
#define NOW 1
#define LATER 0

// passed as the lanes parameter requests multi-threaded operation with one thread per core
#define CONCURRENTLY 0

// passed as the lanes parameter requests single-threaded operation
#define SEQUENTIALLY 1

// passed as the kill switch parameter disables killing the job
#define UNKILLABLE NULL

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- building --------------------------------------------------------------------------------

// create a new kill switch
extern cru_kill_switch
cru_new_kill_switch (int *err);

// return the graph specified by the builder
extern cru_graph
cru_built (cru_builder b, cru_vertex v, cru_kill_switch k, unsigned lanes, int *err);

// form a product of two graphs
extern cru_graph
cru_crossed (cru_graph g, cru_graph h, cru_crosser c, cru_kill_switch k, unsigned lanes, int *err);

// form an isomorphic graph to a given graph
extern cru_graph
cru_fabricated (cru_graph g, cru_fabricator f, cru_kill_switch k, unsigned lanes, int *err);

// --------------- control ---------------------------------------------------------------------------------

// stop the running API function to which the given kill switch has been passed
extern void
cru_kill (cru_kill_switch k, int *err);

// called by a connector function in a builder, assert a connection from its argument to the terminus
extern void
cru_connect (cru_edge label, cru_vertex terminus, int *err);

// called by an expander function in an stretcher, put a new vertex in series with two new edges
extern void
cru_stretch (cru_edge label_in, cru_vertex new_vertex, cru_edge label_out, int *err);

// --------------- analysis --------------------------------------------------------------------------------

// return the number of nodes in a graph
extern uintptr_t
cru_vertex_count (cru_graph g, unsigned lanes, int *err);

// return the number of outgoing edges in a graph
extern uintptr_t
cru_edge_count (cru_graph g, unsigned lanes, int *err);

// compute the result of walking over a graph when the traversal order doesn't matter
extern void *
cru_mapreduced (cru_graph g, cru_mapreducer m, cru_kill_switch k, unsigned lanes, int *err);

// compute the result of walking over a graph when the traversal order matters
extern void *
cru_induced (cru_graph g, cru_inducer i, cru_kill_switch k, unsigned lanes, int *err);

// --------------- classification --------------------------------------------------------------------------

// construct a partition in which related vertices are assigned to the same class
extern cru_partition
cru_partition_of (cru_graph g, cru_classifier c, cru_kill_switch k, unsigned lanes, int *err);

// return the class containing the vertex x with respect to the partition p
extern cru_class
cru_class_of (cru_partition p, cru_vertex x, int *err);

// unite two equivalence classes and return non-zero if successful
extern int
cru_united (cru_partition p, cru_class x, cru_class y, int *err);

// return the number of vertices in a class
extern uintptr_t
cru_class_size (cru_class c, int *err);

// --------------- graph expansion -------------------------------------------------------------------------

// consume and return a graph derived from g by interposing vertices along edges
extern cru_graph
cru_stretched (cru_graph g, cru_stretcher s, cru_kill_switch k, unsigned lanes, int *err);

// consume and return a graph derived from g by fissioning vertices
extern cru_graph
cru_split (cru_graph g, cru_splitter s, cru_kill_switch k, unsigned lanes, int *err);

// consume and return a graph like g with additional edges based on compositions of consecutive edges
extern cru_graph
cru_composed (cru_graph g, cru_composer c, cru_kill_switch k, unsigned lanes, int *err);

// --------------- graph contraction -----------------------------------------------------------------------

// consume and return a graph derived from g by identifying a single vertex with each equivalence class
extern cru_graph
cru_merged (cru_graph g, cru_merger c, cru_kill_switch k, unsigned lanes, int *err);

// consume and return a graph derived from g by deleting selected edges and vertices, and consume g
extern cru_graph
cru_filtered (cru_graph g, cru_filter f, cru_kill_switch k, unsigned lanes, int *err);

// consume and return a graph derived from g by fusing identical edges and vertices
extern cru_graph
cru_deduplicated (cru_graph g, cru_kill_switch k, unsigned lanes, int *err);

// --------------- graph surgery ---------------------------------------------------------------------------

// consume and return a graph derived from g by overwriting vertices and edges
extern cru_graph
cru_mutated (cru_graph g, cru_mutator m, cru_kill_switch k, unsigned lanes, int *err);

// consume and return a graph like g with edges selectively relocated to the termini of their siblings
extern cru_graph
cru_postponed (cru_graph g, cru_postponer p, cru_kill_switch k, unsigned lanes, int *err);

// --------------- reclamation -----------------------------------------------------------------------------

// free a partition
extern void
cru_free_partition (cru_partition p, int now, int *err);

// concurrently reclaim all storage associated with a graph and block until it's done
extern void
cru_free_now (cru_graph g, unsigned lanes, int *err);

// start reclaiming all storage associated with a graph in a single background thread and return immediately
extern void
cru_free_later (cru_graph g, int *err);

// deallocate a kill switch
extern void
cru_free_kill_switch (cru_kill_switch k, int *err);

// --------------- testing utilities for development use only ----------------------------------------------

// map a posix or cru-specific error code to a terse textual description
extern const char*
cru_strerror (int err);

// globally limit the number of successful memory allocations (requires a custom build)
extern void
crudev_limit_allocations (uintptr_t limit, int *err);

// report the number of allocations performed since the limit was set (requires a custom build)
extern uintptr_t
crudev_allocations_performed (int *err);

// return non-zero if the deallocations balance the allocations
extern int
crudev_all_clear (int *err);

#ifdef __cplusplus
}
#endif
#endif
