#include <stdio.h>

#ifdef __ARM_FEATURE_SVE
#   include <arm_sve.h>
#endif

void generic_sum_squares(int N, int *sum)
{
    int res = 0;
    if (N > 0) {
        do {
            res += N * N;
            N--;
        } while (N > 0);
    }
    *sum = res;
}

#ifdef __ARM_FEATURE_SVE
//
// Use ARM SVE intrinsics.
//
void sve_sum_squares(int N, int *sum)
{
    svbool_t pred_N;
    svint32_t svN_tmp;
    svbool_t p_all = svptrue_b32();
    svint32_t acc = svdup_s32(0);

    if (N > 0) {
        svN_tmp = svindex_s32(N, -1);
        pred_N = svcmpgt(p_all, svN_tmp, 0);
        do {
            acc = svmla_m(pred_N, acc, svN_tmp, svN_tmp);
            svN_tmp = svsub_x(p_all, svN_tmp, svcntw());
            pred_N = svcmpgt(p_all, svN_tmp, 0);
        } while (svptest_first(p_all, pred_N));
    }
    *sum = (int)svaddv(p_all, acc);
}
#endif

void main()
{
    int sum = 0, sum2 = 0;

    generic_sum_squares(10, &sum);
    sve_sum_squares(10, &sum2);
    printf("sum = %d, sum2 = %d\n", sum, sum2);
}
