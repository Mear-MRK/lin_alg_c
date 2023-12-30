#include "vec_mat.h"

#include <stdio.h>


void vec_mat_test(void)
{
    puts("+++ vec_mat_test +++");

    mat m = mat_NULL;
    vec vr = vec_NULL;
    vec vl = vec_NULL;
    vec res_2 = vec_NULL;
    vec res_3 = vec_NULL;

    mat_construct(&m, 2, 3);

    FLD_TYP vr_arr[] = {-10 , 1, -10, -1, -10, 2, -10};
    payload pyl_r;
    FLD_TYP vl_arr[] = {-1, 10, 2};
    payload pyl_l;

    payload_prealloc(&pyl_r, vr_arr, 7);
    vec_construct_prealloc(&vr, &pyl_r, 1, 3, 2);
    payload_prealloc(&pyl_l, vl_arr, 3);
    vec_construct_prealloc(&vl, &pyl_l, 0, 2, 2);
    // vec_construct(&vl, 2);
    // vec_copy_arr(&vl, (FLD_TYP[]){-1, 2} );

    vec_construct(&res_2, 2);
    vec_fill_zero(&res_2);
    vec_construct(&res_3, 3);
    vec_fill_zero(&res_3);

    for(int i = 0; i < 6; i++)
        m.pyl->arr[i] = i + 1;

    mat_dot_vec(&res_2, &m, &vr);
    vec_dot_mat(&res_3, &vl, &m);

    char buff[6*32+8];

    vec row = vec_NULL;
    vec col = vec_NULL;

    printf("m:\n%s\n", mat_to_str(&m, buff));
    printf("m row 1:\n%s\n", vec_to_str(mat_row_at(&m, &row, 1), buff));
    vec_destruct(&row);
    printf("m col 1:\n%s\n", vec_to_str(mat_column_at(&m, &col, 1), buff));
    vec_destruct(&col);
    printf("vr:\n%s\n", vec_to_str(&vr, buff));
    printf("vl:\n%s\n", vec_to_str(&vl, buff));
    printf("res m@vr:\n%s\n", vec_to_str(&res_2, buff));
    printf("res vl@m:\n%s\n", vec_to_str(&res_3, buff));


    mat m2 = mat_NULL;
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

    payload_release(&pyl_l);
    payload_release(&pyl_r);


    puts("^^^ vec_mat_test ^^^");

}