#include "mat.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

static FLT_TYP rnd(void)
{
    return 2 * rand() / (FLT_TYP)RAND_MAX - 1;
}

static inline FLT_TYP *arr_lin_fill(FLT_TYP *arr, FLT_TYP start, FLT_TYP end, IND_TYP sz)
{
    if (!arr || sz == 0)
        return arr;

    if (sz == 1)
    {
        arr[0] = start;
        return arr;
    }
    FLT_TYP dlt = (end - start) / (sz - 1);
    for (IND_TYP i = 0; i < sz; i++)
        arr[i] = start + i * dlt;
    return arr;
}

void mat_transposition_test(void)
{
    FLT_TYP arr[6] = {11, 12, 13, 21, 22, 23};
    payload_t pyl = payload_NULL;
    payload_prealloc(&pyl, arr, 6);
    // assert(payload_is_valid(&pyl));
    mat_t m = mat_NULL;
    mat_construct_prealloc(&m, &pyl, 0, 2, 3);
    printf("m: d1:%ld d2:%ld off:%ld sz:%ld pyl:%p pyl_sz:%ld\n", m.d1, m.d2, m.offset, m.size,
           m.pyl, m.pyl->size);
    // assert(mat_is_valid(&m));

    mat_t *r = mat_new(3, 2);
    char str_bf[1024] = {0};
    printf("m:\n%s\n", mat_to_str(&m, str_bf));
    printf("r=m^T:\n%s\n", mat_to_str(mat_transpose(r, &m), str_bf));
    printf("in-place transposition; m:\n%s\n", mat_to_str(mat_T(&m), str_bf));

    mat_del(r);
    mat_destruct(&m);
    payload_release(&pyl);
    puts("-------");
}

void mat_arith_test(void)
{
    char str_bf[4096];

    mat_t *a = mat_new(3, 3);
    arr_lin_fill(a->pyl->arr, 1, 9, 9);
    mat_t *b = mat_new(3, 3);
    arr_lin_fill(b->pyl->arr, -4, 4, 9);
    mat_t *r = mat_new(3, 3);

    printf("a:\n%s\n", mat_to_str(a, str_bf));
    printf("b:\n%s\n", mat_to_str(b, str_bf));
    printf("r=a+b:\n%s\n", mat_to_str(mat_add(r, a, b), str_bf));
    printf("r=a-b:\n%s\n", mat_to_str(mat_sub(r, a, b), str_bf));
    printf("r=a*b:\n%s\n", mat_to_str(mat_mul(r, a, b), str_bf));
    printf("r*=2:\n%s\n", mat_to_str(mat_scale(r, 2), str_bf));
    mat_addto(a, b);
    printf("a+=b:\n%s\n", mat_to_str(a, str_bf));
    mat_subfrom(b, a);
    printf("b-=a:\n%s\n", mat_to_str(b, str_bf));
    mat_mulby(a, b);
    printf("a*=b:\n%s\n", mat_to_str(a, str_bf));

    mat_del(a);
    mat_del(b);
    mat_del(r);
    puts("------");
}

void mat_dot_test(void)
{
    char str_bf[4096];
    srand(time(NULL));

    mat_t *a = mat_new(2, 3);
    arr_lin_fill(a->pyl->arr, 1, 6, 6);
    mat_t *b = mat_new(3, 2);
    arr_lin_fill(b->pyl->arr, -2, 3, 6);
    mat_t *r1 = mat_new(3, 3);
    mat_t *r2 = mat_new(2, 2);

    printf("a:\n%s\n", mat_to_str(a, str_bf));
    printf("b:\n%s\n", mat_to_str(b, str_bf));
    printf("a@b:\n%s\n", mat_to_str(mat_dot(r2, a, b), str_bf));
    printf("b@a:\n%s\n", mat_to_str(mat_dot(r1, b, a), str_bf));

    mat_del(a);
    mat_del(b);
    mat_del(r1);
    mat_del(r2);
    puts("------");
}

void mat_test(void)
{
    puts("+++ mat_test +++");

    mat_transposition_test();

    mat_arith_test();

    mat_dot_test();

    puts("^^^ mat_test ^^^");
}