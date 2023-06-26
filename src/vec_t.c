
#include "vec_t.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

vec_t *vec_construct(vec_t *v, size_t d)
{
    assert(v);
    if (v && !v->arr && d)
    {
        v->d = d;
        v->arr = (FLT_TYPE *)ALIGNED_MALLOC(d * sizeof(FLT_TYPE));
        assert(v->arr);
    }
    return v;
}

void vec_destruct(vec_t *v)
{
    assert(v);
    if (v && v->arr)
    {
        v->d = 0;
        ALIGNED_FREE((void *)v->arr);
        v->arr = NULL;
    }
}

vec_t *vec_init(vec_t *v, FLT_TYPE *arr, size_t d)
{
    assert(v);
    if (v && !v->arr && d)
    {
        v->d = d;
        v->arr = arr;
    }
    return v;
}

vec_t *vec_new(size_t d)
{
    if (d == 0)
        return NULL;
    vec_t *new_v = (vec_t *)calloc(1, sizeof(vec_t));
    return vec_construct(new_v, d);
}

void vec_del(vec_t *v)
{
    if (!v)
        return;
    vec_destruct(v);
    free((void *)v);
}

vec_t *vec_copy_arr(vec_t *v, const FLT_TYPE arr[])
{
    assert(v);
    assert(v->arr && arr);
    if (!v || v->arr == arr)
        return v;
    memcpy(v->arr, arr, v->d * sizeof(FLT_TYPE));
    return v;
}

vec_t *vec_assign(vec_t *dst, const vec_t *src)
{
    assert(dst && src);
    assert(dst->arr && src->arr);
    assert(dst->d == src->d);
    if (!dst || !src || dst->arr == src->arr)
        return dst;
    memcpy(dst->arr, src->arr, dst->d * sizeof(FLT_TYPE));
    return dst;
}

vec_t *vec_rnd_fill(vec_t *v, FLT_TYPE (*rnd)(void))
{   
    assert(v);
    assert(v->arr);
    for (size_t i = 0; i < v->d; i++)
        v->arr[i] = rnd();
    return v;
}

static inline void fill_arr(FLT_TYPE *arr, size_t end, FLT_TYPE val)
{
    assert(end != 0);
    arr[0] = val;
    size_t sz;
    for (sz = 1; sz <= end / 2; sz *= 2)
        memcpy(arr + sz, arr, sz * sizeof(FLT_TYPE));
    if (end != sz)
        memcpy(arr + sz, arr, (end - sz) * sizeof(FLT_TYPE));
}

vec_t *vec_fill_alt(vec_t *v, FLT_TYPE value)
{
    assert(v);
    assert(v->arr);
    fill_arr(v->arr, v->d, value);
    return v;
}

vec_t *vec_fill(vec_t *v, FLT_TYPE value)
{
    assert(v);
    assert(v->arr);
    for (size_t i = 0; i < v->d; i++)
        v->arr[i] = value;
    return v;
}

vec_t *vec_fill_zero(vec_t *v)
{
    assert(v);
    assert(v->arr);
    memset(v->arr, 0, v->d * sizeof(FLT_TYPE));
    return v;
}

char *vec_to_str(const vec_t *v, char *v_str)
{
    assert(v);
    assert(v->arr);
    char buff[64] = {0};
    strcpy(v_str, "[");
    for (size_t i = 0; i < v->d; i++)
    {
        sprintf(buff, (i + 1 != v->d) ? "%g," : "%g", v->arr[i]);
        strcat(v_str, buff);
    }
    strcat(v_str, "]");
    return v_str;
}

vec_t *vec_add(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(result->arr && v_left->arr && v_right->arr);
    assert(v_left->d == v_right->d && v_left->d == result->d);
    VADD(v_left->d, v_left->arr, v_right->arr, result->arr);
    return result;
}

vec_t *vec_sub(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(v_left->arr && v_right->arr && result->arr);
    assert(v_left->d == v_right->d && v_left->d == result->d);
    VSUB(v_left->d, v_left->arr, v_right->arr, result->arr);
    return result;
}

vec_t *vec_mul(vec_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(result->arr && v_left->arr && v_right->arr);
    assert(v_left->d == v_right->d && v_left->d == result->d);
    VMUL(v_left->d, v_left->arr, v_right->arr, result->arr);
    return result;
}

vec_t *vec_sclmul(vec_t *result, const vec_t *v, FLT_TYPE alpha)
{
    assert(result && v);
    assert(v->arr && result->arr);
    assert(v->d == result->d);
    // vec_fill_zero(result);
    // AXPY(v->d, alpha, v->arr, 1, result->arr, 1);
    vsMulI(v->d, v->arr, 1, &alpha, 0, result->arr, 1);
    return result;
}

vec_t *vec_f_addto(vec_t *v, FLT_TYPE f)
{
    assert(v);
    assert(v->arr);
    AXPY(v->d, 1, &f, 0, v->arr, 1);
    return v;
}

vec_t *vec_scale(vec_t *v, FLT_TYPE scale)
{
    assert(v);
    assert(v->arr);
    SCAL(v->d, scale, v->arr, 1);
    return v;
}

vec_t *vec_exp(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(v->arr && result->arr);
    assert(v->d == result->d);
    VEXP(v->d, v->arr, result->arr);
    return result;
}

vec_t *vec_inv(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(result->arr && v->arr);
    assert(result->d == v->d);
    VINV(v->d, v->arr, result->arr);
    return result;
}

vec_t *vec_tanh(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(result->arr && v->arr);
    assert(result->d == v->d);
    VTANH(v->d, v->arr, result->arr);
    return result;
}

vec_t *vec_sigmoid(vec_t *result, const vec_t *v)
{
    assert(v && result);
    assert(result->arr && v->arr);
    assert(result->d == v->d);
    vec_exp(result, vec_sclmul(result, v, -1));
    vec_inv(result, vec_f_addto(result, 1));
    return result;
}

vec_t *vec_relu(vec_t *result, const vec_t *v)
{
    assert(result && v);
    assert(result->arr && v->arr);
    assert(result->d == v->d);
    
    FLT_TYPE zero = 0;
    VFMAXI(v->d, v->arr, 1, &zero, 0, result->arr, 1);
    return result;
}

vec_t *vec_addto(vec_t *v_dst, const vec_t *v_right)
{
    assert(v_dst && v_right);
    assert(v_dst->arr && v_right->arr);
    assert(v_dst->d == v_right->d);
    // AXPY(v_dst->d, 1, v_right->arr, 1, v_dst->arr, 1);
    // return v_dst;
    return vec_add(v_dst, v_dst, v_right);
}

vec_t *vec_subfrom(vec_t *v_dst, const vec_t *v_right)
{
    assert(v_dst && v_right);
    assert(v_dst->arr && v_right->arr);
    assert(v_dst->d == v_right->d);
    // AXPY(v_dst->d, -1, v_right->arr, 1, v_dst->arr, 1);
    // return v_dst;
    return vec_sub(v_dst, v_dst, v_right);
}

vec_t *vec_mulby(vec_t *v_dst, const vec_t *v_right)
{
    assert(v_dst && v_right);
    return vec_mul(v_dst, v_dst, v_right);
}

FLT_TYPE vec_norm_2(const vec_t *v)
{
    assert(v);
    assert(v->arr);
    return NRM2(v->d, v->arr, 1);
}

FLT_TYPE vec_norm_1(const vec_t *v)
{
    assert(v);
    assert(v->arr);
    return ASUM(v->d, v->arr, 1);
}

FLT_TYPE vec_sum(const vec_t *v)
{
    assert(v);
    assert(v->arr);
    FLT_TYPE one = 1;
    return DOT(v->d, v->arr, 1, &one, 0);
}

FLT_TYPE vec_dot(const vec_t *v_1, const vec_t *v_2)
{
    assert(v_1 && v_2);
    assert(v_1->arr && v_2->arr);
    assert(v_1->d == v_2->d);
    return DOT(v_1->d, v_1->arr, 1, v_2->arr, 1);
}

bool vec_is_close(const vec_t *v_1, const vec_t *v_2, FLT_TYPE eps)
{
    assert(v_1 && v_2);
    assert(v_1->arr && v_2->arr);
    assert(v_1->d == v_2->d);
    assert(eps > 0);
    if (v_1->arr == v_2->arr)
        return true;
    FLT_TYPE nrm_ratio = vec_norm_2(v_1);
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

vec_t *vec_apply(vec_t *v, FLT_TYPE (*map)(FLT_TYPE))
{
    assert(v);
    assert(v->arr);
    for (size_t i = 0; i < v->d; i++)
        v->arr[i] = map(v->arr[i]);
    return v;
}
