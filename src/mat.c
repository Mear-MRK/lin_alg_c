#include "mat.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "blaseng.h"

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

    if (!m || d1 <= 0 || d2 <= 0)
        return NULL;

    m->d1 = d1;
    m->d2 = d2;
    m->size = (IND_TYP)d1 * (IND_TYP)d2;
    m->ply->arr = (FLT_TYP *)ALIGNED_MALLOC(m->size * sizeof(FLT_TYP));
    assert(m->ply->arr);

    return m;
}

void mat_destruct(mat_t *m)
{
    if (m && m->ply->arr)
    {
        ALIGNED_FREE((void *)m->ply->arr);
        *m = mat_NULL;
    }
}

mat_t *mat_init_prealloc(mat_t *m, FLT_TYP ply[], IND_TYP d1, IND_TYP d2)
{
    assert(m); //&& !m->ply->arr);
    assert(ply);
    assert(d1 > 0);
    assert(d2 > 0);

    m->size = (IND_TYP)d1 * (IND_TYP)d2;
    m->d1 = d1;
    m->d2 = d2;
    m->ply->arr = ply;

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
    assert(m_dst->ply->arr && m_src->ply->arr);
    assert(m_dst->d1 == m_src->d1 && m_dst->d2 == m_src->d2);

    memcpy(m_dst->ply->arr, m_src->ply->arr, m_dst->size * sizeof(FLT_TYP));

    return m_dst;
}

mat_t *mat_fill_zero(mat_t *m)
{
    assert(m && m->ply->arr);

    memset(m->ply->arr, 0, m->size * sizeof(FLT_TYP));

    return m;
}

mat_t *mat_fill_rnd(mat_t *m, FLT_TYP (*rnd)(void))
{
    assert(m && m->ply->arr);

    for (IND_TYP i = 0; i < m->size; i++)
        m->ply->arr[i] = rnd();

    return m;
}

char *mat_to_str(const mat_t *m, char *m_str)
{
    assert(m && m->ply->arr);

    char buff[64] = {0};
    strcpy(m_str, "[");
    for (IND_TYP i = 0; i < m->d1; i++)
    {
        strcat(m_str, (i != 0) ? " [" : "[");
        for (IND_TYP j = 0; j < m->d2; j++)
        {
            sprintf(buff, (j + 1 != m->d2) ? "%g," : "%g", m->ply->arr[i * m->d2 + j]);
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
    assert(m_trg->ply->arr && m_right->ply->arr);
    assert(m_trg->d1 == m_right->d1);
    assert(m_trg->d2 == m_right->d2);

    AXPY(m_trg->size, alpha, m_right->ply->arr, 1, m_trg->ply->arr, 1);

    return m_trg;
}

FLT_TYP *mat_at(const mat_t *m, IND_TYP i, IND_TYP j)
{
    assert(m);
    assert(m->ply->arr);
    if (i < 0)
        i += m->d1;
    if (j < 0)
        j += m->d2;
    assert(i >= 0 && i < m->d1);
    assert(j >= 0 && j < m->d2);
    return m->ply->arr + (i * m->d2 + j);
}

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

IND_TYP mat_insert(mat_t *trg, const mat_t *src, IND_TYP row_i)
{
    assert(trg);
    assert(src);
    assert(trg->ply->arr);
    assert(src->ply->arr);
    assert(trg->d2 == src->d2);
    if (row_i < 0)
        row_i += trg->d1;
    assert(row_i >= 0 && row_i < trg->d1);
    if (row_i >= trg->d1)
        return 0;
    IND_TYP nbr_rows_replaced = MIN(src->d1, trg->d1 - row_i);
    memcpy(trg->ply->arr + row_i * trg->d2, src->ply->arr, nbr_rows_replaced * src->d2 * sizeof(FLT_TYP));
    return nbr_rows_replaced;
}

size_t mat_serial_size(const mat_t *m)
{
    assert(m);
    return sizeof(size_t) + sizeof(m->d1) + sizeof(m->d2) + m->size * sizeof(FLT_TYP);
}

uint8_t *mat_serialize(const mat_t *m, uint8_t *byte_arr)
{
    assert(m);
    assert(byte_arr);
    size_t sz = 0;
    size_t sr_sz = mat_serial_size(m);
    sz = sizeof(sr_sz);
    memcpy(byte_arr, &sr_sz, sz);
    byte_arr += sz;
    sz = sizeof(m->d1);
    memcpy(byte_arr, &m->d1, sz);
    byte_arr += sz;
    sz = sizeof(m->d2);
    memcpy(byte_arr, &m->d2, sz);
    byte_arr += sz;
    sz = m->size * sizeof(FLT_TYP);
    memcpy(byte_arr, m->ply->arr, sz);
    byte_arr += sz;
    return byte_arr;
}

const uint8_t *mat_deserialize(mat_t *m, const uint8_t *byte_arr)
{
    assert(m);
    assert(byte_arr);
    IND_TYP d1, d2;
    byte_arr += sizeof(size_t);
    size_t sz = 0;
    sz = sizeof(m->d1);
    memcpy(&d1, byte_arr, sz);
    byte_arr += sz;
    sz = sizeof(m->d2);
    memcpy(&d2, byte_arr, sz);
    byte_arr += sz;
    mat_construct(m, d1, d2);
    sz = m->size * sizeof(FLT_TYP);
    memcpy(m->ply->arr, byte_arr, sz);
    byte_arr += sz;
    return byte_arr;
}

mat_t *mat_mul(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(result->ply->arr && m_left->ply->arr && m_right->ply->arr);
    assert(result->d1 == m_left->d1 && result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2 && result->d2 == m_right->d2);

    VMUL(m_left->size, m_left->ply->arr, m_right->ply->arr, result->ply->arr);

    return result;
}

mat_t *mat_div(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(result->ply->arr && m_left->ply->arr && m_right->ply->arr);
    assert(result->d1 == m_left->d1 && result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2 && result->d2 == m_right->d2);

    VDIV(m_left->size, m_left->ply->arr, m_right->ply->arr, result->ply->arr);

    return result;
}

mat_t *mat_mulby(mat_t *m_target, const mat_t *m_right)
{
    assert(m_target && m_right);
    assert(m_target->ply->arr && m_right->ply->arr);
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_mul(m_target, m_target, m_right);
}

mat_t *mat_dot(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(m_left->ply->arr && m_right->ply->arr && result->ply->arr);
    assert(m_left->d2 == m_right->d1);
    assert(result->d1 == m_left->d1 && result->d2 == m_right->d2);
    assert(result->ply->arr != m_left->ply->arr && result->ply->arr != m_right->ply->arr);

    GEMM(CblasRowMajor, CblasNoTrans, CblasNoTrans, m_left->d1, m_right->d2, m_left->d2, 1,
         m_left->ply->arr, m_left->d2, m_right->ply->arr, m_right->d2, 0, result->ply->arr, result->d2);

    return result;
}

FLT_TYP mat_norm_2(const mat_t *m)
{
    assert(m);
    assert(m->ply->arr);

    return NRM2(m->size, m->ply->arr, 1);
}

FLT_TYP mat_sum(const mat_t *m)
{
    assert(m);
    assert(m->ply->arr);
    FLT_TYP one = 1;
    return DOT(m->size, m->ply->arr, 1, &one, 0);
}

mat_t *mat_add(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(m_left->ply->arr && m_right->ply->arr && result->ply->arr);
    assert(m_left->d1 == m_right->d1 && m_left->d1 == result->d1);
    assert(m_left->d2 == m_right->d2 && m_left->d2 == result->d2);

    VADD(m_left->size, m_left->ply->arr, m_right->ply->arr, result->ply->arr);

    return result;
}

mat_t *mat_sub(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(result && m_left && m_right);
    assert(m_left->ply->arr && m_right->ply->arr && result->ply->arr);
    assert(m_left->d1 == m_right->d1 && m_left->d1 == result->d1);
    assert(m_left->d2 == m_right->d2 && m_left->d2 == result->d2);

    VSUB(m_left->size, m_left->ply->arr, m_right->ply->arr, result->ply->arr);

    return result;
}

mat_t *mat_addto(mat_t *m_target, const mat_t *m_right)
{
    assert(m_target && m_right);
    assert(m_target->ply->arr && m_right->ply->arr);
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_add(m_target, m_target, m_right);
}

mat_t *mat_f_addto(mat_t *m, FLT_TYP f)
{
    assert(m);
    assert(m->ply->arr);
    AXPY(m->size, 1, &f, 0, m->ply->arr, 1);
    return m;
}

mat_t *mat_subfrom(mat_t *m_target, const mat_t *m_right)
{
    assert(m_target && m_right);
    assert(m_target->ply->arr && m_right->ply->arr);
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_sub(m_target, m_target, m_right);
}

mat_t *mat_scale(mat_t *m, FLT_TYP scale)
{
    assert(m);
    assert(m->ply->arr);

    SCAL(m->size, scale, m->ply->arr, 1);

    return m;
}

mat_t *mat_square(mat_t *result, const mat_t *m)
{
    assert(result);
    assert(m);
    assert(result->ply->arr);
    assert(m->ply->arr);
    assert(result->d1 == m->d1);
    assert(result->d2 == m->d2);

    VSQR(m->size, m->ply->arr, result->ply->arr);

    return result;
}

mat_t *mat_sqrt(mat_t *result, const mat_t *m)
{
    assert(result);
    assert(m);
    assert(result->ply->arr);
    assert(m->ply->arr);
    assert(result->d1 == m->d1);
    assert(result->d2 == m->d2);

    VSQRT(m->size, m->ply->arr, result->ply->arr);

    return result;
}

mat_t *mat_transpose(mat_t *result, const mat_t *target)
{
    assert(result && target);
    assert(result->ply->arr && target->ply->arr);
    assert(result->d2 == target->d1);
    assert(result->d1 == target->d2);
    assert(result->ply->arr != target->ply->arr);

    OMAT('R', 'T', target->d1, target->d2, 1,
         target->ply->arr, target->d2, result->ply->arr, result->d2);

    return result;
}

mat_t *mat_T(mat_t *m)
{
    assert(m);
    assert(m->ply->arr);

    IMAT('R', 'T', m->d1, m->d2, 1, m->ply->arr, m->d2, m->d1);
    IND_TYP tmp = m->d1;
    m->d1 = m->d2;
    m->d2 = tmp;

    return m;
}

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYP eps)
{
    assert(m_1 && m_2);
    assert(m_1->ply->arr && m_2->ply->arr);
    assert(eps > 0);

    if (m_1->d1 != m_2->d1 || m_1->d2 != m_2->d2)
        return false;
    if (m_1->ply->arr == m_2->ply->arr)
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
