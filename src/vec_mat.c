#include "vec_mat.h"

#include <assert.h>

#include "blaseng.h"

vec_t *mat_dot_vec(vec_t *target, const mat_t *ml, const vec_t *vr)
{
    assert(vec_is_valid(target));
    assert(mat_is_valid(ml));
    assert(vec_is_valid(vr));
    assert(ml->d2 == vr->d);
    assert(target->d == ml->d1);
    assert(target->pyl->arr + target->offset != vr->pyl->arr + vr->offset);

    GEMV(CblasRowMajor, CblasNoTrans, ml->d1, ml->d2, 1,
         ml->pyl->arr + ml->offset, ml->d2,
         vr->pyl->arr + vr->offset, vr->step, 0,
         target->pyl->arr + target->offset, target->step);

    return target;
}

vec_t *vec_dot_mat(vec_t *target, const vec_t *vl, const mat_t *mr)
{
    assert(vec_is_valid(target));
    assert(mat_is_valid(mr));
    assert(vec_is_valid(vl));
    assert(mr->d1 == vl->d);
    assert(target->d == mr->d2);
    assert(target->pyl->arr + target->offset != vl->pyl->arr + vl->offset);

    GEMV(CblasRowMajor, CblasTrans, mr->d1, mr->d2, 1,
         mr->pyl->arr + mr->offset, mr->d2,
         vl->pyl->arr + vl->offset, vl->step, 0,
         target->pyl->arr + target->offset, target->step);

    return target;
}

mat_t *vec_outer(mat_t *target, const vec_t *v_left, const vec_t *v_right)
{
    assert(mat_is_valid(target));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(target->d1 == v_left->d && target->d2 == v_right->d);

    mat_fill_zero(target);
    GER(CblasRowMajor, v_left->d, v_right->d, 1,
        v_left->pyl->arr + v_left->offset, v_left->step,
        v_right->pyl->arr + v_right->offset, v_right->step,
        target->pyl->arr + target->offset, v_right->d);

    return target;
}

mat_t *mat_update_outer(mat_t *target, FLT_TYP alpha, const vec_t *v_left, const vec_t *v_right)
{
    assert(mat_is_valid(target));
    assert(vec_is_valid(v_left));
    assert(vec_is_valid(v_right));
    assert(target->d1 == v_left->d && target->d2 == v_right->d);

    GER(CblasRowMajor, v_left->d, v_right->d, alpha,
        v_left->pyl->arr + v_left->offset, v_left->step,
        v_right->pyl->arr + v_right->offset, v_right->step,
        target->pyl->arr + target->offset, target->d2);

    return target;
}
