#include "slice.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static inline bool same_sign(IND_TYP x, IND_TYP y)
{
    return (x >= 0 && y >= 0) || (x < 0 && y < 0);
}

static inline bool same_sign_diff(IND_TYP x, IND_TYP y, IND_TYP z)
{
    return (x > y && z > 0) || (x < y && z < 0) || (x == y && z == 0);
}

static inline bool ambiguous_sss(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    return (start < -1 && stop > 0) || (start > 0 && stop < -1);
}

static inline bool defined_ind(IND_TYP ind)
{
    return ind != slice_IND_UNDEF && ind != slice_PLUS_END && ind != slice_MINUS_END;
}

static inline bool valid_sss(IND_TYP start, IND_TYP stop, IND_TYP step)
{
    return defined_ind(start) && stop != slice_IND_UNDEF && start != stop &&
           (step < 0 && stop != slice_PLUS_END || step > 0 && stop != slice_MINUS_END) &&
           (stop == slice_PLUS_END || stop == slice_MINUS_END ||
            same_sign_diff(stop, start, step) || ambiguous_sss(start, stop, step));
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
           valid_sss(sly->start, sly->stop, sly->step);
}

slice_t *slice_set(slice_t *sly, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(sly);

    // if (start == slice_IND_UNDEF)
    //     start = (step >= 0) ? 0 : -1;
    // else if (start == slice_PLUS_END)
    //     start = -1;
    // else if (start == slice_MINUS_END)
    //     start = 0;
    // if (stop == slice_IND_UNDEF)
    //     stop = (step >= 0) ? slice_PLUS_END : slice_MINUS_END;

    // assert(valid_sss(start, stop, step));

    if (!valid_sss(start, stop, step))
    {
        *sly = slice_NULL;
        return sly;
    }

    sly->step = step;

    sly->start = start;

    sly->len = slice_IND_UNDEF;

    if (stop != slice_PLUS_END && stop != slice_MINUS_END && same_sign(start, stop))
    {
        sly->len = (stop - start) / step + ((stop - start) % step != 0);
        sly->stop = start + step * sly->len;
    }
    else if (stop == slice_PLUS_END && start < 0)
    {
        sly->len = (-start) / step + ((-start) % step != 0);
        sly->stop = stop;
    }
    else if (stop == slice_MINUS_END && start >= 0)
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

    if (i == slice_IND_UNDEF)
        return slice_IND_UNDEF;
    else if (i == slice_PLUS_END)
        return sly->stop;
    else if (i == slice_MINUS_END)
        if (sly->step >= 0)
        {
            IND_TYP index = sly->start - sly->step;
            return (same_sign(index, sly->start)) ? index : slice_MINUS_END;
        }
        else
        {
            IND_TYP index = sly->start + sly->step;
            return (same_sign(index, sly->start)) ? index : slice_PLUS_END;
        }

    if (i >= 0)
    {
        if (sly->len != slice_IND_UNDEF && i >= sly->len)
            return (sly->step >= 0) ? slice_PLUS_END : slice_MINUS_END;
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
            return (sly->step >= 0) ? slice_MINUS_END : slice_PLUS_END;
    else if (defined_ind(sly->stop))
    {
        IND_TYP index = sly->stop + i * sly->step;
        if (same_sign(index, sly->stop))
            return index;
        else 
            return slice_IND_UNDEF;
    } else {
        return (sly->stop == slice_PLUS_END) ? i * sly->step : i * sly->step - 1; 
    }

    return slice_IND_UNDEF;
}

slice_t *slice_combine(slice_t *result, const slice_t *base, const slice_t *over)
{
    assert(result);
    assert(slice_is_valid(base));
    assert(slice_is_valid(over));

    IND_TYP start, stop, step;

    step = base->step * over->step;

    if (base->len != slice_IND_UNDEF)
    {
        slice_t cp_over = *over;
        slice_regulate(&cp_over, base->len);
        start = slice_index(base, cp_over.start);
        stop = slice_index(base, cp_over.stop);
    }
    else
    {
        start = slice_index(base, over->start);
        stop = slice_index(base, over->stop);
    }
    if (stop == slice_PLUS_END && base->step >= 0 || stop == slice_MINUS_END && base->step < 0)
        stop = base->stop;
    if (start == slice_MINUS_END && base->step >= 0 || base->step < 0 && start == slice_PLUS_END)
        start = base->start;

    slice_set(result, start, stop, step);

    return result;
}

slice_t *slice_regulate(slice_t *sly, IND_TYP d)
{
    assert(slice_is_valid(sly));
    assert(d > 0);

    if (sly->start < 0)
        sly->start += d;
    if (sly->stop < 0 && defined_ind(sly->stop))
        sly->stop += d;

    if (sly->step > 0)
    {
        if (sly->start < 0)
            sly->start = 0;
        if (sly->stop > d || sly->stop == slice_PLUS_END)
            sly->stop = d;
    }
    else
    {
        if (sly->start >= d)
            sly->start = d - 1;
        if (sly->stop < -1 || sly->stop == slice_MINUS_END)
            sly->stop = -1;
    }

    if (sly->start < 0 || sly->start >= d || sly->stop < -1 || sly->stop > d ||
        !same_sign_diff(sly->stop, sly->start, sly->step))
    {
        *sly = slice_NULL;
        return sly;
    }

    sly->len = (sly->stop - sly->start) / sly->step + ((sly->stop - sly->start) % sly->step != 0);
    sly->stop = sly->start + sly->step * sly->len;
    if (sly->stop < 0)
        sly->stop = slice_MINUS_END;

    return sly;
}

char *slice_to_str(const slice_t *sly, char *str)
{
    assert(sly);
    assert(str);

    if (slice_is_null(sly))
    {
        strcpy(str, "slice(NULL)");
        return str;
    }
    assert(slice_is_valid(sly));

    if (slice_is_none(sly))
    {
        strcpy(str, "slice(NONE)");
        return str;
    }

    str[0] = 0;
    char buff[64] = {0};
    strcat(str, "slice(");
    if (defined_ind(sly->start))
        sprintf(buff, "start=%ld ", sly->start);
    else
        strcpy(buff, "start=UNDEF ");
    strcat(str, buff);
    if (defined_ind(sly->stop))
        sprintf(buff, "stop=%ld ", sly->stop);
    else if (sly->stop == slice_PLUS_END)
        strcpy(buff, "stop=PLUS_END ");
    else if (sly->stop == slice_MINUS_END)
        strcpy(buff, "stop=MINUS_END ");
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
