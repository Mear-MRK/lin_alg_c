#pragma once

#include <stdbool.h>

#include "lin_alg_config.h"

typedef struct slice
{
    IND_TYP len;   // number of elements
    IND_TYP start; // start index
    IND_TYP stop;  // stop index
    IND_TYP step;  // step size
} slice;

#define slice_IND_UNDEF IND_MIN
#define slice_IND_P_INF IND_MAX
#define slice_IND_M_INF (IND_MIN + 1)

#define slice_NULL ((const slice){.start = slice_IND_UNDEF, .stop = slice_IND_UNDEF, .step = 0, .len = 0})

#define slice_NONE ((const slice){.start = 0, .stop = slice_IND_P_INF, .step = 1, .len = slice_IND_UNDEF})

slice *slice_set(slice *sly, IND_TYP start, IND_TYP stop, IND_TYP step);

slice *slice_new(IND_TYP start, IND_TYP stop, IND_TYP step);

void slice_del(slice *sly);

bool slice_is_none(const slice *slice);

bool slice_is_null(const slice *sly);
// slice_NONE and slice_NULL both are valid slice.
bool slice_is_valid(const slice *slice);
// slice_NULL is a regulated slice, but slice_NONE is not.
bool slice_is_regulated(const slice *slice);

slice *slice_combine(slice *result, const slice *base, const slice *over);

slice *slice_regulate(slice *sly, IND_TYP d);

IND_TYP slice_index(const slice *sly, IND_TYP i);

char *slice_to_str(const slice *sly, char *str);
