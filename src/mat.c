#include "mat.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "blaseng.h"

bool mat_is_null(const mat *m)
{
    assert(m);

    return memcmp(m, &mat_NULL, sizeof(mat)) == 0;
}

bool mat_is_valid(const mat *m)
{
    return m &&
           !mat_is_null(m) &&
           m->d1 > 0 && m->d2 > 0 &&
           m->size == m->d1 * m->d2 &&
           m->offset >= 0 &&
           payload_is_valid(m->pyl) &&
           m->pyl->size >= (size_t)(m->offset + m->size);
}

mat *mat_construct(mat *m, IND_TYP d1, IND_TYP d2)
{
    assert(m);
    assert(d1 > 0);
    assert(d2 > 0);

    if (!m)
        return NULL;

    if (d1 <= 0 || d2 <= 0)
    {
        *m = mat_NULL;
        return m;
    }

    m->d1 = d1;
    m->d2 = d2;
    m->size = d1 * d2;
    m->offset = 0;
    m->pyl = payload_new(m->size);
    if (!payload_is_valid(m->pyl))
        *m = mat_NULL;
    return m;
}

mat *mat_construct_prealloc(mat *m, payload *pyl, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    assert(m);
    assert(payload_is_valid(pyl));
    assert(d1 > 0);
    assert(d2 > 0);
    assert(offset >= 0);
    assert((size_t)(offset + d1 * d2) <= pyl->size);

    if (!m)
        return NULL;

    m->size = d1 * d2;
    if (d1 <= 0 || d2 <= 0 || offset < 0 || (size_t)(offset + m->size) > pyl->size)
    {
        *m = mat_NULL;
        return m;
    }

    m->d1 = d1;
    m->d2 = d2;
    m->offset = offset;
    m->pyl = payload_share(pyl);
    return m;
}

mat *mat_reform(mat *m, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    assert(mat_is_valid(m));
    assert(d1 > 0);
    assert(d2 > 0);
    assert(offset >= 0);
    m->size = d1 * d2;
    assert((size_t)(offset + m->size) <= m->pyl->size);
    m->d1 = d1;
    m->d2 = d2;
    m->offset = offset;
    return m;
}

mat *mat_view(mat *m, const mat *src, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    assert(mat_is_valid(m));
    assert(mat_is_valid(src));

    payload_release(m->pyl);

    if (offset < 0)
        offset += src->size;

    m->size = d1 * d2;
    m->offset = src->offset + offset;

    if (d1 <= 0 || d2 <= 0 || offset < 0 || (size_t)(m->offset + m->size) > src->pyl->size)
    {
        *m = mat_NULL;
        return m;
    }
    m->d1 = d1;
    m->d2 = d2;

    m->pyl = payload_share(src->pyl);

    return m;
}

mat *mat_view_new(const mat *src, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    mat *new_m = (mat *)malloc(sizeof(mat));
    assert(new_m);
    *new_m = mat_NULL;
    return mat_view(new_m, src, offset, d1, d2);
}

void mat_destruct(mat *m)
{
    if (m)
    {
        payload_release(m->pyl);
        *m = mat_NULL;
    }
}

mat *mat_new(IND_TYP d1, IND_TYP d2)
{
    assert(d1 > 0);
    assert(d2 > 0);

    if (d1 <= 0 || d2 <= 0)
        return NULL;

    mat *new_m = (mat *)malloc(sizeof(mat));
    assert(new_m);
    if (!new_m)
        return NULL;
    *new_m = mat_NULL;
    return mat_construct(new_m, d1, d2);
}

void mat_del(mat *m)
{
    assert(mat_is_valid(m));
    if (m)
    {
        mat_destruct(m);
        free((void *)m);
    }
}

mat *mat_assign(mat *m_dst, const mat *m_src)
{
    assert(mat_is_valid(m_dst));
    assert(mat_is_valid(m_src));
    assert(m_dst->d1 == m_src->d1);
    assert(m_dst->d2 == m_src->d2);

    FLT_TYP *arr_dst = payload_at(m_dst->pyl, m_dst->offset);
    FLT_TYP *arr_src = payload_at(m_src->pyl, m_src->offset);

    if (arr_dst == arr_src)
        return m_dst;

    memcpy(arr_dst, arr_src, m_dst->size * sizeof(FLT_TYP));

    // payload_copy(m_dst->pyl, m_dst->offset, m_src->pyl, m_src->offset, m_dst->size);

    return m_dst;
}

mat *mat_fill_zero(mat *m)
{
    assert(mat_is_valid(m));

    memset(payload_at(m->pyl, m->offset), 0, m->size * sizeof(FLT_TYP));

    return m;
}

mat *mat_fill_rnd(mat *m, FLT_TYP (*rnd)(void))
{
    assert(mat_is_valid(m));
    assert(rnd);

    for (IND_TYP i = m->offset; i < m->offset + m->size; i++)
        *payload_at(m->pyl, i) = rnd();

    return m;
}

mat *mat_fill_gen(mat *m, FLT_TYP (*gen)(const void *param), const void *param)
{
    assert(mat_is_valid(m));
    assert(gen);

    for (IND_TYP i = m->offset; i < m->offset + m->size; i++)
        *payload_at(m->pyl, i) = gen(param);

    return m;
}

char *mat_to_str(const mat *m, char *m_str)
{
    assert(m);
    if (mat_is_null(m))
    {
        strcpy(m_str, "mat_NULL\n");
        return m_str;
    }
    assert(mat_is_valid(m));

    char buff[64] = {0};
    strcpy(m_str, "[");
    for (IND_TYP i = 0; i < m->d1; i++)
    {
        strcat(m_str, (i != 0) ? " [" : "[");
        for (IND_TYP j = 0; j < m->d2; j++)
        {
            sprintf(buff, (j + 1 != m->d2) ? "%g," : "%g", *mat_at(m, i, j));
            strcat(m_str, buff);
        }
        strcat(m_str, (i + 1 != m->d1) ? "],\n" : "]");
    }
    strcat(m_str, "]");
    return m_str;
}

mat *mat_update(mat *m_trg, FLT_TYP alpha, const mat *m_right)
{
    assert(mat_is_valid(m_trg));
    assert(mat_is_valid(m_right));
    assert(m_trg->d1 == m_right->d1);
    assert(m_trg->d2 == m_right->d2);

    AXPY(m_trg->size, alpha,
         payload_at(m_right->pyl, m_right->offset), 1,
         payload_at(m_trg->pyl, m_trg->offset), 1);

    return m_trg;
}

FLT_TYP *mat_at(const mat *m, IND_TYP i, IND_TYP j)
{
    assert(mat_is_valid(m));

    if (i < 0)
        i += m->d1;
    if (j < 0)
        j += m->d2;
    assert(i >= 0 && i < m->d1);
    assert(j >= 0 && j < m->d2);
    return payload_at(m->pyl, m->offset + i * m->d2 + j);
}

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

IND_TYP mat_insert(mat *trg, const mat *src, IND_TYP row_i)
{
    assert(mat_is_valid(trg));
    assert(mat_is_valid(src));
    assert(trg->d2 == src->d2);

    if (row_i < 0)
        row_i += trg->d1;
    assert(row_i >= 0 && row_i < trg->d1);
    if (row_i >= trg->d1)
        return 0;
    IND_TYP nbr_rows_replaced = MIN(src->d1, trg->d1 - row_i);
    memcpy(payload_at(trg->pyl, trg->offset + row_i * trg->d2),
           payload_at(src->pyl, src->offset),
           nbr_rows_replaced * src->d2 * sizeof(FLT_TYP));
    return nbr_rows_replaced;
}

size_t mat_serial_size(const mat *m)
{
    assert(mat_is_valid(m));
    return sizeof(size_t) + sizeof(m->d1) + sizeof(m->d2) + m->size * sizeof(FLT_TYP);
}

uint8_t *mat_serialize(const mat *m, uint8_t *byte_arr)
{
    assert(mat_is_valid(m));
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
    memcpy(byte_arr, payload_at(m->pyl, m->offset), sz);
    byte_arr += sz;
    return byte_arr;
}

const uint8_t *mat_deserialize(mat *m, const uint8_t *byte_arr)
{
    assert(mat_is_valid(m));
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
    memcpy(payload_at(m->pyl, 0), byte_arr, sz);
    byte_arr += sz;
    return byte_arr;
}

mat *mat_mul(mat *result, const mat *m_left, const mat *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VMUL(m_left->size,
         payload_at(m_left->pyl, m_left->offset),
         payload_at(m_right->pyl, m_right->offset),
         payload_at(result->pyl, result->offset));

    return result;
}

mat *mat_div(mat *result, const mat *m_left, const mat *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VDIV(m_left->size,
         payload_at(m_left->pyl, m_left->offset),
         payload_at(m_right->pyl, m_right->offset),
         payload_at(result->pyl, result->offset));

    return result;
}

mat *mat_mulby(mat *m_target, const mat *m_right)
{
    assert(mat_is_valid(m_target));
    assert(mat_is_valid(m_right));
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_mul(m_target, m_target, m_right);
}

mat *mat_dot(mat *result, const mat *m_left, const mat *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d2 == m_right->d2);

    GEMM(CblasRowMajor, CblasNoTrans, CblasNoTrans,
         m_left->d1, m_right->d2, m_left->d2, 1,
         payload_at(m_left->pyl, m_left->offset), m_left->d2,
         payload_at(m_right->pyl, m_right->offset), m_right->d2,
         0, payload_at(result->pyl, result->offset), result->d2);

    return result;
}

FLT_TYP mat_norm_2(const mat *m)
{
    assert(mat_is_valid(m));

    return NRM2(m->size, payload_at(m->pyl, m->offset), 1);
}

FLT_TYP mat_sum(const mat *m)
{
    assert(mat_is_valid(m));

    FLT_TYP one = 1;
    return DOT(m->size, payload_at(m->pyl, m->offset), 1, &one, 0);
}

mat *mat_add(mat *result, const mat *m_left, const mat *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VADD(result->size,
         payload_at(m_left->pyl, m_left->offset),
         payload_at(m_right->pyl, m_right->offset),
         payload_at(result->pyl, result->offset));

    return result;
}

mat *mat_sub(mat *result, const mat *m_left, const mat *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VSUB(result->size,
         payload_at(m_left->pyl, m_left->offset),
         payload_at(m_right->pyl, m_right->offset),
         payload_at(result->pyl, result->offset));

    return result;
}

mat *mat_addto(mat *m_target, const mat *m_right)
{
    assert(mat_is_valid(m_target));
    assert(mat_is_valid(m_right));
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_add(m_target, m_target, m_right);
}

mat *mat_f_addto(mat *m, FLT_TYP f)
{
    assert(mat_is_valid(m));

    AXPY(m->size, 1, &f, 0, payload_at(m->pyl, m->offset), 1);
    return m;
}

mat *mat_subfrom(mat *m_target, const mat *m_right)
{
    assert(mat_is_valid(m_target));
    assert(mat_is_valid(m_right));
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_sub(m_target, m_target, m_right);
}

mat *mat_scale(mat *m, FLT_TYP scale)
{
    assert(mat_is_valid(m));

    SCAL(m->size, scale, payload_at(m->pyl, m->offset), 1);

    return m;
}

mat *mat_square(mat *result, const mat *m)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m));
    assert(result->d1 == m->d1);
    assert(result->d2 == m->d2);

    VSQR(m->size, payload_at(m->pyl, m->offset), payload_at(result->pyl, result->offset));

    return result;
}

mat *mat_sqrt(mat *result, const mat *m)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m));
    assert(result->d1 == m->d1);
    assert(result->d2 == m->d2);

    VSQRT(m->size, payload_at(m->pyl, m->offset), payload_at(result->pyl, result->offset));

    return result;
}

mat *mat_transpose(mat *result, const mat *target)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(target));
    assert(result->d2 == target->d1);
    assert(result->d1 == target->d2);

    OMAT('R', 'T', target->d1, target->d2, 1,
         payload_at(target->pyl, target->offset), target->d2,
         payload_at(result->pyl, result->offset), result->d2);

    return result;
}

mat *mat_T(mat *m)
{
    assert(mat_is_valid(m));

    IMAT('R', 'T', m->d1, m->d2, 1, payload_at(m->pyl, m->offset), m->d2, m->d1);
    IND_TYP tmp = m->d1;
    m->d1 = m->d2;
    m->d2 = tmp;

    return m;
}

bool mat_is_close(const mat *m_1, const mat *m_2, FLT_TYP eps)
{
    assert(mat_is_valid(m_1));
    assert(mat_is_valid(m_2));
    assert(eps > 0);

    if (m_1->d1 != m_2->d1 || m_1->d2 != m_2->d2)
        return false;
    if (payload_at(m_1->pyl, m_1->offset) == payload_at(m_2->pyl,  m_2->offset))
        return true;
    FLT_TYP nrm_ratio = mat_norm_2(m_1);
    nrm_ratio += mat_norm_2(m_2);
    if (nrm_ratio == 0)
        return true;
    mat result = mat_NULL;
    mat_construct(&result, m_1->d1, m_1->d2);
    mat_sub(&result, m_1, m_2);
    nrm_ratio = 2 * mat_norm_2(&result) / nrm_ratio;
    mat_destruct(&result);

    return nrm_ratio < eps;
}
