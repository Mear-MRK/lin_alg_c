#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "lin_alg_config.h"

/**
 * payload - Payload structure.
 *
 * This structure represents a payload, which contains:
 * - size: The number of elements in the payload.
 * - arr: A pointer to the array holding the payload data.
 * - ref_count: A reference count for shared payloads.
 * - flags: Flags indicating properties of the payload.
 */
typedef struct payload
{
    FLT_TYP *arr;
    size_t size;
    int ref_count;
    uint32_t flags;
} payload;

#define payload_FLG_NEW 1u
#define payload_FLG_PREALLOC 2u
#define payload_FLG_RESIZABLE 4u
#define payload_FLG_SHRINKABLE 8u

/**
 * payload_NULL - Null payload constant.
 *
 * This macro defines a null payload constant that can be used to
 * initialize payloads or compare against null payloads. It has a
 * size of 0, null pointer for the array, ref count of 0, and no flags set.
 */
#define payload_NULL ((const payload){.size = 0, .arr = NULL, .ref_count = 0, .flags = 0u})

/**
 * Checks if the given payload is valid.
 *
 * A payload is considered valid if it has a non-NULL array pointer
 * and a size greater than 0.
 *
 * @param pyl Pointer to the payload to check.
 * @return true if the payload is valid, false otherwise.
 */
static inline bool payload_is_valid(const payload *pyl)
{
    return pyl &&
           pyl->size > 0 &&
           pyl->arr &&
           pyl->ref_count >= 0;
}

/**
 * Constructs a new payload.
 *
 * Allocates a FLT_TYP array with the given size and returns a pointer to the payload.
 * The returned payload has its reference count set to 1 and RESIZABLE and SHRINKABLE flags set.
 *
 * @param pyl Pointer to allocate the payload at.
 * @param size The number of elements to allocate space for in the payload.
 * @return A pointer to the constructed payload.
 */
payload *payload_construct(payload *pyl, size_t size);

/**
 * Allocates a new payload object with the given size array.
 *
 * Constructs a new payload by allocating memory for it and setting
 * its size. The payload's reference count is initialized to 1. The NEW flag is set.
 * RESIZABLE and SHRINKABLE flags are set as well.
 *
 * @param size The number of elements to allocate space for in the new payload.
 * @return A pointer to the newly constructed payload.
 */
payload *payload_new(size_t size);


payload *payload_prealloc(payload *pyl, FLT_TYP *arr, size_t size);

/**
 * Increases the reference count of the given payload by 1.
 *
 * This increments the reference count, indicating that another pointer now refers
 * to this payload. The payload will not be freed until the reference count reaches 0.
 *
 * @param pyl Pointer to the payload object.
 * @return The same payload pointer pyl.
 */
payload *payload_share(payload *pyl);

/**
 * Decrements the reference count of the given payload.
 * If the reference count reaches 0 after decrementing, the payload is freed.
 *
 * @param pyl Pointer to the payload object.
 */
void payload_release(payload *pyl);

/**
 * Resizes the given payload to the new size.
 *
 * Reallocates the internal data array to the given size.
 *
 * @param pyl Pointer to the payload to resize.
 * @param new_size The new size to resize the payload to.
 * @return A pointer to the resized payload.
 */
payload *payload_resize(payload *pyl, size_t new_size);

/**
 * Copies a region of the source payload into the destination payload.
 *
 * Copies cp_size elements from src starting at src_off to dest starting at dest_off.
 * The source and destination payloads must already be allocated.
 *
 * @param dest Pointer to destination payload to copy into
 * @param dest_off Offset in destination payload to start copying to
 * @param src Pointer to source payload to copy from
 * @param src_off Offset in source payload to start copying from
 * @param cp_size Number of elements to copy
 * @return Number of elements copied
 */
size_t payload_copy(payload *dest, size_t dest_off, const payload *src, size_t src_off, size_t cp_size);


size_t payload_clear_value(payload *trg, size_t offset, size_t end);

static inline FLT_TYP *payload_at(payload *pyl, IND_TYP i)
{
    assert(i >= 0 && (size_t)i < pyl->size);
    return pyl->arr + i;
}