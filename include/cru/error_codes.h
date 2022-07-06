
#ifndef CRU_ERROR_CODES_H
#define CRU_ERROR_CODES_H 1

// Error codes are reported by cru API functions via the integer
// pointer err passed as the last parameter to each. A short
// explanation of each can be returned in a string by the diagnostic
// function cru_strerror, which is not threadsafe. Codes between 0 and
// -CRU_MIN_ERR are reserved for the nthm library, a dependence of
// cru. Codes from there down to -CRU_MAX_FAIL are reserved for cru.

// range of negative numbers reserved for all user-facing and internal error codes
#define CRU_MIN_ERR 512
#define CRU_INT_ERR (CRU_UNDVPR - 1)
#define CRU_MAX_ERR 1708

// internal error codes precede this number and test failure codes start from it
#define CRU_MIN_FAIL 1709
#define CRU_MAX_FAIL 5973

// --------------- invalid api function parameters ---------------------------------------------------------

#define CRU_BADCLS (-512)
#define CRU_BADGPH (-513)
#define CRU_BADKIL (-514)
#define CRU_BADPRT (-515)

// --------------- developer use only ----------------------------------------------------------------------

#define CRU_DEVDBF (-516)
#define CRU_DEVIAL (-517)
#define CRU_DEVMLK (-518)
#define CRU_DEVPBL (-519)

// --------------- inconsistently defined fields -----------------------------------------------------------

#define CRU_INCCON (-520)
#define CRU_INCFIL (-521)
#define CRU_INCINV (-522)
#define CRU_INCMRG (-523)
#define CRU_INCMUT (-524)
#define CRU_INCTRP (-525)
#define CRU_INCVEL (-526)

// --------------- interruptions ---------------------------------------------------------------------------

#define CRU_INTKIL (-527)
#define CRU_INTOVF (-528)
#define CRU_INTOOC (-529)

// --------------- null api function parameters ------------------------------------------------------------

#define CRU_NULCLS (-530)
#define CRU_NULCSF (-531)
#define CRU_NULKIL (-532)
#define CRU_NULPRT (-533)

// --------------- partitioning errors ---------------------------------------------------------------------

#define CRU_PARDPV (-534)
#define CRU_PARVNF (-535)

// --------------- type conflicts --------------------------------------------------------------------------

#define CRU_TPCMPR (-536)
#define CRU_TPCMUT (-537)

// --------------- undefined required fields ---------------------------------------------------------------

#define CRU_UNDANA (-538)
#define CRU_UNDCAT (-539)
#define CRU_UNDCBO (-540)
#define CRU_UNDCON (-541)
#define CRU_UNDCQO (-542)
#define CRU_UNDCTO (-543)
#define CRU_UNDEFB (-544)
#define CRU_UNDEPR (-545)
#define CRU_UNDEQU (-546)
#define CRU_UNDEXP (-547)
#define CRU_UNDHSH (-548)
#define CRU_UNDMAP (-549)
#define CRU_UNDPOP (-550)
#define CRU_UNDRED (-551)
#define CRU_UNDVAC (-552)
#define CRU_UNDVFB (-553)
#define CRU_UNDVPR (-554)

#endif
