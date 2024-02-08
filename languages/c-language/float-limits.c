#include <stdio.h>
#include <float.h>

int main()
{                                                                   // i86_64
    printf("FLT_ROUNDS      = %d\n", FLT_ROUNDS);                   // 1
    printf("FLT_RADIX       = %d\n", FLT_RADIX);                    // 2
    printf("\n");
    printf("FLT_MANT_DIG    = %d\n", FLT_MANT_DIG);                 // 24
    printf("FLT_EPSILON     = %.*e\n", 1 + FLT_DIG, FLT_EPSILON);   // 1.1920929e-07
    printf("FLT_DIG         = %d\n", FLT_DIG);                      // 6
    printf("FLT_MAX_EXP     = %d\n", FLT_MAX_EXP);                  // 128
    printf("FLT_MAX         = %.*e\n", 1 + FLT_DIG, FLT_MAX);       // 3.4028235e+38
    printf("FLT_MAX_10_EXP  = %d\n", FLT_MAX_10_EXP);               // 38
    printf("FLT_MIN_EXP     = %d\n", FLT_MIN_EXP);                  // -125
    printf("FLT_MIN         = %.*e\n", 1 + FLT_DIG, FLT_MIN);       // 1.1754944e-38
    printf("FLT_MIN_10_EXP  = %d\n", FLT_MIN_10_EXP);               // -37
    printf("\n");
    printf("DBL_MANT_DIG    = %d\n", DBL_MANT_DIG);                 // 53
    printf("DBL_EPSILON     = %.*e\n", 1 + DBL_DIG, DBL_EPSILON);   // 2.2204460492503131e-16
    printf("DBL_DIG         = %d\n", DBL_DIG);                      // 15
    printf("DBL_MAX_EXP     = %d\n", DBL_MAX_EXP);                  // 1024
    printf("DBL_MAX         = %.*e\n", 1 + DBL_DIG, DBL_MAX);       // 1.7976931348623157e+308
    printf("DBL_MAX_10_EXP  = %d\n", DBL_MAX_10_EXP);               // 308
    printf("DBL_MIN_EXP     = %d\n", DBL_MIN_EXP);                  // -1021
    printf("DBL_MIN         = %.*e\n", 1 + DBL_DIG, DBL_MIN);       // 2.2250738585072014e-308
    printf("DBL_MIN_10_EXP  = %d\n", DBL_MIN_10_EXP);               // -307
    printf("\n");
    printf("LDBL_MANT_DIG   = %d\n", LDBL_MANT_DIG);                // 64
    printf("LDBL_EPSILON    = %.*Le\n", 1 + LDBL_DIG, LDBL_EPSILON);// 1.0842021724855044340e-19
    printf("LDBL_DIG        = %d\n", LDBL_DIG);                     // 18
    printf("LDBL_MAX_EXP    = %d\n", LDBL_MAX_EXP);                 // 16384
    printf("LDBL_MAX        = %.*Le\n", 1 + LDBL_DIG, LDBL_MAX);    // 1.1897314953572317650e+4932
    printf("LDBL_MAX_10_EXP = %d\n", LDBL_MAX_10_EXP);              // 4932
    printf("LDBL_MIN_EXP    = %d\n", LDBL_MIN_EXP);                 // -16381
    printf("LDBL_MIN        = %.*Le\n", 1 + LDBL_DIG, LDBL_MIN);    // 3.3621031431120935063e-4932
    printf("LDBL_MIN_10_EXP = %d\n", LDBL_MIN_10_EXP);              // -4931
}
