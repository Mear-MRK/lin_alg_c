#ifndef MAT_T_H_INCLUDED
#define MAT_T_H_INCLUDED 1

#include "lin_alg_config.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    FLT_TYPE *arr;
    size_t size;
    size_t d1;
    size_t d2;
} mat_t;

#define mat_NULL {.arr = NULL, .size = 0, .d1 = 0, .d2 = 0}

mat_t *mat_construct(mat_t *m, size_t d1, size_t d2);

void mat_destruct(mat_t *m);

mat_t *mat_new(size_t d1, size_t d2);

void mat_del(mat_t* m);

mat_t *mat_init(mat_t *m, FLT_TYPE src_arr[], size_t d1, size_t d2);

mat_t *mat_assign(mat_t *m_dst, const mat_t *m_src);

mat_t *mat_fill_zero(mat_t *m);

mat_t *mat_rnd_fill(mat_t *m, FLT_TYPE(*rnd)(void));

mat_t *mat_add(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_sub(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_mul(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_dot(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_addto(mat_t *m_target, const mat_t *m_right);

mat_t *mat_subfrom(mat_t *m_target, const mat_t *m_right);

mat_t *mat_mulby(mat_t *m_target, const mat_t *m_right);

mat_t *mat_scale(mat_t *m, FLT_TYPE scale);

mat_t *mat_transpose(mat_t *result, const mat_t *target);

mat_t *mat_T(mat_t *m);

FLT_TYPE mat_norm_2(const mat_t *m);

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYPE eps);
mat_t *mat_fill(mat_t *m, FLT_TYPE value);
// mat_t *mat_diag_init(mat_t *m, const vec_t *v);

char *mat_to_str(const mat_t *m, char *str_buff);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MAT_T_H_INCLUDED */