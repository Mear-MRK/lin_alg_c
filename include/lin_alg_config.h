#ifndef LIN_ALG_CONFIG_H_INCLUDED
#define LIN_ALG_CONFIG_H_INCLUDED 1

#include <stdint.h>

#ifdef FLT64
#define FLT_TYP double
#else
#define FLT32 32
#define FLT_TYP float
#endif

#ifdef INT32
#define IND_TYP int32_t
#else
#define INT64 64
#define IND_TYP int64_t
#endif 

#endif /* LIN_ALG_CONFIG_H_INCLUDED */
