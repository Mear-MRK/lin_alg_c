#include "vec_t.h"

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

static vec_t *vec_sigmoid_alt(vec_t *result, const vec_t *v)
{
    vec_tanh(result, vec_sclmul(result, v, (FLT_TYPE)0.5));
    vec_f_addto(result, 1);
    vec_scale(result, (FLT_TYPE)0.5);
    return result;
}

static void sigmoid_test(void)
{
    vec_t v = vec_NULL;
    vec_t sv = vec_NULL;
    vec_t sv2 = vec_NULL;
    const int sz = 64;
    vec_construct(&v, sz);
    vec_construct(&sv, sz);
    vec_construct(&sv2, sz);

    srand(time(NULL));
    clock_t start, end;
    double sig_elp = 0, sig2_elp = 0;
    printf("clk_s: %ld\n", CLOCKS_PER_SEC);
    for(int r = 0; r < 10000; r++)
    {
        vec_rnd_fill(&v, rnd);
        vec_assign(&sv, &v);
        vec_assign(&sv2, &v);
        start = clock();
        for(int i = 0; i < 100; i++)
            vec_sigmoid(&sv, &sv);
        end = clock();
        sig_elp+= (end-start)/(double)CLOCKS_PER_SEC;
        start = clock();
        for(int i = 0; i < 100; i++)
            vec_sigmoid_alt(&sv2, &sv2);
        end = clock();
        sig2_elp+=(end-start)/(double)CLOCKS_PER_SEC;
        
        if(! vec_is_close(&sv, &sv2, 1E-6) )
        {
            vec_t* df = vec_new(sz);
            char str_buff[4096] = {0};
            printf("diff: %s\n", vec_to_str(vec_sub(df, &sv, &sv2), str_buff));
            vec_del(df);
            break;
        }
    }
    printf("sig elapsed: %g\n", sig_elp);
    printf("sia elapsed: %g\n", sig2_elp);
    vec_destruct(&v);
    vec_destruct(&sv);
    vec_destruct(&sv2);
}

void relu_test(void)
{
    const int sz = 7;
    vec_t* v = vec_new(sz);
    vec_t* r = vec_new(sz);
    
    arr_lin_fill(v->arr, -3, 3, 7);
    
    char str_bf[1024] = {0};
    
    printf("v:       %s\n", vec_to_str(v, str_bf));
    printf("relu(v): %s\n", vec_to_str(vec_relu(r, v), str_bf));
    
    
    vec_del(r);
    vec_del(v);
    puts("--------");
}

void fill_vec_test(void)
{
    vec_t v1 = vec_NULL;
    vec_t v2 = vec_NULL;
    const unsigned sz = 677;
    vec_construct(&v1, sz);
    vec_construct(&v2, sz);
    
    clock_t start = clock();
    for(float f = 0; f < 1000000; f+=1.0001)
        vec_fill(&v1, f);
    clock_t end = clock();
    printf("    fill: %g\n", (end-start)/(double)CLOCKS_PER_SEC);
    start = clock();
    for(float f = 0; f < 1000000; f+=1.0001)
        vec_fill_alt(&v2, f);
    end = clock();
    printf("rec_fill: %g\n", (end-start)/(double)CLOCKS_PER_SEC);

    if (! vec_is_close(&v1, &v2, 1E-7)){
        char bf[4096];
        printf("v1 != v2\n %s\n", vec_to_str(&v2, bf));
    }

    vec_destruct(&v1);
    vec_destruct(&v2);

}

void vec_test(void)
{
    puts("+++ vec_test +++");
    char str_buff[4096] = {0};

    float a_arr[3] = {1., 2., 3.};
    float b_arr[3] = {-1, 1, 2};

    vec_t va = vec_NULL;
    vec_t vb = vec_NULL;
    vec_t* vr = vec_new(3);


    vec_init(&va, a_arr, 3);
    vec_init(&vb, b_arr, 3);

    printf("va: %s, norm2: %g, norm1: %g, sum: %g\n", vec_to_str(&va, str_buff)
        , vec_norm_2(&va), vec_norm_1(&va), vec_sum(&va));
    printf("vb: %s, norm2: %g, norm1: %g, sum: %g\n", vec_to_str(&vb, str_buff)
        , vec_norm_2(&vb), vec_norm_1(&vb), vec_sum(&vb));
    
    printf("vb@va: %g\n", vec_dot(&vb, &va));

    vec_mul(vr, &va, &vb);

    vec_to_str(vr, str_buff);
    printf("va*vb: %s\n", str_buff);

    vec_f_addto(&va, -3.14f);
    vec_to_str(&va, str_buff);
    printf("va+=-3.14: %s\n", str_buff);
    vec_addto(&vb, &va);
    printf("vb+=va: %s\n", vec_to_str(&vb, str_buff));
    vec_subfrom(&va, &vb);
    printf("va-=vb: %s\n", vec_to_str(&va, str_buff));
    vec_exp(&va, &va);
    printf("va=exp(va): %s\n", vec_to_str(&va, str_buff));
    vec_sclmul(&va, &vb, 3.14f);
    printf("va=vb*3.14: %s\n", vec_to_str(&va, str_buff));

    vec_del(vr);

    // fill_vec_test();
    
    relu_test();

    puts("^^^ vec_test ^^^");
}