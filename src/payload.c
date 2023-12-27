#include "payload.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

#define MIN(x, y) ((x) <= (y) ? (x) : (y))

payload *payload_construct(payload *pyl, size_t size)
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

static void payload_destruct(payload *pyl)
{
    assert(payload_is_valid(pyl));

    if (!pyl || !pyl->arr)
        return;
    if (!(pyl->flags & payload_FLG_PREALLOC))
        free((void *)pyl->arr);
    *pyl = payload_NULL;
}

payload *payload_new(size_t size)
{
    payload *pyl = (payload *)calloc(1, sizeof(payload ));
    assert(pyl);
    payload_construct(pyl, size);
    pyl->flags |= payload_FLG_NEW;
    return pyl;
}

static void payload_del(payload *pyl)
{
    if (!pyl || !(pyl->flags & payload_FLG_NEW))
        return;
    payload_destruct(pyl);
    free((void *)pyl);
}

payload *payload_prealloc(payload *pyl, FLT_TYP *arr, size_t size)
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

payload *payload_share(payload *pyl)
{
    assert(payload_is_valid(pyl));

    pyl->ref_count++;
    if (pyl->ref_count > 1)
        pyl->flags &= ~payload_FLG_SHRINKABLE;
    return pyl;
}

void payload_release(payload *pyl)
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
        log_msg(LOG_ERR, "payload_release: ref_count is negative: %d\n", pyl->ref_count);
    }
}

payload *payload_resize(payload *pyl, size_t new_size)
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

size_t payload_copy(payload *dest, size_t dest_off, const payload *src, size_t src_off, size_t cp_size)
{
    assert(payload_is_valid(dest));
    assert(payload_is_valid(src));
    assert(src_off < src->size);
    assert(dest_off < dest->size);

    if(dest->arr == src->arr)
        return 0;

    if (cp_size == 0)
        cp_size = src->size - src_off;
    else
        cp_size = MIN(cp_size, src->size - src_off);
    cp_size = MIN(cp_size, dest->size - dest_off);

    memcpy(dest->arr + dest_off, src->arr + src_off, cp_size * sizeof(FLT_TYP));

    return cp_size;
}
