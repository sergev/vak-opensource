//
// Multiply two int32 matrices, with int32 result.
//
#include <stdio.h>
#include <stdint.h>

//
// Multiply two matrices 4x4.
//
void matmul(int32_t result[4][4], const int32_t matrix1[4][4], const int32_t matrix2[4][4])
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += matrix1[r][k] * matrix2[k][c];
            }
            result[r][c] = sum;
        }
    }
}

//
// Print matrix 4x4.
//
void print(const int32_t matrix[4][4])
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            printf(" %d,", matrix[r][c]);
        }
        printf("\n");
    }
}

//
// Multiply two matrices 4x4 using SSE instructions.
// Note: this code can be compiled only on Intel/AMD x86-64 architecture.
//
#if __x86_64__
#include <immintrin.h>

void matmul_simd(int32_t result[4][4], const int32_t matrix1[4][4], const int32_t matrix2[4][4])
{
    for (int r = 0; r < 4; ++r) {

        // Sum for this iteration: one row of int32 values.
        __m128i sum = _mm_setzero_si128();

        for (unsigned k = 0; k < 4; k++) {

            // Load one element from matrix A, replicate.
            const __m128i a = _mm_set1_epi32(matrix1[r][k]);

            // Load one row from matrix B.
            const __m128i b = _mm_loadu_si128((const __m128i *)&matrix2[k][0]);

            // Multiply element-wise.
            const __m128i product = _mm_mullo_epi32(a, b);

            // Accumulate.
            sum = _mm_add_epi32(sum, product);
        }

        _mm_storeu_si128((__m128i *)&result[r][0], sum);
    }
}
#endif // __x86_64__

//
// Multiply two matrices 4x4 using ARM Neon instructions.
// Note: this code can be compiled only on ARM architecture.
//
#if __aarch64__
#include <arm_neon.h>

void matmul_simd(int32_t result[4][4], const int32_t matrix1[4][4], const int32_t matrix2[4][4])
{
    for (int r = 0; r < 4; ++r) {

        // Sum for this iteration: one row of int32 values.
        int32x4_t sum = vmovq_n_s32(0);

        for (unsigned k = 0; k < 4; k++) {

            // Load one element from matrix A, replicate.
            const int32x4_t a = vdupq_n_s32(matrix1[r][k]);

            // Load one row from matrix B.
            const int32x4_t b = vld1q_s32(&matrix2[k][0]);

            // Multiply element-wise.
            const int32x4_t product = vmulq_s32(a, b);

            // Accumulate.
            sum = vaddq_s32(sum, product);
        }

        vst1q_s32(&result[r][0], sum);
    }
}
#endif // __ARM_NEON__

int main()
{
    int32_t matrix1[4][4] = {
        {  -6, -59,  41,    8 },
        {  94,  26, -69,   15 },
        {  20,  82, -76, -108 },
        {  82, 107,  42,    3 },
    };
    int32_t matrix2[4][4] = {
        { -61, -53,  48,  105 },
        {  60, -80, -94, -109 },
        { 127, -57,  86,  -90 },
        { -92,  76,  97,  -62 },
    };
    int32_t result[4][4];

    matmul(result, matrix1, matrix2);
    printf("Traditional result:\n");
    print(result);

    matmul_simd(result, matrix1, matrix2);
    printf("SSE result:\n");
    print(result);

    // Expect:
    // 1297, 3309, 9560, 1615,
    // -14317, -1989, -2411, 12316,
    // 3984, -11496, -23760, 6698,
    // 6476, -15072, -2219, -7019,

    return 0;
}
