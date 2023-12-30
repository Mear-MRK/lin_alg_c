#pragma once

#include <stdint.h>

#ifdef FLD_FLT64
#define FLD_FLT64 64
#define FLD_TYP double
#define FLT_TYP double
#elif defined(FLD_CLX64)
#define FLD_CLX64 128
#define FLD_TYP double complex
#define FLT_TYP double
#elif defined(FLD_CLX32)
#define FLD_CLX32 64
#define FLD_TYP float complex
#define FLT_TYP float
#else
#define FLD_FLT32 32
#define FLD_TYP float
#define FLT_TYP float
#endif

#ifdef IND_INT32
#define IND_INT32 32
#define IND_TYP int32_t
#define IND_MAX INT32_MAX
#define IND_MIN INT32_MIN
#else
#define IND_INT64 64
#define IND_TYP int64_t
#define IND_MAX INT64_MAX
#define IND_MIN INT64_MIN
#endif 
