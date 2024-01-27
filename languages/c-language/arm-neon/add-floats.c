#include <stdio.h>
#if __aarch64__
#   include <arm_neon.h>
#endif

//
// Add two float arrays.
// From: https://community.arm.com/arm-community-blogs/b/operating-systems-blog/posts/arm-neon-programming-quick-reference
//
// More examples are available: https://github.com/projectNe10/Ne10/tree/master/modules/dsp
//
void add_float_generic(float *dst, const float *src1, const float *src2, int count)

{
    int i;

    for (i = 0; i < count; i++)
        dst[i] = src1[i] + src2[i];
}

#if defined(__aarch64__)
void add_float_neon(float *dst, const float *src1, const float *src2, int count)

{
    int i;

    for (i = 0; i < count; i += 4) {
        float32x4_t in1, in2, out;

        in1 = vld1q_f32(src1);
        src1 += 4;
        in2 = vld1q_f32(src2);
        src2 += 4;
        out = vaddq_f32(in1, in2);
        vst1q_f32(dst, out);
        dst += 4;
    }
}
#endif

void main()
{
#define N 20
    const float a[N] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 18, 20 };
    const float b[N] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 18, 20, 21 };
    float sum[N];
    float sum2[N];

    add_float_generic(sum, a, b, N);
    add_float_neon(sum2, a, b, N);
    printf("sum[0] = %f, sum2[0] = %f\n", sum[0], sum2[0]);
}
