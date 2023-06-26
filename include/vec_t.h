#ifndef VEC_T_H_INCLUDED
#define VEC_T_H_INCLUDED 1

#include <stdbool.h>

#include "lin_alg_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Basic vector type; it comprises of a size
 * and a pointer to FLT_TYPE data.
 */
typedef struct 
{
    FLT_TYPE* arr;
    size_t d;
} vec_t;

/* 
 * The empty (NULL) vector.
 * It's a good practice to initialize any
 * declared vec_t with this value.
 */
 #define vec_NULL {.arr = NULL, .d = 0}

/*  
 * Allocates d FLT_TYPE for v->arr.
 * d must be non-zero; v->arr must be NULL; to do so,
 * always initialize any declared vec_t with vec_NULL.
 * vec_destruct must be called on v before it goes out of scope.
 * Content of v->arr is garbage.
 */
vec_t* vec_construct(vec_t* v, size_t d);

/*
 * Frees v->arr and sets v = vec_NULL.
 * Should be called on v's that are constructed by vec_*_construct.
 */
void vec_destruct(vec_t* v);

/* 
 * Initializes a vec_t v with pre-allocated arr.
 * v must not already be constructed/initialized
 * and v->arr should be NULL.
 */
vec_t* vec_init(vec_t* v, FLT_TYPE* arr, size_t d);

/*
 * Allocates a vec_t in heap, constructs it and returns it.
 * One should call vec_del on it to destruct and ALIGNED_FREE the resources.
 */
vec_t* vec_new(size_t d);

/*
 * Destructs vec_t v and frees v 
 * which must be allocated by vec_new.
 */
void vec_del(vec_t* v);

/* NOT implemented yet.
vec_t* vec_nil_construct(vec_t* v, size_t d);
vec_t* vec_unit_construct(vec_t* v, size_t d, size_t i);
vec_t* vec_copy_construct(vec_t* v, const vec_t* oth);
*/

// v_dst = v_src : copies src->arr to dst->arr; dimensions must be the same.
vec_t* vec_assign(vec_t* v_dst, const vec_t* v_src);

// Copies arr to v->arr
vec_t* vec_copy_arr(vec_t* v, const FLT_TYPE arr[]);

// Sets all elements of v->arr to 0
vec_t* vec_fill_zero(vec_t *v);

// Fills v->arr with random numbers generated by rnd_gen
vec_t* vec_rnd_fill(vec_t* v, FLT_TYPE (*rnd_gen)(void));

// Fills v->arr with value
vec_t* vec_fill(vec_t* v, FLT_TYPE value);
// Same as vec_fill with alternative geometric memcpy algorithm.
vec_t* vec_fill_alt(vec_t* v, FLT_TYPE value);

// Gives string representation of vec_t v; be sure str_buff is big enough.
char* vec_to_str(const vec_t* v, char* str_buff);

// Checks closeness of two vectors by comparing norm(v_1-v_2) and (norm(v_1)+norm(v_2))/2 * eps.
bool vec_is_close(const vec_t *v_1, const vec_t *v_2, FLT_TYPE eps);

// result = v_left + v_right
vec_t* vec_add(vec_t* result, const vec_t* v_left, const vec_t* v_right);
// result = v_left - v_right
vec_t* vec_sub(vec_t* result, const vec_t* v_left, const vec_t* v_right);
// result = v_left * v_right (element-wise *)
vec_t* vec_mul(vec_t* result, const vec_t* v_left, const vec_t* v_right);
// result = alpha * v
vec_t* vec_sclmul(vec_t* result, const vec_t* v, FLT_TYPE alpha);
// v_dst += v_right
vec_t* vec_addto(vec_t* v_dst, const vec_t* v_right);
// v_dst -= v_right
vec_t* vec_subfrom(vec_t* v_dst, const vec_t* v_right);
// v_dst *= v_right (element-wise *)
vec_t* vec_mulby(vec_t* v_dst, const vec_t* v_right);
// v += f
vec_t* vec_f_addto(vec_t* v, FLT_TYPE f);
// v *= scale
vec_t* vec_scale(vec_t* v, FLT_TYPE scale);
// v_left @ v_right : @ = dot product
FLT_TYPE vec_dot(const vec_t* v_left, const vec_t* v_right);
// Euclidean norm of v : sqrt(sum_i v->arr[i]^2)
FLT_TYPE vec_norm_2(const vec_t* v);
// Sum of absolute values of v->arr
FLT_TYPE vec_norm_1(const vec_t* v);
// Sum of elements of v
FLT_TYPE vec_sum(const vec_t* v);

/* NOT implemented yet.
FLT_TYPE vec_mean(const vec_t* v);
FLT_TYPE vec_std(const vec_t* v);
*/


/* Applies map on each element of v->arr.
 * Don't use this in the cases where the function already 
 * defined in the library. */
vec_t* vec_apply(vec_t* v, FLT_TYPE (*map)(FLT_TYPE));

// result = exp(v)
vec_t* vec_exp(vec_t* result, const vec_t* v);
// result = 1 / v (element-wise inversion)
vec_t* vec_inv(vec_t* result, const vec_t* v);
// result = tanh(v)
vec_t* vec_tanh(vec_t* result, const vec_t* v);
// result = sigmoid(v) 
vec_t* vec_sigmoid(vec_t* result, const vec_t* v);
// result = ReLu(v)
vec_t *vec_relu(vec_t *result, const vec_t *v);
/* NOT implemented yet.
vec_t* vec_square(vec_t* result, const vec_t* v);
FLT_TYPE vec_max(const vec_t* v);
size_t vec_argmax(const vec_t* v);
vec_t* vec_softargmax(vec_t* result, const vec_t* v);
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VEC_T_H_INCLUDED */