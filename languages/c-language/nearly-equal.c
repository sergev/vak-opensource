#include <stdio.h>
#include <stdbool.h>
#include <math.h>

//
// Compare two floating point values.
// Return true when the values are nearly equal.
// The difference must not exceed the given number of lower bits of mantissa.
// The `ignore_nbits` argument can be in range in range 0...51.
//
bool nearly_equal(double a, double b, int ignore_nbits)
{
    if (a == b)
        return true;

    // Split values into mantissa and exponent.
    int a_exponent, b_exponent;
    double a_mantissa = frexp(a, &a_exponent);
    double b_mantissa = frexp(b, &b_exponent);

    // Exponents must match.
    if (a_exponent != b_exponent)
        return false;

    // Subtract mantissas, giving a positive delta.
    double delta = fabs(a_mantissa - b_mantissa);

    // Upper bound of discrepancy.
    double epsilon = ldexp(1.0, ignore_nbits - 52);

    return delta < epsilon;
}

void try(double a, double b, int ignore_nbits)
{
    bool eq = nearly_equal(a, b, ignore_nbits);
    printf("a ~= b ? %s\n", eq ? "True" : "False");
}

int main()
{
    try(1.000000001, 1.000000002, 22);
    try(1.0000000001, 1.0000000002, 18);
    try(1.000000000000001, 1.000000000000002, 2);
}
