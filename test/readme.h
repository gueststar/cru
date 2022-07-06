// Programs in this directory attempt to exercise all aspects of the
// library by building and operating on hypercubic graphs. Hypercubes
// are built of the dimension defined below. Some tests fail for
// dimensions under 4 or over 20 because of degeneracy or arithmetic
// overflow, but the library routines are applicable to graphs of any
// size. Each dimensional increment at least doubles the time and
// memory usage. If all of these tests pass, running the ktest script
// from the build directory additionally tests the library by
// exercising some of the error handling code paths taken by killed
// jobs, and running the mtest script tests all of the recovery paths
// from allocation errors.

#include <errno.h>

#define DIMENSION 6
#define NUMBER_OF_VERTICES (1 << DIMENSION)

// This parameter passed to API functions chooses the number of
// threads to run. A value of 0 requests one thread per core.
#define LANES 3

// the n-th test failure error code
#define THE_FAIL(n) (-n)

// macro for raising a test failure error without overwriting an
// existing error
#define FAIL(n) (*err = (*err ? *err : THE_FAIL(n)))

// macro for reporting failures in functions lacking an error parameter
#define GLOBAL_FAIL(n) (global_err ? 1 : (global_err = THE_FAIL(n)))

// macro for defining a non-magic number
#define MUGGLE(n) (n)

// always 32 on 64 bit machines
#define HALF_POINTER_WIDTH (((sizeof (void *) >> 1)) << 3)

// a word of zeros in the upper half and ones in the lower half
#define MASK ((((uintptr_t) 1) << HALF_POINTER_WIDTH) - 1)

// the last statement in each test; this is modified automatically by ktest and mtest
#define EXIT exit (err ? EXIT_FAILURE : EXIT_SUCCESS)

// used in test programs to report not otherwise reportable errors in detected in hash functions
static int global_err = 0;
