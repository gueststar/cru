#ifndef CRU_DATA_TYPES_H
#define CRU_DATA_TYPES_H 1

#include <stdint.h>

// --------------- pointers to opaque types ----------------------------------------------------------------

typedef void *cru_edge;
typedef void *cru_vertex;

// opaque type representing a graph
typedef struct cru_graph_s *cru_graph;

// opaque type respresenting a kill switch
typedef struct cru_kill_switch_s *cru_kill_switch;

// opaque type representing a generally non-injective function from vertices to classes
typedef struct cru_partition_s *cru_partition;

// opaque type representing a value to which a vertex is mapped by a cru_classification
typedef struct cru_class_s *cru_class;

// --------------- conditional operators -------------------------------------------------------------------

// a specification for conditionally creating a new edge label during postponement

typedef struct cru_cbop_s
{
  cru_bpred bpred;                           // non-zero requests relocating the left edge to follow the right
  cru_bop bop;                               // to create a new label
} *cru_cbop;

// a specification for conditionally modifying, creating, or deleting an edge when splitting

typedef struct cru_ctop_s
{
  cru_tpred tpred;                           // a non-zero result requests edge creation or modification
  cru_top top;                               // to make a new edge label
} *cru_ctop;

// a specification for conditionally creating a new edge label during composition

typedef struct cru_cqop_s
{
  cru_qpred qpred;                           // a non-zero result requests edge creation
  cru_qop qop;                               // to make a new edge label
} *cru_cqop;

// --------------- base structures -------------------------------------------------------------------------

// for comparing vertices, edges, and properties

typedef struct cru_order_s
{
  cru_hash hash;                             // must map equivalent values or data to identical hashes
  cru_bpred equal;                           // must be transitive, reflexive, and symmetric
} *cru_order;

// a specification to tell cru how to reclaim a graph

typedef struct cru_destructor_pair_s
{
  cru_destructor v_free;                     // used to reclaim vertices
  cru_destructor e_free;                     // used to reclaim edge labels
} *cru_destructor_pair;

// a specification for computing something about a set of vertices or edges in a graph

typedef struct cru_fold_s
{
  cru_top map;                               // takes an individual vertex or edge to a redex of a user-defined type
  cru_bop reduction;                         // combines two redexes into a single redex
  cru_nop vacuous_case;                      // returns the result associated with an empty set of vertices or edges
  cru_destructor r_free;                     // deallocates the reduction result type
  cru_destructor m_free;                     // deallocates the map result type
} *cru_fold;

// a specification of the subgraph reachable from an initial vertex if offset is non-zero

typedef struct cru_zone_s
{
  int offset;                                // non-zero starts the walk from the given initial vertex instead of the base
  cru_vertex initial_vertex;                 // if offset is non-zero, the walk starts from here
  int backwards;                             // non-zero directs the walk toward incident edges instead of outgoing edges
} *cru_zone;

// --------------- derived structures ----------------------------------------------------------------------

// a pair of orders for the vertices and the edges in a graph

typedef struct cru_order_pair_s
{
  struct cru_order_s v_order;                // for sorting vertices and detecting vertex collisions
  struct cru_order_s e_order;                // for sorting edges and detecting edge label collisions
} *cru_order_pair;

// a specification for operating on a vertex and allowed to depend on its adjacent edges

typedef struct cru_prop_s
{
  struct cru_fold_s vertex;                  // map applies to the vertex and the results of both edge reductions
  struct cru_fold_s incident;                // map applies to the local vertex, an incident edge label, and a remote vertex
  struct cru_fold_s outgoing;                // the same for outgoing edges
} *cru_prop;

// a combined specification for performing a related operation on vertices and edges

typedef struct cru_kernel_s
{
  struct cru_prop_s v_op;                    // the operation to perform on vertices
  struct cru_fold_s e_op;                    // the operation to perform on edges
}* cru_kernel;

// a specification a subgraph and a traversal order

typedef struct cru_plan_s
{
  int local_first;                           // non-zero constrains the traversal to visit nodes in top down order
  int remote_first;                          // non-zero constrains the traversal to visit nodes in bottom up order
  int ignore_deadlock;                       // disable deadlock detection when non-zero
  struct cru_zone_s zone;
} *cru_plan;

// guard conditions on outgoing edges from the initial and intermediate vertex in an edge composition operation

typedef struct cru_prop_pair_s
{
  struct cru_prop_s local;                   // to compute the local redex parameter to the compostion
  struct cru_prop_s adjacent;                // to compute the adjacent redex parameter to the composition
}* cru_prop_pair;

// a specification for splitting one edge into two

typedef struct cru_ctop_pair_s
{
  struct cru_ctop_s ana_labeler;             // computes the label of an edge connected to the ana vertex
  struct cru_ctop_s cata_labeler;            // computes the label of an edge connected to the cata vertex
} *cru_ctop_pair;

// a specification for splitting one edge into two in each of two cases

typedef struct cru_ctop_quad_s
{
  struct cru_ctop_pair_s inward;             // modifies and creates incoming edges
  struct cru_ctop_pair_s outward;            // modifies and creates outgoing edges
} *cru_ctop_quad;

// a specification to tell cru how to deduplicate or reclaim a graph

typedef struct cru_sig_s
{
  uintptr_t vertex_limit;                    // kill a build or expansion if the number of vertices exceeds this number
  struct cru_order_pair_s orders;            // for organizing efficient temporary storage during graph traversal
  struct cru_destructor_pair_s destructors;  // for unmaking the graph
} *cru_sig;

// --------------- top level structures for building -------------------------------------------------------

// a specification to tell cru how to build a graph

typedef struct cru_builder_s
{
  cru_connector connector;                   // calls cru_connect unconditionally for every outgoing edge from a given vertex
  cru_subconnector subconnector;             // calls cru_connect for each of a subset of edges determined by an incident edge
  struct cru_sig_s bu_sig;                   // summary of graph vertex and edge properties
} *cru_builder;

// a specification for combining two graphs into their product

typedef struct cru_crosser_s
{
  cru_bop v_prod;                            // to create combined vertices
  struct cru_sig_s cr_sig;                   // describes the combined graph
  struct cru_cbop_s e_prod;                  // to create combined edges
} *cru_crosser;

// a specification for building an isomorphic graph to a given graph

typedef struct cru_fabricator_s
{
  cru_uop v_fab;                             // to fabricate corresponding vertices
  cru_uop e_fab;                             // to fabricate corresponding edges
  struct cru_sig_s fa_sig;                   // describes the fabricated graph
} *cru_fabricator;

// --------------- top level structures for analysis -------------------------------------------------------

// a specification for computing something about a graph

typedef struct cru_mapreducer_s
{
  struct cru_zone_s ma_zone;                 // optionally restricts the operation to a subgraph
  struct cru_prop_s ma_prop;                 // encapsulates the work to be done on each vertex
} *cru_mapreducer;

// a specification for computing something about a graph by backward or forward induction

typedef struct cru_inducer_s
{
  cru_uop boundary_value;                    // optionally determine a result for vertices having no prerequisites
  struct cru_zone_s in_zone;                 // specifies the direction and starting point
  struct cru_fold_s in_fold;                 // encapsulates the work to be done at each vertex
} *cru_inducer;

// --------------- top level structures for classification -------------------------------------------------

// a specification for defining equivalence classes over a graph's vertices

typedef struct cru_classifier_s
{
  struct cru_prop_s cl_prop;                 // to extract the property all members of a class have in common
  struct cru_order_s cl_order;               // for comparing properties
} *cru_classifier;

// --------------- top level structures for expansion ------------------------------------------------------

// a specification for transforming a graph to one with new vertices interposed in series

typedef struct cru_stretcher_s
{
  int st_fix;                                // non-zero requests exhaustive iteration
  cru_tpred expander;                        // to compute the interposing vertices and edge labels
  struct cru_prop_s st_prop;                 // to initialize user specified properties of each vertex
} *cru_stretcher;

// a specification for transforming a graph to one with new vertices interposed in parallel

typedef struct cru_splitter_s
{
  cru_bop ana;                               // computes the value of a new vertex
  cru_bop cata;                              // computes the value of the other new vertex
  cru_bpred fissile;                         // requests vertex fission when non-zero
  struct cru_prop_s sp_prop;                 // to initialize user specified properties of each vertex
  struct cru_ctop_quad_s sp_ctops;           // modifies and creates edges
} *cru_splitter;

// a specification for transforming a graph to one with some vertices bypassed by a composition of edges

typedef struct cru_composer_s
{
  int co_fix;                                // non-zero requests exhaustive iteration
  int destructive;                           // non-zero requests deletion of the launching edge
  struct cru_cqop_s labeler;                 // to create the bypassing edges
  struct cru_prop_pair_s co_props;           // to evaluate conditions enabling an edge composition
} *cru_composer;

// --------------- top level structures for contraction ----------------------------------------------------

// a specification for transforming a graph to one of equivalence classes over its vertices

typedef struct cru_merger_s
{
  struct cru_classifier_s me_classifier;     // to organize vertices into classes
  struct cru_order_pair_s me_orders;         // to define the post-merge graph ordering
  struct cru_kernel_s me_kernel;             // to combine vertices and edges within a class
  cru_pruner pruner;                         // optionally for selectively deleting outgoing edges from a class
} *cru_merger;

// a specification for transforming a graph to one with some edges or vertices deleted

typedef struct cru_filter_s
{
  struct cru_zone_s fi_zone;                 // where to filter
  struct cru_kernel_s fi_kernel;             // only vertices and edges for which the result is non-null are retained
  struct cru_order_s fi_order;               // optionally to identify classes of edges jointly filterable
} *cru_filter;

// --------------- top level structures for surgery --------------------------------------------------------

// a specification for modifying a graph in place

typedef struct cru_mutator_s
{
  struct cru_plan_s mu_plan;                 // where to mutate and in what order
  struct cru_kernel_s mu_kernel;             // how to mutate
  struct cru_order_pair_s mu_orders;         // to define the post-mutation graph ordering
} *cru_mutator;

// a specification for selectively laying parallel edges end to end

typedef struct cru_postponer_s
{
  int po_fix;                                // non-zero requests exhaustive iteration
  cru_tpred postponable;                     // for partitioning the edges into two classes
  struct cru_cbop_s postponement;            // for possibly relocating and modifying an edge
} *cru_postponer;

#endif
