#include "slice.h"

#include <stdio.h>
#include <math.h>

void prn_tst(slice_t *s1, slice_t *s2)
{
    char str[1024] = {0};
    slice_t s = slice_NULL;
    printf("s1: %s\n", slice_to_str(s1, str));
    printf("s2: %s\n", slice_to_str(s2, str));
    slice_combine(&s, s1, &slice_NONE);
    printf("s=s1*NN: %s\n", slice_to_str(&s, str));
    slice_combine(&s, &slice_NONE, s1);
    printf("s=NN*s1: %s\n", slice_to_str(&s, str));
    slice_combine(&s, &slice_NONE, s2);
    printf("s=s2*NN: %s\n", slice_to_str(&s, str));
    slice_combine(&s, s2, &slice_NONE);
    printf("s=NN*s2: %s\n", slice_to_str(&s, str));
    slice_combine(&s, s1, s2);
    printf("s=s1*s2: %s\n", slice_to_str(&s, str));
    slice_combine(&s, s2, s1);
    printf("s=s2*s1: %s\n", slice_to_str(&s, str));
    puts("-----");
}

void slice_test(void)
{
    puts("+++ slice_test +++");

    slice_t *s1 = slice_new(1, 8, 2);
    slice_t *s2 = slice_new(2, 5, 1);
    prn_tst(s1, s2);

    slice_set(s1, 1, 9, 2);
    slice_set(s2, -3, -9, -1);
    prn_tst(s1, s2);

    slice_set(s1, 1, 8, 2);
    slice_set(s2, -1, slice_MINUS_END, -1);
    prn_tst(s1, s2);

    slice_set(s1, -8, 4, 2);
    slice_set(s2, 3, slice_PLUS_END, 1);
    prn_tst(s1, s2);

    slice_set(s1, -5, 4, 2);
    prn_tst(s1, s2);

    slice_del(s1);
    slice_del(s2);
    puts("^^^ slice_test ^^^");
}