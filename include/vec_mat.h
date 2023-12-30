#ifndef VEC_MAT_H_INCLUDED
#define VEC_MAT_H_INCLUDED 1

#include "lin_alg_config.h"
#include "vec.h"
#include "mat.h"

// result = m_left @ v_right : @ = dot product
vec *mat_dot_vec(vec *result, const mat *m_left, const vec *v_right);

// result = v_left @ m_right : @ = dot product
vec *vec_dot_mat(vec *result, const vec *v_left, const mat *m_right);

// result = v_left (*) v_right : (*) = outer product
mat *vec_outer(mat *result, const vec *v_left, const vec *v_right);

// target += alpha * v_left (*) v_right : (*) = outer product
mat *mat_update_outer(mat *target, FLD_TYP alpha, const vec *v_left, const vec *v_right);

// Give vec corresponing to row i; payload is shared
vec *mat_row_at(mat *m, vec *row, IND_TYP i);

// Give vec corresponing to column j; payload is shared
vec *mat_column_at(mat *m, vec *col, IND_TYP j);

#endif /* VEC_MAT_H_INCLUDED */