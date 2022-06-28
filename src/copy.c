/*
  Cru -- co-recursion utilities

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
#include <string.h>
#include <stdlib.h>
#include "copy.h"
#include "errs.h"
#include "route.h"
#include "wrap.h"


// pads added to malloc arguments to simulate distinct sizes among types
// so that the malloc and free wrappers in wrap.c can tell them apart
static size_t pad[DED + 1];


// --------------- initialization --------------------------------------------------------------------------


#ifdef WRAP

static size_t
base_size (t, err)
	  router_tag t;
	  int *err;

	  // Return the size measured in memory quanta of the data type
	  // corresponding to the given tag.
{
  void *v;

  switch (t)
	 {
	 case FIL: return sizeof (*((cru_filter) v)) >> QUANTUM;
	 case BUI: return sizeof (*((cru_builder) v)) >> QUANTUM;
	 case MUT: return sizeof (*((cru_mutator) v)) >> QUANTUM;
	 case IND: return sizeof (*((cru_inducer) v)) >> QUANTUM;
	 case CRO: return sizeof (*((cru_crosser) v)) >> QUANTUM;
	 case EXT: return sizeof (*((cru_stretcher) v)) >> QUANTUM;
	 case SPL: return sizeof (*((cru_splitter) v)) >> QUANTUM;
	 case COM: return sizeof (*((cru_composer) v)) >> QUANTUM;
	 case POS: return sizeof (*((cru_postponer) v)) >> QUANTUM;
	 case CLU: return sizeof (*((cru_merger) v)) >> QUANTUM;
	 case MAP: return sizeof (*((cru_mapreducer) v)) >> QUANTUM;
	 case FAB: return sizeof (*((cru_fabricator) v)) >> QUANTUM;
	 case DED: return sizeof (*((cru_classifier) v)) >> QUANTUM;
	 case NON:
	 default: IER(668);
	 }
  return 0;
}

#endif




int
_cru_open_copy (err)
	  int *err;

	  // Minimally adjust the pads to ensure distinct sizes.
{
  router_tag i, j;

  memset (pad, 0, DED * sizeof (pad[0]));
#ifdef WRAP
  for (i = NON + 1; i <= DED; i += (j == i))
	 for (j = NON + 1; j < i; j++)
		if (((base_size (j, err) + pad[j]) == (base_size (i, err) + pad[i])) ? ++(pad[i]) : 0)
		  break;
  for (i = FIL; i <= DED; i++)
	 pad[i] <<= QUANTUM;
#endif
  return ! *err;
}






void
_cru_close_copy ()

{
}




// --------------- memory management -----------------------------------------------------------------------





cru_builder
_cru_builder_copy (i, err)
	  cru_builder i;
	  int *err;
{
  cru_builder o;

  if ((! i) ? IER(669) : ((o = (cru_builder) _cru_malloc (sizeof (*o) + pad[BUI]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}






void
_cru_free_builder (o)
	  cru_builder o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[BUI] + sizeof (*o), o);
#else
  free (o);
#endif
}









cru_crosser
_cru_crosser_copy (i, err)
	  cru_crosser i;
	  int *err;
{
  cru_crosser o;

  if ((! i) ? IER(670) : ((o = (cru_crosser) _cru_malloc (sizeof (*o) + pad[CRO]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}






void
_cru_free_crosser (o)
	  cru_crosser o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[CRO] + sizeof (*o), o);
#else
  free (o);
#endif
}










cru_mapreducer
_cru_mapreducer_copy (i, err)
	  cru_mapreducer i;
	  int *err;
{
  cru_mapreducer o;

  if ((! i) ? IER(671) : ((o = (cru_mapreducer) _cru_malloc (sizeof (*o) + pad[MAP]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_mapreducer (o)
	  cru_mapreducer o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[MAP] + sizeof (*o), o);
#else
  free (o);
#endif
}








cru_inducer
_cru_inducer_copy (i, err)
	  cru_inducer i;
	  int *err;
{
  cru_inducer o;

  if ((! i) ? IER(672) : ((o = (cru_inducer) _cru_malloc (sizeof (*o) + pad[IND]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_inducer (o)
	  cru_inducer o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[IND] + sizeof (*o), o);
#else
  free (o);
#endif
}










cru_composer
_cru_composer_copy (i, err)
	  cru_composer i;
	  int *err;
{
  cru_composer o;

  if ((! i) ? IER(673) : ((o = (cru_composer) _cru_malloc (sizeof (*o) + pad[COM]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_composer (o)
	  cru_composer o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[COM] + sizeof (*o), o);
#else
  free (o);
#endif
}











cru_stretcher
_cru_stretcher_copy (i, err)
	  cru_stretcher i;
	  int *err;
{
  cru_stretcher o;

  if ((! i) ? IER(674) : ((o = (cru_stretcher) _cru_malloc (sizeof (*o) + pad[EXT]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_stretcher (o)
	  cru_stretcher o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[EXT] + sizeof (*o), o);
#else
  free (o);
#endif
}









cru_splitter
_cru_splitter_copy (i, err)
	  cru_splitter i;
	  int *err;
{
  cru_splitter o;

  if ((! i) ? IER(675) : ((o = (cru_splitter) _cru_malloc (sizeof (*o) + pad[SPL]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_splitter (o)
	  cru_splitter o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[SPL] + sizeof (*o), o);
#else
  free (o);
#endif
}











cru_classifier
_cru_classifier_copy (i, err)
	  cru_classifier i;
	  int *err;
{
  cru_classifier o;

  if ((! i) ? IER(676) : ((o = (cru_classifier) _cru_malloc (sizeof (*o) + pad[DED]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_classifier (o)
	  cru_classifier o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[DED] + sizeof (*o), o);
#else
  free (o);
#endif
}










cru_merger
_cru_merger_copy (i, err)
	  cru_merger i;
	  int *err;
{
  cru_merger o;

  if ((! i) ? IER(677) : ((o = (cru_merger) _cru_malloc (sizeof (*o) + pad[CLU]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_merger (o)
	  cru_merger o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[CLU] + sizeof (*o), o);
#else
  free (o);
#endif
}







cru_mutator
_cru_mutator_copy (i, err)
	  cru_mutator i;
	  int *err;
{
  cru_mutator o;

  if ((! i) ? IER(678) : ((o = (cru_mutator) _cru_malloc (sizeof (*o) + pad[MUT]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_mutator (o)
	  cru_mutator o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[MUT] + sizeof (*o), o);
#else
  free (o);
#endif
}









cru_filter
_cru_filter_copy (i, err)
	  cru_filter i;
	  int *err;
{
  cru_filter o;

  if ((! i) ? IER(679) : ((o = (cru_filter) _cru_malloc (sizeof (*o) + pad[FIL]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_filter (o)
	  cru_filter o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[FIL] + sizeof (*o), o);
#else
  free (o);
#endif
}









cru_postponer
_cru_postponer_copy (i, err)
	  cru_postponer i;
	  int *err;
{
  cru_postponer o;

  if ((! i) ? IER(680) : ((o = (cru_postponer) _cru_malloc (sizeof (*o) + pad[POS]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_postponer (o)
	  cru_postponer o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[POS] + sizeof (*o), o);
#else
  free (o);
#endif
}









cru_fabricator
_cru_fabricator_copy (i, err)
	  cru_fabricator i;
	  int *err;
{
  cru_fabricator o;

  if ((! i) ? IER(681) : ((o = (cru_fabricator) _cru_malloc (sizeof (*o) + pad[FAB]))) ? 0 : RAISE(ENOMEM))
	 return NULL;
  memcpy (o, i, sizeof (*o));
  return o;
}








void
_cru_free_fabricator (o)
	  cru_fabricator o;
{
  if (! o)
	 return;
#ifdef WRAP
  _cru_free_wrapper (__LINE__, __FILE__, pad[FAB] + sizeof (*o), o);
#else
  free (o);
#endif
}
