#include "vec_mat.h"

#include <stdio.h>


void vec_mat_test(void)
{
    puts("+++ vec_mat_test +++");

    mat_t m = mat_NULL;
    vec_t vr = vec_NULL;
    vec_t vl = vec_NULL;
    vec_t res_2 = vec_NULL;
    vec_t res_3 = vec_NULL;

    mat_construct(&m, 2, 3);

    FLT_TYP vr_arr[3] = {1, -1, 2};
    FLT_TYP vl_arr[2] = {-1, 2};
    FLT_TYP res_2_arr[2] = {0};
    FLT_TYP res_3_arr[3] = {0};
    vec_init_prealloc(&vr, vr_arr, 3);
    vec_init_prealloc(&vl, vl_arr, 2);
    vec_init_prealloc(&res_2, res_2_arr, 2);
    vec_init_prealloc(&res_3, res_3_arr, 3);

    for(int i = 0; i < 6; i++)
        m.arr[i] = i + 1;

    mat_dot_vec(&res_2, &m, &vr);
    vec_dot_mat(&res_3, &vl, &m);

    char buff[6*32+8];

    printf("m:\n%s\n", mat_to_str(&m, buff));
    printf("vr:\n%s\n", vec_to_str(&vr, buff));
    printf("vl:\n%s\n", vec_to_str(&vl, buff));
    printf("res m@vr:\n%s\n", vec_to_str(&res_2, buff));
    printf("res vl@m:\n%s\n", vec_to_str(&res_3, buff));

    FLT_TYP m2_arr[6] = {0};
    mat_t m2 = mat_NULL;
    mat_init_prealloc(&m2, m2_arr, 2, 3);
    vec_outer(&m2, &vl, &vr);
    printf("outer vl*vr:\n%s\n", mat_to_str(&m2, buff));
    
    mat_destruct(&m);

    puts("^^^ vec_mat_test ^^^");

}