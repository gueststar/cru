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

#ifndef CRU_ROUTE_H
#define CRU_ROUTE_H 1

#include <pthread.h>
#include "ptr.h"

// arbitrary magic number for consistency checks
#define ROUTER_MAGIC 624758381

// default task parameter to router allocating functions
#define NO_TASK NULL

// Deadlock is regarded as a symptom of buggy user code attempting to
// use a remote-first traversal order on a cyclic graph. DEAD_POOL is
// the number of previously attained state hashes worth storing for
// deadlock detection by comparison with the current one. The time
// taken for deadlock detection is probabilistic. Any positive value
// for DEAD_POOL is valid, but increasing this number decreases the
// expected detection time and increases the probability of a false
// positive. The time increases with the number of worker threads and
// the connectivity of the graph as implemented in sync.c. The number
// is currently chosen to make the total size of a router a round
// number of cache lines.
#define DEAD_POOL 60

// identifies the struct type in the router union
typedef enum {NON, FIL, BUI, MUT, COM, CLU, MAP, IND, EXT, SPL, POS, CRO, FAB, DED} router_tag;

// the type of function called by a runner passed to _cru_create
typedef void *(*task)(port, int *);

// a record of data common to an ensemble of workers on the same graph

struct router_s
{
  int valid;                             // holds ROUTER_MAGIC if no pthread operations have failed
  int killed;                            // set to non-zero when a job should be stopped prematurely
  int ro_status;                         // set to an error status returned by user code or an internal error if detected
  int quiescent;                         // non-zero when no progress is possible
  router_tag tag;                        // discriminates the union
  union
  {
	 struct cru_filter_s filter;          // to specify deletable vertices and edges
	 struct cru_inducer_s inducer;        // to specify an induction operation to be applied
	 struct cru_crosser_s crosser;        // for graph products
	 struct cru_splitter_s splitter;      // to specify a graph splitting operation
	 struct cru_stretcher_s stretcher;    // to specify a graph extension operation
	 struct cru_mapreducer_s mapreducer;  // to specify a map-reduce operation to be applied
	 struct cru_fabricator_s fabricator;  // to specify a graph fabrication operation
	 struct cru_postponer_s postponer;    // to specify an edge postponement operation
	 struct cru_composer_s composer;      // for edge composition transformations
 	 struct cru_mutator_s mutator;        // to specify a mutation to be applied to a graph
	 struct cru_builder_s builder;        // to specify a graph to be built
	 struct cru_merger_s merger;          // to specify a merging operation to be applied
  };
  struct cru_sig_s ro_sig;               // a copy of the sig associated with the graph under consideration
  struct cru_plan_s ro_plan;             // a copy of the plan in the mutator or filter struct, if any
  pthread_cond_t transition;             // signaled when the router changes from non-quiescent to quiescent
  cru_partition partition;               // used for constructing equivalence classes of vertices
  node_list base_register;               // used during partitioning and induction
  pthread_mutex_t lock;                  // secures mutually exclusive access to this structure during non-atomic operations
  pthread_t *threads;                    // pointer to an array of one thread id for each port
  unsigned running;                      // the number worker threads currently running with this router and not waiting
  uintptr_t shared;                      // reference count for shared routers
  unsigned lanes;                        // the number of ports in the array, with one for each possible worker
  port *ports;                           // an array of ports for all workers associated with this router
  task work;                             // the function that runs in each worker thread
#ifdef DEADLOCK_DETECTION
  uintptr_t state_hash[DEAD_POOL];       // deadlock is inferred when any of these repeats while progress is stalled
#endif
};

#ifdef __cplusplus
extern "C"
{
#endif

// --------------- initialization --------------------------------------------------------------------------

// allocate a router with a mutator
extern router
_cru_mutating_router (cru_mutator m, cru_sig s, unsigned lanes, int *err);

// allocate a router with an inducer
extern router
_cru_inducing_router (cru_inducer i, cru_sig s, unsigned lanes, int *err);

// allocate a router with a mapreducer
extern router
_cru_mapreducing_router (cru_mapreducer m, cru_sig s, unsigned lanes, int *err);

// allocate a router with a filter
extern router
_cru_filtering_router (cru_filter f, cru_sig s, unsigned lanes, int *err);

// allocate a router with a composer
extern router
_cru_composing_router (cru_composer y, cru_sig s, unsigned lanes, int *err);

// allocate a router with an stretcher
extern router
_cru_stretching_router (cru_stretcher x, cru_sig s, unsigned lanes, int *err);

// allocate a router with an splitter
extern router
_cru_splitting_router (cru_splitter x, cru_sig s, unsigned lanes, int *err);

// allocate a router with an splitter
extern router
_cru_postponing_router (cru_postponer p, cru_sig s, unsigned lanes, int *err);

// allocate a router with a classifier and a base register
extern router
_cru_classifying_router (cru_classifier c, node_list b, cru_sig s, unsigned lanes, int *err);

// allocate a router with a merger
extern router
_cru_merging_router (cru_merger c, cru_sig s, unsigned lanes, int *err);

// allocate a router with a fabricator
extern router
_cru_fabricating_router (cru_fabricator a, unsigned lanes, int *err);

// --------------- initialization with tasks ---------------------------------------------------------------

// reinitialize the task of a router between jobs
extern router
_cru_reset (router r, task t, int *err);

// allocate a router with just a task
extern router
_cru_router (task t, unsigned lanes, int *err);

// allocate a router with just a destructors and a task
extern router
_cru_razing_router (cru_destructor_pair z, task t, unsigned lanes, int *err);

// allocate a router with a builder
extern router
_cru_building_router (cru_builder b, task t, unsigned lanes, int *err);

// allocate a router with a crosser
extern router
_cru_crossing_router (cru_crosser c, task t, unsigned lanes, int *err);

// --------------- reclamation -----------------------------------------------------------------------------

// make a shared copy of a router
extern router
_cru_shared (router r);

// free or unshare a router
extern void
_cru_free_router (router r, int *err);

// tear down a router and the graph nodes stored in its ports
extern void
_cru_sweep (router r, int *err);

#ifdef __cplusplus
}
#endif
#endif
