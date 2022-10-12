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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <nthm/nthm.h>
#include "classes.h"
#include "edges.h"
#include "errs.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "props.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"

#define CONTROL_ENTRY                  \
  if (err ? NULL : (err = &ignored))    \
    ignored = 0;                         \





// --------------- partition memory management -------------------------------------------------------------





static size_t
map_size (p, err)
	  unsigned p;
	  int *err;

	  // Compute the size of an array of vertex maps.
{
  vertex_map v;
  size_t result;

  if ((! p) ? IER(602) : ((result = p * sizeof (v)) < p) ? IER(603) : (result < sizeof (v)) ? IER(604) : 0)
	 return 0;
  return result;
}








cru_partition
_cru_partition (p, err)
	  unsigned p;
	  int *err;

	  // Allocate a partition with plurality p.
{
  size_t m;
  cru_partition h;

  if ((m = map_size (p, err)) ? *err : IER(605))
	 return NULL;
  if ((h = (cru_partition) _cru_malloc (sizeof (*h))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (h, 0, sizeof (*h));
  h->plurality = p;
  if (pthread_rwlock_init (&(h->loch), NULL) ? IER(606) : 0)
	 goto a;
  if ((h->maps = (vertex_map *) _cru_malloc (m)) ? 0 : RAISE(ENOMEM))
	 goto a;
  memset (h->maps, 0, m);
  h->parted = PARTITION_MAGIC;
  return h;
 a: _cru_free (h);
  return NULL;
}









static void
class_freeing_slacker (h)
	  cru_partition h;

	  // Free a partition and throw a global error if
	  // unsuccessful. This function is meant to be run in the
	  // background as an nthm_slacker.
{
  unsigned p;
  int err;

  if ((err = (h ? 0 : THE_IER(607))))
	 goto a;
  if ((err = ((h->parted == PARTITION_MAGIC) ? 0 : THE_IER(608))))
	 goto a;
  if ((err = (h->maps ? (h->plurality ? 0 : THE_IER(609)) : THE_IER(610))))
	 goto a;
  for (p = h->plurality; p--;)
	 _cru_free_vertex_map (h->maps[p], &err);
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, map_size (h->plurality, &err), h->maps);
#else
  free (h->maps);
#endif
  if (pthread_rwlock_destroy (&(h->loch)))
	 err = THE_IER(611);
  h->parted = MUGGLE(1);
  _cru_free (h);
 a: _cru_globally_throw (err);
}







// --------------- class memory management -----------------------------------------------------------------







static cru_class
new_class (err)
	  int *err;

	  // Allocate a class.
{
  cru_class h;

  if ((h = (cru_class) _cru_malloc (sizeof (*h))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memset (h, 0, sizeof (*h));
  h->classed = CLASS_MAGIC;
  return h;
}







void
_cru_free_class (h, err)
	  cru_class h;
	  int *err;

	  // Free a class.
{
  if ((! h) ? 1 : (h->classed != CLASS_MAGIC) ? IER(612) : 0)
	 return;
  if ((h->references)--)
	 return;
  h->classed = MUGGLE(2);
  _cru_free (h);
}









static cru_class
copied_class (h, err)
	  cru_class h;
	  int *err;

	  // Return a shared copy of a class.
{
  if ((! h) ? 1 : (h->classed != CLASS_MAGIC) ? IER(613) : 0)
	 return NULL;
  if (++(h->references))
	 return h;
  h->classed = MUGGLE(3);
  return NULL;
}







// --------------- partition building ----------------------------------------------------------------------








static void *
classifying_task (source, err)
	  port source;
	  int *err;

	  // Build this worker's share of the partition in the router by
	  // associating every incoming vertex with a class depending on
	  // the hash of its property.
{
#define UNEQUAL(a,b) \
(*err ? 0 : (! b) ? (! IER(614)) : (a->vertex_property == b->vertex_property) ? 0 : \
FAILED(r->merger.me_classifier.cl_order.equal, a->vertex_property, b->vertex_property))

  packet_table collisions;    // previous incoming packets
  packet_list incoming;
  int ux, ut, unequal;
  unsigned sample;
  intptr_t status;
  packet_list *c;
  packet_list *p;
  vertex_map *m;
  node_set seen;
  packet_pod d;
  node_list n;
  cru_hash h;
  int killed;                 // non-zero when the job is killed
  router r;

  sample = 0;
  seen = NULL;
  collisions = NULL;
  if ((! source) ? IER(615) : (source->gruntled != PORT_MAGIC) ? IER(616) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(617) : (r->valid != ROUTER_MAGIC) ? IER(618) : (killed = 0))
	 goto a;
  if ((! (d = source->peers)) ? IER(619) : (r->tag != CLU) ? IER(620) : 0)
	 goto b;
  if ((! (r->partition)) ? IER(621) : (! (r->partition->maps)) ? IER(622) : 0)
	 goto b;
  m = &(r->partition->maps[source->own_index]);
  if (r->merger.me_classifier.cl_order.equal ? 0 : IER(623))
	 goto b;
  h = r->merger.me_classifier.cl_order.hash;
  if (h ? 0 : r->ro_sig.destructors.v_free ? IER(624) : ! (h = (cru_hash) _cru_scalar_hash))
	 goto b;
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(2);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed ? 1 : ((n = incoming->receiver)) ? _cru_member (n, seen) : IER(625))
		  goto c;
		if (! (c = _cru_collision (incoming->hash_value, &collisions, err)))
		  goto c;
		if (*c)
		  for (; (unequal = UNEQUAL(n, (*c)->receiver)) ? (*c)->next_packet : NULL; c = &((*c)->next_packet));
		if (! (_cru_scattered_by_hashes (n, h, d, BY_CLASS, err) ? _cru_set_membership (n, &seen, err) : 0))
		  goto c;
		if (*c ? (! unequal) : 0)
		  goto d;
		_cru_associate (m, n->vertex, new_class (err), err);    // first vertex associated with this class
		p = (*c ? &((*c)->next_packet) : c);
		*p = _cru_popped_packet (&incoming, err);
		continue;
	 d: if ((*c)->receiver ? 1 : ! IER(626))
		  _cru_associate (m, n->vertex, copied_class (_cru_image (*m, (*c)->receiver->vertex, err), err), err);
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
 	 }
  _cru_nack (_cru_unpacked (collisions), err);
  _cru_forget_members (seen);
 a: status = *err;
  return (void *) status;
 b: return _cru_abort_status (source, d, err);
}








cru_partition
_cru_partition_of (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Concurrently derive a partition from the given router and
	  // consume the router.
{
  cru_hash h;
  node_list b;
  cru_partition result;

  result = NULL;
  _cru_disable_killing (k, err);
  if ((! r) ? 1 : (r->valid != ROUTER_MAGIC) ? IER(627) : (! g) ? IER(628) : (! (b = g->base_node)) ? IER(629) : 0)
	 goto a;
  if ((r->tag != CLU) ? IER(630) : 0)
	 goto a;
  h = r->merger.me_classifier.cl_order.hash;
  if (h ? 0 : r->ro_sig.destructors.v_free ? RAISE(CRU_UNDHSH) : ! (h = (cru_hash) _cru_scalar_hash))
	 goto a;
  if (! _cru_set_properties (g, k, r, err))
	 {
		cru_free_partition (r->partition, NOW, err);
		goto a;
	 }
  if (_cru_status_launched (k, b, h (b->vertex_property), _cru_reset (r, (task) classifying_task, err), err))
	 if ((result = r->partition) ? 1 : ! IER(631))
		goto b;
  cru_free_partition (r->partition, (*err == CRU_INTKIL) ? LATER : NOW, err);
 b: _cru_unset_properties (g, r, err);
 a: _cru_free_router (r, err);
  return result;
}





// --------------- public API ------------------------------------------------------------------------------











uintptr_t
cru_class_size (c, err)
	  cru_class c;
	  int *err;

	  // Return the number of vertices associated with a class.
{
  int ignored;
  uintptr_t s;

  CONTROL_ENTRY;
  if (c ? ((c->classed == CLASS_MAGIC) ? 0 : RAISE(CRU_BADCLS)) : RAISE(CRU_NULCLS))
	 return 0;
  return ((((s = c->references + 1) ? 0 : IER(632)) ? (c->classed = MUGGLE(4)) : 0) ? 0 : s);
}










void
cru_free_partition (p, now, err)
	  cru_partition p;
	  int now;
	  int *err;

	  // If now is non-zero, free a partition and block until
	  // freed. Otherwise, free it in the background and return
	  // immediately. In either case, the operation is single threaded
	  // because there would be too much overhead in lockng the
	  // reference counts if multiple maps were freed concurrently.
{
  unsigned i;
  int ignored;

  CONTROL_ENTRY;
  if (p ? ((p->parted == PARTITION_MAGIC) ? 0 : RAISE(CRU_BADPRT)) : 1)
	 return;
  if (((!(p->maps)) != ! (p->plurality)) ? RAISE(CRU_BADPRT) : 0)
	 return;
  if (!(p->maps))
	 goto a;
  if (now ? 0 : NOMEM ? 0 : nthm_send ((nthm_slacker) class_freeing_slacker, p, err))
	 return;
  for (i = p->plurality; i--;)
	 _cru_free_vertex_map (p->maps[i], err);
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, map_size (p->plurality, err), p->maps);
#else
  free (p->maps);
#endif
 a: if (pthread_rwlock_destroy (&(p->loch)))
	 IER(633);
  p->parted = MUGGLE(5);
  _cru_free (p);
}








cru_class
cru_class_of (p, x, err)
	  cru_partition p;
	  cru_vertex x;
	  int *err;

	  // Return the class associated with a vertex x by the partition p.
{
  int ignored;
  cru_class i;
  unsigned n;

  CONTROL_ENTRY;
  if (p ? ((p->parted == PARTITION_MAGIC) ? 0 : RAISE(CRU_BADPRT)) : RAISE(CRU_PARVNF))
	 return NULL;
  for (n = 0; n < p->plurality; n++)
	 if ((i = _cru_image (p->maps[n], x, err)))
		break;
  if (i ? 0 : (*err == ENOMEM) ? 1 : RAISE(CRU_PARVNF))
	 return NULL;
  if ((pthread_rwlock_rdlock (&(p->loch)) ? IER(634) : 0) ? (p->parted = MUGGLE(6)) : 0)
	 return NULL;
  while (i->superclass)
	 i = i->superclass;
  if ((pthread_rwlock_unlock (&(p->loch)) ? IER(635) : 0) ? (p->parted = MUGGLE(7)) : 0)
	 return NULL;
  return i;
}







int
cru_united (p, x, y, err)
	  cru_partition p;
	  cru_class x;
	  cru_class y;
	  int *err;

	  // Unite two equivalence classes and return non-zero if
	  // successful. Effects are undefined if the two classes aren't
	  // both associated with the given partition.
{
  int ignored, dblx;
  uintptr_t xs, ys;
  cru_class xl, yl, s;

  CONTROL_ENTRY;
  if ((((xl = x)) ? ((yl = y)) : NULL) ? 0 : RAISE(CRU_NULCLS))
	 return 0;
  if (p ? 0 : RAISE(CRU_NULPRT))
	 return 0;
  if ((p->parted == PARTITION_MAGIC) ? 0 : RAISE(CRU_BADPRT))
	 return 0;
  if ((pthread_rwlock_wrlock (&(p->loch)) ? IER(636) : 0) ? (p->parted = MUGGLE(8)) : 0)
	 return 0;
  dblx = 0;
  for (xs = 0; ((x->classed != CLASS_MAGIC) ? ((dblx = CRU_BADCLS)) : 0) ? NULL : x->superclass; x = x->superclass)
	 xs += x->references;
  for (ys = 0; ((y->classed != CLASS_MAGIC) ? ((dblx = CRU_BADCLS)) : dblx) ? NULL : y->superclass; y = y->superclass)
	 ys += y->references;
  if (dblx ? 1 : (x == y))
	 goto a;
  if (xs < ys)
	 {
		for (; ((s = xl->superclass)); xl = s)
		  xl->superclass = y;
		x->superclass = y;
	 }
  else
	 {
		for (; ((s = yl->superclass)); yl = s)
		  yl->superclass = x;
		y->superclass = x;
	 }
 a: RAISE(dblx);
  if ((pthread_rwlock_unlock (&(p->loch)) ? IER(637) : 0) ? (p->parted = MUGGLE(9)) : 0)
	 return 0;
  return ! dblx;
}

