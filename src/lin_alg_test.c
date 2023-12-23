#include "lin_alg.h"

void vec_mat_test(void);
void vec_test(void);
void mat_test(void);
void slice_test(void);

int main()
{
    vec_test();
    mat_test();
    vec_mat_test();
    slice_test();

    return 0;
}