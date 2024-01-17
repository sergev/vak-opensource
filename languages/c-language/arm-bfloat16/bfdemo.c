//
// Type BFloat16 is supported in hardware on Apple M2 processors.
// Here is a demo.
// Compile with command:
//
//      gcc-13 -march=armv8.6-a+bf16 bfdemo.c
//
#include <stdio.h>

int main ()
{
    float f = 123.456;
    __bf16 b;

    b = __builtin_aarch64_bfcvtbf(f);
    //b = vcvth_bf16_f32(f);
    printf ("From float32 to bfloat16: %.3f -> %#04hx\n", f, b);

    f = __builtin_aarch64_bfcvtsf(b);
    //f = vcvtah_f32_bf16(b);
    printf ("From bfloat16 to float32: %#04hx -> %.3f\n", b, f);
}
