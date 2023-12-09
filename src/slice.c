#include "slice.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static bool valid_sss(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    return (stop > start && step > 0) ||
           (stop < start && step < 0) ||
           (stop == start && step == 0);
}

bool slice_is_none(const slice_t *sly)
{
    return memcmp(sly, &slice_NONE, sizeof(slice_t)) == 0;
}

bool slice_is_null(const slice_t *sly)
{
    return memcmp(sly, &slice_NULL, sizeof(slice_t)) == 0;
}

bool slice_is_valid(const slice_t *sly)
{
    return sly &&
           ((sly->nbr == 0 && sly->step == 0 && sly->start == sly->stop) ||
            (sly->nbr > 0 &&
             ((sly->step > 0 &&
               sly->start + sly->step * (sly->nbr - 1) < sly->stop &&
               sly->start + sly->step * sly->nbr >= sly->stop) ||
              (sly->step < 0 &&
               sly->start + sly->step * (sly->nbr - 1) > sly->stop &&
               sly->start + sly->step * sly->nbr <= sly->stop))));
}

slice_t *slice_combine(slice_t *result, const slice_t *base, const slice_t *over)
{
    assert(result);
    assert(slice_is_valid(base));
    assert(slice_is_valid(over));
    if (slice_is_none(over))
        *result = *base;
    else if (slice_is_none(base))
        *result = *over;
    else
    {
        result->start = base->start + over->start * base->step;
        result->step = base->step * over->step;
        result->stop = base->start + over->nbr * result->step;
        result->nbr = over->nbr;
    }
    return result;
}

slice_t *slice_canonical(slice_t *sly, IND_TYP d)
{
    assert(slice_is_valid(sly));
    assert(d >= 0);
    if (d == 0)
    {
        sly->nbr = 0;
        sly->stop = sly->start = 0;
        return sly;
    }

    if (sly->stop < 0)
        sly->stop += d;
    if (sly->start < 0)
        sly->start += d;

    if (sly->stop > d)
        sly->stop = d;
    else if (sly->stop < -1)
        sly->stop = -1;
    if (sly->start < 0)
        sly->start = 0;
    else if (sly->start >= d)
        sly->start = d - 1;

    if (!slice_is_valid(sly))
    {
        *sly = slice_NULL;
        return sly;
    }

    sly->nbr = sly->step ? (sly->stop - sly->start) / sly->step +
                               (IND_TYP)(0 != ((sly->stop - sly->start) % sly->step))
                         : (IND_TYP)(sly->start != sly->stop);
    sly->stop = sly->nbr * sly->step + sly->start;

    return sly;
}

IND_TYP slice_index(const slice_t *sly, IND_TYP i)
{
    assert(slice_is_valid(sly));
    if(i < 0)
        i += sly->nbr;
    assert(i >= 0 && i < sly->nbr);
    return sly->start + sly->step * i;
}

slice_t *slice_construct(slice_t *sly, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(sly);
    if (!valid_sss(start, stop, step))
    {
        *sly = slice_NULL;
        return sly;
    }

    sly->start = start;
    sly->step = step;
    sly->nbr = step ? (stop - start) / step + (IND_TYP)(0 != ((stop - start) % step))
                    : (IND_TYP)(start != stop);
    sly->stop = start + sly->nbr * step;

    return sly;
}

slice_t *slice_new(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    slice_t *sly = (slice_t *)calloc(1, sizeof(slice_t));
    assert(sly);
    if (!sly)
        return NULL;
    return slice_construct(sly, start, stop, step);
}

void slice_del(slice_t *sly)
{
    if (sly)
    {
        free((void *)sly);
    }
}
