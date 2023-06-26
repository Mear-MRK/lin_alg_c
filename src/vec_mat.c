#include "vec_mat.h"

#include <assert.h>

vec_t *mat_dot_vec(vec_t *result, const mat_t *ml, const vec_t *vr)
{
    assert(result && ml && vr);
    assert(ml->arr && result->arr && vr->arr);
    assert(ml->d2 == vr->d);
    assert(result->arr != vr->arr);
    
    GEMV(CblasRowMajor, CblasNoTrans, ml->d1, ml->d2, 1, ml->arr, ml->d2, vr->arr,
         1, 0, result->arr, 1);

    return result;
}

vec_t *vec_dot_mat(vec_t *result, const vec_t *vl, const mat_t *mr)
{
    assert(result && vl && mr);
    assert(vl->arr && mr->arr && result->arr);
    assert(vl->d == mr->d1);
    assert(result->arr != vl->arr);
    
    GEMV(CblasRowMajor, CblasTrans, mr->d1, mr->d2, 1, mr->arr, mr->d2, vl->arr,
         1, 0, result->arr, 1);

    return result;
}

mat_t *vec_outer(mat_t *result, const vec_t *v_left, const vec_t *v_right)
{
    assert(result && v_left && v_right);
    assert(v_left->arr && v_right->arr && result->arr);
    assert(result->d1 == v_left->d && result->d2 == v_right->d);
    
    mat_fill_zero(result);
    GER(CblasRowMajor, v_left->d, v_right->d, 1, v_left->arr, 1, v_right->arr, 1,
        result->arr, v_right->d);
    
    return result;
}
