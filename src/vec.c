
#include "vec.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "blaseng.h"

bool vec_is_null(const vec_t *v)
{
    assert(v);

    if (!v)
        return false;
    return memcmp(v, &vec_NULL, sizeof(vec_t)) == 0;
}

bool vec_is_valid(const vec_t *v)
{
    IND_TYP end = v->offset + (v->d - 1) * v->step;
    return v &&
           !vec_is_null(v) &&
           v->d > 0 &&
           payload_is_valid(v->pyl) &&
           v->offset >= 0 &&
           end >= 0 &&
           v->pyl->size > v->offset &&
           v->pyl->size > end;
}

vec_t *vec_construct(vec_t *v, IND_TYP d)
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

vec_t *vec_construct_prealloc(vec_t *v, payload_t *pyl, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(v);
    assert(payload_is_valid(pyl));

    if (stop < 0)
        stop += pyl->size;
    if (start < 0)
        start += pyl->size;

    if (start < 0 ||
        stop > pyl->size ||
        step == 0 ||
        stop == start ||
        stop > start && step < 0 ||
        stop < start && step > 0)
    {
        *v = vec_NULL;
        return v;
    }
    v->offset = start;
    v->step = step;
    v->d = (stop - start) / step + (IND_TYP)(0 != ((stop - start) % step));
    v->pyl = payload_share(pyl);

    return v;
}

vec_t *vec_view(vec_t *view, const vec_t *src, IND_TYP start, IND_TYP stop, IND_TYP step)
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
        stop > start && step < 0 ||
        stop < start && step > 0)
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

void vec_destruct(vec_t *v)
{
    // assert(vec_is_valid(v));
    if (v)
    {
        payload_release(v->pyl);
        *v = vec_NULL;
    }
}

vec_t *vec_new(IND_TYP d)
{
    assert(d > 0);

    if (d <= 0)
        return NULL;
    vec_t *new_v = (vec_t *)malloc(sizeof(vec_t));
    assert(new_v);
    *new_v = vec_NULL;
    return vec_construct(new_v, d);
}

vec_t *vec_new_view(const vec_t *src, IND_TYP start, IND_TYP stop, IND_TYP step)
{
    assert(vec_is_valid(src));

    vec_t *new_v = (vec_t *)malloc(sizeof(vec_t));
    assert(new_v);
    *new_v = vec_NULL;
    return vec_view(new_v, src, start, stop, step);
}

void vec_del(vec_t *v)
{
    assert(vec_is_valid(v));
    if (v)
    {
        vec_destruct(v);
        free((void *)v);
    }
}

vec_t *vec_copy_arr(vec_t *v, const FLT_TYP arr[])
{
    assert(vec_is_valid(v));
    assert(arr);

    COPY(v->d, arr, 1, v->pyl->arr + v->offset, v->step);

    return v;
}

vec_t *vec_assign(vec_t *dst, const vec_t *src)
{
    assert(vec_is_valid(dst));
    assert(vec_is_valid(src));
    assert(dst->d == src->d);

    COPY(dst->d,
         src->pyl->arr + src->offset, src->step,
         dst->pyl->arr + dst->offset, dst->step);

    return dst;
}

vec_t *vec_fill_rnd(vec_t *v, FLT_TYP (*rnd)(void))
{
    assert(vec_is_valid(v));

    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        v->pyl->arr[i] = rnd();
    }
    return v;
}

/*
static inline void fill_arr(FLT_TYP *pyl, IND_TYP end, FLT_TYP val)
{
    assert(end != 0);
    pyl[0] = val;
    IND_TYP sz;
    for (sz = 1; sz <= end / 2; sz *= 2)
        memcpy(pyl + sz, pyl, sz * sizeof(FLT_TYP));
    if (end != sz)
        memcpy(pyl + sz, pyl, (end - sz) * sizeof(FLT_TYP));
}

vec_t *vec_fill_altimp(vec_t *v, FLT_TYP value)
{
    assert(vec_is_valid(v));
    assert(v->pyl->arr);
    fill_arr(v->pyl->arr, v->d, value);
    return v;
}
*/

vec_t *vec_fill(vec_t *v, FLT_TYP value)
{
    assert(vec_is_valid(v));

    COPY(v->d, &value, 0, v->pyl->arr + v->offset, v->step);

    return v;
}

vec_t *vec_fill_zero(vec_t *v)
{
    assert(vec_is_valid(v));

    if (v->step == 1)
        memset(v->pyl->arr + v->offset, 0, v->d * sizeof(FLT_TYP));
    else
        vec_fill(v, 0);

    return v;
}

char *vec_to_str(const vec_t *v, char *v_str)
{
    assert(vec_is_valid(v));

    char buff[64] = {0};
    strcpy(v_str, "[");
    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        sprintf(buff, (j + 1 != v->d) ? "%g," : "%g", v->pyl->arr[i]);
        strcat(v_str, buff);
    }
    strcat(v_str, "]");
    return v_str;
}

vec_t *vec_add(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VADDI(result->d,
          v_left->pyl->arr + v_left->offset, v_left->step,
          v_right->pyl->arr + v_right->offset, v_right->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_sub(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VSUBI(result->d,
          v_left->pyl->arr + v_left->offset, v_left->step,
          v_right->pyl->arr + v_right->offset, v_right->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_mul(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VMULI(result->d,
          v_left->pyl->arr + v_left->offset, v_left->step,
          v_right->pyl->arr + v_right->offset, v_right->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_div(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(v_left->d == v_right->d && v_left->d == result->d);

    VDIVI(result->d,
          v_left->pyl->arr + v_left->offset, v_left->step,
          v_right->pyl->arr + v_right->offset, v_right->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_sclmul(vec_t *result, const vec_t *v, FLT_TYP alpha)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VMULI(v->d,
          v->pyl->arr + v->offset, v->step,
          &alpha, 0,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_f_addto(vec_t *v, FLT_TYP f)
{
    assert(vec_is_valid(v));

    AXPY(v->d, 1,
         &f, 0,
         v->pyl->arr + v->offset, v->step);

    return v;
}

vec_t *vec_f_sub(vec_t *result, FLT_TYP f, const vec_t *v_right)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v_right));
    assert(v_right->d == result->d);

    VSUBI(result->d,
          &f, 0,
          v_right->pyl->arr + v_right->offset, v_right->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_scale(vec_t *v, FLT_TYP scale)
{
    assert(vec_is_valid(v));

    SCAL(v->d, scale,
         v->pyl->arr + v->offset, v->step);

    return v;
}

vec_t *vec_update(vec_t *v_dst, FLT_TYP alpha, const vec_t *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));
    assert(v_dst->d == v_right->d);

    AXPY(v_dst->d, alpha,
         v_right->pyl->arr + v_right->offset, v_right->step,
         v_dst->pyl->arr + v_dst->offset, v_dst->step);

    return v_dst;
}

vec_t *vec_exp(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VEXPI(result->d,
          v->pyl->arr + v->offset, v->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_log2(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VLOG2I(result->d,
           v->pyl->arr + v->offset, v->step,
           result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_inv(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VINVI(result->d,
          v->pyl->arr + v->offset, v->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_sqrt(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VSQRTI(result->d,
           v->pyl->arr + v->offset, v->step,
           result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_square(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VSQRI(result->d,
          v->pyl->arr + v->offset, v->step,
          result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_tanh(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    VTANHI(result->d,
           v->pyl->arr + v->offset, v->step,
           result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_sigmoid(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    vec_exp(result, vec_sclmul(result, v, -1));
    vec_inv(result, vec_f_addto(result, 1));

    return result;
}

vec_t *vec_relu(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    FLT_TYP zero = 0;
    VFMAXI(v->d,
           v->pyl->arr + v->offset, v->step,
           &zero, 0,
           result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_softmax(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    vec_exp(result, v);
    return vec_scale(result, 1 / vec_sum(result));
}

FLT_TYP vec_max(const vec_t *v)
{
    assert(vec_is_valid(v));

    FLT_TYP max = v->pyl->arr[v->offset];
    for (IND_TYP j = 1; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        if (v->pyl->arr[i] > max)
        {
            max = v->pyl->arr[i];
        }
    }
    return max;
}

IND_TYP vec_argmax(const vec_t *v)
{
    assert(vec_is_valid(v));

    FLT_TYP max = v->pyl->arr[v->offset];
    IND_TYP j_m = 0;
    for (IND_TYP j = 1; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        if (v->pyl->arr[i] > max)
        {
            max = v->pyl->arr[i];
            j_m = j;
        }
    }
    return j_m;
}

FLT_TYP *vec_at(const vec_t *v, IND_TYP i)
{
    assert(vec_is_valid(v));

    if (i < 0)
        i += v->d;
    assert(i >= 0 && i < v->d);

    return v->pyl->arr + (v->offset + i * v->step);
}

vec_t *vec_addto(vec_t *v_dst, const vec_t *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));
    assert(v_dst->d == v_right->d);

    AXPY(v_dst->d, 1,
         v_right->pyl->arr + v_right->offset, v_right->step,
         v_dst->pyl->arr + v_dst->offset, v_dst->step);

    return v_dst;
}

vec_t *vec_subfrom(vec_t *v_dst, const vec_t *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));
    assert(v_dst->d == v_right->d);

    AXPY(v_dst->d, -1,
         v_right->pyl->arr + v_right->offset, v_right->step,
         v_dst->pyl->arr + v_dst->offset, v_dst->step);

    return v_dst;
}

vec_t *vec_mulby(vec_t *v_dst, const vec_t *v_right)
{
    assert(vec_is_valid(v_dst));
    assert(vec_is_valid(v_right));

    return vec_mul(v_dst, v_dst, v_right);
}

FLT_TYP vec_norm_2(const vec_t *v)
{
    assert(vec_is_valid(v));

    return NRM2(v->d, v->pyl->arr + v->offset, v->step);
}

FLT_TYP vec_norm_1(const vec_t *v)
{
    assert(vec_is_valid(v));

    return ASUM(v->d, v->pyl->arr + v->offset, v->step);
}

FLT_TYP vec_sum(const vec_t *v)
{
    assert(vec_is_valid(v));

    FLT_TYP one = 1;
    return DOT(v->d,
               v->pyl->arr + v->offset, v->step,
               &one, 0);
}

vec_t *vec_sign(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    FLT_TYP one = 1;
    VCOPYSIGNI(v->d,
               &one, 0,
               v->pyl->arr + v->offset, v->step,
               result->pyl->arr + result->offset, result->step);

    return result;
}

vec_t *vec_theta(vec_t *result, const vec_t *v)
{
    assert(vec_is_valid(result));
    assert(vec_is_valid(v));
    assert(v->d == result->d);

    FLT_TYP half = 0.5;
    VCOPYSIGNI(v->d,
               &half, 0,
               v->pyl->arr + v->offset, v->step,
               result->pyl->arr + result->offset, result->step);
    vec_f_addto(result, 0.5);

    return result;
}

FLT_TYP vec_dot(const vec_t *v_1, const vec_t *v_2)
{
    assert(vec_is_valid(v_1));
    assert(vec_is_valid(v_2));
    assert(v_1->d == v_2->d);

    return DOT(v_1->d,
               v_1->pyl->arr + v_1->offset, v_1->step,
               v_2->pyl->arr + v_2->offset, v_2->step);
}

bool vec_is_close(const vec_t *v_1, const vec_t *v_2, FLT_TYP eps)
{
    assert(vec_is_valid(v_2));
    assert(v_1->d == v_2->d);
    assert(eps > 0);

    // if (v_1->pyl->arr == v_2->pyl->arr)
    //     return true;

    FLT_TYP nrm_ratio = vec_norm_2(v_1);
    nrm_ratio += vec_norm_2(v_2);
    if (nrm_ratio == 0)
        return true;
    vec_t result = vec_NULL;
    vec_construct(&result, v_1->d);
    vec_sub(&result, v_1, v_2);
    nrm_ratio = 2 * vec_norm_2(&result) / nrm_ratio;
    vec_destruct(&result);
    return nrm_ratio < eps;
}

vec_t *vec_apply(vec_t *v, FLT_TYP (*map)(FLT_TYP))
{
    assert(vec_is_valid(v));

    for (IND_TYP j = 0; j < v->d; j++)
    {
        IND_TYP i = v->offset + j * v->step;
        v->pyl->arr[i] = map(v->pyl->arr[i]);
    }

    return v;
}

size_t vec_serial_size(const vec_t *v)
{
    assert(vec_is_valid(v));

    return sizeof(size_t) + sizeof(v->d) + v->d * sizeof(FLT_TYP);
}

uint8_t *vec_serialize(const vec_t *v, uint8_t *byte_arr)
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
    sz = v->d * sizeof(FLT_TYP);
    COPY(v->d,
         v->pyl->arr + v->offset, v->step,
         (FLT_TYP *)byte_arr, 1);
    byte_arr += sz;
    return byte_arr;
}

const uint8_t *vec_deserialize(vec_t *v, const uint8_t *byte_arr)
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
    sz = v->d * sizeof(FLT_TYP);
    memcpy(v->pyl->arr, byte_arr, sz);
    byte_arr += sz;
    return byte_arr;
}
