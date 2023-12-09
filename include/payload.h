#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "lin_alg_config.h"

typedef struct payload_struct
{
    size_t size;
    FLT_TYP *arr;
    int ref_count;
    unsigned flags;
} payload_t;

#define payload_FLG_NEW 1u
#define payload_FLG_PREALLOC 2u
#define payload_FLG_RESIZABLE 4u
#define payload_FLG_SHRINKABLE 8u

#define payload_NULL ((const payload_t){.size = 0, .arr = NULL, .ref_count = 0, .flags = 0u})

bool payload_is_valid(const payload_t* pyl);

payload_t *payload_construct(payload_t *pyl, size_t size);

payload_t *payload_new(size_t size);

// void payload_destruct(payload_t *pyl);

// void payload_del(payload_t *pyl);

payload_t *payload_prealloc(payload_t *pyl, FLT_TYP *arr, size_t size);

payload_t *payload_share(payload_t *pyl);

void payload_release(payload_t *pyl);

payload_t *payload_resize(payload_t *pyl, size_t new_size);
