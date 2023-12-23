#pragma once

#include <stdbool.h>

#include "lin_alg_config.h"


typedef struct slice_struct
{
    IND_TYP start;  // start index
    IND_TYP stop;   // stop index
    IND_TYP step;   // step size
    IND_TYP len;    // number of elements
} slice_t;

#define slice_IND_UNDEF  MIN_IND
#define slice_PLUS_END   MAX_IND
#define slice_MINUS_END  (MIN_IND+1)

#define slice_NULL ((const slice_t){.start = slice_IND_UNDEF, .stop = slice_IND_UNDEF, .step = 0, .len = 0})

#define slice_NONE ((const slice_t){.start = 0, .stop = slice_PLUS_END, .step = 1, .len = slice_IND_UNDEF})

slice_t *slice_set(slice_t *sly, IND_TYP start, IND_TYP stop, IND_TYP step);

slice_t *slice_new(IND_TYP start, IND_TYP stop, IND_TYP step);

void slice_del(slice_t *sly);

bool slice_is_none(const slice_t *slice);

bool slice_is_null(const slice_t *sly);
// both none and null are valid slices
bool slice_is_valid(const slice_t *slice);

slice_t *slice_combine(slice_t *result, const slice_t *base, const slice_t *over);

slice_t *slice_regulate(slice_t *sly, IND_TYP d);

IND_TYP slice_index(const slice_t *sly, IND_TYP i);

char *slice_to_str(const slice_t *sly, char *str);
