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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "classes.h"
#include "cthread.h"
#include "edges.h"
#include "errs.h"
#include "graph.h"
#include "nodes.h"
#include "launch.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "repl.h"
#include "route.h"
#include "wrap.h"

// default values for unspecified router fields

#define NOSIG NULL
#define NOFIL NULL
#define NOBUI NULL
#define NOIND NULL
#define NOMUT NULL
#define NOCOM NULL
#define NOCLU NULL
#define NOMAP NULL
#define NOEXT NULL
#define NOSPL NULL
#define NOPOS NULL
#define NOCRO NULL
#define NOFAB NULL


// --------------- initialization --------------------------------------------------------------------------



static size_t
pointer_array_size (l, err)
	  unsigned l;
	  int *err;

	  // Return the size in bytes of an array of l pointers.
{
  size_t size;
  void *pointer;

  if (((size = (l * sizeof (pointer))) < l) ? IER(1440) : (size < sizeof (pointer)) ? IER(1441) : 0)
	 return 0;
  return size;
}





static size_t
thread_id_array_size (l, err)
	  unsigned l;
	  int *err;

	  // Return the size in bytes of an array of l thread ids.
{
  size_t size;
  pthread_t thread;

  if (((size = (l * sizeof (thread))) < l) ? IER(1442) : (size < sizeof (thread)) ? IER(1443) : 0)
	 return 0;
  return size;
}





static router
router_of (b, w, m, c, f, y, i, x, n, p, e, a, t, s, lanes, err)
	  cru_builder b;
	  cru_mutator w;
	  cru_mapreducer m;
	  cru_merger c;
	  cru_filter f;
	  cru_composer y;
	  cru_inducer i;
	  cru_stretcher x;
	  cru_splitter n;
	  cru_postponer p;
	  cru_crosser e;
	  cru_fabricator a;
	  task t;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Create a new packet router.
{
// disallow attacks from malicious code attempting to create silly numbers of threads
#define THREAD_LIMIT 1024

  size_t port_size, thread_size;
  unsigned o;
  router r;
  int u;

  r = NULL;
  if (NPROC ? (lanes > NPROC) : 0)
	 lanes = NPROC;
  else if (NPROC ? 0 : (lanes > THREAD_LIMIT))
	 lanes = THREAD_LIMIT;
  port_size = pointer_array_size (lanes, err);
  thread_size = thread_id_array_size (lanes, err);
  if (*err ? 1 : (! lanes) ? IER(1444) : 0)
	 goto a;
  if ((r = (router) _cru_malloc (sizeof (*r))) ? 0 : RAISE(ENOMEM))
	 goto a;
  memset (r, 0, sizeof (*r));
  r->lanes = lanes;
  if (s)
	 memcpy (&(r->ro_sig), s, sizeof (r->ro_sig));
  if (_cru_mutex_init (&(r->lock), err))
	 goto b;
  if (_cru_cond_init (&(r->transition), err))
	 goto c;
  if (((r->threads = (pthread_t *) _cru_malloc (thread_size))) ? 0 : RAISE(ENOMEM))
	 goto d;
  memset (r->threads, 0, thread_size);
  if ((u = ((r->ports = (port *) _cru_malloc (port_size)) ? 0 : RAISE(ENOMEM))))
	 goto d;
  memset (r->ports, 0, port_size);
  if ((f ? (r->tag = FIL) : 0) ? (u++ ? IER(1445) : ! memcpy (&(r->filter), f, sizeof (r->filter))) : 0)
	 goto d;
  if ((w ? (r->tag = MUT) : 0) ? (u++ ? IER(1446) : ! memcpy (&(r->mutator), w, sizeof (r->mutator))) : 0)
	 goto d;
  if ((i ? (r->tag = IND) : 0) ? (u++ ? IER(1447) : ! memcpy (&(r->inducer), i, sizeof (r->inducer))) : 0)
	 goto d;
  if ((b ? (r->tag = BUI) : 0) ? (u++ ? IER(1448) : ! memcpy (&(r->builder), b, sizeof (r->builder))) : 0)
	 goto d;
  if ((e ? (r->tag = CRO) : 0) ? (u++ ? IER(1449) : ! memcpy (&(r->crosser), e, sizeof (r->crosser))) : 0)
	 goto d;
  if ((x ? (r->tag = EXT) : 0) ? (u++ ? IER(1450) : ! memcpy (&(r->stretcher), x, sizeof (r->stretcher))) : 0)
	 goto d;
  if ((n ? (r->tag = SPL) : 0) ? (u++ ? IER(1451) : ! memcpy (&(r->splitter), n, sizeof (r->splitter))) : 0)
	 goto d;
  if ((y ? (r->tag = COM) : 0) ? (u++ ? IER(1452) : ! memcpy (&(r->composer), y, sizeof (r->composer))) : 0)
	 goto d;
  if ((p ? (r->tag = POS) : 0) ? (u++ ? IER(1453) : ! memcpy (&(r->postponer), p, sizeof (r->postponer))) : 0)
	 goto d;
  if ((c ? (r->tag = CLU) : 0) ? (u++ ? IER(1454) : ! memcpy (&(r->merger), c, sizeof (r->merger))) : 0)
	 goto d;
  if ((m ? (r->tag = MAP) : 0) ? (u++ ? IER(1455) : ! memcpy (&(r->mapreducer), m, sizeof (r->mapreducer))) : 0)
	 goto d;
  if ((a ? (r->tag = FAB) : 0) ? (u++ ? IER(1456) : ! memcpy (&(r->fabricator), a, sizeof (r->fabricator))) : 0)
	 goto d;
  r->work = t;
  r->valid = ROUTER_MAGIC;
  for (o = 0; o < r->lanes; o++)
	 if (!(r->ports[o] = _cru_new_port (o, r, err)))
		goto d;
  return r;
 d: _cru_free_router (r, err);
  return NULL;
 c: pthread_mutex_destroy (&(r->lock));
 b: r->valid = MUGGLE(52);
  _cru_free (r);
 a: return NULL;
}








static void
plan (p, r, err)
	  cru_plan p;
	  router r;
	  int *err;

	  // Enable an exponential backoff protocol if a traversal order is
	  // specified.
{
  uintptr_t i;

  if ((! p) ? IER(1457) : (! r) ? IER(1458) : (r->ports ? 0 : IER(1459)) ? (r->valid = MUGGLE(53)) : 0)
	 return;
  memcpy (&(r->ro_plan), p, sizeof (r->ro_plan));
  if (! (p->remote_first ? 1 : p->local_first))
	 return;
  for (i = 0; i < r->lanes; i++)
	 {
		if ((r->ports[i] ? 0 : IER(1460)) ? (r->valid = MUGGLE(54)) : 0)
		  return;
		(r->ports[i])->backoff = 1;
	 }
}







router
_cru_mutating_router (m, s, lanes, err)
	  cru_mutator m;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a mutator.
{
  router r;

  r = router_of (NOBUI, m, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
  if (r ? m : NULL)
	 plan (&(m->mu_plan), r, err);
  return r;
}









router
_cru_inducing_router (i, s, lanes, err)
	  cru_inducer i;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with an inducer.
{
  router r;
  struct cru_plan_s p;

  r = router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, i, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
  if ((! r) ? 1 : ! i)
	 goto a;
  memset (&p, 0, sizeof (p));
  p.remote_first = 1;
  memcpy (&(p.zone), &(i->in_zone), sizeof (p.zone));
  plan (&p, r, err);
 a: return r;
}










router
_cru_mapreducing_router (m, s, lanes, err)
	  cru_mapreducer m;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a mapreducer.
{
  router r;

  r = router_of (NOBUI, NOMUT, m, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
  if (r ? m : NULL)
	 memcpy (&(r->ro_plan.zone), &(m->ma_zone), sizeof (r->ro_plan.zone));
  return r;
}









router
_cru_filtering_router (f, s, lanes, err)
	  cru_filter f;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a filter.
{
  router r;

  r = router_of (NOBUI, NOMUT, NOMAP, NOCLU, f, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
  if (r ? f : NULL)
	 memcpy (&(r->ro_plan.zone), &(f->fi_zone), sizeof (r->ro_plan.zone));
  return r;
}









router
_cru_composing_router (y, s, lanes, err)
	  cru_composer y;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a composer.
{
  return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, y, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
}








router
_cru_stretching_router (x, s, lanes, err)
	  cru_stretcher x;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with an stretcher.
{
  return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, x, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
}








router
_cru_splitting_router (x, s, lanes, err)
	  cru_splitter x;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with an stretcher.
{
  return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, x, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
}







router
_cru_postponing_router (p, s, lanes, err)
	  cru_postponer p;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with an postponer.
{
  return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, p, NOCRO, NOFAB, NO_TASK, s, lanes, err);
}










router
_cru_classifying_router (c, b, s, lanes, err)
	  cru_classifier c;
	  node_list b;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a classifier and a partition.
{
  struct cru_merger_s o;
  cru_partition h;
  router r;

  if ((! c) ? IER(1461) : (! s) ? IER(1462) : 0)
	 return NULL;
  if (! (h = _cru_partition (lanes, err)))
	 return NULL;
  memset (&o, 0, sizeof (o));
  if (c)
	 memcpy (&(o.me_classifier), c, sizeof (o.me_classifier));
  if (! (r = _cru_merging_router (&o, s, lanes, err)))
	 goto a;
  r->base_register = b;
  r->partition = h;
  return r;
 a: cru_free_partition (h, NOW, err);
  return NULL;
}







router
_cru_merging_router (c, s, lanes, err)
	  cru_merger c;
	  cru_sig s;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a merger.
{
  return router_of (NOBUI, NOMUT, NOMAP, c, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, NO_TASK, s, lanes, err);
}







router
_cru_fabricating_router (a, lanes, err)
	  cru_fabricator a;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a fabricator.
{
  router r;

  r = router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, a, NO_TASK, NOSIG, lanes, err);
  if (r ? a : NULL)
	 memcpy (&(r->ro_sig), &(a->fa_sig), sizeof (r->ro_sig));
  return r;
}




// --------------- initialization with tasks ---------------------------------------------------------------






router
_cru_reset (r, t, err)
	  router r;
	  task t;
	  int *err;

	  // Reinitialize a packet router between passes for algorithms
	  // that need persistent storage, such as reachability analysis.
{
  uintptr_t i;
  port s;

  if ((! r) ? IER(1463) : (r->valid != ROUTER_MAGIC) ? IER(1464) : r->ports ? 0 : IER(1465))
	 return NULL;
  for (i = 0; i < r->lanes; i++)
	 if ((s = r->ports[i]) ? ((s->gruntled == PORT_MAGIC) ? 0 : IER(1466)) : IER(1467))
		r->valid = MUGGLE(55);
	 else
		{
		  s->dismissed = s->waiting = 0;
		  s->backoff = (s->backoff ? 1 : 0);
		}
  r->quiescent = 0;
  r->running = 0;
  r->ro_status = 0;
  r->killed = 0;
  r->work = t;
  return r;
}










router
_cru_router (t, lanes, err)
	  task t;
	  unsigned lanes;
	  int *err;

	  // Allocate an unconfigured router.
{
 return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, t, NOSIG, lanes, err);
}








router
_cru_razing_router (z, t, lanes, err)
	  cru_destructor_pair z;
	  task t;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with just a destructors and a task.
{
  struct cru_sig_s s;

  memset (&s, 0, sizeof (s));
  if (z)
	 memcpy (&(s.destructors), z, sizeof (s.destructors));
  return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, t, &s, lanes, err);
}








router
_cru_building_router (b, t, lanes, err)
	  cru_builder b;
	  task t;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a builder.
{
  cru_sig s;
  router r;

  s = (b ? &(b->bu_sig) : NULL);
  r = router_of (b, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, NOCRO, NOFAB, t, s, lanes, err);
  if (r)
	 r->ro_store = (b ? b->attribute : NULL);
  return r;
}







router
_cru_crossing_router (c, t, lanes, err)
	  cru_crosser c;
	  task t;
	  unsigned lanes;
	  int *err;

	  // Allocate a router with a builder.
{
  cru_sig s;

  s = (c ? &(c->cr_sig) : NULL);
  return router_of (NOBUI, NOMUT, NOMAP, NOCLU, NOFIL, NOCOM, NOIND, NOEXT, NOSPL, NOPOS, c, NOFAB, t, s, lanes, err);
}







// --------------- memory management -----------------------------------------------------------------------





extern router
_cru_stored (g, r, err)
	  cru_graph g;
	  router r;
	  int *err;

	  // Store the user-defined storage of a graph in a router.
{
  if ((! g) ? IER(1468) : (g->glad != GRAPH_MAGIC) ? IER(1469) : (! r) ? 1 : (r->valid != ROUTER_MAGIC) ? IER(1470) : 0)
	 return NULL;
  r->ro_store = g->g_store;
  return r;
}







router
_cru_shared (r)
	  router r;

	  // Make a shared copy of a router.
{
  if (r)
	 (r->shared)++;
  return r;
}








void
_cru_free_router (r, err)
	  router r;
	  int *err;

	  // Tear down a router.
{
  uintptr_t o;

  if (r ? (r->shared)-- : 1)
	 return;
  if (r->threads)
#ifdef WRAP
	 _cru_free_wrapper (__LINE__, __FILE__, thread_id_array_size (r->lanes, err), r->threads);
#else
	 free (r->threads);
#endif
  if (! (r->ports))
	 goto a;
  for (o = 0; o < r->lanes; o++)
	 _cru_free_port (&(r->ports[o]), err);
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pointer_array_size (r->lanes, err), r->ports);
#else
  free (r->ports);
#endif
 a: if (pthread_mutex_destroy (&(r->lock)))
	 IER(1471);
  if (pthread_cond_destroy (&(r->transition)))
	 IER(1472);
  RAISE(r->ro_status);
  if (*err ? 0 : r->killed)
	 RAISE(CRU_INTKIL);
  r->ro_status = THE_IER(1473);
  r->valid = MUGGLE(56);
  _cru_free (r);
}







void
sweep (s, z, err)
	  port s;
	  cru_destructor_pair z;
	  int *err;

	  // Free the deleted nodes in the port.
{
  if ((! s) ? IER(1474) : (s->gruntled != PORT_MAGIC) ? IER(1475) : (! z) ? IER(1476) : 0)
	 return;
  _cru_free_edges_and_labels (z->e_free, s->disconnections, err);
  _cru_free_nodes (s->survivors, z, err);
  _cru_free_nodes (s->deletions, z, err);
  s->survivors = s->deletions = NULL;
  s->disconnections = NULL;
}







static void *
sweeping_task (source, err)
	  port source;
	  int *err;

	  // Free the deleted nodes safely and concurrently.
{
  packet_list incoming;
  intptr_t status;
  router r;

  if ((! source) ? IER(1477) : (source->gruntled != PORT_MAGIC) ? IER(1478) : ! _cru_pingback (source, err))
	 goto a;
  if ((! (r = source->local)) ? IER(1479) : (r->valid != ROUTER_MAGIC) ? IER(1480) : 0)
	 goto a;
  sweep (source, &(r->ro_sig.destructors), err);
 a: status = *err;
  return (void *) status;
}







void
_cru_sweep (r, err)
	  router r;
	  int *err;

	  // Tear down a router and the graph nodes stored in its
	  // ports. Prefer to do it concurrently if there is significant
	  // storage to reclaim.
{
  int sweepable;
  unsigned i;
  port s;

  if (r ? r->shared : 0)
	 goto a;
  if ((! r) ? 1 : (r->valid != ROUTER_MAGIC) ? IER(1481) : r->ports ? 0 : IER(1482))
	 goto a;
  sweepable = 0;
  for (i = 0; sweepable ? 0 : (i < r->lanes); i++)
	 if (((s = r->ports[i])) ? 1 : ! IER(1483))
		sweepable = (s->survivors ? 1 : s->deletions ? 1 : ! ! (s->disconnections));
  if (! sweepable)
	 goto a;
  if ((r->lanes == 1) ? 0 : (! *err) ? 1 : (*err == CRU_INTKIL))
	 if (_cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) sweeping_task, err)), err))
		goto a;
  for (i = 0; i < r->lanes; i++)
	 sweep (r->ports[i], &(r->ro_sig.destructors), err);
 a: _cru_free_router (r, err);
}
