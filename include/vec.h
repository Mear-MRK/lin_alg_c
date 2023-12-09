#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "payload.h"
#include "lin_alg_config.h"

/*
 */
typedef struct vec_struct
{
    payload_t *pyl;
    IND_TYP d;
    IND_TYP offset;
    IND_TYP step;
} vec_t;

/*
 * The empty (NULL) vector.
 * It's a good practice to initialize any
 * declared vec_t with this value.
 */
#define vec_NULL ((const vec_t){.pyl = NULL, .offset = 0, .step = 0})

/*
 * Checks to find if vec_t v is vec_NULL
 */
bool vec_is_null(const vec_t *v);

/*
*/
bool vec_is_valid(const vec_t *v);

/*
 * Allocates `d` FLT_TYP for v->pyl->arr.
 * `d` must be non-zero;
 * vec_destruct must be called on v at the end of its lifetime.
 * Content of v->pyl->arr is garbage.
 */
vec_t *vec_construct(vec_t *v, IND_TYP d);

vec_t *vec_construct_prealloc(vec_t *v, payload_t *pyl, IND_TYP offset, IND_TYP d, IND_TYP step);

/*
 * Frees v->pyl->arr and sets *v = vec_NULL.
 * Should be called on v's that are constructed by vec_construct*.
 */
void vec_destruct(vec_t *v);

/*
 * Allocates a vec_t in heap, constructs it and returns it.
 * One should call vec_del on it to destruct and free the resources.
 */
vec_t *vec_new(IND_TYP d);

/*
 * Destructs v and frees v
 * which must be allocated by vec_new.
 */
void vec_del(vec_t *v);

/* NOT implemented yet.
vec_t* vec_construct_nil(vec_t* v, IND_TYP d);
vec_t* vec_construct_unit(vec_t* v, IND_TYP d, IND_TYP i);
vec_t* vec_construct_copy(vec_t* v, const vec_t* oth);
*/

vec_t *vec_reform(vec_t *v, IND_TYP offset, IND_TYP d, IND_TYP step);

vec_t *vec_view(vec_t *view, const vec_t *src, IND_TYP start, IND_TYP stop, IND_TYP step);

vec_t *vec_new_view(const vec_t *src, IND_TYP start, IND_TYP stop, IND_TYP step);

// v_dst = v_src : copies src->pyl->arr to dst->pyl->arr; dimensions must be the same.
vec_t *vec_assign(vec_t *v_dst, const vec_t *v_src);

// Copies src_arr to v->pyl->arr
vec_t *vec_copy_arr(vec_t *v, const FLT_TYP src_arr[]);

// Sets all elements of v->pyl->arr to 0
vec_t *vec_fill_zero(vec_t *v);

// Fills v->pyl->arr with random numbers generated by rnd_gen
vec_t *vec_fill_rnd(vec_t *v, FLT_TYP (*rnd_gen)(void));

vec_t *vec_fill_gen(vec_t *v, FLT_TYP (*gen)(const void*), const void *param);

// Fills v->pyl->arr with value
vec_t *vec_fill(vec_t *v, FLT_TYP value);

/* Same as vec_fill with alternative geometric memcpy algorithm.
vec_t *vec_fill_altimp(vec_t *v, FLT_TYP value);
*/

// Gives string representation of vec_t v; be sure str_buff is big enough.
char *vec_to_str(const vec_t *v, char *str_buff);

// Checks closeness of two vectors by comparing norm(v_1-v_2) and (norm(v_1)+norm(v_2))/2 * eps.
bool vec_is_close(const vec_t *v_1, const vec_t *v_2, FLT_TYP eps);

// result = v_left + v_right
vec_t *vec_add(vec_t *result, const vec_t *v_left, const vec_t *v_right);
// result = v_left - v_right
vec_t *vec_sub(vec_t *result, const vec_t *v_left, const vec_t *v_right);
// result = v_left * v_right (element-wise *)
vec_t *vec_mul(vec_t *result, const vec_t *v_left, const vec_t *v_right);
// result = v_left / v_right (element-wise /)
vec_t *vec_div(vec_t *result, const vec_t *v_left, const vec_t *v_right);
// result = alpha * v
vec_t *vec_sclmul(vec_t *result, const vec_t *v, FLT_TYP alpha);
// v_dst += v_right
vec_t *vec_addto(vec_t *v_dst, const vec_t *v_right);
// v_dst -= v_right
vec_t *vec_subfrom(vec_t *v_dst, const vec_t *v_right);
// v_dst *= v_right (element-wise *)
vec_t *vec_mulby(vec_t *v_dst, const vec_t *v_right);
// v += f
vec_t *vec_f_addto(vec_t *v, FLT_TYP f);
// result = f - v_right
vec_t *vec_f_sub(vec_t *result, FLT_TYP f, const vec_t *v_right);
// v *= scale
vec_t *vec_scale(vec_t *v, FLT_TYP scale);
// v_dst += alpha * v_right
vec_t *vec_update(vec_t *v_dst, FLT_TYP alpha, const vec_t *v_right);
// v_left @ v_right : @ = dot product
FLT_TYP vec_dot(const vec_t *v_left, const vec_t *v_right);
// Euclidean norm of v : sqrt(sum_i v->pyl->arr[i]^2)
FLT_TYP vec_norm_2(const vec_t *v);
// Sum of absolute values of v->pyl->arr
FLT_TYP vec_norm_1(const vec_t *v);
// Sum of elements of v
FLT_TYP vec_sum(const vec_t *v);
// Sign of elements of v
vec_t *vec_sign(vec_t *result, const vec_t *v);
// Theta: step function
vec_t *vec_theta(vec_t *result, const vec_t *v);

/* NOT implemented yet.
FLT_TYP vec_mean(const vec_t* v);
FLT_TYP vec_std(const vec_t* v);
*/

/* Applies map on each element of v->pyl->arr.
 * Don't use this in the cases where the function already
 * defined in the library. */
vec_t *vec_apply(vec_t *v, FLT_TYP (*map)(FLT_TYP));

// result = exp(v)
vec_t *vec_exp(vec_t *result, const vec_t *v);
// reslut = ln(v)
vec_t *vec_ln(vec_t *result, const vec_t *v);
// result = log2(v)
vec_t *vec_log2(vec_t *result, const vec_t *v);
// result = 1 / v (element-wise inversion)
vec_t *vec_inv(vec_t *result, const vec_t *v);
// result = sqrt(v)
vec_t *vec_sqrt(vec_t *result, const vec_t *v);
// result = v^2
vec_t *vec_square(vec_t *result, const vec_t *v);
// result = tanh(v)
vec_t *vec_tanh(vec_t *result, const vec_t *v);
// result = sigmoid(v)
vec_t *vec_sigmoid(vec_t *result, const vec_t *v);
// result = ReLu(v)
vec_t *vec_relu(vec_t *result, const vec_t *v);
// result = softmax(v)
vec_t* vec_softmax(vec_t* result, const vec_t* v);
// return max element of v (first occurence)
FLT_TYP vec_max(const vec_t* v);
// return index of max element of v (first occurence)
IND_TYP vec_argmax(const vec_t* v);
// Gives a pointer to v->pyl->arr[i]; i can be negative
FLT_TYP *vec_at(const vec_t *v, IND_TYP i);
// Gives the serial size of vector v
size_t vec_serial_size(const vec_t *v);
// Returns the pointer to the first byte just after the last written byte to byte_arr
uint8_t *vec_serialize(const vec_t *v, uint8_t *byte_arr);
// Returns the pointer to the first byte just after the last read byte from byte_arr
const uint8_t *vec_deserialize(vec_t *v, const uint8_t *byte_arr);
