#ifndef LIN_ALG_CONFIG_H_INCLUDED
#define LIN_ALG_CONFIG_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

// #define FLT32 32
// #define FLT64 64
// #define INT32 32
// #define INT64 64

#ifndef FEILD_T
#define FEILD_T FLT32
#endif
#if FEILD_T == FLT32
#define FLT_TYP float
#elif FEILD_T == FLT64
#define FLT_TYP double
#endif /* FEILD_T */

#ifndef INDEX_T
#define INDEX_T INT32
#endif
#if INDEX_T == INT32
#define IND_TYP int32_t
#elif INDEX_T == INT64
#define IND_TYP int64_t
#endif /* INDEX_T */

/* If using mkl */
#include "blaseng_mkl.h"

#endif /* LIN_ALG_CONFIG_H_INCLUDED */