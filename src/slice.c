#include "slice.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static inline bool same_sign(IND_TYP x, IND_TYP y)
{
    return (x >= 0 && y >= 0) || (x < 0 && y < 0);
}

static inline bool same_diff_sign(IND_TYP x, IND_TYP y, IND_TYP z)
{
    return (x >= y && z >= 0) || (x < y && z < 0);
}

static inline bool ambiguous_dir_sss(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    return (start < -1 && stop > 0) || (start > 0 && stop < -1);
}

static inline bool is_definite_ind(IND_TYP ind)
{
    return ind != slice_IND_UNDEF && ind != slice_IND_P_INF && ind != slice_IND_M_INF;
}

static inline bool valid_sss(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    return is_definite_ind(start) && stop != slice_IND_UNDEF && start != stop && step != 0 &&
           ((step < 0 && stop != slice_IND_P_INF) || (step > 0 && stop != slice_IND_M_INF)) &&
           (stop == slice_IND_P_INF || stop == slice_IND_M_INF ||
            same_diff_sign(stop, start, step) || ambiguous_dir_sss(start, stop, step));
}

static IND_TYP sly_idx(const slice_t *sly, IND_TYP i)
{
    assert(slice_is_valid(sly));

    if (i == slice_IND_UNDEF)
        return slice_IND_UNDEF;
    else if (i == slice_IND_P_INF)
        return sly->stop;
    else if (i == slice_IND_M_INF)
        if (sly->step >= 0)
        {
            IND_TYP index = sly->start - sly->step;
            return (same_sign(index, sly->start)) ? index : slice_IND_M_INF;
        }
        else
        {
            IND_TYP index = sly->start + sly->step;
            return (same_sign(index, sly->start)) ? index : slice_IND_P_INF;
        }

    if (i >= 0)
    {
        if (sly->len != slice_IND_UNDEF && i >= sly->len)
            return (sly->step >= 0) ? slice_IND_P_INF : slice_IND_M_INF;
        IND_TYP index = sly->start + sly->step * i;
        if (same_sign(index, sly->start))
            return index;
        else
            return slice_IND_UNDEF;
    }
    else if (sly->len != slice_IND_UNDEF)
        if (i >= -sly->len)
            return sly->start + sly->step * (sly->len + i);
        else
            return (sly->step >= 0) ? slice_IND_M_INF : slice_IND_P_INF;
    else if (is_definite_ind(sly->stop))
    {
        IND_TYP index = sly->stop + i * sly->step;
        if (same_sign(index, sly->stop))
            return index;
        else
            return slice_IND_UNDEF;
    }
    else
    {
        return (sly->stop == slice_IND_P_INF) ? i * sly->step : i * sly->step - 1;
    }

    return slice_IND_UNDEF;
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
    return sly && (slice_is_null(sly) || valid_sss(sly->start, sly->stop, sly->step));
}

bool slice_is_regulated(const slice_t *slice)
{
    assert(slice);
    return slice->len >= 0 && slice->len != slice_IND_UNDEF;
}

slice_t *slice_set(slice_t *sly, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(sly);

    // if (start == slice_IND_UNDEF)
    //     start = (step >= 0) ? 0 : -1;
    // else if (start == slice_AFTER)
    //     start = -1;
    // else if (start == slice_BEFORE)
    //     start = 0;
    // if (stop == slice_IND_UNDEF)
    //     stop = (step >= 0) ? slice_AFTER : slice_BEFORE;

    // assert(valid_sss(start, stop, step));

    if (!valid_sss(start, stop, step))
    {
        *sly = slice_NULL;
        return sly;
    }

    sly->step = step;

    sly->start = start;

    sly->len = slice_IND_UNDEF;

    if (stop != slice_IND_P_INF && stop != slice_IND_M_INF && same_sign(start, stop))
    {
        sly->len = (stop - start) / step + ((stop - start) % step != 0);
        sly->stop = start + step * sly->len;
    }
    else if (stop == slice_IND_P_INF && start < 0)
    {
        sly->len = (-start) / step + ((-start) % step != 0);
        sly->stop = stop;
    }
    else if (stop == slice_IND_M_INF && start >= 0)
    {
        sly->len = (start + 1) / (-step) + ((start + 1) % (-step) != 0);
        sly->stop = stop;
    }
    else
    {
        sly->len = slice_IND_UNDEF;
        sly->stop = stop;
    }

    return sly;
}

IND_TYP slice_index(const slice_t *sly, IND_TYP i)
{
    assert(slice_is_valid(sly));
    if (sly->len == slice_IND_UNDEF || i == slice_IND_UNDEF || i >= sly->len || i < -sly->len)
        return slice_IND_UNDEF;

    return sly->start + sly->step * (i + ((i >= 0) ? 0 : sly->len));
}

slice_t *slice_combine(slice_t *result, const slice_t *base, const slice_t *over)
{
    assert(result);
    assert(base);
    assert(over);

    if (slice_is_null(base) || slice_is_null(over))
    {
        *result = slice_NULL;
        return result;
    }
    assert(slice_is_valid(base));
    assert(slice_is_valid(over));

    IND_TYP start, stop, step;

    step = base->step * over->step;

    if (base->len != slice_IND_UNDEF)
    {
        slice_t cp_over = *over;
        slice_regulate(&cp_over, base->len);
        start = sly_idx(base, cp_over.start);
        stop = sly_idx(base, cp_over.stop);
    }
    else
    {
        start = sly_idx(base, over->start);
        stop = sly_idx(base, over->stop);
    }
    if ((stop == slice_IND_P_INF && base->step >= 0) || (stop == slice_IND_M_INF && base->step < 0))
        stop = base->stop;
    if ((start == slice_IND_M_INF && base->step >= 0) || (base->step < 0 && start == slice_IND_P_INF))
        start = base->start;

    slice_set(result, start, stop, step);

    return result;
}

slice_t *slice_regulate(slice_t *sly, IND_TYP len)
{
    assert(len >= 0);

    if (slice_is_null(sly))
        return sly;
    if (len <= 0)
    {
        *sly = slice_NULL;
        return sly;
    }
    assert(slice_is_valid(sly));

    if (sly->start < 0)
        sly->start += len;
    if (sly->stop < 0 && is_definite_ind(sly->stop))
        sly->stop += len;

    if (sly->step > 0)
    {
        if (sly->start < 0)
            sly->start = 0;
        if (sly->stop > len || sly->stop == slice_IND_P_INF)
            sly->stop = len;
    }
    else
    {
        if (sly->start >= len)
            sly->start = len - 1;
        if (sly->stop < -1 || sly->stop == slice_IND_M_INF)
            sly->stop = -1;
    }

    if (sly->start < 0 || sly->start >= len || sly->stop < -1 || sly->stop > len ||
        !same_diff_sign(sly->stop, sly->start, sly->step))
    {
        *sly = slice_NULL;
        return sly;
    }

    sly->len = (sly->stop - sly->start) / sly->step + ((sly->stop - sly->start) % sly->step != 0);
    sly->stop = sly->start + sly->step * sly->len;
    if (sly->stop < 0)
        sly->stop = slice_IND_M_INF;

    return sly;
}

char *slice_to_str(const slice_t *sly, char *str)
{
    assert(sly);
    assert(str);

    if (slice_is_null(sly))
    {
        strcpy(str, "slice_NULL");
        return str;
    }
    assert(slice_is_valid(sly));

    if (slice_is_none(sly))
    {
        strcpy(str, "slice_NONE");
        return str;
    }

    str[0] = 0;
    char buff[64] = {0};
    strcat(str, "slice(");
    if (is_definite_ind(sly->start))
        sprintf(buff, "start=%ld ", sly->start);
    else
        strcpy(buff, "start=UNDEF ");
    strcat(str, buff);
    if (is_definite_ind(sly->stop))
        sprintf(buff, "stop=%ld ", sly->stop);
    else if (sly->stop == slice_IND_P_INF)
        strcpy(buff, "stop=AFTER ");
    else if (sly->stop == slice_IND_M_INF)
        strcpy(buff, "stop=BEFORE ");
    else
        strcpy(buff, "stop=UNDEF ");
    strcat(str, buff);
    sprintf(buff, "step=%ld ", sly->step);
    strcat(str, buff);
    if (sly->len != slice_IND_UNDEF)
        sprintf(buff, "len=%ld)", sly->len);
    else
        strcpy(buff, "len=UNDEF)");
    strcat(str, buff);

    return str;
}

slice_t *slice_new(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    slice_t *sly = (slice_t *)calloc(1, sizeof(slice_t));
    assert(sly);
    if (!sly)
        return NULL;
    return slice_set(sly, start, stop, step);
}

void slice_del(slice_t *sly)
{
    if (sly)
    {
        free((void *)sly);
    }
}
