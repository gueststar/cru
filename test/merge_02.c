// Create a hypercubic graph with exogenous vertices and edges, and
// fuse any vertices differing in only their least significant bit.
// Check that the result is half the size of the original. Exercise
// the merger property generator by needlessly inspecting the
// incident and outgoing edges from each vertex. Also filter out
// any edge labeled by zero in the result.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1617343470
#define FUSED_VERTEX_MAGIC 2135411170
#define EDGE_MAGIC 1454920852
#define REDEX_MAGIC 1837066853
#define MAPEX_MAGIC 1900837178
#define PROPERTY_MAGIC 1972835742
#define INCIDENT_MAPEX_MAGIC 1615930631
#define INCIDENT_REDEX_MAGIC 1103414114
#define OUTGOING_REDEX_MAGIC 1675779927

// a count of allocated edges to detect memory leaks
static uintptr_t edge_count;

// needed to lock the count
static pthread_mutex_t edge_lock;

// a count of allocated vertices to detect memory leaks
static uintptr_t vertex_count;

// needed to lock the count
static pthread_mutex_t vertex_lock;

// a count of allocated redexes to detect memory leaks
static uintptr_t redex_count;

// needed to lock the count
static pthread_mutex_t redex_lock;

typedef struct edge_s
{
  int e_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t e_value;
} *edge;

typedef struct vertex_s
{
  int v_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t v_value;
} *vertex;

typedef struct redex_s
{
  int r_magic;              // arbitrary constant used to detect double-free errors
  uintptr_t r_value;
} *redex;








redex
redex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a redex.
{
  redex r;

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(3838))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3839) : ++redex_count ? 0 : FAIL(3840))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(3841) : 0))
	 return r;
 a: free (r);
  return NULL;
}







void
free_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex.
{
  if ((! r) ? FAIL(3842) : (r->r_magic != REDEX_MAGIC) ? FAIL(3843) : 0)
	 return;
  r->r_magic = MUGGLE(157);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3844) : redex_count-- ? 0 : FAIL(3845))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3846) : 0)
	 return;
  free (r);
}




void
free_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex.
{
  if ((! r) ? FAIL(3847) : (r->r_magic != MAPEX_MAGIC) ? FAIL(3848) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
}







void
free_property (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a property for graph merging. This
	  // function cast to a cru_destructor is passed to the cru library
	  // as c.me_classifier.cl_prop.vertex.m_free in the merger c.
{
  if ((! r) ? FAIL(3849) : (r->r_magic != PROPERTY_MAGIC) ? FAIL(3850) : 0)
	 return;
  r->r_magic = MUGGLE(158);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3851) : redex_count-- ? 0 : FAIL(3852))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3853) : 0)
	 return;
  free (r);
}







void
free_incident_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a result of traversing the incident
	  // edges on a vertex during a merge operation. This
	  // function cast to a cru_destructor is passed to the cru library
	  // as c.me_classifier.cl_prop.incident.r_free in the merger c.
{
  if ((! r) ? FAIL(3854) : (r->r_magic != INCIDENT_REDEX_MAGIC) ? FAIL(3855) : 0)
	 return;
  r->r_magic = MUGGLE(159);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3856) : redex_count-- ? 0 : FAIL(3857))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3858) : 0)
	 return;
  free (r);
}






void
free_incident_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex created as from an incident
	  // edges on a vertex during a merge operation.
{
  if ((! r) ? FAIL(3859) : (r->r_magic != INCIDENT_MAPEX_MAGIC) ? FAIL(3860) : 0)
	 return;
  r->r_magic = INCIDENT_REDEX_MAGIC;
  free_incident_redex (r, err);
}







void
free_outgoing_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a result of traversing the incident
	  // edges on a vertex during a merge operation. This
	  // function cast to a cru_destructor is passed to the cru library
	  // as c.me_classifier.cl_prop.outgoing.r_free in the merger c.
{
  if ((! r) ? FAIL(3861) : (r->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3862) : 0)
	 return;
  r->r_magic = MUGGLE(160);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3863) : redex_count-- ? 0 : FAIL(3864))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3865) : 0)
	 return;
  free (r);
}










int
equal_properties (a, b, err)
	  redex a;
	  redex b;
	  int *err;

	  // Compare two redexes and return non-zero if they are
	  // equal. This function cast to a cru_bpred is passed as
	  // c.me_classifier.cl_order.equal in the merger c.
{
  if ((! a) ? FAIL(3866) : (a->r_magic != PROPERTY_MAGIC) ? FAIL(3867) : 0)
	 return 0;
  if ((! b) ? FAIL(3868) : (b->r_magic != PROPERTY_MAGIC) ? FAIL(3869) : 0)
	 return 0;
  return a->r_value == b->r_value;
}










uintptr_t
property_hash (r)
	  redex r;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed as c.me_classifier.cl_order.hash in the merger c.
{
  size_t i;
  uintptr_t u;

  if ((! r) ? GLOBAL_FAIL(3870) : (r->r_magic != PROPERTY_MAGIC) ? GLOBAL_FAIL(3871) : 0)
	 return 0;
  u = r->r_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}








edge
edge_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create an edge.
{
  edge e;

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(3872))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3873) : ++edge_count ? 0 : FAIL(3874))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(3875) : 0))
	 return e;
 a: free (e);
  return NULL;
}







void
free_edge (e, err)
	  edge e;
	  int *err;

	  // Reclaim an edge. This function cast to a cru_destructor is
	  // passed to the cru library as b.bu_sig.destructors.e_free in the builder
	  // b.
{
  if ((! e) ? FAIL(3876) : (e->e_magic != EDGE_MAGIC) ? FAIL(3877) : 0)
	 return;
  e->e_magic = MUGGLE(161);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3878) : edge_count-- ? 0 : FAIL(3879))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(3880) : 0)
	 return;
  free (e);
}







uintptr_t
edge_hash (e)
	  edge e;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed as b.bu_sig.orders.e_order.hash in the builder b,
	  // but is needed only for merging and not building.
{
  size_t i;
  uintptr_t u;

  if ((! e) ? GLOBAL_FAIL(3881) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(3882) : 0)
	 return 0;
  u = e->e_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







int
equal_edges (a, b, err)
	  edge a;
	  edge b;
	  int *err;

	  // Compare two edges and return non-zero if they are
	  // equal. This function cast to a cru_bpred is passed to the
	  // cru library as b.bu_sig.orders.e_order.equal in the builder b,
	  // but is needed only for merging.
{
  if ((! a) ? FAIL(3883) : (a->e_magic != EDGE_MAGIC) ? FAIL(3884) : 0)
	 return 0;
  if ((! b) ? FAIL(3885) : (b->e_magic != EDGE_MAGIC) ? FAIL(3886) : 0)
	 return 0;
  return a->e_value == b->e_value;
}








vertex
vertex_of (n, err)
	  uintptr_t n;
	  int *err;

	  // Create a vertex.
{
  vertex v;

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(3887))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3888) : ++vertex_count ? 0 : FAIL(3889))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(3890) : 0))
	 return v;
 a: free (v);
  return NULL;
}








void
free_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex. This function cast to a cru_destructor is
	  // passed to the cru library as b.bu_sig.destructors.v_free in the builder
	  // b.
{
  if ((! v) ? FAIL(3891) : (v->v_magic != VERTEX_MAGIC) ? FAIL(3892) : 0)
	 return;
  v->v_magic = MUGGLE(162);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3893) : vertex_count-- ? 0 : FAIL(3894))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3895) : 0)
	 return;
  free (v);
}








void
free_fused_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex resulting from a fusion operation.
{
  if ((! v) ? FAIL(3896) : (v->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3897) : 0)
	 return;
  v->v_magic = MUGGLE(163);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3898) : vertex_count-- ? 0 : FAIL(3899))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3900) : 0)
	 return;
  free (v);
}







int
equal_vertices (a, b, err)
	  vertex a;
	  vertex b;
	  int *err;

	  // Compare two vertices and return non-zero if they are
	  // equal. This function cast to a cru_bpred is passed to the
	  // cru library as b.bu_sig.orders.v_order.equal in the builder b.
{
  if ((! a) ? FAIL(3901) : (a->v_magic != VERTEX_MAGIC) ? FAIL(3902) : 0)
	 return 0;
  if ((! b) ? FAIL(3903) : (b->v_magic != VERTEX_MAGIC) ? FAIL(3904) : 0)
	 return 0;
  return a->v_value == b->v_value;
}






int
fused_equal_vertices (a, b, err)
	  vertex a;
	  vertex b;
	  int *err;

	  // Compare two fused vertices and return non-zero if they are
	  // equal.
{
  if ((! a) ? FAIL(3905) : (a->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3906) : 0)
	 return 0;
  if ((! b) ? FAIL(3907) : (b->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3908) : 0)
	 return 0;
  return a->v_value == b->v_value;
}







uintptr_t
vertex_hash (v)
	  vertex v;

	  // Mix up low-entropy numeric values. This function cast to a
	  // cru_hash is passed to the cru library as b.bu_sig.orders.v_order.hash
	  // in the builder b.
{
  size_t i;
  uintptr_t u;

  if ((! v) ? GLOBAL_FAIL(3909) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(3910) : 0)
	 return 0;
  u = v->v_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}









uintptr_t
fused_vertex_hash (v)
	  vertex v;

	  // Mix up low-entropy numeric values.
{
  size_t i;
  uintptr_t u;

  if ((! v) ? GLOBAL_FAIL(3911) : (v->v_magic != FUSED_VERTEX_MAGIC) ? GLOBAL_FAIL(3912) : 0)
	 return 0;
  u = v->v_value;
  for (i = 0; i < sizeof (u); i++)  // ignore overflow
	 u += (u + 1) << 8;
  return (u << HALF_POINTER_WIDTH) | (u >> HALF_POINTER_WIDTH);
}







void
building_rule (given_vertex, err)
	  vertex given_vertex;
	  int *err;

	  // Declare one outgoing edge from the given vertex along each
	  // dimensional axis to a vertex whose binary encoding differs
	  // from that of the given vertex in exactly one bit. Number the
	  // edges consecutively from zero. This function cast to a
	  // cru_connector is passed to the cru library as b.connector in
	  // the builder b.
{
  uintptr_t outgoing_edge;
  vertex remote_vertex;

  if ((! given_vertex) ? FAIL(3913) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3914) : 0)
	 return;
  for (outgoing_edge = 0; outgoing_edge < DIMENSION; outgoing_edge++)
	 {
		remote_vertex = vertex_of (given_vertex->v_value ^ (uintptr_t) (1 << outgoing_edge), err);
		cru_connect (edge_of (outgoing_edge, err), remote_vertex, err);
	 }
}










redex
edge_checker (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return 1 if an edge is labeled by the index of the bit in
	  // which its endpoints differ, and 0 otherwise.
{
  if (*err ? 1 : (! connecting_edge) ? FAIL(3915) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3916) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3917) : (local_vertex->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3918) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3919) : (remote_vertex->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3920) : 0)
	 return NULL;
  if ((((local_vertex->v_value ^ remote_vertex->v_value) << 1) != ( 1 << connecting_edge->e_value)) ? FAIL(3921) : 0)
	 return NULL;
  return redex_of (1, err);
}








redex
vertex_checker (edges_in, given_vertex, edges_out, err)
	  redex edges_in;
	  vertex given_vertex;
	  redex edges_out;
	  int *err;

	  // Validate a vertex based on the incoming and outgoing
	  // edges being valid.
{
  if (*err ? 1 : (! given_vertex) ? FAIL(3922) : (given_vertex->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3923) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(3924) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(3925) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(3926) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(3927) : 0)
	 return NULL;
  if ((edges_in->r_value != DIMENSION - 1) ? FAIL(3928) : (edges_out->r_value != DIMENSION - 1) ? FAIL(3929) : 0)
	 return NULL;
  return redex_of (1, err);
}







redex
sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // Compute the sum of two numbers checking for overflow and other
	  // errors.
{
  uintptr_t s;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(3930) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(3931) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3932) : (s < (r ? r->r_value : 0)) ? FAIL(3933) : 0)
	 return NULL;
  return redex_of (s, err);
}









redex
incident_sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // Compute the sum of two numbers checking for overflow and other
	  // errors.
{
  uintptr_t s;
  redex t;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != INCIDENT_MAPEX_MAGIC) ? FAIL(3934) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != INCIDENT_REDEX_MAGIC) ? FAIL(3935) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3936) : (s < (r ? r->r_value : 0)) ? FAIL(3937) : 0)
	 return NULL;
  if ((t = redex_of (s, err)))
	 t->r_magic = INCIDENT_REDEX_MAGIC;
  return t;
}









redex
outgoing_sum (l, r, err)
	  redex l;
	  redex r;
	  int *err;

	  // Compute the sum of two numbers checking for overflow and other
	  // errors.
{
  uintptr_t s;
  redex t;

  if (*err)
	 return NULL;
  if ((! l) ? 0 : (l->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3938) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3939) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3940) : (s < (r ? r->r_value : 0)) ? FAIL(3941) : 0)
	 return NULL;
  if ((t = redex_of (s, err)))
	 t->r_magic = OUTGOING_REDEX_MAGIC;
  return t;
}







redex
zero (err)
	  int *err;

	  // Return a redex with a value of zero, for use as the vacuous case in
	  // a mapreduction over redexes.
{
  return redex_of (0, err);
}








redex
incident_zero (err)
	  int *err;

	  // Return a redex with a value of zero, for use as the vacuous case in
	  // a mapreduction over redexes corresponding to incident edges.
{
  redex r;

  if ((r = zero (err)))
	 r->r_magic = INCIDENT_REDEX_MAGIC;
  return r;
}






int
valid (g, err)
	  cru_graph g;
	  int *err;

	  // Check for correct vertex counts, edge counts, and edge labels
	  // the hard way by using redexes for numbers.
{
  struct cru_mapreducer_s m = {
	 .ma_prop = {
		.vertex = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) zero,
		  .map = (cru_top) vertex_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.incident = {
		  .reduction = (cru_bop) sum,
		  .vacuous_case = (cru_nop) zero,
		  .map = (cru_top) edge_checker,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex},
		.outgoing = {
		  .reduction = (cru_bop) sum,
		  .map = (cru_top) edge_checker,
		  .vacuous_case = (cru_nop) zero,
		  .m_free = (cru_destructor) free_redex,
		  .r_free = (cru_destructor) free_redex}}};

  redex r;
  int v;

  if (*err)
	 return 0;
  if ((cru_vertex_count (g, LANES, err) == (NUMBER_OF_VERTICES >> 1)) ? 0 : FAIL(3942))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == (((DIMENSION - 1) * NUMBER_OF_VERTICES) >> 1)) ? 0 : FAIL(3943))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(3944))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(3945))
	 return 0;
  if (!(v = (r->r_value == NUMBER_OF_VERTICES >> 1)))
	 FAIL(3946);
  free_redex (r, err);
  return v;
}








redex
common_property (edges_in, given_vertex, edges_out, err)
	  redex edges_in;
	  vertex given_vertex;
	  redex edges_out;
	  int *err;

	  // Return the high order bits of a vertex. This function cast as
	  // a cru_top is passed as c.me_classifier.cl_prop.vertex.map in the
	  // cru_merger c.
{
#define D ((DIMENSION * (DIMENSION - 1)) >> 1)

  redex r;

  if (*err ? 1 : (! given_vertex) ? FAIL(3947) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3948) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(3949) : (edges_in->r_magic != INCIDENT_REDEX_MAGIC) ? FAIL(3950) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(3951) : (edges_out->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3952) : 0)
	 return NULL;
  if ((edges_in->r_value != D) ? FAIL(3953) : (edges_out->r_value != D) ? FAIL(3954) : 0)
	 return NULL;
  if ((r = redex_of (given_vertex->v_value >> 1, err)))
	 r->r_magic = PROPERTY_MAGIC;
  return r;
}











redex
incident_map (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return an incident redex of the edge label.
{
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(3955) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3956) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3957) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3958) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3959) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3960) : 0)
	 return NULL;
  if ((r = redex_of (connecting_edge->e_value, err)))
	 r->r_magic = INCIDENT_MAPEX_MAGIC;
  return r;
}











redex
outgoing_map (local_vertex, connecting_edge, remote_vertex, err)
	  vertex local_vertex;
	  edge connecting_edge;
	  vertex remote_vertex;
	  int *err;

	  // Return an outgoing redex of the edge label.
{
  redex r;

  if (*err ? 1 : (! connecting_edge) ? FAIL(3961) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3962) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3963) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3964) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3965) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3966) : 0)
	 return NULL;
  if ((r = redex_of (connecting_edge->e_value, err)))
	 r->r_magic = OUTGOING_REDEX_MAGIC;
  return r;
}










vertex
vertex_map (edges_in, given_vertex, edges_out, err)
	  void *edges_in;
	  vertex given_vertex;
	  void *edges_out;
	  int *err;

	  // Return a vertex with half the value of the given vertex and of
	  // a fused vertex type. This function cast as a cru_top is passed
	  // as c.me_kernel.v_op.vertex.map in the merger c.
{
  vertex v;

  if (edges_in ? FAIL(3967) : edges_out ? FAIL(3968) : 0)
	 return NULL;
  if ((! given_vertex) ? FAIL(3969) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3970) : 0)
	 return NULL;
  if ((v = vertex_of (given_vertex->v_value >> 1, err)))
	 v->v_magic = FUSED_VERTEX_MAGIC;
  return v;
}










vertex
vertex_fusion (a, b, err)
	  vertex a;
	  vertex b;
	  int *err;

	  // Take a pair of vertices each resulting from a fusion and fuse
	  // them even more into a single vertex. This function cast as a
	  // cru_bop is passed as c.me_kernel.v_op.vertex.reduction in the
	  // merger c.
{
  vertex v;

  if ((! a) ? FAIL(3971) : (a->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3972) : 0)
	 return NULL;
  if ((! b) ? FAIL(3973) : (b->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3974) : 0)
	 return NULL;
  if ((a->v_value != b->v_value) ? FAIL(3975) : 0)
	 return NULL;
  if ((v = vertex_of (a->v_value, err)))
	 v->v_magic = FUSED_VERTEX_MAGIC;
  return v;
}









int
initialized (count, lock, lock_created, err)
	  uintptr_t *count;
	  pthread_mutex_t *lock;
	  int *lock_created;
	  int *err;
{
  pthread_mutexattr_t mutex_attribute;

  if (count ? (*count = 0) : 1)
	 FAIL(3976);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(3977);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(3978) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(3979) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(3980) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(3981) : *err);
}









int
predicate (everywhere, outgoing_edge, err)
	  int everywhere;
	  edge outgoing_edge;
	  int *err;

	  // Request removal of edges labeled by zero.
{
  if ((! outgoing_edge) ? FAIL(3982) : (outgoing_edge->e_magic != EDGE_MAGIC) ? FAIL(3983) : (! everywhere) ? FAIL(3984) : 0)
	 return 0;
  return ! ! (outgoing_edge->e_value);
}









int
main (argc, argv)
	  int argc;
	  char **argv;
{
  int vertex_lock_created;
  int redex_lock_created;
  int edge_lock_created;
  uintptr_t limit;
  cru_graph g;
  int v, err;

  struct cru_builder_s b = {
	 .connector = (cru_connector) building_rule,
	 .bu_sig = {
		.orders = {
		  .e_order = {
			 .hash = (cru_hash) edge_hash,
			 .equal = (cru_bpred) equal_edges},
		  .v_order = {
			 .hash = (cru_hash) vertex_hash,
			 .equal = (cru_bpred) equal_vertices}},
		.destructors = {
		  .e_free = (cru_destructor) free_edge,
		  .v_free = (cru_destructor) free_vertex}}};

  struct cru_merger_s c = {
	 .me_kernel = {
		.v_op = {
		  .vertex = {
			 .map = (cru_top) vertex_map,
			 .reduction = (cru_bop) vertex_fusion,
			 .m_free = (cru_destructor) free_fused_vertex,
			 .r_free = (cru_destructor) free_fused_vertex}}},
	 .me_orders = {
		.v_order = {
		  .hash = (cru_hash) fused_vertex_hash,
		  .equal = (cru_bpred) fused_equal_vertices}},
	 .pruner = (cru_pruner) predicate,
	 .me_classifier = {
		.cl_prop = {
		  .incident = {
			 .map = (cru_top) incident_map,
			 .reduction = (cru_bop) incident_sum,
			 .vacuous_case = (cru_nop) incident_zero,
			 .m_free = (cru_destructor) free_incident_mapex,
			 .r_free = (cru_destructor) free_incident_redex},
		  .outgoing = {
			 .map = (cru_top) outgoing_map,
			 .reduction = (cru_bop) outgoing_sum,
			 .m_free = (cru_destructor) free_outgoing_redex,
			 .r_free = (cru_destructor) free_outgoing_redex},
		  .vertex = {
			 .map = (cru_top) common_property,
			 .m_free = (cru_destructor) free_property}},
		.cl_order = {
		  .hash = (cru_hash) property_hash,
		  .equal = (cru_bpred) equal_properties}}};

  err = 0;
  if ((argc > 1) ? (limit = strtoull (argv[1], NULL, 0)) : 0)
	 crudev_limit_allocations (limit, &err);
  if (! initialized (&edge_count, &edge_lock, &edge_lock_created, &err))
	 goto a;
  if (! initialized (&vertex_count, &vertex_lock, &vertex_lock_created, &err))
	 goto a;
  if (! initialized (&redex_count, &redex_lock, &redex_lock_created, &err))
	 goto a;
  g = cru_built (&b, vertex_of (0, &err), UNKILLABLE, LANES, &err);
  g = cru_merged (g, &c, UNKILLABLE, LANES, &err);
  v = valid (g, &err);
  cru_free_now (g, LANES, &err);
  nthm_sync (&err);
  if (! err)
	 err = (edge_count ? THE_FAIL(3985) : vertex_count ? THE_FAIL(3986) : redex_count ? THE_FAIL(3987) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3988);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(3989);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
