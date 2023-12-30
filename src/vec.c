#include "vec.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "vector_eng.h"

bool vec_is_null(const vec *v)
{
    assert(v);

    if (!v)
        return false;
    return memcmp(v, &vec_NULL, sizeof(vec)) == 0;
}

bool vec_is_valid(const vec *v)
{
    IND_TYP end = v->offset + (v->d - 1) * v->step;
    return v &&
           !vec_is_null(v) &&
           v->d > 0 &&
           payload_is_valid(v->pyl) &&
           v->offset >= 0 &&
           end >= 0 &&
           v->pyl->size > (size_t)v->offset &&
           v->pyl->size > (size_t)end;
}

vec *vec_construct(vec *v, IND_TYP d)
{
    assert(v);
    assert(d > 0);

    if (!v)
        return NULL;
    if (d <= 0)
    {
        *v = vec_NULL;
        return v;
    }

    v->d = d;
    v->offset = 0;
    v->step = 1;
    v->pyl = payload_new(d);
    if (!payload_is_valid(v->pyl))
        *v = vec_NULL;

    return v;
}

vec *vec_construct_prealloc(vec *v, payload *pyl, IND_TYP offset, IND_TYP d, IND_TYP step)
{
    assert(v);
    assert(payload_is_valid(pyl));

    payload_release(v->pyl);

    IND_TYP end = offset + step * (d - 1);

    if (d <= 0 ||
        offset < 0 || (size_t)offset >= pyl->size ||
        end < 0 || (size_t)end >= pyl->size ||
        (end > offset && step < 0) || (end < offset && step > 0) ||
        !payload_is_valid(pyl))
    {
        *v = vec_NULL;
        return v;
    }
    v->offset = offset;
    v->step = step;
    v->d = d;
    v->pyl = payload_share(pyl);

    return v;
}

vec *vec_reform(vec *v, IND_TYP offset, IND_TYP d, IND_TYP step)
{
    assert(vec_is_valid(v));
    assert(offset >= 0 && (size_t)offset < v->pyl->size);
    assert(d > 0);
    assert(step != 0);
#ifdef DEBUG
    IND_TYP end = offset + step * (d - 1);
    assert(end >= 0 && (size_t)end < v->pyl->size);
    assert(end == offset || (end > offset && step > 0) || (end < offset && step < 0));
#endif
    v->offset = offset;
    v->d = d;
    v->step = step;
    return v;
}

vec *vec_view(vec *view, const vec *src, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(view);
    assert(vec_is_valid(src));

    payload_release(view->pyl);

    if (stop < 0)
        stop += src->d;
    if (start < 0)
        start += src->d;

    if (stop > src->d)
        stop = src->d;
    else if (stop < -1)
        stop = -1;
    if (start < 0)
        start = 0;
    else if (start >= src->d)
        start = src->d - 1;

    if (step == 0 ||
        stop == start ||
        (stop > start && step < 0) ||
        (stop < start && step > 0))
    {
        *view = vec_NULL;
        return view;
    }

    view->offset = start * src->step + src->offset;
    // view->stop = stop * src->step + src->start;
    view->step = step * src->step;
    view->d = (stop - start) / step + (IND_TYP)(0 != ((stop - start) % step));
    view->pyl = payload_share(src->pyl);

    return view;
}

void vec_destruct(vec *v)
{
    // assert(vec_is_valid(v));
    if (v)
    {
        payload_release(v->pyl);
        *v = vec_NULL;
    }
}

vec *vec_new(IND_TYP d)
{
    assert(d > 0);

    if (d <= 0)
        return NULL;
    vec *new_v = (vec *)malloc(sizeof(vec));
    assert(new_v);
    *new_v = vec_NULL;
    return vec_construct(new_v, d);
}

vec *vec_new_view(const vec *src, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(vec_is_valid(src));

    vec *new_v = (vec *)malloc(sizeof(vec));
    assert(new_v);
    *new_v = vec_NULL;
    return vec_view(new_v, src, start, stop, step);
}

void vec_del(vec *v)
{
    assert(vec_is_valid(v));
    if (v)
    {
        vec_destruct(v);
        free((void *)v);
    }
}

vec *vec_copy_arr(vec *v, const FLD_TYP arr[])
{
    assert(vec_is_valid(v));
    assert(arr);

    COPY(v->d, arr, 1, payload_at(v->pyl, v->offset), v->step);

    return v;
}

vec *vec_assign(vec *dst, const vec *src)
{
    assert(vec_is_valid(dst));
    assert(vec_is_valid(src));
    assert(dst->d == src->d);

    COPY(dst->d,
         payload_at(src->pyl, src->offset), src->step,
         payload_at(dst->pyl, dst->offset), dst->step);

    return dst;
}

vec *vec_fill_rnd(vec *v, FLD_TYP (*rnd)(void))
{
    assert(vec_is_valid(v));
    assert(rnd);

    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        *payload_at(v->pyl, i) = rnd();
    }
    return v;
}

vec *vec_fill_gen(vec *v, FLD_TYP (*gen)(const void *), const void *param)
{
    assert(vec_is_valid(v));
    assert(gen);

    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        *payload_at(v->pyl, i) = gen(param);
    }
    return v;
}

/*
static inline void fill_arr(FLD_TYP *pyl, IND_TYP end, FLD_TYP val)
{
    assert(end != 0);
    pyl[0] = val;
    IND_TYP sz;
    for (sz = 1; sz <= end / 2; sz *= 2)
        memcpy(pyl + sz, pyl, sz * sizeof(FLD_TYP));
    if (end != sz)
        memcpy(pyl + sz, pyl, (end - sz) * sizeof(FLD_TYP));
}

vec *vec_fill_altimp(vec *v, FLD_TYP value)
{
    assert(vec_is_valid(v));
    fill_arr(payload_at(v->pyl, 0), v->d, value);
    return v;
}
*/

vec *vec_fill(vec *v, FLD_TYP value)
{
    assert(vec_is_valid(v));

    COPY(v->d, &value, 0, payload_at(v->pyl, v->offset), v->step);

    return v;
}

vec *vec_fill_zero(vec *v)
{
    assert(vec_is_valid(v));

    if (v->step == 1)
        memset(payload_at(v->pyl, v->offset), 0, v->d * sizeof(FLD_TYP));
    else
        vec_fill(v, 0);

    return v;
}

char *vec_to_str(const vec *v, char *v_str)
{
    assert(vec_is_valid(v));

    char buff[64] = {0};
    strcpy(v_str, "[");
    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        sprintf(buff, (j + 1 != v->d) ? "%g," : "%g", *payload_at(v->pyl, i));
        strcat(v_str, buff);
    }
    strcat(v_str, "]");
    return v_str;
}

vec *vec_add(vec *result, const vec *v_left, const vec *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VADDI(result->d,
          payload_at(v_left->pyl, v_left->offset), v_left->step,
          payload_at(v_right->pyl, v_right->offset), v_right->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_sub(vec *result, const vec *v_left, const vec *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VSUBI(result->d,
          payload_at(v_left->pyl, v_left->offset), v_left->step,
          payload_at(v_right->pyl, v_right->offset), v_right->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_mul(vec *result, const vec *v_left, const vec *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VMULI(result->d,
          payload_at(v_left->pyl, v_left->offset), v_left->step,
          payload_at(v_right->pyl, v_right->offset), v_right->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_div(vec *result, const vec *v_left, const vec *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VDIVI(result->d,
          payload_at(v_left->pyl, v_left->offset), v_left->step,
          payload_at(v_right->pyl, v_right->offset), v_right->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_sclmul(vec *result, const vec *v, FLD_TYP alpha)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VMULI(v->d,
          payload_at(v->pyl, v->offset), v->step,
          &alpha, 0,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_f_addto(vec *v, FLD_TYP f)
{
    assert(vec_is_valid(v));

    AXPY(v->d, 1,
         &f, 0,
         payload_at(v->pyl, v->offset), v->step);

    return v;
}

vec *vec_f_sub(vec *result, FLD_TYP f, const vec *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_right));
    assert(v_right->d == result->d);

    VSUBI(result->d,
          &f, 0,
          payload_at(v_right->pyl, v_right->offset), v_right->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_scale(vec *v, FLD_TYP scale)
{
    assert(vec_is_valid(v));

    SCAL(v->d, scale,
         payload_at(v->pyl, v->offset), v->step);

    return v;
}

vec *vec_update(vec *v_dst, FLD_TYP alpha, const vec *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));
    assert(v_dst->d == v_right->d);

    AXPY(v_dst->d, alpha,
         payload_at(v_right->pyl, v_right->offset), v_right->step,
         payload_at(v_dst->pyl, v_dst->offset), v_dst->step);

    return v_dst;
}

vec *vec_exp(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VEXPI(result->d,
          payload_at(v->pyl, v->offset), v->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_log2(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VLOG2I(result->d,
           payload_at(v->pyl, v->offset), v->step,
           payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_inv(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VINVI(result->d,
          payload_at(v->pyl, v->offset), v->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_sqrt(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VSQRTI(result->d,
           payload_at(v->pyl, v->offset), v->step,
           payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_square(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VSQRI(result->d,
          payload_at(v->pyl, v->offset), v->step,
          payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_tanh(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VTANHI(result->d,
           payload_at(v->pyl, v->offset), v->step,
           payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_sigmoid(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    vec_exp(result, vec_sclmul(result, v, -1));
    vec_inv(result, vec_f_addto(result, 1));

    return result;
}

vec *vec_relu(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    FLD_TYP zero = 0;
    VFMAXI(v->d,
           payload_at(v->pyl, v->offset), v->step,
           &zero, 0,
           payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_softmax(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    vec_exp(result, v);
    return vec_scale(result, 1 / vec_sum(result));
}

FLD_TYP vec_max(const vec *v)
{
    assert(vec_is_valid(v));

    FLD_TYP max = *payload_at(v->pyl, v->offset);
    for (IND_TYP j = 1; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        if (*payload_at(v->pyl, i) > max)
        {
            max = *payload_at(v->pyl, i);
        }
    }
    return max;
}

IND_TYP vec_argmax(const vec *v)
{
    assert(vec_is_valid(v));

    FLD_TYP max = *payload_at(v->pyl, v->offset);
    IND_TYP j_m = 0;
    for (IND_TYP j = 1; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        if (*payload_at(v->pyl, i) > max)
        {
            max = *payload_at(v->pyl, i);
            j_m = j;
        }
    }
    return j_m;
}

FLD_TYP *vec_at(const vec *v, IND_TYP i)
{
    assert(vec_is_valid(v));

    if (i < 0)
        i += v->d;
    assert(i >= 0 && i < v->d);

    return payload_at(v->pyl, v->offset + i * v->step);
}

vec *vec_addto(vec *v_dst, const vec *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));
    assert(v_dst->d == v_right->d);

    AXPY(v_dst->d, 1,
         payload_at(v_right->pyl, v_right->offset), v_right->step,
         payload_at(v_dst->pyl, v_dst->offset), v_dst->step);

    return v_dst;
}

vec *vec_subfrom(vec *v_dst, const vec *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));
    assert(v_dst->d == v_right->d);

    AXPY(v_dst->d, -1,
         payload_at(v_right->pyl, v_right->offset), v_right->step,
         payload_at(v_dst->pyl, v_dst->offset), v_dst->step);

    return v_dst;
}

vec *vec_mulby(vec *v_dst, const vec *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));

    return vec_mul(v_dst, v_dst, v_right);
}

FLT_TYP vec_norm_2(const vec *v)
{
    assert(vec_is_valid(v));

    return NRM2(v->d, payload_at(v->pyl, v->offset), v->step);
}

FLT_TYP vec_norm_1(const vec *v)
{
    assert(vec_is_valid(v));

    return ASUM(v->d, payload_at(v->pyl, v->offset), v->step);
}

FLD_TYP vec_sum(const vec *v)
{
    assert(vec_is_valid(v));

    FLD_TYP one = 1;
    return DOT(v->d,
               payload_at(v->pyl, v->offset), v->step,
               &one, 0);
}

vec *vec_sign(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    FLD_TYP one = 1;
    VCOPYSIGNI(v->d,
               &one, 0,
               payload_at(v->pyl, v->offset), v->step,
               payload_at(result->pyl, result->offset), result->step);

    return result;
}

vec *vec_theta(vec *result, const vec *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    FLD_TYP half = 0.5;
    VCOPYSIGNI(v->d,
               &half, 0,
               payload_at(v->pyl, v->offset), v->step,
               payload_at(result->pyl, result->offset), result->step);
    vec_f_addto(result, 0.5);

    return result;
}

FLD_TYP vec_dot(const vec *v_1, const vec *v_2)
{
    assert(vec_is_valid(v_1));
    assert(vec_is_valid(v_2));
    assert(v_1->d == v_2->d);

    return DOT(v_1->d,
               payload_at(v_1->pyl, v_1->offset), v_1->step,
               payload_at(v_2->pyl, v_2->offset), v_2->step);
}

bool vec_is_close(const vec *v_1, const vec *v_2, FLD_TYP eps)
{
    assert(vec_is_valid(v_2));
    assert(v_1->d == v_2->d);
    assert(eps > 0);

    FLD_TYP nrm_ratio = vec_norm_2(v_1);
    nrm_ratio += vec_norm_2(v_2);
    if (nrm_ratio == 0)
        return true;
    vec result = vec_NULL;
    vec_construct(&result, v_1->d);
    vec_sub(&result, v_1, v_2);
    nrm_ratio = 2 * vec_norm_2(&result) / nrm_ratio;
    vec_destruct(&result);
    return nrm_ratio < eps;
}

vec *vec_apply(vec *v, FLD_TYP (*map)(FLD_TYP))
{
    assert(vec_is_valid(v));

    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        *payload_at(v->pyl, i) = map(*payload_at(v->pyl, i));
    }

    return v;
}

size_t vec_serial_size(const vec *v)
{
    assert(vec_is_valid(v));

    return sizeof(size_t) + sizeof(v->d) + v->d * sizeof(FLD_TYP);
}

uint8_t *vec_serialize(const vec *v, uint8_t *byte_arr)
{
    assert(vec_is_valid(v));
    assert(byte_arr);

    size_t sz = 0;
    sz = sizeof(size_t);
    size_t sr_sz = vec_serial_size(v);
    memcpy(byte_arr, &sr_sz, sz);
    byte_arr += sz;
    sz = sizeof(v->d);
    memcpy(byte_arr, &v->d, sz);
    byte_arr += sz;
    sz = v->d * sizeof(FLD_TYP);
    COPY(v->d,
         payload_at(v->pyl, v->offset), v->step,
         (FLD_TYP *)byte_arr, 1);
    byte_arr += sz;
    return byte_arr;
}

const uint8_t *vec_deserialize(vec *v, const uint8_t *byte_arr)
{
    assert(vec_is_valid(v));
    assert(byte_arr);

    vec_destruct(v);

    IND_TYP d;
    byte_arr += sizeof(size_t);
    size_t sz = 0;
    sz = sizeof(v->d);
    memcpy(&d, byte_arr, sz);
    byte_arr += sz;
    vec_construct(v, d);
    sz = v->d * sizeof(FLD_TYP);
    memcpy(payload_at(v->pyl, 0), byte_arr, sz);
    byte_arr += sz;
    return byte_arr;
}
