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
    payload_t pyl_r;
    // FLT_TYP vl_arr[2] = {-1, 2};
    // payload_t pyl_l;

    payload_prealloc(&pyl_r, vr_arr, 3);
    vec_construct_prealloc(&vr, &pyl_r, 0, pyl_r.size, 1);
    // payload_prealloc(&pyl_l, vl_arr, 2);
    // vec_construct_prealloc(&vl, &pyl_l, 0, pyl_l.size, 1);
    vec_construct(&vl, 2);
    vec_copy_arr(&vl, (FLT_TYP[]){-1, 2} );

    vec_construct(&res_2, 2);
    vec_fill_zero(&res_2);
    vec_construct(&res_3, 3);
    vec_fill_zero(&res_3);

    for(int i = 0; i < 6; i++)
        m.pyl->arr[i] = i + 1;

    mat_dot_vec(&res_2, &m, &vr);
    vec_dot_mat(&res_3, &vl, &m);

    char buff[6*32+8];

    vec_t row = vec_NULL;
    vec_t col = vec_NULL;

    printf("m:\n%s\n", mat_to_str(&m, buff));
    printf("m row 1:\n%s\n", vec_to_str(mat_row_at(&m, &row, 1), buff));
    vec_destruct(&row);
    printf("m col 1:\n%s\n", vec_to_str(mat_column_at(&m, &col, 1), buff));
    vec_destruct(&col);
    printf("vr:\n%s\n", vec_to_str(&vr, buff));
    printf("vl:\n%s\n", vec_to_str(&vl, buff));
    printf("res m@vr:\n%s\n", vec_to_str(&res_2, buff));
    printf("res vl@m:\n%s\n", vec_to_str(&res_3, buff));


    mat_t m2 = mat_NULL;
    mat_construct(&m2, 2, 3);
    mat_fill_zero(&m2);
    vec_outer(&m2, &vl, &vr);
    printf("outer vl*vr:\n%s\n", mat_to_str(&m2, buff));

    vec_destruct(&res_2);
    vec_destruct(&res_3);
    
    mat_destruct(&m);
    mat_destruct(&m2);
    vec_destruct(&vl);
    vec_destruct(&vr);

    // payload_release(&pyl_l);
    payload_release(&pyl_r);


    puts("^^^ vec_mat_test ^^^");

}