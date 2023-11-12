#include "payload.h"

#include <assert.h>

bool payload_is_valid(const payload_t *pyl)
{
    return pyl &&
           pyl->size > 0 &&
           pyl->arr &&
           pyl->ref_count >= 0;
}

payload_t *payload_construct(payload_t *pyl, size_t size)
{
    assert(pyl);
    assert(size > 0);

    if (!pyl)
        return NULL;
    if (size == 0)
    {
        *pyl = payload_NULL;
        return pyl;
    }

    pyl->size = size;
    pyl->arr = (FLT_TYP *)aligned_alloc(64, size * sizeof(FLT_TYP));
    assert(pyl->arr);
    if (!pyl->arr)
    {
        *pyl = payload_NULL;
        return pyl;
    }
    pyl->ref_count = 1;
    pyl->flags = 0u;

    return pyl;
}

static void payload_destruct(payload_t *pyl)
{
    assert(payload_is_valid(pyl));

    if (!pyl || !pyl->arr)
        return;
    if (!(pyl->flags & PAYLOAD_FLG_PREALLOC))
        free((void *)pyl->arr);
    *pyl = payload_NULL;
}

payload_t *payload_new(size_t size)
{
    payload_t *pyl = (payload_t *)calloc(1, sizeof(payload_t));
    assert(pyl);
    payload_construct(pyl, size);
    pyl->flags = PAYLOAD_FLG_NEW;
    return pyl;
}

static void payload_del(payload_t *pyl)
{
    if (!pyl || !(pyl->flags & PAYLOAD_FLG_NEW))
        return;
    payload_destruct(pyl);
    free((void *)pyl);
}

payload_t *payload_prealloc(payload_t *pyl, FLT_TYP *arr, size_t size)
{
    assert(pyl);
    assert(arr);
    assert(size > 0);

    if (!pyl)
        return NULL;
    if (size == 0 || !arr)
    {
        *pyl = payload_NULL;
        return pyl;
    }

    pyl->size = size;
    pyl->arr = arr;
    pyl->ref_count = 1;
    pyl->flags = PAYLOAD_FLG_PREALLOC;

    return pyl;
}

payload_t *payload_share(payload_t *pyl)
{
    assert(payload_is_valid(pyl));

    pyl->ref_count++;

    return pyl;
}

void payload_release(payload_t *pyl)
{
    // assert(payload_is_valid(pyl));
    if (!payload_is_valid(pyl))
        return;

    pyl->ref_count--;
    if (pyl->ref_count <= 0)
        if (pyl->flags & PAYLOAD_FLG_NEW)
            payload_del(pyl);
        else
            payload_destruct(pyl);
}
