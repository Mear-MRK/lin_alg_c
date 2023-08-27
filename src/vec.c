
#include "vec.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

const vec_t vec_NULL = {.arr = NULL, .size = 0};

bool vec_is_null(const vec_t *v)
{
    assert(v);
    if (!v)
        return false;
    return memcmp(v, &vec_NULL, sizeof(vec_t)) == 0;
}

vec_t *vec_construct(vec_t *v, IND_TYP size)
{
    assert(v);
    if (v && vec_is_null(v) && size > 0)
    {
        v->size = size;
        v->arr = (FLT_TYP *)ALIGNED_MALLOC(size * sizeof(FLT_TYP));
        assert(v->arr);
    }
    return v;
}

void vec_destruct(vec_t *v)
{
    assert(v);
    if (v && v->arr)
    {
        v->size = 0;
        ALIGNED_FREE((void *)v->arr);
        v->arr = NULL;
    }
}

vec_t *vec_init_prealloc(vec_t *v, FLT_TYP *arr, IND_TYP size)
{
    assert(v);
    if (v && size > 0)
    {
        v->size = size;
        v->arr = arr;
    }
    return v;
}

vec_t *vec_new(IND_TYP size)
{
    assert(size > 0);
    if (size <= 0)
        return NULL;
    vec_t *new_v = (vec_t *)calloc(1, sizeof(vec_t));
    return vec_construct(new_v, size);
}

void vec_del(vec_t *v)
{
    if (!v)
        return;
    vec_destruct(v);
    free((void *)v);
}

vec_t *vec_copy_arr(vec_t *v, const FLT_TYP arr[])
{
    assert(v);
    assert(v->arr && arr);
    if (!v || v->arr == arr)
        return v;
    memcpy(v->arr, arr, v->size * sizeof(FLT_TYP));
    return v;
}

vec_t *vec_assign(vec_t *dst, const vec_t *src)
{
    assert(dst && src);
    assert(dst->arr && src->arr);
    assert(dst->size == src->size);
    if (!dst || !src || dst->arr == src->arr)
        return dst;
    memcpy(dst->arr, src->arr, dst->size * sizeof(FLT_TYP));
    return dst;
}

vec_t *vec_fill_rnd(vec_t *v, FLT_TYP (*rnd)(void))
{   
    assert(v);
    assert(v->arr);
    for (IND_TYP i = 0; i < v->size; i++)
        v->arr[i] = rnd();
    return v;
}

static inline void fill_arr(FLT_TYP *arr, IND_TYP end, FLT_TYP val)
{
    assert(end != 0);
    arr[0] = val;
    IND_TYP sz;
    for (sz = 1; sz <= end / 2; sz *= 2)
        memcpy(arr + sz, arr, sz * sizeof(FLT_TYP));
    if (end != sz)
        memcpy(arr + sz, arr, (end - sz) * sizeof(FLT_TYP));
}

vec_t *vec_fill_altimp(vec_t *v, FLT_TYP value)
{
    assert(v);
    assert(v->arr);
    fill_arr(v->arr, v->size, value);
    return v;
}

vec_t *vec_fill(vec_t *v, FLT_TYP value)
{
    assert(v);
    assert(v->arr);
    for (IND_TYP i = 0; i < v->size; i++)
        v->arr[i] = value;
    return v;
}

vec_t *vec_fill_zero(vec_t *v)
{
    assert(v);
    assert(v->arr);
    memset(v->arr, 0, v->size * sizeof(FLT_TYP));
    return v;
}

char *vec_to_str(const vec_t *v, char *v_str)
{
    assert(v);
    assert(v->arr);
    char buff[64] = {0};
    strcpy(v_str, "[");
    for (IND_TYP i = 0; i < v->size; i++)
    {
        sprintf(buff, (i + 1 != v->size) ? "%g," : "%g", v->arr[i]);
        strcat(v_str, buff);
    }
    strcat(v_str, "]");
    return v_str;
}

vec_t *vec_add(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(result->arr && v_left->arr && v_right->arr);
    assert(v_left->size == v_right->size && v_left->size == result->size);
    VADD(v_left->size, v_left->arr, v_right->arr, result->arr);
    return result;
}

vec_t *vec_sub(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(v_left->arr && v_right->arr && result->arr);
    assert(v_left->size == v_right->size && v_left->size == result->size);
    VSUB(v_left->size, v_left->arr, v_right->arr, result->arr);
    return result;
}

vec_t *vec_mul(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(result->arr && v_left->arr && v_right->arr);
    assert(v_left->size == v_right->size && v_left->size == result->size);
    VMUL(v_left->size, v_left->arr, v_right->arr, result->arr);
    return result;
}

vec_t *vec_sclmul(vec_t *result, const vec_t *v, FLT_TYP alpha)
{
    assert(result && v);
    assert(v->arr && result->arr);
    assert(v->size == result->size);
    // vec_fill_zero(result);
    // AXPY(v->size, alpha, v->arr, 1, result->arr, 1);
    VMULI(v->size, v->arr, 1, &alpha, 0, result->arr, 1);
    return result;
}

vec_t *vec_f_addto(vec_t *v, FLT_TYP f)
{
    assert(v);
    assert(v->arr);
    AXPY(v->size, 1, &f, 0, v->arr, 1);
    return v;
}

vec_t *vec_f_sub(vec_t *result, FLT_TYP f, const vec_t *v_right)
{
    assert(v_right);
    assert(result);
    assert(v_right->size == result->size);
    VSUBI(result->size, &f, 0, v_right->arr, 1, result->arr, 1);
    return result;
}

vec_t *vec_scale(vec_t *v, FLT_TYP scale)
{
    assert(v);
    assert(v->arr);
    SCAL(v->size, scale, v->arr, 1);
    return v;
}

vec_t *vec_update(vec_t *v_dst, FLT_TYP alpha, const vec_t *v_right)
{
    assert(v_dst);
    assert(v_right);
    assert(v_dst->arr && v_right->arr);
    assert(v_dst->size == v_right->size);

    AXPY(v_dst->size, alpha, v_right->arr, 1, v_dst->arr, 1);
    return v_dst;
}

vec_t *vec_exp(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(v->arr && result->arr);
    assert(v->size == result->size);
    VEXP(v->size, v->arr, result->arr);
    return result;
}

vec_t *vec_inv(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(result->arr && v->arr);
    assert(result->size == v->size);
    VINV(v->size, v->arr, result->arr);
    return result;
}

vec_t *vec_tanh(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(result->arr && v->arr);
    assert(result->size == v->size);
    VTANH(v->size, v->arr, result->arr);
    return result;
}

vec_t *vec_sigmoid(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(result->arr && v->arr);
    assert(result->size == v->size);
    vec_exp(result, vec_sclmul(result, v, -1));
    vec_inv(result, vec_f_addto(result, 1));
    return result;
}

vec_t *vec_relu(vec_t *result, const vec_t *v)
{
    assert(result && v);
    assert(result->arr && v->arr);
    assert(result->size == v->size);
    
    FLT_TYP zero = 0;
    VFMAXI(v->size, v->arr, 1, &zero, 0, result->arr, 1);
    return result;
}

vec_t *vec_addto(vec_t *v_dst, const vec_t *v_right)
{
    assert(v_dst && v_right);
    assert(v_dst->arr && v_right->arr);
    assert(v_dst->size == v_right->size);
    // AXPY(v_dst->size, 1, v_right->arr, 1, v_dst->arr, 1);
    // return v_dst;
    return vec_add(v_dst, v_dst, v_right);
}

vec_t *vec_subfrom(vec_t *v_dst, const vec_t *v_right)
{
    assert(v_dst && v_right);
    assert(v_dst->arr && v_right->arr);
    assert(v_dst->size == v_right->size);
    // AXPY(v_dst->size, -1, v_right->arr, 1, v_dst->arr, 1);
    // return v_dst;
    return vec_sub(v_dst, v_dst, v_right);
}

vec_t *vec_mulby(vec_t *v_dst, const vec_t *v_right)
{
    assert(v_dst && v_right);
    return vec_mul(v_dst, v_dst, v_right);
}

FLT_TYP vec_norm_2(const vec_t *v)
{
    assert(v);
    assert(v->arr);
    return NRM2(v->size, v->arr, 1);
}

FLT_TYP vec_norm_1(const vec_t *v)
{
    assert(v);
    assert(v->arr);
    return ASUM(v->size, v->arr, 1);
}

FLT_TYP vec_sum(const vec_t *v)
{
    assert(v);
    assert(v->arr);
    FLT_TYP one = 1;
    return DOT(v->size, v->arr, 1, &one, 0);
}

vec_t *vec_sign(vec_t *result, const vec_t *v)
{
    assert(result);
    assert(v);
    assert(result->size == v->size);
    FLT_TYP one = 1;
    VCOPYSIGNI(v->size, &one, 0, v->arr, 1, result->arr, 1);
    return result;
}

vec_t *vec_theta(vec_t *result, const vec_t *v)
{
    assert(result);
    assert(v);
    assert(result->size == v->size);
    FLT_TYP half = 0.5;
    VCOPYSIGNI(v->size, &half, 0, v->arr, 1, result->arr, 1);
    vec_f_addto(result, 0.5);
    return result;
}

FLT_TYP vec_dot(const vec_t *v_1, const vec_t *v_2)
{
    assert(v_1 && v_2);
    assert(v_1->arr && v_2->arr);
    assert(v_1->size == v_2->size);
    return DOT(v_1->size, v_1->arr, 1, v_2->arr, 1);
}

bool vec_is_close(const vec_t *v_1, const vec_t *v_2, FLT_TYP eps)
{
    assert(v_1 && v_2);
    assert(v_1->arr && v_2->arr);
    assert(v_1->size == v_2->size);
    assert(eps > 0);
    if (v_1->arr == v_2->arr)
        return true;
    FLT_TYP nrm_ratio = vec_norm_2(v_1);
    nrm_ratio += vec_norm_2(v_2);
    if (nrm_ratio == 0)
        return true;
    vec_t result = vec_NULL;
    vec_construct(&result, v_1->size);
    vec_sub(&result, v_1, v_2);
    nrm_ratio = 2 * vec_norm_2(&result) / nrm_ratio;
    vec_destruct(&result);
    return nrm_ratio < eps;
}

vec_t *vec_apply(vec_t *v, FLT_TYP (*map)(FLT_TYP))
{
    assert(v);
    assert(v->arr);
    for (IND_TYP i = 0; i < v->size; i++)
        v->arr[i] = map(v->arr[i]);
    return v;
}
