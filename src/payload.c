#include "payload.h"

#include <assert.h>

bool payload_is_valid(const payload_t *pyl)
{
    return pyl &&
           pyl->size > 0 &&
           pyl->arr &&
           pyl->ref_count > 0;
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

    return pyl;
}

void payload_destruct(payload_t *pyl)
{
    assert(pyl);

    if (pyl)
    {
        if (pyl->arr)
            free((void *)pyl->arr);
        *pyl = payload_NULL;
    }
}

/*
payload_t *payload_prealloc(payload_t *pyl, FLT_TYP *arr, size_t size)
{
    assert(pyl);
    assert(arr);
    assert(size > 0);
    if (!pyl)
        return NULL;

    pyl->size = size;
    pyl->arr = arr;
    pyl->nbr_refs = 1;

    return pyl;
}


payload_t *payload_new(size_t size)
{
    assert(size > 0);
    if (size <= 0)
        return NULL;
    payload_t *new_pyl = (payload_t *)calloc(1, sizeof(payload_t));
    assert(new_pyl);
    return payload_construct(new_pyl, size);
}

void payload_del(payload_t *pyl)
{
    if (!pyl)
        return;
    payload_destruct(pyl);
    free((void *)pyl);
}
*/

payload_t *payload_share(payload_t *pyl)
{
    assert(payload_is_valid(pyl));

    pyl->ref_count++;

    return pyl;
}

void payload_release(payload_t *pyl)
{
    // assert(payload_is_valid(pyl));
    if(!payload_is_valid(pyl))
        return;

    pyl->ref_count--;
    if (pyl->ref_count == 0)
        payload_destruct(pyl);
}
