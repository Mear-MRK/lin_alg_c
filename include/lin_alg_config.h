#pragma once

#include <stdint.h>

#ifdef FLT64
#define FLT_TYP double
#else
#define FLT32 32
#define FLT_TYP float
#endif

#ifdef INT32
#define IND_TYP int32_t
#define MAX_IND INT32_MAX
#define MIN_IND INT32_MIN
#else
#define INT64 64
#define IND_TYP int64_t
#define MAX_IND INT64_MAX
#define MIN_IND INT64_MIN
#endif 
