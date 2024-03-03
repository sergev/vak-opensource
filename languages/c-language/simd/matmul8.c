//
// Multiply two int32 matrices, with int32 result.
//
// Copyright (C) 2024 Serge Vakulenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <stdio.h>
#include <stdint.h>

//
// Multiply two matrices 8x8.
//
void matmul(int32_t result[8][8], const int32_t foo[8][8], const int32_t bar[8][8])
{
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int sum = 0;
            for (int k = 0; k < 8; k++) {
                sum += foo[r][k] * bar[k][c];
            }
            result[r][c] = sum;
        }
    }
}

//
// Print matrix 8x8.
//
void print(const int32_t matrix[8][8])
{
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            printf(" %d,", matrix[r][c]);
        }
        printf("\n");
    }
}

//
// Multiply two matrices 8x8 using SSE instructions.
// Note: this code can be compiled only on Intel/AMD x86-64 architecture.
//
#if __x86_64__
#include <immintrin.h>

void matmul_simd(int32_t result[8][8], const int32_t foo[8][8], const int32_t bar[8][8])
{
    for (int r = 0; r < 8; ++r) {

        // Sum for this iteration: one row of int32 values.
        __m256i sum = _mm256_setzero_si256();

        for (unsigned k = 0; k < 8; k++) {

            // Load one element from matrix A, replicate.
            const __m256i a = _mm256_set1_epi32(foo[r][k]);

            // Load one row from matrix B.
            const __m256i b = _mm256_loadu_si256((const __m256i *)&bar[k][0]);

            // Multiply element-wise.
            const __m256i product = _mm256_mullo_epi32(a, b);

            // Accumulate.
            sum = _mm256_add_epi32(sum, product);
        }

        _mm256_storeu_si256((__m256i *)&result[r][0], sum);
    }
}
#endif // __x86_64__

//
// Multiply two matrices 8x8 using ARM Neon instructions.
// Note: this code can be compiled only on ARM architecture.
//
#if __aarch64__
#include <arm_neon.h>

void matmul_simd(int32_t result[8][8], const int32_t foo[8][8], const int32_t bar[8][8])
{
    for (int r = 0; r < 8; ++r) {

        // Sum for this iteration: one row of int32 values.
        int32x4_t sum_lo = vmovq_n_s32(0);
        int32x4_t sum_hi = vmovq_n_s32(0);

        for (unsigned k = 0; k < 8; k++) {

            // Load one element from matrix A, replicate.
            const int32x4_t a = vdupq_n_s32(foo[r][k]);

            // Load one row from matrix B.
            const int32x4_t b_lo = vld1q_s32(&bar[k][0]);
            const int32x4_t b_hi = vld1q_s32(&bar[k][4]);

            // Multiply element-wise.
            const int32x4_t product_lo = vmulq_s32(a, b_lo);
            const int32x4_t product_hi = vmulq_s32(a, b_hi);

            // Accumulate.
            sum_lo = vaddq_s32(sum_lo, product_lo);
            sum_hi = vaddq_s32(sum_hi, product_hi);
        }

        vst1q_s32(&result[r][0], sum_lo);
        vst1q_s32(&result[r][4], sum_hi);
    }
}
#endif // __ARM_NEON__

int main()
{
    int32_t foo[8][8] = {
        {  -6, -59,  41,    8,   43,  -38, -91,  -30 },
        {  94,  26, -69,   15, -127,   42,  32,  -90 },
        {  20,  82, -76, -108,    4,   48,  82,  -41 },
        {  82, 107,  42,    3,  -70, -104, 126,  125 },
        {  11, -47,  98,   32,   15,  -86, 122,   51 },
        {  28, -20,  44,   25,   -7,   -5,  28,  -29 },
        {  74,  42, -61,  -36,  113,  -35,  94,   49 },
        { -32,   3,  19,   80,   68,    1, -82, -120 },
    };
    int32_t bar[8][8] = {
        { -61, -53,  48,  105,   48,  78,  90, 100 },
        {  60, -80, -94, -109,    9,  89, -73,  63 },
        { 127, -57,  86,  -90, -106,  96,  76,  80 },
        { -92,  76,  97,  -62,  -36,  90, -68,  -3 },
        { 100, -17,  36,  -47,   40, -97, -39,  82 },
        {  29,  64, 125,  -93,  -15,  46,  59, -14 },
        { -32, -10, -60,   28,  -56, -26, 100, -94 },
        {  52, -34, -94, -109,  -17,  39,  34, 117 },
    };
    int32_t result[8][8];

    matmul(result, foo, bar);
    printf("Traditional result:\n");
    print(result);

    matmul_simd(result, foo, bar);
    printf("SIMD result:\n");
    print(result);

    // Expect:
    // 5847, 2076, 14638, 3850, 2443, -5786, -7700, 8041,
    // -31503, 5598, 4807, 25085, 5548, 14281, 7869, -19067,
    // 1020, -7918, -18682, 8811, 9187, -10069, 6864, -11439,
    // -1072, -26048, -37049, -4154, -10082, 23826, 16001, 16789,
    // 3765, -8690, -5846, 624, -17244, 2369, 17968, 2816,
    // -2869, 13, 9602, 4349, -5680, 5468, 7416, -1544,
    // 3396, -13308, -19687, 6149, 10640, -12690, 6000, 11927,
    // 398, 10261, 26349, -2862, 2934, -2303, -21968, -2501,

    return 0;
}
