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

#include <string.h>
#include <stdlib.h>
#include "edges.h"
#include "errs.h"
#include "defo.h"
#include "duplex.h"
#include "graph.h"
#include "killers.h"
#include "nodes.h"
#include "launch.h"
#include "pack.h"
#include "pods.h"
#include "ports.h"
#include "props.h"
#include "route.h"
#include "scatter.h"
#include "table.h"
#include "wrap.h"

// arguments to unpropped
#define PROPERTIES_ONLY 0
#define DOPPLEGANGERS_TOO 1

// the prop used for computing vertex properties in terms of a router x depending on its tag
#define PROBE_OF(x) ( \
   (! x) ? NULL : \
	(x->tag == SPL) ? &(x->splitter.sp_prop) :           \
	(x->tag == EXT) ? &(x->stretcher.st_prop) :           \
	(x->tag == CLU) ? &(x->merger.me_classifier.cl_prop) : \
	NULL)


// --------------- property unsetting ----------------------------------------------------------------------




static void *
property_unsetting_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively free vertex properties in the event of a
	  // cancellation.
{
  packet_list incoming;
  intptr_t status;
  node_set seen;
  packet_pod d;
  cru_prop p;
  node_list n;
  router r;
  int ux;

  seen = NULL;
  if ((! source) ? IER(1348) : (source->gruntled != PORT_MAGIC) ? IER(1349) : 0)
	 goto a;
  if ((!(r = source->local)) ? IER(1350) : (r->valid != ROUTER_MAGIC) ? IER(1351) : 0)
	 goto a;
  if ((d = source->peers) ? ((! (p = PROBE_OF(r))) ? IER(1352) : *err) : IER(1353))
	 return _cru_abort_status (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		if ((! (n = (node_list) incoming->payload)) ? IER(1354) : _cru_test_and_set_membership (n, &seen, err) ? 1 : *err)
		  goto b;
		_cru_scatter_out (n, d, err);
		if (n->vertex_property ? p->vertex.m_free : NULL)
		  APPLY(p->vertex.m_free, n->vertex_property);
		n->vertex_property = NULL;
		if ((r->tag == CLU) ? 1 : (r->tag == EXT))
		  n->class = NULL;
	 b: _cru_nack (_cru_popped_packet (&incoming, err), err);
 	 }
  _cru_forget_members (seen);
 a: status = *err;
  return (void *) status;
}







static void
unprop (n, d, dopplegangers_too, err)
	  node_list n;
	  cru_destructor d;
	  int dopplegangers_too;
	  int *err;

	  // Free the vertex properties from a list of nodes.
{
  int ux;

  for (; n; n = n->next_node)
	 {
		if (n->vertex_property ? d : NULL)
		  APPLY(d,n->vertex_property);
		n->vertex_property = NULL;
		if (dopplegangers_too)
		  n->doppleganger = NULL;
	 }
}







node_list
_cru_unpropped (n, d, err)
	  node_list n;
	  cru_destructor d;
	  int *err;

	  // Free the vertex properties from a list of nodes and return it.
{
  unprop (n, d, DOPPLEGANGERS_TOO, err);
  return n;
}







void
_cru_unset_properties (g, r, err)
	  cru_graph g;
	  router r;
	  int *err;

	  // Unset the property fields in a graph assuming the graph is
	  // still connected. Do it concurrently only if there is no error
	  // on entry or the error is a user interrupt, and sequentially
	  // otherwise or if the concurrent attempt fails.
{
  cru_prop p;

  if ((! g) ? IER(1355) : (! r) ? IER(1356) : (r->valid != ROUTER_MAGIC) ? IER(1357) : ((p = PROBE_OF(r))) ? 0 : IER(1358))
	 return;
  if (((*err == CRU_INTKIL) ? 1 : ! *err) ? _cru_reset (r, (task) property_unsetting_task, err) : NULL)
	 if (_cru_launched (UNKILLABLE, g->base_node, _cru_shared (r), err) ? (! *err) : 0)
		return;
  unprop (g->nodes, p->vertex.m_free, (r->tag == CLU) ? DOPPLEGANGERS_TOO : PROPERTIES_ONLY, err);
}








// --------------- property setting ------------------------------------------------------------------------





int
_cru_propped (n, i, o, p, err)
	  node_list n;
	  edge_list i;
	  edge_list o;
	  cru_prop p;
	  int *err;

 	  // Compute the properties associated with a single vertex in a
	  // graph. Store the result in the node's vertex_property
	  // field. Properties are passed as arguments to user defined poser
	  // functions that stretch the graph by inserting new vertices and
	  // edges between adjacent vertices.
{
  int ux;
  void *v;
  struct node_list_s s;

  if (*err ? 1 : (! n) ? IER(1359) : (! p) ? IER(1360) : n->vertex_property ? IER(1361) : 0)
	 goto a;
  memcpy (&s, n, sizeof (s));
  s.edges_in = i;
  s.edges_out = o;
  n->vertex_property = _cru_mapped_node_with_locks (p, &s, err);
  if (! *err)
	 return 1;
  if (n->vertex_property ? p->vertex.m_free : NULL)
	 APPLY(p->vertex.m_free, n->vertex_property);
  n->vertex_property = NULL;
 a: return 0;
}









static void *
property_setting_task (source, err)
	  port source;
	  int *err;

	  // Co-operatively initialize all the vertex properties in a
	  // graph.
{
  router r;
  int killed;
  cru_prop p;
  node_list n;
  packet_pod d;
  node_set seen;
  intptr_t status;
  unsigned sample;
  packet_list incoming;

  d = NULL;
  killed = 0;
  sample = 0;
  seen = NULL;
  if ((! source) ? IER(1362) : (source->gruntled != PORT_MAGIC) ? IER(1363) : 0)
	 return NULL;
  if ((!(r = source->local)) ? IER(1364) : (r->valid != ROUTER_MAGIC) ? IER(1365) : 0)
	 return NULL;
  if ((!(d = source->peers)) ? IER(1366) : (! (p = PROBE_OF(r))) ? IER(1367) : 0)
	 return _cru_abort_status (source, d, err);
  for (incoming = NULL; incoming ? incoming : (incoming = _cru_exchanged (source, d, err));)
	 {
		KILL_SITE(26);
		killed = (killed ? 1 : KILLED);
		if (*err ? 1 : killed)
		  goto a;
		if ((! (n = (node_list) incoming->payload)) ? IER(1368) : _cru_test_and_set_membership (n, &seen, err) ? 1 : *err)
		  goto a;
		_cru_scatter_out (n, d, err);
		if (! *err)
		  _cru_propped (n, n->edges_in, n->edges_out, p, err);
	 a: _cru_nack (_cru_popped_packet (&incoming, err), err);
	 }
  _cru_forget_members (seen);
  _cru_nack (incoming, err);
  status = *err;
  return (void *) status;
}









int
_cru_set_properties (g, k, r, err)
	  cru_graph g;
	  cru_kill_switch k;
	  router r;
	  int *err;

	  // Assign the properties fields in a graph using a prepared
	  // router, and leave the graph in half-duplex form.
{
  unsigned worker;
  cru_prop p;
  node_list n;
  router z;

  if ((! g) ? IER(1369) : (! r) ? IER(1370) : (r->valid != ROUTER_MAGIC) ? IER(1371) : ((p = PROBE_OF(r))) ? 0 : IER(1372))
	 return 0;
  if (_cru_empty_fold (&(p->incident)) ? 0 : _cru_half_duplex (g, err))
	 if (! _cru_launched (k, g->base_node, _cru_router ((task) _cru_full_duplexing_task, r->lanes, err), err))
		return 0;
  if (*err ? 1 : ! _cru_launched (k, g->base_node, _cru_shared (_cru_reset (r, (task) property_setting_task, err)), err))
	 return 0;
  if (*err)
	 goto a;
  if (_cru_half_duplex (g, err))
	 return 1;
  if (! (z = _cru_razing_router (&(r->ro_sig.destructors), (task) _cru_half_duplexing_task, r->lanes, err)))
	 goto b;
  if (! _cru_launched (k, g->base_node, z, err))
	 goto b;
  if (! *err)
	 return 1;
 a: for (n = g->nodes; n; n = n->next_node)
	 {
		_cru_free_edges (n->edges_in, err);
		n->edges_in = NULL;
	 }
 b: _cru_unset_properties (g, r, err);
  return 0;
}







// --------------- property clearing -----------------------------------------------------------------------







static void *
property_clearing_task (source, err)
	  port source;
	  int *err;

	  // Concurrently free all the vertex properties in a graph, whose
	  // vertices at this point are distributed and stored in the
	  // survivors lists of each port.
{
  intptr_t status;
  cru_prop p;
  router r;

  if ((! source) ? IER(1373) : (source->gruntled != PORT_MAGIC) ? IER(1374) : ! _cru_pingback (source, err))
	 goto a;
  if ((! (r = source->local)) ? IER(1375) : (r->valid != ROUTER_MAGIC) ? IER(1376) : ((p = PROBE_OF(r))) ? 0 : IER(1377))
	 goto a;
  unprop (source->survivors, p->vertex.m_free, DOPPLEGANGERS_TOO, err);
 a: status = *err;
  return (void *) status;
}









void
_cru_clear_properties (r, err)
	  router r;
	  int *err;

	  // Clear the properties fields in a graph whose nodes are stored
	  // in a router's ports' survivors lists.
{
  cru_prop p;
  unsigned port_number;

  if ((! r) ? IER(1378) : (r->valid != ROUTER_MAGIC) ? IER(1379) : (! (r->ports)) ? IER(1380) : 0)
	 return;
  if (((p = PROBE_OF(r))) ? 0 : IER(1381))
	 return;
  if ((! *err) ? 1 : (*err == CRU_INTKIL))
	 if (_cru_launched (UNKILLABLE, NO_BASE, _cru_shared (_cru_reset (r, (task) property_clearing_task, err)), err))
		return;
 a: for (port_number = 0; port_number < r->lanes; port_number++)
	 unprop ((r->ports[port_number])->survivors, p->vertex.m_free, DOPPLEGANGERS_TOO, err);
}
