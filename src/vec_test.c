#include "vec.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "vector_eng.h"

static FLD_TYP rnd(void)
{
    return 2*rand()/(FLD_TYP)RAND_MAX - 1;
}

static inline FLD_TYP* arr_lin_fill(FLD_TYP* arr, FLD_TYP start, FLD_TYP end, IND_TYP sz)
{
    if (!arr || sz == 0)
        return arr;

    if (sz == 1)
    {
        arr[0] = start;
        return arr;
    }
    FLD_TYP dlt = (end - start) / (sz-1);
    for(IND_TYP i = 0; i < sz; i++)
        arr[i] = start + i * dlt;
    return arr;
}

static vec *vec_sigmoid_alt(vec *result, const vec *v)
{
    vec_tanh(result, vec_sclmul(result, v, (FLD_TYP)0.5));
    vec_f_addto(result, 1);
    vec_scale(result, (FLD_TYP)0.5);
    return result;
}

static void sigmoid_test(void)
{
    vec v = vec_NULL;
    vec sv = vec_NULL;
    vec sv2 = vec_NULL;
    const int sz = 64;
    vec_construct(&v, sz);
    vec_construct(&sv, sz);
    vec_construct(&sv2, sz);

    srand(time(NULL));
    clock_t start, end;
    double sig_elp = 0, sig2_elp = 0;
    printf("clk_s: %ld\n", CLOCKS_PER_SEC);
    for(int r = 0; r < 1000; r++)
    {
        vec_fill_rnd(&v, rnd);
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
            vec* df = vec_new(sz);
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
    vec* v = vec_new(sz);
    vec* r = vec_new(sz);
    
    arr_lin_fill(v->pyl->arr, -3, 3, 7);
    
    char str_bf[1024] = {0};
    
    printf("v:       %s\n", vec_to_str(v, str_bf));
    printf("relu(v): %s\n", vec_to_str(vec_relu(r, v), str_bf));
    printf("sign(v): %s\n", vec_to_str(vec_sign(r, v), str_bf));
    printf("theta(v): %s\n", vec_to_str(vec_theta(r, v), str_bf));

    vec_del(r);
    vec_del(v);
    puts("--------");
}

void fill_vec_test(void)
{
    vec v1 = vec_NULL;
    vec v2 = vec_NULL;
    const unsigned sz = 677;
    vec_construct(&v1, sz);
    vec_construct(&v2, sz);
    
    clock_t start = clock();
    for(FLD_TYP f = 0; f < 1000000; f+=1.0001)
        vec_fill(&v1, f);
    clock_t end = clock();
    printf("    fill: %g\n", (end-start)/(double)CLOCKS_PER_SEC);
    // start = clock();
    // // for(FLD_TYP f = 0; f < 1000000; f+=1.0001)
    // //     vec_fill_altimp(&v2, f);
    // end = clock();
    // printf("rec_fill: %g\n", (end-start)/(double)CLOCKS_PER_SEC);

    // if (! vec_is_close(&v1, &v2, 1E-7)){
    //     char bf[4096];
    //     printf("v1 != v2\n %s\n", vec_to_str(&v2, bf));
    // }

    vec_destruct(&v1);
    vec_destruct(&v2);

}

void vec_test(void)
{
    puts("+++ vec_test +++");
    char str_buff[4096] = {0};

    vec va = vec_NULL;
    vec vb = vec_NULL;
    vec* vr = vec_new(3);


    vec_construct(&va, 3);
    vec_copy_arr(&va, (FLD_TYP[]){1, 2, 3});
    // vec_construct(&vb, 3);
    payload pyl_b;
    payload_prealloc(&pyl_b, (FLD_TYP[]){-10, -1, -10, 1, -10, 2, -10}, 7);
    vec_construct_prealloc(&vb, &pyl_b, 1, 3, 2);

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
    
    vec* vw = vec_new_view(&vb, 0, vb.d, 2);
    printf("vw=view(vb): %s\n", vec_to_str(vw, str_buff));
    vec_fill(vw, 111);
    printf("vw=111; vb: %s\n", vec_to_str(&vb, str_buff)); 

    vec_del(vw);
    vec_del(vr);
    vec_destruct(&va);
    vec_destruct(&vb);
    payload_release(&pyl_b);

    // fill_vec_test();
    
    relu_test();
    sigmoid_test();

    puts("^^^ vec_test ^^^");
}