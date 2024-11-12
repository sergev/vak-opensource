#include <stdio.h>
#include <stdint.h>

int64_t a = 5000000000;
int64_t b = 6000000000;

int main()
{
    int64_t c;
    if (__builtin_mul_overflow(a, b, &c)) {
        printf("%ld * %ld = overflow\n", a, b);
    } else {
        printf("%ld * %ld = %ld\n", a, b, c);
    }
}
