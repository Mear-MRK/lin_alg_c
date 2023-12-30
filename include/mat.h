#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "lin_alg_config.h"
#include "payload.h"

typedef struct mat
{
    payload *pyl;
    IND_TYP size;
    IND_TYP d1;
    IND_TYP d2;
    IND_TYP offset;
} mat;

#define mat_NULL ((const mat){.pyl = NULL, .size = 0, .d1 = 0, .d2 = 0, .offset = 0})

bool mat_is_null(const mat *m);

bool mat_is_valid(const mat *m);

mat *mat_construct(mat *m, IND_TYP d1, IND_TYP d2);

mat *mat_construct_prealloc(mat *m, payload *pyl, IND_TYP offset, IND_TYP d1, IND_TYP d2);

mat *mat_reform(mat *m, IND_TYP offset, IND_TYP d1, IND_TYP d2);

mat *mat_view(mat *m, const mat *src, IND_TYP offset, IND_TYP d1, IND_TYP d2);

mat *mat_view_new(const mat *src, IND_TYP offset, IND_TYP d1, IND_TYP d2);

void mat_destruct(mat *m);

mat *mat_new(IND_TYP d1, IND_TYP d2);

void mat_del(mat *m);

mat *mat_assign(mat *m_dst, const mat *m_src);

mat *mat_fill_zero(mat *m);

mat *mat_fill_rnd(mat *m, FLD_TYP (*rnd)(void));

mat *mat_fill_gen(mat *m, FLD_TYP (*gen)(const void *param), const void *param);

mat *mat_add(mat *result, const mat *m_left, const mat *m_right);

mat *mat_sub(mat *result, const mat *m_left, const mat *m_right);

mat *mat_mul(mat *result, const mat *m_left, const mat *m_right);

mat *mat_div(mat *result, const mat *m_left, const mat *m_right);

mat *mat_dot(mat *result, const mat *m_left, const mat *m_right);

mat *mat_addto(mat *m_target, const mat *m_right);
// m += f
mat *mat_f_addto(mat *m, FLD_TYP f);

mat *mat_subfrom(mat *m_target, const mat *m_right);

mat *mat_mulby(mat *m_target, const mat *m_right);

mat *mat_scale(mat *m, FLD_TYP scale);
// result = m^2 (element-wise)
mat *mat_square(mat *result, const mat *m);
// result = sqrt(m)
mat *mat_sqrt(mat *result, const mat *m);

mat *mat_transpose(mat *result, const mat *target);

mat *mat_T(mat *m);

FLT_TYP mat_norm_2(const mat *m);

FLD_TYP mat_sum(const mat *m);

bool mat_is_close(const mat *m_1, const mat *m_2, FLD_TYP eps);
// mat *mat_fill(mat *m, FLD_TYP value);
// mat *mat_diag_init(mat *m, const vec *v);

char *mat_to_str(const mat *m, char *str_buff);
// trg += alpha * m_right
mat *mat_update(mat *trg, FLD_TYP alpha, const mat *m_right);
// Gives pointer to m->ply->arr[i][j]; i or j can be negative
FLD_TYP *mat_at(const mat *m, IND_TYP i, IND_TYP j);
// Replace trg at specified row i and below with src and returns nbr of rows replaced
IND_TYP mat_insert(mat *trg, const mat *src, IND_TYP row_i);

size_t mat_serial_size(const mat *m);
// Returns the pointer to the first byte just after the last written byte to byte_arr
uint8_t *mat_serialize(const mat *m, uint8_t *byte_arr);
// Returns the pointer to the first byte just after the last read byte from byte_arr
const uint8_t *mat_deserialize(mat *m, const uint8_t *byte_arr);
