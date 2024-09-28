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

#include <string.h>
#include <errno.h>
#include "brig.h"
#include "defo.h"
#include "duplex.h"
#include "edges.h"
#include "errs.h"
#include "getset.h"
#include "graph.h"
#include "killers.h"
#include "launch.h"
#include "merge.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "props.h"
#include "repl.h"
#include "route.h"
#include "scatter.h"
#include "stretch.h"
#include "table.h"
#include "wrap.h"




// --------------- vertex fusing ---------------------------------------------------------------------------




static int
assimilated (incoming, extant_class, err)
	  packet_list incoming;
	  packet_list extant_class;
	  int *err;

	  // Assume the incoming packet carries a node belonging to a class
	  // already created and push it into the tail of the the nodes in
	  // the class. Pushing into the tail is necessary to preserve the
	  // base node in the graph.
{
  if ((! extant_class) ? IER(1136) : (! incoming) ? IER(1137) : (! (incoming->receiver)) ? IER(1138) : 0)
	 return 0;
  if ((incoming == extant_class) ? IER(1139) : extant_class->receiver ? 0 : IER(1140))
	 return 0;
  _cru_pushed_node (_cru_half_severed (incoming->receiver), &(extant_class->receiver->next_node), err);
  incoming->receiver = NULL;
  return 1;
}





static void
fuse_vertices (incoming, c, s, err)
	  packet_list incoming;
	  cru_merger c;
	  cru_sig s;
	  int *err;

	  // Given a packet list in which each receiver is the list of
	  // nodes in an equivalence class, reduce the vertices to their
	  // composite vertex, leave the composite vertex at the head of
	  // the list, reclaim all of the original vertices, and point the
	  // class of each node to the head node, which will remain after
	  // the others are pruned. Classes have to be written with locking
	  // because other workers might need to read them in the course of
	  // error recovery.
{
  node_list n;
  void *composite_vertex;

  if ((! c) ? IER(1141) : (! s) ? IER(1142) : 0)
	 return;
  for (; incoming; incoming = incoming->next_packet)
	 if (incoming->receiver ? 1 : ! IER(1143))
		{
		  if (*err ? 1 : (c->me_kernel.v_op.vertex.map != _cru_undefined_top) ? 1 : incoming->receiver->next_node ? IER(1144) : 0)
			 {
				composite_vertex = (*err ? NULL : _cru_reduced_nodes (&(c->me_kernel.v_op), incoming->receiver, err));
				_cru_free_vertices (incoming->receiver, s->destructors.v_free, err);
				incoming->receiver->vertex = composite_vertex;
			 }
		  for (n = incoming->receiver; n; n = n->next_node)
			 _cru_write ((void **) &(n->class), incoming->receiver, err);
		}
}




// --------------- vertex classifying ----------------------------------------------------------------------







static void *
vertex_merging_task (s, err)
	  port s;
	  int *err;

	  // Collect incoming vertices into equivalence classes. Leave each
	  // class as a list disconnected from the rest of the graph with
	  // the fused vertex at the head and the rest of the vertices
	  // cleared.
{
#define UNEQUAL(a,b) \
(*err ? 0 : (! b) ? (! IER(1145)) : (a->vertex_property == b->vertex_property) ? 0 : \
FAILED(r->merger.me_classifier.cl_order.equal, a->vertex_property, b->vertex_property))

#define RECORDED(x) (_cru_member (x, seen) ? 1 : (*err ? 0 : ! killed) ? 0 : _cru_listed (x, s->deletions))

  struct packet_list_s buffer;   // statically allocated packet
  packet_table collisions;       // previous incoming packets
  packet_list incoming;
  unsigned sample;
  intptr_t status;
  packet_list *c;
  packet_list *p;
  node_set seen;
  packet_pod d;
  node_list n;
  int ux, ut;
  int unequal;
  int killed;                    // non-zero when the job is killed
  router r;
  void *v;

  sample = 0;
  seen = NULL;
  collisions = NULL;
  memset (&buffer, 0, sizeof (buffer));
  if ((! s) ? IER(1146) : (s->gruntled != PORT_MAGIC) ? IER(1147) : 0)
	 goto a;
  if ((!(r = s->local)) ? IER(1148) : (r->valid != ROUTER_MAGIC) ? IER(1149) : (killed = 0))
	 goto a;
  if ((! (d = s->peers)) ? IER(1150) : (r->tag != CLU) ? IER(1151) : 0)
	 goto b;
  if (r->merger.me_classifier.cl_order.equal ? 0 : IER(1152))
	 goto b;
  _cru_set_storage (r->ro_store, err);
  _cru_set_kill_switch (&(r->killed), err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (s, d, err));)
	 {
		KILL_SITE(20);
		killed = (killed ? 1 : KILLED);
		if ((! (n = incoming->receiver)) ? IER(1153) : RECORDED(n))
		  goto c;
		if (*err ? 1 : killed ? 1 : _cru_set_membership (n, &seen, err) ? *err : 1)
		  goto d;
		if (_cru_scattered_by_hashes (n, r->merger.me_classifier.cl_order.hash, d, BY_CLASS, err) ? *err : 1)
		  goto e;
		_cru_write (&(n->class_mark), SCATTERED, err);
		if ((c = _cru_collision (incoming->hash_value, &collisions, err)) ? *err : 1)
		  goto d;
		if (*c)
		  for (; (unequal = UNEQUAL(n, (*c)->receiver)) ? (*c)->next_packet : NULL; c = &((*c)->next_packet));
		if ((*c ? (! unequal) : 0) ? (assimilated (incoming, *c, err) ? 1 : IER(1154)) : 0)
		  goto c;
		p = (*c ? &((*c)->next_packet) : c);
		if ((((*p = _cru_popped_packet (&incoming, err))) ? (n = _cru_half_severed ((*p)->receiver)) : NULL) ? 1 : ! IER(1155))
		  n->previous = &((*p)->receiver);
		continue;
	 c: _cru_nack (_cru_popped_packet (&incoming, err), err);
		continue;
	 e: _cru_clear_membership (n, &seen);
 	 d: _cru_cull (&incoming, &buffer, r->merger.me_classifier.cl_order.hash, &(s->deletions), d, BY_CLASS, err);
 	 }
  _cru_forget_members (seen);
  fuse_vertices (s->deferred = _cru_unpacked (collisions), &(r->merger), &(r->ro_sig), err);
 a: status = *err;
  return (void *) status;
 b: return _cru_abort_status (s, d, err);
}







static void
demerge_vertices (s, c, r, err)
	  port s;
	  cru_merger c;
	  cru_destructor_pair r;
	  int *err;

	  // Reclaim whatever was allocated by the vertex merging task for
	  // the given port. At this point, the graph nodes stored in the
	  // deletions list can be reclaimed using the original graph
	  // deallocators found in r. New vertices accessed through the
	  // deferred packet list can be reclaimed by the vertex destructor
	  // in the kernel prop of the merger, but their edges must still
	  // be recaimed by the original edge destructor in the graph. In
	  // either case, the vertex properties must be reclaimed by the
	  // destructor in the vertex fold of the merger's property prop.
{
  struct cru_destructor_pair_s z;

  if ((! s) ? IER(1156) : (s->gruntled != PORT_MAGIC) ? IER(1157) : (! c) ? IER(1158) : (! r) ? IER(1159) : 0)
	 return;
  memset (&z, 0, sizeof (z));
  z.e_free = r->e_free;
  z.v_free = c->me_kernel.v_op.vertex.r_free;
  for (; s->deferred; _cru_nack (_cru_popped_packet (&(s->deferred), err), err))
	 _cru_free_nodes (_cru_unpropped (s->deferred->receiver, c->me_classifier.cl_prop.vertex.m_free, err), &z, err);
  _cru_free_nodes (_cru_unpropped (s->deletions, c->me_classifier.cl_prop.vertex.m_free, err), r, err);
  s->deletions = NULL;
}






// --------------- edge fusing -----------------------------------------------------------------------------





static void
filter_edges (f, t, s, edges, o, d, err)
	  cru_pruner f;
	  multiset_table t;         // stores the multiplicity of each label (numbered from zero)
	  uintptr_t s;              // class size
	  edge_list *edges;
	  cru_order o;
	  cru_destructor d;
	  int *err;

	  // Delete any edges that don't satisfy the predicate and also
	  // free the table. The test f takes an edge label and an integer
	  // flag, which is non-zero iff at least one edge with the given
	  // label emanates from every member of the class. This capability
	  // is provided only to enable applications to perform a
	  // particular Petri net reachability graph optimization.
{
  cru_hash h;                  // hash function of labels
  edge_list q;
  edge_list *l;
  int ux, ut, e;

  q = NULL;
  if ((! (l = edges)) ? IER(1160) : 0)
	 goto a;
  if ((! f) ? 1 : (! s--) ? IER(1161) : (! s) ? 0 : (! o) ? IER(1162) : (h = o->hash) ? 0 : IER(1163))
	 goto a;
  while (*l)
	 {
		e = (*err ? 0 : (! s) ? 1 : (_cru_multiplicity (h ((*l)->label), o->equal, (*l)->label, t, err) == s));
		if (PASSED(f, e, (*l)->label) ? (l = &((*l)->next_edge)) : NULL)
		  continue;
		_cru_push_edge (_cru_popped_edge (l, err), &q, err);
	 }
 a: _cru_discount (t);
  _cru_free_edges_and_labels (d, q, err);     // deferred until now to avoid corrupting the table
}









static edge_list
fused_edges (n, c, d, o, err)
	  node_list n;              // a list of nodes in a class
	  cru_merger c;
	  cru_destructor d;         // edge destructor
	  cru_order o;              // edge ordering
	  int *err;

	  // Derive the resultant outgoing edges from the member nodes n in
	  // a class, and consume the members' edges.
{
  brigade b;
  edge_list e;
  multiset_table q;            // the set of outgoing edges from an individual class member
  multiset_table t;            // the multiset union of outgoing edges from all members of the class
  uintptr_t class_size;        // the number of members in the class
  void *v;

  t = NULL;
  e = NULL;
  if ((! c) ? IER(1164) : (! o) ? IER(1165) : (! n) ? IER(1166) : 0)
	 return NULL;
  v = n->vertex;
  for (class_size = 0; n ? ++class_size : 0; n = n->next_node)
	 {
		for (q = NULL; n->edges_out;)
		  {
			 _cru_push_edge (_cru_popped_edge (&(n->edges_out), err), &e, err);
			 if (*err ? 0 : (! (c->pruner)) ? 0 : ! ((! e) ? IER(1167) : (! (o->hash)) ? IER(1168) : 0))
				_cru_record_edge ((o->hash) (e->label), o->equal, e->label, &q, err);
		  }
		_cru_merge (&t, q, o->equal, err);
	 }
  filter_edges (c->pruner, t, class_size, &e, o, d, err);
  b = _cru_marshalled (&e, BY_CLASS, d, err);
  _cru_free_edges_and_labels (d, e, err);
  if (_cru_empty_fold (&(c->me_kernel.e_op)))
	 return _cru_deduplicated_brigade (o, b, BY_CLASS, d, err);
  return _cru_reduced_brigade (&(c->me_kernel.e_op), v, b, BY_CLASS, d, err);
}





// --------------- edge classifying ------------------------------------------------------------------------







static void *
edge_merging_task (s, err)
	  port s;
	  int *err;

	  // Combine the outgoing edges from each class of nodes accessed
	  // through the deferred packet list of the given port. Move
	  // successfully processed classes to the survivors list. Notify
	  // other workers to do the same but don't otherwise interact with
	  // them.
{
  cru_destructor_pair d;
  unsigned sample;
  cru_merger c;
  int killed;
  cru_sig z;
  router r;

  killed = 0;
  sample = 0;
  if ((! s) ? IER(1169) : (s->gruntled != PORT_MAGIC) ? IER(1170) : ! _cru_pingback (s, err))
	 return NULL;
  if ((! (r = s->local)) ? IER(1171) : (r->valid != ROUTER_MAGIC) ? IER(1172) : (r->tag != CLU) ? IER(1173) : 0)
	 return NULL;
  z = &(r->ro_sig);
  d = &(z->destructors);
  for (c = &(r->merger); s->deferred; _cru_nack (_cru_popped_packet (&(s->deferred), err), err))
	 {
		KILL_SITE(21);
		killed = (killed ? 1 : KILLED);
		if (*err)
		  _cru_free_outgoing_edges_and_labels (s->deferred->receiver, d->e_free, err);
		else
		  s->deferred->receiver->edges_out = fused_edges (s->deferred->receiver, c, d->e_free, &(z->orders.e_order), err);
		if ((s->survivors = _cru_cat_nodes (s->deferred->receiver, s->survivors)))
		  s->survivors->previous = &(s->survivors);
		s->deferred->receiver = NULL;
	 }
  return NULL;
}







static void
demerge_edges (s, c, r, err)
	  port s;
	  cru_merger c;
	  cru_destructor_pair r;
	  int *err;

	  // Undo the job of edge merging for a single port to recover
	  // from an error. Assume edges and nodes accessed through the
	  // survivors list can be reclaimed by destructors obtained from
	  // the merger unless the edge kernel operator is indifferent,
	  // in which case use the edge label destructor from the original
	  // graph.
{
  struct cru_destructor_pair_s z;

  if ((! s) ? IER(1174) : (s->gruntled != PORT_MAGIC) ? IER(1175) : (! c) ? IER(1176) : 0)
	 return;
  memset (&z, 0, sizeof (z));
  z.v_free = c->me_kernel.v_op.vertex.r_free;
  z.e_free = (c->me_kernel.e_op.r_free ? c->me_kernel.e_op.r_free : r->e_free);
  _cru_free_nodes (_cru_unpropped (s->survivors, c->me_classifier.cl_prop.vertex.m_free, err), &z, err);
  s->survivors = NULL;
}






// --------------- combined classifying --------------------------------------------------------------------





static void *
demerging_task (s, err)
	  port s;
	  int *err;

	  // Undo edge merging and vertex merging on a single port
	  // concurrently with other workers in the event of an error.
{
  router r;

  if ((! s) ? IER(1177) : (s->gruntled != PORT_MAGIC) ? IER(1178) : ! _cru_pingback (s, err))
	 return NULL;
  if ((! (r = s->local)) ? IER(1179) : (r->valid != ROUTER_MAGIC) ? IER(1180) : (r->tag != CLU) ? IER(1181) : 0)
	 return NULL;
  demerge_vertices (s, &(r->merger), &(r->ro_sig.destructors), err);
  demerge_edges (s, &(r->merger), &(r->ro_sig.destructors), err);
  return NULL;
}








static void
merge (g, k, r, s, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  cru_sig s;
	  int *err;

	  // Cluster the vertices and the edges of a graph whose properties
	  // have been set in two passes, and collect the survivors on a
	  // third pass.
{
  cru_hash h;
  unsigned i;
  uintptr_t q;

  if ((! r) ? IER(1182) : (r->valid != ROUTER_MAGIC) ? IER(1183) : (! g) ? IER(1184) : (! (g->base_node)) ? IER(1185) : *err)
	 return;
  if (((! (r->ports)) ? IER(1186) : (! (r->lanes)) ? IER(1187) : 0) ? (r->valid = MUGGLE(33)) : s ? 0 : IER(1188))
	 return;
  if ((r->tag != CLU) ? IER(1189) : (! (h = r->merger.me_classifier.cl_order.hash)) ? IER(1190) : 0)
	 return;
  _cru_reset (r, (task) vertex_merging_task, err);
  if (! _cru_status_launched (k, g->base_node, q = h (g->base_node->vertex_property), r, err))
	 return;
  g->nodes = NULL;
  memcpy (&(g->g_sig), s, sizeof (g->g_sig));
  if (*err ? 1 : _cru_status_launched (k, g->base_node, q, _cru_reset (r, (task) edge_merging_task, err), err) ? *err : 1)
	 goto a;
  memcpy (&(r->ro_sig.orders.v_order), &(r->merger.me_classifier.cl_order), sizeof (r->ro_sig.orders.v_order));
  if (_cru_queue_launched (k, g->base_node, q, _cru_reset (r, (task) _cru_pruning_task, err), &(g->nodes), err))
	 if (g->nodes)
		g->nodes->previous = &(g->nodes);
 a: if (*err)
	 g->base_node = NULL;
  if (_cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) demerging_task, err)), err))
	 return;
  for (i = 0; i < r->lanes; i++)
	 {
		demerge_vertices (r->ports[i], &(r->merger), &(r->ro_sig.destructors), err);
		demerge_edges (r->ports[i], &(r->merger), &(r->ro_sig.destructors), err);
	 }
}







cru_graph
_cru_merged (g, k, r, s, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  cru_sig s;
	  int *err;

	  // Build a graph of equivalence classes and consume the
	  // original.
{
  router z;

  _cru_disable_killing (k, err);
  if (*err ? 1 : ! g)
	 goto a;
  if ((! r) ? IER(1191) : (r->valid != ROUTER_MAGIC) ? IER(1192) : (r->tag != CLU) ? IER(1193) : (! (r->ports)) ? IER(1194) : 0)
	 goto a;
  if (_cru_half_duplex (g, err))
	 goto b;
  if (! (z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err)))
	 goto a;
  if (! _cru_launched (k, g->base_node, z, err))
	 goto a;
 b: if (*err ? 1 : ! _cru_set_properties (g, k, r, err))
	 goto a;
  merge (g, k, r, s, err);
  _cru_unset_properties (g, r, err);
 a: _cru_free_router (r, err);
  if (*err)
	 _cru_free_now (g, err);
  return (*err ? NULL : g);
}
