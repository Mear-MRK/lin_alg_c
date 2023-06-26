#include "mat_t.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

mat_t *mat_construct(mat_t *m, size_t d1, size_t d2)
{
    assert(m && !m->arr);
    assert(d1 != 0 && d2 != 0);
    
    m->size = d1 * d2;
    if (m && !m->arr && m->size)
    {
        m->d1 = d1;
        m->d2 = d2;
        m->arr = (FLT_TYPE *)ALIGNED_MALLOC(m->size * sizeof(FLT_TYPE));
        assert(m->arr);
    }
    
    return m;
}

void mat_destruct(mat_t *m)
{
    assert(m && m->arr);
    
    if (m && m->arr)
    {
        m->d1 = m->d2 = 0;
        ALIGNED_FREE((void *)m->arr);
        m->arr = NULL;
    }
}

mat_t *mat_init(mat_t *m, FLT_TYPE arr[], size_t d1, size_t d2)
{
    assert(m && !m->arr);
    assert(arr);
    assert(d1 != 0 && d2 != 0);
    
    if (!m || m->arr)
        return m;
    m->size = d1 * d2;
    m->d1 = d1;
    m->d2 = d2;
    m->arr = arr;
    
    return m;
}

mat_t *mat_new(size_t d1, size_t d2)
{
    if (d1 == 0 || d2 == 0)
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
    m = NULL;
}

mat_t *mat_assign(mat_t *m_dst, const mat_t *m_src)
{
    assert(m_dst && m_src);
    assert(m_dst->arr && m_src->arr);
    assert(m_dst->d1 == m_src->d1 && m_dst->d2 == m_src->d2);
    
    memcpy(m_dst->arr, m_src->arr, m_dst->size*sizeof(FLT_TYPE));
    
    return m_dst;
}

mat_t *mat_fill_zero(mat_t *m)
{
    assert(m && m->arr);
    
    memset(m->arr, 0, m->size*sizeof(FLT_TYPE));
    
    return m;
}

mat_t *mat_rnd_fill(mat_t *m, FLT_TYPE (*rnd)(void))
{
    assert(m && m->arr);
    
    for(size_t i = 0; i < m->size; i++)
        m->arr[i] = rnd();
    
    return m;
}

char *mat_to_str(const mat_t *m, char *m_str)
{
    assert(m && m->arr);
    
    char buff[64] = {0};
    strcpy(m_str, "[");
    for (size_t i = 0; i < m->d1; i++)
    {
        strcat(m_str, (i != 0) ? " [" : "[");
        for (size_t j = 0; j < m->d2; j++)
        {
            sprintf(buff, (j + 1 != m->d2) ? "%g," : "%g", m->arr[i * m->d2 + j]);
            strcat(m_str, buff);
        }
        strcat(m_str, (i + 1 != m->d1) ? "],\n" : "]");
    }
    strcat(m_str, "]");
    return m_str;
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

FLT_TYPE mat_norm_2(const mat_t *m)
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

mat_t *mat_scale(mat_t *m, FLT_TYPE scale)
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
    size_t tmp = m->d1;
    m->d1 = m->d2;
    m->d2 = tmp;
    
    return m;
}

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYPE eps)
{
    assert(m_1 && m_2);
    assert(m_1->arr && m_2->arr);
    assert(eps > 0);
    
    if (m_1->d1 != m_2->d1 || m_1->d2 != m_2->d2)
        return false;
    if (m_1->arr == m_2->arr)
        return true;
    FLT_TYPE nrm_ratio = mat_norm_2(m_1);
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
