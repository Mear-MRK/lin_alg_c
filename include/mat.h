#ifndef MAT_H_INCLUDED
#define MAT_H_INCLUDED 1

#include <stdbool.h>

#include "lin_alg_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct mat_struct
{
    FLT_TYP *arr;
    IND_TYP size;
    IND_TYP d1;
    IND_TYP d2;
} mat_t;

extern const mat_t mat_NULL;

bool mat_is_null(const mat_t *m);

mat_t *mat_construct(mat_t *m, IND_TYP d1, IND_TYP d2);

void mat_destruct(mat_t *m);

mat_t *mat_new(IND_TYP d1, IND_TYP d2);

void mat_del(mat_t* m);

mat_t *mat_init_prealloc(mat_t *m, FLT_TYP src_arr[], IND_TYP d1, IND_TYP d2);

mat_t *mat_assign(mat_t *m_dst, const mat_t *m_src);

mat_t *mat_fill_zero(mat_t *m);

mat_t *mat_fill_rnd(mat_t *m, FLT_TYP(*rnd)(void));

mat_t *mat_add(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_sub(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_mul(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_dot(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_addto(mat_t *m_target, const mat_t *m_right);

mat_t *mat_subfrom(mat_t *m_target, const mat_t *m_right);

mat_t *mat_mulby(mat_t *m_target, const mat_t *m_right);

mat_t *mat_scale(mat_t *m, FLT_TYP scale);

mat_t *mat_transpose(mat_t *result, const mat_t *target);

mat_t *mat_T(mat_t *m);

FLT_TYP mat_norm_2(const mat_t *m);

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYP eps);
mat_t *mat_fill(mat_t *m, FLT_TYP value);
// mat_t *mat_diag_init(mat_t *m, const vec_t *v);

char *mat_to_str(const mat_t *m, char *str_buff);
// trg += alpha * m_right
mat_t *mat_update(mat_t *trg, FLT_TYP alpha, const mat_t* m_right);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MAT_H_INCLUDED */