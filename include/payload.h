#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "lin_alg_config.h"

typedef struct payload_struct
{
    size_t size;
    FLT_TYP *arr;
    int ref_count;
} payload_t;

#define payload_NULL ((const payload_t){.size = 0, .arr = NULL, .ref_count = 0})

bool payload_is_valid(const payload_t* pyl);

payload_t *payload_construct(payload_t *pyl, size_t size);

void payload_destruct(payload_t *pyl);

// payload_t *payload_prealloc(payload_t *pyl, FLT_TYP *arr, size_t size);
// payload_t *payload_new(size_t size);
// void payload_del(payload_t *pyl);

payload_t *payload_share(payload_t *pyl);

void payload_release(payload_t *pyl);
