#include "vec_mat.h"

#include <assert.h>

vec_t *mat_dot_vec(vec_t *target, const mat_t *ml, const vec_t *vr)
{
    assert(target && ml && vr);
    assert(ml->arr && target->arr && vr->arr);
    assert(ml->d2 == vr->size);
    assert(target->arr != vr->arr);
    
    GEMV(CblasRowMajor, CblasNoTrans, ml->d1, ml->d2, 1, ml->arr, ml->d2, vr->arr,
         1, 0, target->arr, 1);

    return target;
}

vec_t *vec_dot_mat(vec_t *target, const vec_t *vl, const mat_t *mr)
{
    assert(target && vl && mr);
    assert(vl->arr && mr->arr && target->arr);
    assert(vl->size == mr->d1);
    assert(target->arr != vl->arr);
    
    GEMV(CblasRowMajor, CblasTrans, mr->d1, mr->d2, 1, mr->arr, mr->d2, vl->arr,
         1, 0, target->arr, 1);

    return target;
}

mat_t *vec_outer(mat_t *target, const vec_t *v_left, const vec_t *v_right)
{
    assert(target && v_left && v_right);
    assert(v_left->arr && v_right->arr && target->arr);
    assert(target->d1 == v_left->size && target->d2 == v_right->size);
    
    mat_fill_zero(target);
    GER(CblasRowMajor, v_left->size, v_right->size, 1, v_left->arr, 1, v_right->arr, 1,
        target->arr, v_right->size);
    
    return target;
}

mat_t *mat_update_outer(mat_t *target, FLT_TYP alpha, const vec_t *v_left, const vec_t *v_right)
{
    assert(target && v_left && v_right);
    assert(v_left->arr && v_right->arr && target->arr);
    assert(target->d1 == v_left->size && target->d2 == v_right->size);

    GER(CblasRowMajor, v_left->size, v_right->size, alpha, v_left->arr, 1, v_right->arr, 1,
        target->arr, target->d2);
    
    return target;
}
