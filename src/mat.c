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

bool mat_is_valid(const mat_t *m)
{
    return m &&
           !mat_is_null(m) &&
           m->d1 > 0 && m->d2 > 0 &&
           m->size == m->d1 * m->d2 &&
           m->offset >= 0 &&
           payload_is_valid(m->pyl) &&
           m->pyl->size >= (m->offset + m->size);
}

mat_t *mat_construct(mat_t *m, IND_TYP d1, IND_TYP d2)
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

mat_t *mat_construct_prealloc(mat_t *m, payload_t *pyl, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    assert(m);
    assert(payload_is_valid(pyl));
    assert(d1 > 0);
    assert(d2 > 0);
    assert(offset >= 0);
    assert(offset + d1*d2 <= pyl->size);

    if (!m)
        return NULL;

    m->size = d1 * d2;
    if (d1 <= 0 || d2 <= 0 || offset < 0 || (offset + m->size) > pyl->size)
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

mat_t *mat_reform(mat_t *m, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    assert(mat_is_valid(m));
    assert(d1 > 0);
    assert(d2 > 0);
    assert(offset >= 0);
    m->size = d1 * d2;
    assert((offset + m->size) <= m->pyl->size);
    m->d1 = d1;
    m->d2 = d2;
    m->offset = offset;    
    return m;
}

mat_t *mat_view(mat_t *m, const mat_t *src, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    assert(mat_is_valid(m));
    assert(mat_is_valid(src));

    payload_release(m->pyl);

    if (offset < 0)
        offset += src->size;

    m->size = d1 * d2;
    m->offset = src->offset + offset;

    if (d1 <= 0 || d2 <= 0 || offset < 0 || m->offset + m->size > src->pyl->size)
    {
        *m = mat_NULL;
        return m;
    }
    m->d1 = d1;
    m->d2 = d2;

    m->pyl = payload_share(src->pyl);

    return m;
}

mat_t *mat_view_new(const mat_t *src, IND_TYP offset, IND_TYP d1, IND_TYP d2)
{
    mat_t *new_m = (mat_t *)malloc(sizeof(mat_t));
    assert(new_m);
    *new_m = mat_NULL;
    return mat_view(new_m, src, offset, d1, d2);
}

void mat_destruct(mat_t *m)
{
    if (m)
    {
        payload_release(m->pyl);
        *m = mat_NULL;
    }
}

mat_t *mat_new(IND_TYP d1, IND_TYP d2)
{
    assert(d1 > 0);
    assert(d2 > 0);

    if (d1 <= 0 || d2 <= 0)
        return NULL;

    mat_t *new_m = (mat_t *)malloc(sizeof(mat_t));
    assert(new_m);
    if (!new_m)
        return NULL;
    *new_m = mat_NULL;
    return mat_construct(new_m, d1, d2);
}

void mat_del(mat_t *m)
{
    assert(mat_is_valid(m));
    if (m)
    {
        mat_destruct(m);
        free((void *)m);
    }
}

mat_t *mat_assign(mat_t *m_dst, const mat_t *m_src)
{
    assert(mat_is_valid(m_dst));
    assert(mat_is_valid(m_src));
    assert(m_dst->d1 == m_src->d1);
    assert(m_dst->d2 == m_src->d2);

    memcpy(m_dst->pyl->arr + m_dst->offset,
           m_src->pyl->arr + m_src->offset,
           m_dst->size * sizeof(FLT_TYP));

    return m_dst;
}

mat_t *mat_fill_zero(mat_t *m)
{
    assert(mat_is_valid(m));

    memset(m->pyl->arr + m->offset, 0, m->size * sizeof(FLT_TYP));

    return m;
}

mat_t *mat_fill_rnd(mat_t *m, FLT_TYP (*rnd)(void))
{
    assert(mat_is_valid(m));
    assert(rnd);

    for (IND_TYP i = 0; i < m->size; i++)
        m->pyl->arr[m->offset + i] = rnd();

    return m;
}

mat_t *mat_fill_gen(mat_t *m, FLT_TYP (*gen)(const void *param), const void *param)
{
    assert(mat_is_valid(m));
    assert(gen);

    for (IND_TYP i = 0; i < m->size; i++)
        m->pyl->arr[m->offset + i] = gen(param);

    return m;
}

char *mat_to_str(const mat_t *m, char *m_str)
{
    assert(m);
    if(mat_is_null(m))
    {
        strcpy(m_str,"mat_NULL\n");
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

mat_t *mat_update(mat_t *m_trg, FLT_TYP alpha, const mat_t *m_right)
{
    assert(mat_is_valid(m_trg));
    assert(mat_is_valid(m_right));
    assert(m_trg->d1 == m_right->d1);
    assert(m_trg->d2 == m_right->d2);

    AXPY(m_trg->size, alpha,
         m_right->pyl->arr + m_right->offset, 1,
         m_trg->pyl->arr + m_trg->offset, 1);

    return m_trg;
}

FLT_TYP *mat_at(const mat_t *m, IND_TYP i, IND_TYP j)
{
    assert(mat_is_valid(m));

    if (i < 0)
        i += m->d1;
    if (j < 0)
        j += m->d2;
    assert(i >= 0 && i < m->d1);
    assert(j >= 0 && j < m->d2);
    return m->pyl->arr + (m->offset + i * m->d2 + j);
}

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

IND_TYP mat_insert(mat_t *trg, const mat_t *src, IND_TYP row_i)
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
    memcpy(trg->pyl->arr + trg->offset + row_i * trg->d2,
           src->pyl->arr + src->offset,
           nbr_rows_replaced * src->d2 * sizeof(FLT_TYP));
    return nbr_rows_replaced;
}

size_t mat_serial_size(const mat_t *m)
{
    assert(mat_is_valid(m));
    return sizeof(size_t) + sizeof(m->d1) + sizeof(m->d2) + m->size * sizeof(FLT_TYP);
}

uint8_t *mat_serialize(const mat_t *m, uint8_t *byte_arr)
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
    memcpy(byte_arr, m->pyl->arr + m->offset, sz);
    byte_arr += sz;
    return byte_arr;
}

const uint8_t *mat_deserialize(mat_t *m, const uint8_t *byte_arr)
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
    memcpy(m->pyl->arr, byte_arr, sz);
    byte_arr += sz;
    return byte_arr;
}

mat_t *mat_mul(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VMUL(m_left->size,
         m_left->pyl->arr + m_left->offset,
         m_right->pyl->arr + m_right->offset,
         result->pyl->arr + result->offset);

    return result;
}

mat_t *mat_div(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VDIV(m_left->size,
         m_left->pyl->arr + m_left->offset,
         m_right->pyl->arr + m_right->offset,
         result->pyl->arr + result->offset);

    return result;
}

mat_t *mat_mulby(mat_t *m_target, const mat_t *m_right)
{
    assert(mat_is_valid(m_target));
    assert(mat_is_valid(m_right));
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_mul(m_target, m_target, m_right);
}

mat_t *mat_dot(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d2 == m_right->d2);

    GEMM(CblasRowMajor, CblasNoTrans, CblasNoTrans,
         m_left->d1, m_right->d2, m_left->d2, 1,
         m_left->pyl->arr + m_left->offset, m_left->d2,
         m_right->pyl->arr + m_right->offset, m_right->d2,
         0, result->pyl->arr + result->offset, result->d2);

    return result;
}

FLT_TYP mat_norm_2(const mat_t *m)
{
    assert(mat_is_valid(m));

    return NRM2(m->size, m->pyl->arr + m->offset, 1);
}

FLT_TYP mat_sum(const mat_t *m)
{
    assert(mat_is_valid(m));

    FLT_TYP one = 1;
    return DOT(m->size, m->pyl->arr + m->offset, 1, &one, 0);
}

mat_t *mat_add(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VADD(result->size,
         m_left->pyl->arr + m_left->offset,
         m_right->pyl->arr + m_right->offset,
         result->pyl->arr + result->offset);

    return result;
}

mat_t *mat_sub(mat_t *result, const mat_t *m_left, const mat_t *m_right)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m_left));
    assert(mat_is_valid(m_right));
    assert(result->d1 == m_left->d1);
    assert(result->d1 == m_right->d1);
    assert(result->d2 == m_left->d2);
    assert(result->d2 == m_right->d2);

    VSUB(result->size,
         m_left->pyl->arr + m_left->offset,
         m_right->pyl->arr + m_right->offset,
         result->pyl->arr + result->offset);

    return result;
}

mat_t *mat_addto(mat_t *m_target, const mat_t *m_right)
{
    assert(mat_is_valid(m_target));
    assert(mat_is_valid(m_right));
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_add(m_target, m_target, m_right);
}

mat_t *mat_f_addto(mat_t *m, FLT_TYP f)
{
    assert(mat_is_valid(m));

    AXPY(m->size, 1, &f, 0, m->pyl->arr + m->offset, 1);
    return m;
}

mat_t *mat_subfrom(mat_t *m_target, const mat_t *m_right)
{
    assert(mat_is_valid(m_target));
    assert(mat_is_valid(m_right));
    assert(m_target->d1 == m_right->d1);
    assert(m_target->d2 == m_right->d2);

    return mat_sub(m_target, m_target, m_right);
}

mat_t *mat_scale(mat_t *m, FLT_TYP scale)
{
    assert(mat_is_valid(m));

    SCAL(m->size, scale, m->pyl->arr + m->offset, 1);

    return m;
}

mat_t *mat_square(mat_t *result, const mat_t *m)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m));
    assert(result->d1 == m->d1);
    assert(result->d2 == m->d2);

    VSQR(m->size, m->pyl->arr + m->offset, result->pyl->arr + result->offset);

    return result;
}

mat_t *mat_sqrt(mat_t *result, const mat_t *m)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(m));
    assert(result->d1 == m->d1);
    assert(result->d2 == m->d2);

    VSQRT(m->size, m->pyl->arr + m->offset, result->pyl->arr + result->offset);

    return result;
}

mat_t *mat_transpose(mat_t *result, const mat_t *target)
{
    assert(mat_is_valid(result));
    assert(mat_is_valid(target));
    assert(result->d2 == target->d1);
    assert(result->d1 == target->d2);

    OMAT('R', 'T', target->d1, target->d2, 1,
         target->pyl->arr + target->offset, target->d2,
         result->pyl->arr + result->offset, result->d2);

    return result;
}

mat_t *mat_T(mat_t *m)
{
    assert(mat_is_valid(m));

    IMAT('R', 'T', m->d1, m->d2, 1, m->pyl->arr + m->offset, m->d2, m->d1);
    IND_TYP tmp = m->d1;
    m->d1 = m->d2;
    m->d2 = tmp;

    return m;
}

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYP eps)
{
    assert(mat_is_valid(m_1));
    assert(mat_is_valid(m_2));
    assert(eps > 0);

    if (m_1->d1 != m_2->d1 || m_1->d2 != m_2->d2)
        return false;
    if (m_1->pyl->arr + m_1->offset == m_2->pyl->arr + m_2->offset)
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
