#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "payload.h"
#include "lin_alg_config.h"

typedef struct mat_struct
{
    payload_t *pyl;
    IND_TYP size;
    IND_TYP d1;
    IND_TYP d2;
    IND_TYP offset;
} mat_t;

#define mat_NULL ((const mat_t){.pyl = NULL, .size = 0, .d1 = 0, .d2 = 0, .offset = 0})

bool mat_is_null(const mat_t *m);

bool mat_is_valid(const mat_t *m);

mat_t *mat_construct(mat_t *m, IND_TYP d1, IND_TYP d2);

mat_t *mat_construct_prealloc(mat_t *m, payload_t *pyl, IND_TYP d1, IND_TYP d2, IND_TYP offset);

mat_t *mat_view(mat_t *m, const mat_t *src, IND_TYP offset, IND_TYP d1, IND_TYP d2);

mat_t *mat_view_new(const mat_t *src, IND_TYP offset, IND_TYP d1, IND_TYP d2);

void mat_destruct(mat_t *m);

mat_t *mat_new(IND_TYP d1, IND_TYP d2);

void mat_del(mat_t *m);

mat_t *mat_assign(mat_t *m_dst, const mat_t *m_src);

mat_t *mat_fill_zero(mat_t *m);

mat_t *mat_fill_rnd(mat_t *m, FLT_TYP (*rnd)(void));

mat_t *mat_add(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_sub(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_mul(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_div(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_dot(mat_t *result, const mat_t *m_left, const mat_t *m_right);

mat_t *mat_addto(mat_t *m_target, const mat_t *m_right);
// m += f
mat_t *mat_f_addto(mat_t *m, FLT_TYP f);

mat_t *mat_subfrom(mat_t *m_target, const mat_t *m_right);

mat_t *mat_mulby(mat_t *m_target, const mat_t *m_right);

mat_t *mat_scale(mat_t *m, FLT_TYP scale);
// result = m^2 (element-wise)
mat_t *mat_square(mat_t *result, const mat_t *m);
// result = sqrt(m)
mat_t *mat_sqrt(mat_t *result, const mat_t *m);

mat_t *mat_transpose(mat_t *result, const mat_t *target);

mat_t *mat_T(mat_t *m);

FLT_TYP mat_norm_2(const mat_t *m);

FLT_TYP mat_sum(const mat_t *m);

bool mat_is_close(const mat_t *m_1, const mat_t *m_2, FLT_TYP eps);
// mat_t *mat_fill(mat_t *m, FLT_TYP value);
// mat_t *mat_diag_init(mat_t *m, const vec_t *v);

char *mat_to_str(const mat_t *m, char *str_buff);
// trg += alpha * m_right
mat_t *mat_update(mat_t *trg, FLT_TYP alpha, const mat_t *m_right);
// Gives pointer to m->ply->arr[i][j]; i or j can be negative
FLT_TYP *mat_at(const mat_t *m, IND_TYP i, IND_TYP j);
// Replace trg at specified row i and below with src and returns nbr of rows replaced
IND_TYP mat_insert(mat_t *trg, const mat_t *src, IND_TYP row_i);

size_t mat_serial_size(const mat_t *m);
// Returns the pointer to the first byte just after the last written byte to byte_arr
uint8_t *mat_serialize(const mat_t *m, uint8_t *byte_arr);
// Returns the pointer to the first byte just after the last read byte from byte_arr
const uint8_t *mat_deserialize(mat_t *m, const uint8_t *byte_arr);
