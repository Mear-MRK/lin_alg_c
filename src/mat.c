#include "mat.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "blaseng.h"

const mat_t mat_NULL = {.arr = NULL, .size = 0, .d1 = 0, .d2 = 0};

bool mat_is_null(const mat_t *m)
{
    assert(m);
    if (!m)
        return false;
    return memcmp(m, &mat_NULL, sizeof(mat_t)) == 0;
}

mat_t *mat_construct(mat_t *m, IND_TYP d1, IND_TYP d2)
{
    assert(m);
    assert(d1 > 0);
    assert(d2 > 0);

    if (!m || !mat_is_null(m) || d1 <= 0 || d2 <= 0)
        return m;

    m->d1 = d1;
    m->d2 = d2;
    m->size = d1 * d2;
    m->arr = (FLT_TYP *)ALIGNED_MALLOC(m->size * sizeof(FLT_TYP));
    assert(m->arr);

    return m;
}

void mat_destruct(mat_t *m)
{
    if (m && m->arr)
    {
        ALIGNED_FREE((void *)m->arr);
        *m = mat_NULL;
    }
}

mat_t *mat_init_prealloc(mat_t *m, FLT_TYP arr[], IND_TYP d1, IND_TYP d2)
{
    assert(m && !m->arr);
    assert(arr);
    assert(d1 > 0 && d2 > 0);

    if (!m || d1 <= 0 || d2 <= 0)
        return m;

    m->size = d1 * d2;
    m->d1 = d1;
    m->d2 = d2;
    m->arr = arr;

    return m;
}

mat_t *mat_new(IND_TYP d1, IND_TYP d2)
{
    assert(d1 > 0);
    assert(d2 > 0);
    if (d1 <= 0 || d2 <= 0)
        return NULL;
    mat_t *new_m = (mat_t *)calloc(1, sizeof(mat_t));
    assert(new_m);
    return mat_construct(new_m, d1, d2);
}

void mat_del(mat_t *m)
{
    assert(m);

    if (!m)
        return;
    mat_destruct(m);
    free((void *)m);
}

mat_t *mat_assign(mat_t *m_dst, const mat_t *m_src)
{
    assert(m_dst && m_src);
    assert(m_dst->arr && m_src->arr);
    assert(m_dst->d1 == m_src->d1 && m_dst->d2 == m_src->d2);

    memcpy(m_dst->arr, m_src->arr, m_dst->size * sizeof(FLT_TYP));

    return m_dst;
}

mat_t *mat_fill_zero(mat_t *m)
{
    assert(m && m->arr);

    memset(m->arr, 0, m->size * sizeof(FLT_TYP));

    return m;
}

mat_t *mat_fill_rnd(mat_t *m, FLT_TYP (*rnd)(void))
{
    assert(m && m->arr);

    for (IND_TYP i = 0; i < m->size; i++)
        m->arr[i] = rnd();

    return m;
}

char *mat_to_str(const mat_t *m, char *m_str)
{
    assert(m && m->arr);

    char buff[64] = {0};
    strcpy(m_str, "[");
    for (IND_TYP i = 0; i < m->d1; i++)
    {
        strcat(m_str, (i != 0) ? " [" : "[");
        for (IND_TYP j = 0; j < m->d2; j++)
        {
            sprintf(buff, (j + 1 != m->d2) ? "%g," : "%g", m->arr[i * m->d2 + j]);
            strcat(m_str, buff);
        }
        strcat(m_str, (i + 1 != m->d1) ? "],\n" : "]");
    }
    strcat(m_str, "]");
    return m_str;
}

mat_t *mat_update(mat_t *m_trg, FLT_TYP alpha, const mat_t *m_right)
{
    assert(m_trg);
    assert(m_right);
    assert(m_trg->arr && m_right->arr);
    assert(m_trg->d1 == m_right->d1);
    assert(m_trg->d2 == m_right->d2);

    AXPY(m_trg->size, alpha, m_right->arr, 1, m_trg->arr, 1);

    return m_trg;
}

FLT_TYP *mat_at(const mat_t *m, IND_TYP i, IND_TYP j)
{
    assert(m);
    assert(m->arr);
    i = ((i % m->d1) + m->d1) % m->d1;
    j = ((j % m->d2) + m->d2) % m->d2;
    return m->arr + (i * m->d2 + j);
}

size_t mat_serial_size(const mat_t *m)
{
    assert(m);
    return sizeof(m->d1) + sizeof(m->d2) + m->size * sizeof(FLT_TYP);
}

uint8_t *mat_serialize(const mat_t *m, uint8_t *byte_arr)
{
    assert(m);
    assert(byte_arr);
    size_t sz = 0;
    sz = sizeof(m->d1);
    memcpy(byte_arr, &m->d1, sz);
    byte_arr += sz;
    sz = sizeof(m->d2);
    memcpy(byte_arr, &m->d2, sz);
    byte_arr += sz;
    sz = m->size * sizeof(FLT_TYP);
    memcpy(byte_arr, m->arr, sz);
    byte_arr += sz;
    return byte_arr;
}

const uint8_t *mat_deserialize(mat_t *m, const uint8_t *byte_arr)
{
    assert(m);
    assert(byte_arr);
    IND_TYP d1, d2;
    size_t sz = 0;
    sz = sizeof(m->d1);
    memcpy(&d1, byte_arr, sz);
    byte_arr += sz;
    sz = sizeof(m->d2);
    memcpy(&d2, byte_arr, sz);
    byte_arr += sz;
    mat_construct(m, d1, d2);
    sz = m->size * sizeof(FLT_TYP);
    memcpy(m->arr, byte_arr, sz);
    byte_arr += sz;
    return byte_arr;
}

mat_t *mat_mul(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(result->arr && m_left->arr && m_right->arr);
    assert(result->d1 == m_left->d1 && result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2 && result->d2 == m_right->d2);

    VMUL(m_left->size, m_left->arr, m_right->arr, result->arr);

    return result;
}

mat_t *mat_mulby(mat_t *m_target, const mat_t *m_right)
{
    assert(m_target && m_right);
    assert(m_target->arr && m_right->arr);
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_mul(m_target, m_target, m_right);
}

mat_t *mat_dot(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(m_left->arr && m_right->arr && result->arr);
    assert(m_left->d2 == m_right->d1);
    assert(result->d1 == m_left->d1 && result->d2 == m_right->d2);
    assert(result->arr != m_left->arr && result->arr != m_right->arr);

    GEMM(CblasRowMajor, CblasNoTrans, CblasNoTrans, m_left->d1, m_right->d2, m_left->d2, 1,
         m_left->arr, m_left->d2, m_right->arr, m_right->d2, 0, result->arr, result->d2);

    return result;
}

FLT_TYP mat_norm_2(const mat_t *m)
{
    assert(m);
    assert(m->arr);

    return NRM2(m->size, m->arr, 1);
}

mat_t *mat_add(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(m_left->arr && m_right->arr && result->arr);
    assert(m_left->d1 == m_right->d1 && m_left->d1 == result->d1);
    assert(m_left->d2 == m_right->d2 && m_left->d2 == result->d2);

    VADD(m_left->size, m_left->arr, m_right->arr, result->arr);

    return result;
}

mat_t *mat_sub(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(m_left->arr && m_right->arr && result->arr);
    assert(m_left->d1 == m_right->d1 && m_left->d1 == result->d1);
    assert(m_left->d2 == m_right->d2 && m_left->d2 == result->d2);

    VSUB(m_left->size, m_left->arr, m_right->arr, result->arr);

    return result;
}

mat_t *mat_addto(mat_t *m_target, const mat_t *m_right)
{
    assert(m_target && m_right);
    assert(m_target->arr && m_right->arr);
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_add(m_target, m_target, m_right);
}

mat_t *mat_subfrom(mat_t *m_target, const mat_t *m_right)
{
    assert(m_target && m_right);
    assert(m_target->arr && m_right->arr);
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_sub(m_target, m_target, m_right);
}

mat_t *mat_scale(mat_t *m, FLT_TYP scale)
{
    assert(m);
    assert(m->arr);

    SCAL(m->size, scale, m->arr, 1);

    return m;
}

mat_t *mat_transpose(mat_t *result, const mat_t *target)
{
    assert(result && target);
    assert(result->arr && target->arr);
    assert(result->d2 == target->d1);
    assert(result->d1 == target->d2);
    assert(result->arr != target->arr);

    OMAT('R', 'T', target->d1, target->d2, 1,
         target->arr, target->d2, result->arr, result->d2);

    return result;
}

mat_t *mat_T(mat_t *m)
{
    assert(m);
    assert(m->arr);

    IMAT('R', 'T', m->d1, m->d2, 1, m->arr, m->d2, m->d1);
    IND_TYP tmp = m->d1;
    m->d1 = m->d2;
    m->d2 = tmp;

    return m;
}

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYP eps)
{
    assert(m_1 && m_2);
    assert(m_1->arr && m_2->arr);
    assert(eps > 0);

    if (m_1->d1 != m_2->d1 || m_1->d2 != m_2->d2)
        return false;
    if (m_1->arr == m_2->arr)
        return true;
    FLT_TYP nrm_ratio = mat_norm_2(m_1);
    nrm_ratio += mat_norm_2(m_2);
    if (nrm_ratio == 0)
        return true;
    mat_t result = mat_NULL;
    mat_construct(&result, m_1->d1, m_1->d2);
    mat_sub(&result, m_1, m_2);
    nrm_ratio = 2 * mat_norm_2(&result) / nrm_ratio;
    mat_destruct(&result);

    return nrm_ratio < eps;
}
