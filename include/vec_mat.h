#ifndef VEC_MAT_H_INCLUDED
#define VEC_MAT_H_INCLUDED 1

#include "lin_alg_config.h"
#include "vec.h"
#include "mat.h"

// result = m_left @ v_right : @ = dot product
vec_t *mat_dot_vec(vec_t *result, const mat_t *m_left, const vec_t *v_right);

// result = v_left @ m_right : @ = dot product
vec_t *vec_dot_mat(vec_t *result, const vec_t *v_left, const mat_t *m_right);

// result = v_left (*) v_right : (*) = outer product
mat_t *vec_outer(mat_t *result, const vec_t *v_left, const vec_t *v_right);

// target += alpha * v_left (*) v_right : (*) = outer product
mat_t *mat_update_outer(mat_t *target, FLT_TYP alpha, const vec_t *v_left, const vec_t *v_right);

// Give vec_t corresponing to row i; payload is shared
vec_t *mat_row_at(mat_t *m, vec_t *row, IND_TYP i);

// Give vec_t corresponing to column j; payload is shared
vec_t *mat_column_at(mat_t *m, vec_t *col, IND_TYP j);

#endif /* VEC_MAT_H_INCLUDED */