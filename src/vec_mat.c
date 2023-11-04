#include "vec_mat.h"

#include <assert.h>

#include "blaseng.h"

vec_t *mat_dot_vec(vec_t *target, const mat_t *ml, const vec_t *vr)
{
    assert(target && ml && vr);
    assert(ml->ply->arr);
    assert(target->ply->arr);
    assert(vr->ply->arr);
    assert(ml->d2 == vr->d);
    assert(target->ply->arr != vr->ply->arr);
    
    GEMV(CblasRowMajor, CblasNoTrans, ml->d1, ml->d2, 1, ml->ply->arr, ml->d2, vr->ply->arr,
         1, 0, target->ply->arr, 1);

    return target;
}

vec_t *vec_dot_mat(vec_t *target, const vec_t *vl, const mat_t *mr)
{
    assert(target && vl && mr);
    assert(vl->ply->arr);
    assert(mr->ply->arr);
    assert(target->ply->arr);
    assert(vl->d == mr->d1);
    assert(target->ply->arr != vl->ply->arr);
    
    GEMV(CblasRowMajor, CblasTrans, mr->d1, mr->d2, 1, mr->ply->arr, mr->d2, vl->ply->arr,
         1, 0, target->ply->arr, 1);

    return target;
}

mat_t *vec_outer(mat_t *target, const vec_t *v_left, const vec_t *v_right)
{
    assert(target && v_left && v_right);
    assert(v_left->ply->arr && v_right->ply->arr && target->ply->arr);
    assert(target->d1 == v_left->d && target->d2 == v_right->d);
    
    mat_fill_zero(target);
    GER(CblasRowMajor, v_left->d, v_right->d, 1, v_left->ply->arr, 1, v_right->ply->arr, 1,
        target->ply->arr, v_right->d);
    
    return target;
}

mat_t *mat_update_outer(mat_t *target, FLT_TYP alpha, const vec_t *v_left, const vec_t *v_right)
{
    assert(target && v_left && v_right);
    assert(v_left->ply->arr && v_right->ply->arr && target->ply->arr);
    assert(target->d1 == v_left->d && target->d2 == v_right->d);

    GER(CblasRowMajor, v_left->d, v_right->d, alpha, v_left->ply->arr, 1, v_right->ply->arr, 1,
        target->ply->arr, target->d2);
    
    return target;
}
