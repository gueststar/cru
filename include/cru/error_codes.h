
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
#define CRU_MAX_ERR 1725

// internal error codes precede this number and test failure codes start from it
#define CRU_MIN_FAIL 1726
#define CRU_MAX_FAIL 5990

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

#define CRU_INTDLK (-527)
#define CRU_INTKIL (-528)
#define CRU_INTOVF (-529)
#define CRU_INTOOC (-530)

// --------------- null api function parameters ------------------------------------------------------------

#define CRU_NULCLS (-531)
#define CRU_NULCSF (-532)
#define CRU_NULKIL (-533)
#define CRU_NULPRT (-534)

// --------------- partitioning errors ---------------------------------------------------------------------

#define CRU_PARDPV (-535)
#define CRU_PARVNF (-536)

// --------------- type conflicts --------------------------------------------------------------------------

#define CRU_TPCMPR (-537)
#define CRU_TPCMUT (-538)

// --------------- undefined required fields ---------------------------------------------------------------

#define CRU_UNDANA (-539)
#define CRU_UNDCAT (-540)
#define CRU_UNDCBO (-541)
#define CRU_UNDCON (-542)
#define CRU_UNDCQO (-543)
#define CRU_UNDCTO (-544)
#define CRU_UNDEFB (-545)
#define CRU_UNDEPR (-546)
#define CRU_UNDEQU (-547)
#define CRU_UNDEXP (-548)
#define CRU_UNDHSH (-549)
#define CRU_UNDMAP (-550)
#define CRU_UNDPOP (-551)
#define CRU_UNDRED (-552)
#define CRU_UNDVAC (-553)
#define CRU_UNDVFB (-554)
#define CRU_UNDVPR (-555)

#endif
