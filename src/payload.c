#include "payload.h"

#include <assert.h>
#include <stdio.h>

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
    pyl->flags = payload_FLG_RESIZABLE;
    pyl->flags |= payload_FLG_SHRINKABLE;

    return pyl;
}

static void payload_destruct(payload_t *pyl)
{
    assert(payload_is_valid(pyl));

    if (!pyl || !pyl->arr)
        return;
    if (!(pyl->flags & payload_FLG_PREALLOC))
        free((void *)pyl->arr);
    *pyl = payload_NULL;
}

payload_t *payload_new(size_t size)
{
    payload_t *pyl = (payload_t *)calloc(1, sizeof(payload_t));
    assert(pyl);
    payload_construct(pyl, size);
    pyl->flags |= payload_FLG_NEW;
    return pyl;
}

static void payload_del(payload_t *pyl)
{
    if (!pyl || !(pyl->flags & payload_FLG_NEW))
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
    pyl->flags = payload_FLG_PREALLOC;

    return pyl;
}

payload_t *payload_share(payload_t *pyl)
{
    assert(payload_is_valid(pyl));

    pyl->ref_count++;
    if (pyl->ref_count > 1)
        pyl->flags &= ~payload_FLG_SHRINKABLE;
    return pyl;
}

void payload_release(payload_t *pyl)
{
    // assert(payload_is_valid(pyl));
    if (!payload_is_valid(pyl))
        return;

    pyl->ref_count--;
    if (pyl->ref_count == 1 && (pyl->flags & payload_FLG_RESIZABLE))
        pyl->flags |= payload_FLG_SHRINKABLE;
    else if (pyl->ref_count == 0)
    {
        if (pyl->flags & payload_FLG_NEW)
            payload_del(pyl);
        else
            payload_destruct(pyl);
    }
    else if (pyl->ref_count < 0)
    {
#ifdef DEBUG
        fprintf(stderr, "payload_release: WARN ref_count is negative: %d\n", pyl->ref_count);
#endif
    }
}

payload_t *payload_resize(payload_t *pyl, size_t new_size)
{
    assert(payload_is_valid(pyl));

    if (new_size == pyl->size)
        return pyl;
    else if (!pyl->arr ||
             !(pyl->flags & payload_FLG_RESIZABLE) ||
             (new_size < pyl->size && !(pyl->flags & payload_FLG_SHRINKABLE)))
        return NULL;
    
    void *ptr = NULL;
    ptr = realloc(pyl->arr, new_size);
    assert(ptr);
    if (!ptr)
        return NULL;
    pyl->arr = (FLT_TYP *)ptr;
    pyl->size = new_size;
    return pyl;
}
