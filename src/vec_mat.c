#include "vec_mat.h"

#include <assert.h>

#include "vector_eng.h"

vec *mat_dot_vec(vec *target, const mat *ml, const vec *vr)
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

vec *vec_dot_mat(vec *target, const vec *vl, const mat *mr)
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

mat *vec_outer(mat *target, const vec *v_left, const vec *v_right)
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

mat *mat_update_outer(mat *target, FLD_TYP alpha, const vec *v_left, const vec *v_right)
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

vec *mat_row_at(mat *m, vec *row, IND_TYP i)
{
    assert(mat_is_valid(m));
    assert(row);
    payload_release(row->pyl);
    if (i < 0)
        i += m->d1;
    assert(i >= 0 && i < m->d1);
    row->pyl = payload_share(m->pyl);
    row->offset = m->offset + i * m->d2;
    row->step = 1;
    row->d = m->d2;
    return row;
}

vec *mat_column_at(mat *m, vec *col, IND_TYP j)
{
    assert(mat_is_valid(m));
    assert(col);
    payload_release(col->pyl);
    if (j < 0)
        j += m->d2;
    assert(j >= 0 && j < m->d2);
    col->pyl = payload_share(m->pyl);
    col->offset = m->offset + j;
    col->step = m->d2;
    col->d = m->d1;
    return col;
}
