#pragma once

#include <stdbool.h>

#include "lin_alg_config.h"

typedef struct slice_struct
{
    IND_TYP start;
    IND_TYP stop;
    IND_TYP step;
    IND_TYP nbr;
} slice_t;

#define slice_NULL ((const slice_t){.start = 0, .stop = 0, .step = 0, .nbr = 0})
#define slice_NONE ((const slice_t){.start = 0, .stop = MAX_IND, .step = 1, .nbr = MAX_IND})

slice_t *slice_construct(slice_t *sly, IND_TYP start, IND_TYP stop, IND_TYP step);
slice_t *slice_new(IND_TYP start, IND_TYP stop, IND_TYP step);
void slice_del(slice_t *sly);

bool slice_is_none(const slice_t *slice);
bool slice_is_null(const slice_t *sly);
// both none and null are valid slices
bool slice_is_valid(const slice_t *slice);

slice_t *slice_combine(slice_t *result, const slice_t *base, const slice_t *over);
slice_t *slice_canonical(slice_t *sly, IND_TYP d);

IND_TYP slice_index(const slice_t *sly, IND_TYP i);
