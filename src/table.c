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

#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include "brig.h"
#include "classes.h"
#include "edges.h"
#include "emu.h"
#include "errs.h"
#include "nodes.h"
#include "pack.h"
#include "table.h"
#include "wrap.h"

// aliases for inadequately descriptive or hard-to-spell types defined by the Judy array library

#ifdef JUDY
#define JUDY_ARRAY_KEY Word_t
#define JUDY_ARRAY_SIZE Word_t
#define JUDY_ARRAY_ENTRY Word_t
#define COTERMINAL_TABLE Pvoid_t
#define POINTER_TO_JUDY_ARRAY_ENTRY PWord_t
#endif

// --------------- packet tables ---------------------------------------------------------------------------




packet_list *
_cru_collision (index, seen, err)
	  uintptr_t index;
	  packet_table *seen;    // previously received packets
	  int *err;

	  // Return the address of a hash table bin given its index, which
	  // stores a list of colliding entries. If no entries have been
	  // stored previously in a bin corresponding to this hash, create
	  // a new empty bin in the table and return a pointer to it so the
	  // caller can assign to it.
{
#ifdef JUDY
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (seen ? 0 : IER(1686))
	 return NULL;
  p = PJERR;
  if (MEMAVAIL)
	 JLI(p, *seen, (JUDY_ARRAY_KEY) index);
  return (packet_list *) (((p == PJERR) ? RAISE(ENOMEM) : 0) ? NULL : p);
#else
  return (packet_list *) seen;                       // without judy arrays, the whole hash table has only one bin
#endif
}






packet_list
_cru_unpacked (seen)
	  packet_table seen;

	  // Consume a packet table and return the concatenation of its
	  // entries.
{
#ifdef JUDY
  JUDY_ARRAY_KEY index;
  POINTER_TO_JUDY_ARRAY_ENTRY p;
  JUDY_ARRAY_SIZE freed_size;
  packet_list f, b, r;

  if (! seen)
	 return NULL;
  r = NULL;
  index = 0;
  JLF(p, seen, index);
  while (p)
	 {
		if (! (f = (packet_list) *p))
		  goto a;
		for (b = f; b->next_packet; b = b->next_packet);
		b->next_packet = r;
		r = f;
	 a: JLN(p, seen, index);
	 }
  JLFA (freed_size, seen);
  return r;
#else
  return (packet_list) seen;
#endif
}










void
_cru_forget_collisions (seen, err)
	  packet_table seen;
	  int *err;

	  // Shallowly free a table of packet lists by freeing only the
	  // table and the packets but not their payloads.
{
#ifdef JUDY
  JUDY_ARRAY_KEY index;
  POINTER_TO_JUDY_ARRAY_ENTRY p;
  JUDY_ARRAY_SIZE freed_size;

  if (! seen)
	 return;
  index = 0;
  JLF(p, seen, index);
  while (p)
	 {
		_cru_nack ((packet_list) *p, err);
		JLN(p, seen, index);
	 }
  JLFA(freed_size, seen);
#else
  _cru_nack (seen, err);
#endif
}





// --------------- coterminal tables -----------------------------------------------------------------------





#ifdef JUDY

static void
unwind (e, b, p, index, t, err)
	  edge_list *e;
	  brigade *b;
	  POINTER_TO_JUDY_ARRAY_ENTRY p;
	  JUDY_ARRAY_KEY index;
	  COTERMINAL_TABLE t;
	  int *err;

	  // Free a coterminal table, and if necessary undo an unfinished
	  // marshall or rally to avoid a memory leak.
{
  JUDY_ARRAY_SIZE freed_size;

  if ((! e) ? IER(1687) : (! b) ? IER(1688) : ! *err)
	 goto a;
  *e = _cru_cat_edges (_cru_unbundled (*b), *e);
  *b = NULL;
  while (p)
	 {
		*e = _cru_cat_edges (*((edge_list *) p), *e);
		JLN(p, t, index);
	 }
 a: JLFA(freed_size, t);
}

#endif






brigade
_cru_marshalled (e, by_class, d, err)
	  edge_list *e;
	  int by_class;
	  cru_destructor d;
	  int *err;

	  // Make a brigade of edge lists such that coterminal edges share
	  // a bucket using Judy arrays if possible. The algorithm is to
	  // throw everything into a Judy array of edge lists indexed by
	  // their termini or terminus classes, and then to pull them out.
{
#ifdef JUDY
  brigade b;
  node_list o;
  COTERMINAL_TABLE t;
  JUDY_ARRAY_KEY index;
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (e ? 0 : IER(1689))
	 return NULL;
  for (t = NULL; *err ? NULL : *e;)
	 {
		if (((o = (by_class ? CLASS_OF((*e)->remote.node) : (*e)->remote.node))) ? 0 : IER(1690))
		  break;
		p = PJERR;
		if (MEMAVAIL)
		  JLI(p, t, (JUDY_ARRAY_KEY) o);
		if ((p == PJERR) ? RAISE(ENOMEM) : p ? 0 : IER(1691))
		  break;
		_cru_push_edge (_cru_popped_edge (e, err), (edge_list *) p, err);
	 }
  index = 0;
  JLF(p, t, index);
  for (b = NULL; *err ? NULL : p;)
	 {
		_cru_push_bucket ((edge_list) *p, &b, d, err);
		JLN(p, t, index);
	 }
  unwind (e, &b, p, index, t, err);
  return b;
#else
  return _cru_bucketed (e, by_class, err);         // equivalent result in quadratic time
#endif
}






brigade
_cru_rallied (h, r, e, err)
	  cru_hash h;
	  cru_bpred r;
	  edge_list *e;
	  int *err;

	  // Make a brigade of edge lists such that related edges share a
	  // bucket using Judy arrays if possible by the same algorithm as
	  // above but using a hash of the edge labels as an index and
	  // allowing for hash collisions.
{
#ifdef JUDY
  brigade b;
  COTERMINAL_TABLE t;
  JUDY_ARRAY_KEY index;
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  t = NULL;
  if ((! e) ? IER(1692) : h ? 0 : IER(1693))
	 return NULL;
  while (*err ? NULL : *e)
	 {
		p = PJERR;
		if (MEMAVAIL)
		  JLI(p, t, (JUDY_ARRAY_KEY) h ((*e)->label));
		if ((p == PJERR) ? RAISE(ENOMEM) : p ? 0 : IER(1694))
		  break;
		_cru_push_edge (_cru_popped_edge (e, err), (edge_list *) p, err);
	 }
  index = 0;
  JLF(p, t, index);
  for (b = NULL; *err ? NULL : p;)
	 {
		b = _cru_cat_brigades (_cru_bundled (r, (edge_list *) p, err), b);
		if (*err)
		  break;                // p hasn't been consumed
		JLN(p, t, index);
 	 }
  unwind (e, &b, p, index, t, err);
  return b;
#else
  return _cru_bundled (r, e, err);         // equivalent result in quadratic time
#endif
}





// --------------- multiset tables -------------------------------------------------------------------------




void
_cru_record_edge (h, e, l, t, err)
	  uintptr_t h;                       // hash value
	  cru_bpred e;                    // edge label equality
	  void *l;                           // edge label
	  multiset_table *t;
	  int *err;

	  // Include an edge in the table but don't bump the multiplicity.
{
#ifdef JUDY
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (t ? 0 : IER(1695))
	 return;
  p = PJERR;
  if (MEMAVAIL)
	 JLI(p, *t, (JUDY_ARRAY_KEY) h);
  if ((p == PJERR) ? RAISE(ENOMEM) : 0)
	 return;
  _cru_multirecord (e, l, (multiset_table_bin *) p, err);
#else
  _cru_multirecord (e, l, (multiset_table_bin *) t, err);
#endif
}







static multiset_table_bin
lookup (h, t, err)
	  uintptr_t h;
	  multiset_table t;
	  int *err;

	  // Return the edge multiset corresponding to a given hash.
{
#ifdef JUDY
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (! t)
	 return NULL;
  p = PJERR;
  if (MEMAVAIL)
	 JLG(p, t, (JUDY_ARRAY_KEY) h);
  return (((p == PJERR) ? RAISE(ENOMEM) : 0) ? NULL : p ? (multiset_table_bin) *p : NULL);
#else
  return (multiset_table_bin) t;
#endif
}






int
_cru_already_recorded (h, e, l, t, err)
	  uintptr_t h;
	  cru_bpred e;
	  void *l;
	  multiset_table t;
	  int *err;

	  // Return non-zero if an edge is in the table.
{
  multiset_table_bin p;
  int ux, ut;

  if ((! t) ? 1 : e ? 0 : IER(1696))
	 return 0;
  for (p = lookup (h, t, err); (! p) ? 0 : FAILED(e, l, p->edge_label); p = p->other_edges);
  return (*err ? 0 : ! ! p);
}






void
_cru_merge (t, q, e, err)
	  multiset_table *t;
	  multiset_table q;
	  cru_bpred e;
	  int *err;

	  // Merge two multiset tables and accumulate the multiplicities.
{
#ifdef JUDY
  JUDY_ARRAY_KEY h;
  POINTER_TO_JUDY_ARRAY_ENTRY p;
#endif

  if ((! q) ? 1 : t ? 0 : IER(1697))
	 return;
#ifdef JUDY
  h = 0;
  JLF(p, q, h);
  while (p ? (! *err) : 0)
	 {
		_cru_multimerge ((multiset_table_bin *) _cru_collision (h, (packet_table *) t, err), (multiset_table_bin *) p, e, err);
		JLN(p, q, h);
	 }
  _cru_discount (q);
#else
  _cru_multimerge ((multiset_table_bin *) t, (multiset_table_bin *) &q, e, err);
#endif
}






uintptr_t
_cru_multiplicity (h, e, l, t, err)
	  uintptr_t h;
	  cru_bpred e;
	  void *l;
	  multiset_table t;
	  int *err;

	  // Return the multiplicity of a multiset table entry.
{
  multiset_table_bin p;
  int ux, ut;

  if (e ? 0 : IER(1698))
	 return 0;
  for (p = lookup (h, t, err); (! p) ? 0: FAILED(e, l, p->edge_label); p = p->other_edges);
  return (p ? p->multiplicity : 0);
}






void
_cru_redirect (t, n, err)
	  multiset_table t;
	  node_list n;
	  int *err;

	  // Point the edges in a multiset table to a node n.
{
#ifdef JUDY
  JUDY_ARRAY_KEY i;
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (! t)
	 return;
  i = 0;
  JLF(p, t, i);
  while (p)
	 {
		_cru_multipoint ((multiset_table_bin) *p, n, err);
		JLN(p, t, i);
	 }
#else
  _cru_multipoint ((multiset_table_bin) t, n, err);
#endif
}






void
_cru_discount (t)
	  multiset_table t;

	  // Free a multiset table.
{
#ifdef JUDY
  JUDY_ARRAY_KEY i;
  JUDY_ARRAY_SIZE freed_size;
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (! t)
	 return;
  i = 0;
  JLF(p, t, i);
  while (p)
	 {
		_cru_multifree ((multiset_table_bin) *p);
		JLN(p, t, i);
	 }
  JLFA (freed_size, t);
#else
  _cru_multifree ((multiset_table_bin) t);
#endif
}





// --------------- node sets -------------------------------------------------------------------------------






int
_cru_member (node, seen)
	  node_list node;
	  node_set seen;

	  // Return non-zero if the given node is in the set.
{
#ifdef JUDY
  int bit;

  if (! seen)
	 return 0;
  J1T(bit, seen, (JUDY_ARRAY_KEY) node);
  return bit;
#else
  return _cru_member_nj (node, (node_set_nj) seen);
#endif
}






int
_cru_set_membership (node, seen, err)
	  node_list node;
	  node_set *seen;
	  int *err;

	  // Return non-zero if the incoming node is successfully stored in
	  // the set.
{
#ifdef JUDY
  int bit;

  if (seen ? 0 : IER(1699))
	 return 0;
  bit = JERR;
  if (MEMAVAIL)
	 J1S(bit, *seen, (JUDY_ARRAY_KEY) node);
  return ! ((! bit) ? IER(1700) : (bit == JERR) ? RAISE(ENOMEM) : 0);
#else
  return _cru_set_membership_nj (node, (node_set_nj *) seen, err);
#endif
}






void
_cru_clear_membership (node, seen)
	  node_list node;
	  node_set *seen;

	  // Remove the node from the set if it's present.
{
#ifdef JUDY
  int bit;       // ignored

  if (seen)
	 J1U(bit, *seen, (JUDY_ARRAY_KEY) node);
#else
  _cru_clear_membership_nj (node, (node_set_nj *) seen);
#endif
}






int
_cru_test_and_set_membership (node, seen, err)
	  node_list node;
	  node_set *seen;
	  int *err;

	  // Return non-zero if the incoming node has been previously
	  // stored in the set, and if not then return 0 and store it.
{
#ifdef JUDY
  int bit;

  if (seen ? 0 : IER(1701))
	 return 0;
  J1T(bit, *seen, (JUDY_ARRAY_KEY) node);
  if (bit)
	 return 1;
  bit = JERR;
  if (MEMAVAIL)
	 J1S(bit, *seen, (JUDY_ARRAY_KEY) node);
  if ((! bit) ? IER(1702) : (bit == JERR))
	 RAISE(ENOMEM);
  return 0;
#else
  return _cru_test_and_set_membership_nj (node, (node_set_nj *) seen, err);
#endif
}





void
_cru_forget_members (seen)
	  node_set seen;

	  // Free the record of previously stored nodes.
{
#ifdef JUDY
  JUDY_ARRAY_SIZE freed_size;

  if (seen)
	 J1FA (freed_size, seen);
#else
  _cru_forget_members_nj ((node_set_nj) seen);
#endif
}




// --------------- vertex maps -----------------------------------------------------------------------------






cru_class
_cru_image (m, v, err)
	  vertex_map m;
	  void *v;
	  int *err;

	  // Return the class corresponding to a vertex if any.
{
#ifdef JUDY
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (! m)
	 return NULL;
  p = PJERR;
  if (MEMAVAIL)
	 JLG(p, m, (JUDY_ARRAY_KEY) v);
  return (((p == PJERR) ? RAISE(ENOMEM) : 0) ? NULL : p ? (cru_class) *p : NULL);
#else
  return _cru_image_nj ((vertex_map_nj) m, v, err);
#endif
}






void
_cru_free_vertex_map (m, err)
	  vertex_map m;
	  int *err;

	  // Free a vertex map.
{
#ifdef JUDY
  JUDY_ARRAY_KEY i;
  JUDY_ARRAY_SIZE freed_size;
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if (! m)
	 return;
  i = 0;
  JLF(p, m, i);
  while (p)
	 {
		_cru_free_class ((cru_class) *p, err);
		JLN(p, m, i);
	 }
  JLFA (freed_size, m);
#else
  _cru_free_vertex_map_nj ((vertex_map_nj) m, err);
#endif
}







void
_cru_associate (m, v, h, err)
	  vertex_map *m;
	  void *v;
	  cru_class h;
	  int *err;

	  // Store an association of a class with a vertex in the map and
	  // consume the class.
{
#ifdef JUDY
  POINTER_TO_JUDY_ARRAY_ENTRY p;

  if ((! h) ? 1 : (h->classed != CLASS_MAGIC) ? IER(1703) : m ? 0 : IER(1704))
	 goto a;
  p = PJERR;
  if (MEMAVAIL)
	 JLI(p, *m, (JUDY_ARRAY_KEY) v);
  if ((p == PJERR) ? RAISE(ENOMEM) : (! p) ? IER(1705) : *p ? RAISE(CRU_PARDPV) : 0)
	 goto a;
  *p = (JUDY_ARRAY_ENTRY) h;
  return;
 a: _cru_free_class (h, err);
#else
  _cru_associate_nj ((vertex_map_nj *) m, v, h, err);
#endif
}
