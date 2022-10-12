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
#include "errs.h"
#include "getset.h"
#include "wrap.h"

// used to retrieve thread-local edge storage for edges created in cru_connect
static pthread_key_t edge_storage;

// used to retrieve thread-local graph specification storage so that it's not needed as a parameter to cru_connect
static pthread_key_t destructors_storage;

// used to retrieve thread-local kill switch storage so that it's not needed as a parameter to cru_connect
static pthread_key_t killed_storage;

// used to retrieve thread-local edge map storage so that it's not needed as a parameter to cru_stretch
static pthread_key_t edge_map_storage;

// used to retrieve thread-local context storage for sanity checking in cru_connect and cru_stretch
static pthread_key_t context_storage;




// --------------- initialization and teardown -------------------------------------------------------------






int
_cru_open_getset (err)
  int *err;

	  // Initialize static storage.
{
  if (pthread_key_create (&edge_storage, NULL) ? IER(983) : 0)
	 return 0;
  if (pthread_key_create (&killed_storage, NULL) ? IER(984) : 0)
	 goto a;
  if (pthread_key_create (&destructors_storage, NULL) ? IER(985) : 0)
	 goto b;
  if (pthread_key_create (&edge_map_storage, NULL) ? IER(986) : 0)
	 goto c;
  if (pthread_key_create (&context_storage, NULL) ? IER(987) : 0)
	 goto d;
  return 1;
 d: pthread_key_delete (edge_map_storage);
 c: pthread_key_delete (destructors_storage);
 b: pthread_key_delete (killed_storage);
 a: pthread_key_delete (edge_storage);
  return 0;
}







static void
release_pthread_resources (err)
	  int *err;

	  // Release pthread related resources.
{
  if (pthread_key_delete (context_storage))
	 IER(988);
  if (pthread_key_delete (edge_map_storage))
	 IER(989);
  if (pthread_key_delete (edge_storage))
	 IER(990);
  if (pthread_key_delete (killed_storage))
	 IER(991);
  if (pthread_key_delete (destructors_storage))
	 IER(992);
}






void
_cru_close_getset ()

	  // Do this when the process exits.
{
  int err;

  err = 0;
  release_pthread_resources (&err);
  _cru_globally_throw (err);
}





// --------------- setters and getters for thread-specific storage -----------------------------------------





int
_cru_set_context (c, err)
	  context *c;
	  int *err;

	  // Store a context in the thread specific storage area for contexts.
{
  if (NOMEM)
	 return *err;
  return (pthread_setspecific (context_storage, (void *) c) ? IER(993) : 0);
}





int
_cru_set_edges (edge, err)
	  edge_list *edge;
	  int *err;

	  // Store an edge list in the thread specific storage area for edge lists.
{
  if (NOMEM)
	 return *err;
  return (pthread_setspecific (edge_storage, (void *) edge) ? IER(994) : 0);
}






int
_cru_set_edge_maps (pair, err)
	  edge_map *pair;
	  int *err;

	  // Store an edge map in the thread specific storage area for edge maps.
{
  if (NOMEM)
	 return *err;
  return (pthread_setspecific (edge_map_storage, (void *) pair) ? IER(995) : 0);
}









int
_cru_set_destructors (destructors, err)
	  cru_destructor_pair destructors;
	  int *err;

	  // Store a destructors in the thread specific storage area for destructorss.
{
  if (NOMEM)
	 return *err;
  return (pthread_setspecific (destructors_storage, (void *) destructors) ? IER(996) : 0);
}








int
_cru_set_kill_switch (killed, err)
	  int *killed;
	  int *err;

	  // Store a kill switch in the thread specific storage area for
	  // kill switches.
{
  if (NOMEM)
	 return *err;
  return (pthread_setspecific (killed_storage, (void *) killed) ? IER(997) : 0);
}







context *
_cru_get_context ()

	  // Retrieve the edge list from thread specific storage.
{
  uintptr_t s;

  s = (uintptr_t) pthread_getspecific (context_storage);
  return (context *) s;
}






edge_list *
_cru_get_edges ()

	  // Retrieve the edge list from thread specific storage.
{
  return (edge_list *) pthread_getspecific (edge_storage);
}








edge_map *
_cru_get_edge_maps ()

	  // Retrieve the edge map from thread specific storage.
{
  return (edge_map *) pthread_getspecific (edge_map_storage);
}










cru_destructor_pair
_cru_get_destructors ()

	  // Retrieve the destructors from thread specific storage.
{
  return (cru_destructor_pair) pthread_getspecific (destructors_storage);
}







int *
_cru_get_kill_switch ()

	  // Retrieve the kill switch from thread specific storage. This
	  // function isn't currently used but is included for the sake of
	  // completeness.
{
  return (int *) pthread_getspecific (killed_storage);
}
