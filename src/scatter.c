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
#include <strings.h>
#include <stdlib.h>
#include "cthread.h"
#include "edges.h"
#include "emap.h"
#include "errs.h"
#include "nodes.h"
#include "pack.h"
#include "pods.h"
#include "repl.h"
#include "scatter.h"
#include "wrap.h"



// --------------- best effort scattering ------------------------------------------------------------------





void
_cru_scatter (p, t, err)
	  packet_list p;
	  packet_pod t;
	  int *err;

	  // Scatter and consume a list of packets prepared by the caller.
{
  packet_list o;

  if ((! t) ? IER(1449) : (! (t->pod)) ? IER(1450) : (! (t->arity)) ? IER(1451) : 0)
	 return;
  while (*err ? NULL : p ? (o = _cru_popped_packet (&p, err)) : NULL)
	 if (! _cru_push_packet (o, &(t->pod[o->hash_value % t->arity]), err))
		_cru_nack (o, err);
  _cru_nack (p, err);
}








int
_cru_scattered (e, t, err)
	  edge_list e;
	  packet_pod t;
	  int *err;

	  // Send an initial packet to another worker for each edge and
	  // return non-zero if successful
{
  packet_list p;
  uintptr_t q;

  if ((! t) ? IER(1452) : (! (t->arity)) ? IER(1453) : t->pod ? 0 : IER(1454))
	 return 0;
  for (; e; e = e->next_edge)
	 {
		p = _cru_initial_packet_of (e->remote.node, q = _cru_scalar_hash (e->remote.node), err);
		if (! _cru_push_packet (p, &(t->pod[q % t->arity]), err))
		  return 0;
	 }
  return 1;
}








edge_list
_cru_scatter_out (n, t, err)
	  node_list n;
	  packet_pod t;
	  int *err;

	  // For each outgoing edge, send a packet whose payload is its
	  // terminus, whose sender field is the node originating the
	  // message, and whose carrier field stores a copy of the edge.
{
  packet_list p;
  edge_list e;
  uintptr_t q;

  if ((! n) ? IER(1455) : (! t) ? IER(1456) : (! (t->pod)) ? IER(1457) : (! (t->arity)) ? IER(1458) : 0)
	 return NULL;
  for (e = n->edges_out; e; e = e->next_edge)
	 if ((p = _cru_packet_of (e->remote.node, q = _cru_scalar_hash (e->remote.node), n, e, err)))
		_cru_push_packet (p, &(t->pod[q % t->arity]), err);
	 else
		break;
  return e;
}







void
_cru_scatter_in (n, t, err)
	  node_list n;
	  packet_pod t;
	  int *err;

	  // For each incoming edge, send a packet whose payload is its
	  // terminus, whose sender field is the node originating the
	  // message, and whose carrier field stores a copy of the edge.
{
  packet_list p;
  edge_list e;
  uintptr_t q;

  if ((! n) ? IER(1459) : (! t) ? IER(1460) : (!(t->pod)) ? IER(1461) : (! (t->arity)) ? IER(1462) : 0)
	 return;
  for (e = n->edges_in; e; e = e->next_edge)
	 if ((p = _cru_packet_of (e->remote.node, q = _cru_scalar_hash (e->remote.node), n, e, err)))
		_cru_push_packet (p, &(t->pod[q % t->arity]), err);
	 else
		break;
}







void
_cru_send_from (v, c, sender, t, err)
	  void *v;                            // payload
	  edge_list c;                        // carrier
	  node_list sender;
	  packet_pod t;
	  int *err;

	  // Send a packet with a sender field to another worker.
{
  packet_list p;
  uintptr_t q;

  if ((! t) ? IER(1463) : (! (t->pod)) ? IER(1464) : (! (t->arity)) ? IER(1465) : 0)
	 return;
  if ((p = _cru_packet_of (v, q = _cru_scalar_hash (c ? c->remote.node : sender), sender, c, err)))
	 _cru_push_packet (p, &(t->pod[q % t->arity]), err);
}









int
_cru_received_by (v, c, recipient, t, err)
	  void *v;                                // payload
	  edge_list c;                            // carrier
	  node_list recipient;
	  packet_pod t;
	  int *err;

	  // Send a packet with no sender but a specified payload, carrier,
	  // and receiver to a worker indexed by the hash of the receiver.
{
  packet_list p;
  uintptr_t q;

  if ((! t) ? IER(1466) : (! (t->pod)) ? IER(1467) : (! (t->arity)) ? IER(1468) : 0)
	 return 0;
  if (! (p = _cru_packet_of (v, q = _cru_scalar_hash (recipient), NO_SENDER, c, err)))
	 return 0;
  p->receiver = recipient;
  _cru_push_packet (p, &(t->pod[q % t->arity]), err);
  return 1;
}








int
_cru_scattered_by_hashes (n, h, t, by_class, err)
	  node_list n;
	  cru_hash h;
	  packet_pod t;
	  int by_class;   // non-zero requests a hash derived from the vertex property
	  int *err;

	  // Send a vertex-addressed edge-labeled packet to a worker for
	  // each outgoing edge from n. If scattering by classes, ignore
	  // marked remote nodes.
{
#define MARKED(x) ((x == SEEN) ? 1 : (x == SCATTERED) ? 1 : (x == RETIRED) ? 1 : (x == STRETCHED))

  void *c;
  node_list m;
  uintptr_t q;
  edge_list *e;
  packet_list p;

  if ((! n) ? IER(1469) : (! t) ? IER(1470) : (!(t->pod)) ? IER(1471) : (! (t->arity)) ? IER(1472) : h ? 0 : IER(1473))
	 return 0;
  for (e = &(n->edges_out); *e; e = &((*e)->next_edge))
	 if ((m = (*e)->remote.node) ? 0 : IER(1474))
		return 0;
	 else if (((c = _cru_read (&(m->class_mark), err))) ? MARKED(c) : 0)
		continue;
	 else if (! (p = _cru_packet_of (NO_PAYLOAD, q = h (by_class ? m->vertex_property : m->vertex), NO_SENDER, *e, err)))
		return 0;
	 else
		_cru_push_packet (p, &(t->pod[q % t->arity]), err);
  return 1;
}








// --------------- consumptive scattering ------------------------------------------------------------------







int
_cru_unscatterable_by_hashes (n, h, d, t, err)
	  node_list n;
	  cru_hash h;
	  cru_destructor d;
	  packet_pod t;
	  int *err;

	  // Send a vertex-addressed edge-labeled packet to a worker for
	  // each outgoing edge from n and delete the edge if successful.
{
  void *c;
  node_list m;
  uintptr_t q;
  edge_list *e;
  packet_list p;

  if ((! n) ? IER(1475) : (! t) ? IER(1476) : (!(t->pod)) ? IER(1477) : (! (t->arity)) ? IER(1478) : h ? 0 : IER(1479))
	 return 1;
  for (e = &(n->edges_out); *e; _cru_free_edges_and_labels (d, _cru_popped_edge (e, err), err))
	 if ((m = (*e)->remote.node) ? 0 : IER(1480))
		return 1;
	 else if ((p = _cru_packet_of (NO_PAYLOAD, q = h (m->vertex), NO_SENDER, *e, err)))
		_cru_push_packet (p, &(t->pod[q % t->arity]), err);
	 else
		return 1;
  return 0;
}









int
_cru_unscatterable (e, n, d, t, err)
	  edge_list *e;
	  node_list n;
	  cru_destructor d;
	  packet_pod t;
	  int *err;

	  // Send a packet with no carrier to another worker for each edge
	  // in the list, setting its sender to n and its payload to the
	  // node at the other end. Delete from e the edges that are sent
	  // and return non-zero if any edges are unable to be sent due to
	  // an error.
{
  uintptr_t q;
  node_list o;
  edge_list c;
  packet_list p;

  if (e ? 0 : IER(1481))
	 return 0;
  if ((! t) ? IER(1482) : (! (t->arity)) ? IER(1483) : t->pod ? 0 : IER(1484))
	 goto a;
  for (; (! *e) ? 0 : (c = _cru_popped_edge (e, err)) ? 1 : ! IER(1485); _cru_free_edges_and_labels (d, c, err))
	 if (c->remote.node != n)
		{
		  if (! (p = _cru_packet_of (c->remote.node, q = _cru_scalar_hash (c->remote.node), n, NO_CARRIER, err)))
			 {
				*e = _cru_cat_edges (c, *e);
				break;
			 }
		  p->receiver = c->remote.node;
		  _cru_push_packet (p, &(t->pod[q % t->arity]), err);
		}
 a: return ! ! *e;
}







void
_cru_scatter_out_or_consume (n, h, z, t, err)
	  node_list n;
	  cru_hash h;
	  cru_destructor_pair z;
	  packet_pod t;
	  int *err;

	  // Send a packet to another worker for each outgoing edge *e from
	  // n, setting the packet's carrier to *e, its sender to n, and
	  // its payload to the vertex at the other end of *e.
{
  packet_list p;
  edge_list *e;
  uintptr_t q;

  if ((! h) ? IER(1486) : (! t) ? IER(1487) : (! (t->arity)) ? IER(1488) : (! (t->pod)) ? IER(1489) :  (! n) ? IER(1490) : 0)
	 return;
  e = &(n->edges_out);
  for (; *err ? NULL : *e; e = &((*e)->next_edge))
	 if ((p = _cru_packet_of ((*e)->remote.vertex, q = h ((*e)->remote.vertex), n, *e, err)))
		_cru_push_packet (p, &(t->pod[q % t->arity]), err);
	 else
		break;
  _cru_free_edges_and_termini (z, *e, err);                   // in case of an error
  *e = NULL;
}











edge_list
_cru_scatter_and_stretch (z, e, m, h, t, err)
	  cru_destructor_pair z;
	  edge_map e;
	  node_list m;      // common remote node
	  cru_hash h;
	  packet_pod t;
	  int *err;

	  // Scatter and consume the edges in a list of edge maps.
{
  edge_list a, o, l;
  packet_list p;
  node_list i;
  uintptr_t q;

  if ((! h) ? IER(1491) : (! t) ? IER(1492) : (! (t->pod)) ? IER(1493) : (! (t->arity)) ? IER(1494) : 0)
	 return NULL;
  i = NULL;
  for (l = NULL; *err ? NULL : e;)
	 {
		if ((! (e->ante)) ? IER(1495) : (! (e->post)) ? IER(1496) : e->post->remote.node ? IER(1497) : *err)
		  break;
		q = h (e->ante->remote.vertex);
		if (! (i = _cru_node_of (NO_DESTRUCTORS, e->ante->remote.vertex, NO_EDGES_IN, NO_EDGES_OUT, err)))
		  break;
		e->post->remote.node = m;
		i->edges_out = e->post;
		e->post = NULL;
		e->ante->remote.node = i;
		_cru_push_edge (e->ante, &l, err);
		i = NULL;
		if (*err)
		  goto a;
		if ((p = _cru_packet_of (NO_PAYLOAD, q, NO_SENDER, e->ante, err)) ? *err : 1)
		  goto b;
		if (! _cru_push_packet (p, &(t->pod[q % t->arity]), err))
		  goto c;
		e->ante->remote.node->class_mark = STRETCHED;
		e->ante = NULL;
		_cru_pop_map (&e, err);
		continue;
	 c: _cru_nack (p, err);
	 b: _cru_popped_edge (&l, err);
	 a: i = e->ante->remote.node;
		e->ante->remote.vertex = i->vertex;
		i->vertex = NULL;
	 }
  _cru_free_nodes (i, z, err);
  _cru_free_map (z, e, err);
  return l;
}
