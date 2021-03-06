// Create a hypercubic graph with exogenous vertices and edges, and
// fuse any vertices differing in only their least significant bit.
// Check that the result is half the size of the original. Exercise
// the merger property generator by needlessly inspecting the
// incident and outgoing edges from each vertex.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nthm/nthm.h>
#include <cru.h>
#include "readme.h"

#define VERTEX_MAGIC 1786766415
#define FUSED_VERTEX_MAGIC 1368742990
#define EDGE_MAGIC 1624623804
#define REDEX_MAGIC 2070276626
#define MAPEX_MAGIC 2051696076
#define PROPERTY_MAGIC 1265469016
#define INCIDENT_REDEX_MAGIC 2133525114
#define OUTGOING_REDEX_MAGIC 1234820649
#define INCIDENT_MAPEX_MAGIC 1239380273

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

  if ((r = (redex) malloc (sizeof (*r))) ? 0 : FAIL(3567))
	 return NULL;
  memset (r, 0, sizeof (*r));
  r->r_magic = REDEX_MAGIC;
  r->r_value = n;
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3568) : ++redex_count ? 0 : FAIL(3569))
	 goto a;
  if (! (pthread_mutex_unlock (&redex_lock) ? FAIL(3570) : 0))
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
  if ((! r) ? FAIL(3571) : (r->r_magic != REDEX_MAGIC) ? FAIL(3572) : 0)
	 return;
  r->r_magic = MUGGLE(150);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3573) : redex_count-- ? 0 : FAIL(3574))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3575) : 0)
	 return;
  free (r);
}







void
free_mapex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a mapex.
{
  if ((! r) ? FAIL(3576) : (r->r_magic != MAPEX_MAGIC) ? FAIL(3577) : 0)
	 return;
  r->r_magic = REDEX_MAGIC;
  free_redex (r, err);
}







void
free_property (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a property for graph merging
	  // This function cast to a cru_destructor is passed to
	  // the cru library as c.me_classifier.cl_prop.vertex.m_free in the
	  // merger c.
{
  if ((! r) ? FAIL(3578) : (r->r_magic != PROPERTY_MAGIC) ? FAIL(3579) : 0)
	 return;
  r->r_magic = MUGGLE(151);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3580) : redex_count-- ? 0 : FAIL(3581))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3582) : 0)
	 return;
  free (r);
}







void
free_incident_redex (r, err)
	  redex r;
	  int *err;

	  // Reclaim a redex created as a result of traversing the incident
	  // edges on a vertex during a merge operation. This function cast
	  // to a cru_destructor is passed to the cru library as
	  // c.me_classifier.cl_prop.incident.r_free in the merger c.
{
  if ((! r) ? FAIL(3583) : (r->r_magic != INCIDENT_REDEX_MAGIC) ? FAIL(3584) : 0)
	 return;
  r->r_magic = MUGGLE(152);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3585) : redex_count-- ? 0 : FAIL(3586))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3587) : 0)
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
  if ((! r) ? FAIL(3588) : (r->r_magic != INCIDENT_MAPEX_MAGIC) ? FAIL(3589) : 0)
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
  if ((! r) ? FAIL(3590) : (r->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3591) : 0)
	 return;
  r->r_magic = MUGGLE(153);
  if (pthread_mutex_lock (&redex_lock) ? FAIL(3592) : redex_count-- ? 0 : FAIL(3593))
	 return;
  if (pthread_mutex_unlock (&redex_lock) ? FAIL(3594) : 0)
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
  if ((! a) ? FAIL(3595) : (a->r_magic != PROPERTY_MAGIC) ? FAIL(3596) : 0)
	 return 0;
  if ((! b) ? FAIL(3597) : (b->r_magic != PROPERTY_MAGIC) ? FAIL(3598) : 0)
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

  if ((! r) ? GLOBAL_FAIL(3599) : (r->r_magic != PROPERTY_MAGIC) ? GLOBAL_FAIL(3600) : 0)
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

  if ((e = (edge) malloc (sizeof (*e))) ? 0 : FAIL(3601))
	 return NULL;
  memset (e, 0, sizeof (*e));
  e->e_magic = EDGE_MAGIC;
  e->e_value = n;
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3602) : ++edge_count ? 0 : FAIL(3603))
	 goto a;
  if (! (pthread_mutex_unlock (&edge_lock) ? FAIL(3604) : 0))
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
  if ((! e) ? FAIL(3605) : (e->e_magic != EDGE_MAGIC) ? FAIL(3606) : 0)
	 return;
  e->e_magic = MUGGLE(154);
  if (pthread_mutex_lock (&edge_lock) ? FAIL(3607) : edge_count-- ? 0 : FAIL(3608))
	 return;
  if (pthread_mutex_unlock (&edge_lock) ? FAIL(3609) : 0)
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

  if ((! e) ? GLOBAL_FAIL(3610) : (e->e_magic != EDGE_MAGIC) ? GLOBAL_FAIL(3611) : 0)
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
  if ((! a) ? FAIL(3612) : (a->e_magic != EDGE_MAGIC) ? FAIL(3613) : 0)
	 return 0;
  if ((! b) ? FAIL(3614) : (b->e_magic != EDGE_MAGIC) ? FAIL(3615) : 0)
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

  if ((v = (vertex) malloc (sizeof (*v))) ? 0 : FAIL(3616))
	 return NULL;
  memset (v, 0, sizeof (*v));
  v->v_magic = VERTEX_MAGIC;
  v->v_value = n;
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3617) : ++vertex_count ? 0 : FAIL(3618))
	 goto a;
  if (! (pthread_mutex_unlock (&vertex_lock) ? FAIL(3619) : 0))
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
  if ((! v) ? FAIL(3620) : (v->v_magic != VERTEX_MAGIC) ? FAIL(3621) : 0)
	 return;
  v->v_magic = MUGGLE(155);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3622) : vertex_count-- ? 0 : FAIL(3623))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3624) : 0)
	 return;
  free (v);
}








void
free_fused_vertex (v, err)
	  vertex v;
	  int *err;

	  // Reclaim a vertex resulting from a fusion operation.
{
  if ((! v) ? FAIL(3625) : (v->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3626) : 0)
	 return;
  v->v_magic = MUGGLE(156);
  if (pthread_mutex_lock (&vertex_lock) ? FAIL(3627) : vertex_count-- ? 0 : FAIL(3628))
	 return;
  if (pthread_mutex_unlock (&vertex_lock) ? FAIL(3629) : 0)
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
  if ((! a) ? FAIL(3630) : (a->v_magic != VERTEX_MAGIC) ? FAIL(3631) : 0)
	 return 0;
  if ((! b) ? FAIL(3632) : (b->v_magic != VERTEX_MAGIC) ? FAIL(3633) : 0)
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
  if ((! a) ? FAIL(3634) : (a->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3635) : 0)
	 return 0;
  if ((! b) ? FAIL(3636) : (b->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3637) : 0)
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

  if ((! v) ? GLOBAL_FAIL(3638) : (v->v_magic != VERTEX_MAGIC) ? GLOBAL_FAIL(3639) : 0)
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

  if ((! v) ? GLOBAL_FAIL(3640) : (v->v_magic != FUSED_VERTEX_MAGIC) ? GLOBAL_FAIL(3641) : 0)
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

  if ((! given_vertex) ? FAIL(3642) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3643) : 0)
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
  if (*err ? 1 : (! connecting_edge) ? FAIL(3644) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3645) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3646) : (local_vertex->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3647) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3648) : (remote_vertex->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3649) : 0)
	 return NULL;
  if ((local_vertex->v_value == remote_vertex->v_value) ? (! (connecting_edge->e_value)) : 0)
	 goto a;
  if ((((local_vertex->v_value ^ remote_vertex->v_value) << 1) != ( 1 << connecting_edge->e_value)) ? FAIL(3650) : 0)
	 return NULL;
 a: return redex_of (1, err);
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
  if (*err ? 1 : (! given_vertex) ? FAIL(3651) : (given_vertex->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3652) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(3653) : (edges_in->r_magic != REDEX_MAGIC) ? FAIL(3654) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(3655) : (edges_out->r_magic != REDEX_MAGIC) ? FAIL(3656) : 0)
	 return NULL;
  if ((edges_in->r_value != DIMENSION) ? FAIL(3657) : (edges_out->r_value != DIMENSION) ? FAIL(3658) : 0)
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
  if ((! l) ? 0 : (l->r_magic != REDEX_MAGIC) ? FAIL(3659) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != REDEX_MAGIC) ? FAIL(3660) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3661) : (s < (r ? r->r_value : 0)) ? FAIL(3662) : 0)
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
  if ((! l) ? 0 : (l->r_magic != INCIDENT_MAPEX_MAGIC) ? FAIL(3663) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != INCIDENT_REDEX_MAGIC) ? FAIL(3664) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3665) : (s < (r ? r->r_value : 0)) ? FAIL(3666) : 0)
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
  if ((! l) ? 0 : (l->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3667) : 0)
	 return NULL;
  if ((! r) ? 0 : (r->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3668) : 0)
	 return NULL;
  s = (l ? l->r_value : 0) + (r ? r->r_value : 0);
  if ((s < (l ? l->r_value : 0)) ? FAIL(3669) : (s < (r ? r->r_value : 0)) ? FAIL(3670) : 0)
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
  if ((cru_vertex_count (g, LANES, err) == (NUMBER_OF_VERTICES >> 1)) ? 0 : FAIL(3671))
	 return 0;
  if ((cru_edge_count (g, LANES, err) == ((DIMENSION * NUMBER_OF_VERTICES) >> 1)) ? 0 : FAIL(3672))
	 return 0;
  if ((r = (redex) cru_mapreduced (g, &m, UNKILLABLE, LANES, err)) ? 0 : FAIL(3673))
	 return 0;
  if ((r->r_magic == REDEX_MAGIC) ? 0 : FAIL(3674))
	 return 0;
  if (!(v = (r->r_value == NUMBER_OF_VERTICES >> 1)))
	 FAIL(3675);
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

  if (*err ? 1 : (! given_vertex) ? FAIL(3676) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3677) : 0)
	 return NULL;
  if ((! edges_in) ? FAIL(3678) : (edges_in->r_magic != INCIDENT_REDEX_MAGIC) ? FAIL(3679) : 0)
	 return NULL;
  if ((! edges_out) ? FAIL(3680) : (edges_out->r_magic != OUTGOING_REDEX_MAGIC) ? FAIL(3681) : 0)
	 return NULL;
  if ((edges_in->r_value != D) ? FAIL(3682) : (edges_out->r_value != D) ? FAIL(3683) : 0)
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

  if (*err ? 1 : (! connecting_edge) ? FAIL(3684) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3685) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3686) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3687) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3688) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3689) : 0)
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

  if (*err ? 1 : (! connecting_edge) ? FAIL(3690) : (connecting_edge->e_magic != EDGE_MAGIC) ? FAIL(3691) : 0)
	 return NULL;
  if ((! local_vertex) ? FAIL(3692) : (local_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3693) : 0)
	 return NULL;
  if ((! remote_vertex) ? FAIL(3694) : (remote_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3695) : 0)
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

  if (edges_in ? FAIL(3696) : edges_out ? FAIL(3697) : 0)
	 return NULL;
  if ((! given_vertex) ? FAIL(3698) : (given_vertex->v_magic != VERTEX_MAGIC) ? FAIL(3699) : 0)
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

  if ((! a) ? FAIL(3700) : (a->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3701) : 0)
	 return NULL;
  if ((! b) ? FAIL(3702) : (b->v_magic != FUSED_VERTEX_MAGIC) ? FAIL(3703) : 0)
	 return NULL;
  if ((a->v_value != b->v_value) ? FAIL(3704) : 0)
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
	 FAIL(3705);
  if (lock_created ? (*lock_created = 0) : 1)
	 FAIL(3706);
  if (*err ? 1 : pthread_mutexattr_init (&mutex_attribute) ? FAIL(3707) : 0)
	 return 0;
  if (!((pthread_mutexattr_settype (&mutex_attribute, PTHREAD_MUTEX_ERRORCHECK)) ? FAIL(3708) : 0))
	 *lock_created = ! (pthread_mutex_init (lock, &mutex_attribute) ? FAIL(3709) : 0);
  return ! ((pthread_mutexattr_destroy (&mutex_attribute)) ? FAIL(3710) : *err);
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
	 err = (edge_count ? THE_FAIL(3711) : vertex_count ? THE_FAIL(3712) : redex_count ? THE_FAIL(3713) : global_err);
 a: if ((! vertex_lock_created) ? 0 : pthread_mutex_destroy (&(vertex_lock)) ? (! err) : 0)
	 err = THE_FAIL(3714);
  if ((! edge_lock_created) ? 0 : pthread_mutex_destroy (&(edge_lock)) ? (! err) : 0)
	 err = THE_FAIL(3715);
  if (err ? 1 : (! v) ? 1 : ! crudev_all_clear (&err))
	 printf (err ? "%s failed\n%s\n" : "%s failed\n", argv[0], cru_strerror (err));
  else if ((argc > 1) ? (! limit) : 0)
	 printf ("%lu", crudev_allocations_performed (&err));
  else
	 printf ("%s detected no errors\n", argv[0]);
  EXIT;
}
