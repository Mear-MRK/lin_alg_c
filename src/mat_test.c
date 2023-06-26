#include "mat_t.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static FLT_TYPE rnd(void)
{
    return 2*rand()/(FLT_TYPE)RAND_MAX - 1;
}

static inline FLT_TYPE* arr_lin_fill(FLT_TYPE* arr, FLT_TYPE start, FLT_TYPE end, size_t sz)
{
    if (!arr || sz == 0)
        return arr;

    if (sz == 1)
    {
        arr[0] = start;
        return arr;
    }
    FLT_TYPE dlt = (end - start) / (sz-1);
    for(size_t i = 0; i < sz; i++)
        arr[i] = start + i * dlt;
    return arr;
}

void mat_transposition_test(void)
{
    FLT_TYPE arr[6] = {11, 12, 13, 21, 22, 23};
    mat_t m = mat_NULL;
    mat_init(&m, arr, 2, 3);
    
    mat_t *r = mat_new(3, 2);
    char str_bf[1024] = {0};
    printf("m:\n%s\n", mat_to_str(&m, str_bf));
    printf("r=m^T:\n%s\n", mat_to_str(mat_transpose(r, &m), str_bf));
    printf("in-place transposition; m:\n%s\n", mat_to_str(mat_T(&m), str_bf));
        
    mat_del(r);
    puts("-------");
}
    

void mat_arith_test(void)
{
    char str_bf[4096];
    
    mat_t* a = mat_new(3,3);
    arr_lin_fill(a->arr, 1, 9, 9);
    mat_t* b = mat_new(3,3);
    arr_lin_fill(b->arr, -4, 4, 9);
    mat_t* r = mat_new(3,3);
    
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
    
    mat_t* a = mat_new(2,3);
    arr_lin_fill(a->arr, 1, 6, 6);
    mat_t* b = mat_new(3,2);
    arr_lin_fill(b->arr, -2, 3, 6);
    mat_t* r1 = mat_new(3,3);
    mat_t* r2 = mat_new(2,2);
    
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