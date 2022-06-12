#ifndef CRU_FUNCTION_TYPES_H
#define CRU_FUNCTION_TYPES_H 1

#include <stdint.h>

// This file defines prototypes for user-defined callback functions
// called by cru when passed via the API in the fields of structures
// defined in data_types.h. See the cru_function_types(7) manual page
// and the cru_data_types(7) manual page for detailed explanations.
//
// Void pointer arguments can be pointers to user-defined data or
// pointer-sized scalar values. All functions except the
// cru_destructor are assumed to borrow their void pointer arguments
// without consuming them and to treat them as read-only.
//
// All functions except the cru_hash take a pointer to an integer
// error code as their last parameter. They can set it to any non-zero
// value to report an error. See error_codes.h for reserved codes.

// --------------- miscellaneous type signatures -----------------------------------------------------------

typedef uintptr_t (*cru_hash)(void *);
typedef void (*cru_destructor)(void *, int *);
typedef void (*cru_connector)(void *, int *);
typedef void (*cru_subconnector)(int, void *, void *, int *);
typedef int (*cru_pruner)(int, void *, int *);

// --------------- nullary through quarternary operators ---------------------------------------------------

// Operators can return pointers to the heap or pointer-sized values
// cast to pointers. Operators returning pointers are assumed to
// allocate new storage for each call.

typedef void *(*cru_nop)(int *);
typedef void *(*cru_uop)(void *, int *);
typedef void *(*cru_bop)(void *, void *, int *);
typedef void *(*cru_top)(void *, void *, void *, int *);
typedef void *(*cru_qop)(void *, void *, void *, void *, int *);

// --------------- binary through quarternary predicates ---------------------------------------------------

// Predicates return zero for false and any non-zero value for true.

typedef int (*cru_bpred)(void *, void *, int *);
typedef int (*cru_tpred)(void *, void *, void *, int *);
typedef int (*cru_qpred)(void *, void *, void *, void *, int *);

#endif
