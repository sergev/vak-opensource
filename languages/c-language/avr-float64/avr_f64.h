// avr_f64.h

// Original Code by Detlef_a (Nickname in the www.mikrocontroller.net forum).
// Extensions (trigonometric functions et al.) und changes by Florian Königstein, mail@virgusta.eu .

// The following copyright information refers to the code developed by Florian Königstein.

//***********************************************************************************************************//
//***********************************************************************************************************//
//                                                                                                           //
// avr_f64.c and avr_f64.h : Contains ANSI C Funktions for handling of 64 bit IEEE 754 double
// precision      // floating point numbers. // float64_t represents an IEEE 64 bit double precision
// floating point number.                               //
//                                                                                                           //
// Compiler: avr-gcc //
//                                                                                                           //
// Copyright: //
//      You may use this program free of charge for any purpose you wish provided you agree to the
//      following // License: // Warranty of Provenance and Disclaimer of Warranty. Licensor
//      warrants that the copyright              // in and to the Original Work and the patent
//      rights granted herein by Licensor are owned by the        // Licensor or are sublicensed to
//      You under the terms of this License with the permission of the        // contributor(s) of
//      those copyrights and patent rights. Except as expressly stated in the immediately  //
//      preceding sentence, the Original Work is provided under this License on an "AS IS" BASIS and
//      WITHOUT // WARRANTY, either express or implied, including, without limitation, the
//      warranties of                // non-infringement, merchantability or fitness for a
//      particular purpose. THE ENTIRE RISK AS TO THE     // QUALITY OF THE ORIGINAL WORK IS WITH
//      YOU. This DISCLAIMER OF WARRANTY constitutes an essential part  // of this License. No
//      license to the Original Work is granted by this License except under this        //
//      disclaimer. //
//                                                                                                           //
//      Limitation of Liability. Under no circumstances and under no legal theory, whether in tort
//      // (including negligence), contract, or otherwise, shall the Licensor be liable to anyone
//      for any       // indirect, special, incidental, or consequential damages of any character
//      arising as a result of      // this License or the use of the Original Work including,
//      without limitation, damages for loss of      // goodwill, work stoppage, computer failure or
//      malfunction, or any and all other commercial damages    // or losses. This limitation of
//      liability shall not apply to the extent applicable law prohibits such  // limitation. //
//                                                                                                           //
//***********************************************************************************************************//
//***********************************************************************************************************//

// IEEE754 denormalized numbers other than zero are not supported. If a denormalized number is
// passed to a function, it is interpreted as zero. However, the functions f_sd() and f_ds() support
// denormalized single precision floating point numbers. All kinds of IEEE NaNs (Not a Number) are
// treated in the same way. If the result of a calculation is NaN, the IEEE 754 NaN with HEX value
// 7fffffffffffffff is returned. +INF and -INF are supported if F_ONLY_NAN_NO_INFINITY is NOT
// defined. If F_ONLY_NAN_NO_INFINITY is defined, a function may yield NaN or +/-INF, but a +/-INF
// as argument for a function is interpreted in the same way as NaN. Defining F_ONLY_NAN_NO_INFINITY
// results in a little bit smaller code size.

// *** ATTENTION *** : The functions f_to_decimalExp() and f_to_string() return pointers to static
// memory containing the decimal representation of the float64 passed to these functions. The string
// contained in this memory will become invalid if one of the functions f_to_decimalExp(),
// f_to_string(), f_exp(), f_log(), f_sin(), f_cos(), f_tan(), f_arcsin(), f_arccos(), f_arctan() is
// called as these functions will overwrite the memory.

#ifndef avr_f64_h_included
#define avr_f64_h_included

#define USE_AVR

// #define F_ONLY_NAN_NO_INFINITY

// #define F_PREFER_SMALL_BUT_SLOWER_CODE

// *********************************************************************** //
// Use the following defines to control which functions shall be compiled  //
// *********************************************************************** //

// #define F_WITH_sqrt
#define F_WITH_exp
#define F_WITH_log
// #define F_WITH_sin
// #define F_WITH_cos
// #define F_WITH_tan
// #define F_WITH_arcsin
// #define F_WITH_arccos
// #define F_WITH_arctan

// #define F_WITH_fmod
// #define F_WITH_cut_noninteger_fraction
// #define F_WITH_abs

// #define F_WITH_sd
// #define F_WITH_ds

// #define F_WITH_isnan
// #define F_WITH_finite
// #define F_WITH_compare

// #define F_WITH_float64_to_long
// #define F_WITH_long_to_float64
#define F_WITH_to_decimalExp
#define F_WITH_to_string
#define F_WITH_strtod
#define F_WITH_atof

#ifdef USE_AVR
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#else
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned long uint32_t;
typedef signed long int32_t;
typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;

#include <stdlib.h>
#include <string.h>
#endif

typedef uint64_t float64_t; // IEEE 754 double precision floating point number
typedef float float32_t;    // IEEE 754 single precision floating point number

float64_t f_long_to_float64(
    long n); // Converts a long to the float64_t representing the same number

long f_float64_to_long(float64_t x); // Converts a float64_t x to long by cutting the noninteger
// fraction of x and returning the integer fraction. If x is nonnegative and less then 2^^32,
// (unsigned long)f_float64_to_long(x) yields the correct value. If x is negative and not less than
// -2^^31, f_float64_to_long(x) yields the correct value. If the absolute value of x is 2^^32 or
// greater, f_float64_to_long(x) returns zero. ATTENTION: If -2^^32 < x < -2^^31,
// f_float64_to_long(x) returns 2^^32 plus the integer fraction of x.

float64_t f_sd(float32_t fx); // Converts a float32 to the float64 representing the same number.
                              // Denormalized 32-Bit single precision numbers are handled correctly
                              // and result in non denormalized double precision numbers.
float32_t f_ds(float64_t fx); // Converts a float64 to the nearest float32. Although denormalized
                              // float64's other than zero are not supported, f_ds(x) returns a
                              // denormalized float32 if the absolute value of x is small enough or
                              // zero (zero itself is also denormalized).

float64_t f_add(float64_t a, float64_t b);    // Returns a+b . Special case: -INF + INF = NaN
float64_t f_sub(float64_t a, float64_t b);    // Returns a-b . Special case:  INF - INF = NaN
float64_t f_mult(float64_t fa, float64_t fb); // Returns a*b . Special case: +/-INF * 0 = NaN
float64_t f_div(
    float64_t x,
    float64_t y); // Returns a/b . Special cases: x/0=NaN , INF/INF = NaN , x/INF = 0 if x!=+/-INF

float64_t f_abs(float64_t x);                     // Returns the absolute value of x
float64_t f_cut_noninteger_fraction(float64_t x); // Returns the integer part of x by cutting the
                                                  // noninteger part.
float64_t f_mod(
    float64_t x, float64_t y,
    float64_t *ganz); // Returns the floating-point
                      // remainder f of x / y such that x = i * y + f, where i is an integer, f has
                      // the same sign as x, and the absolute value of f is less than the absolute
                      // value of y. If ganz!=0 is passed, the value i is assigned to *ganz.

int8_t f_isnan(
    float64_t x); // Returns nonzero if x is an IEEE 754 "Not A Number" and otherwise zero.
int8_t f_finite(float64_t x); // Returns nonzero if x represents a real number and zero otherwise
                              // i.e. if x is +INF, -INF or NaN.
int8_t f_compare(
    float64_t x,
    float64_t y); // If both x and y represent real numbers
                  // (or +/-INF if F_ONLY_NAN_NO_INFINITY is not defined) f_compare returns
                  // zero if x is equal to y, positive nonzero if x > y and negative nonzero if x <
                  // y. If x or y are NaN, f_compare returns zero.

char *f_to_decimalExp(float64_t x, uint8_t anz_dezimal_mantisse,
                      uint8_t MantisseUndExponentGetrennt, int16_t *ExponentBasis10);
// f_to_decimalExp() converts the float64 to the decimal representation of the number x if x is
// a real number or to the strings "+INF", "-INF", "NaN". If x is real, f_to_decimalExp() generates
// a mantisse-exponent decimal representation of x using anz_dezimal_mantisse decimal digits for
// the mantisse. If MantisseUndExponentGetrennt!=0 is passed f_to_decimalExp() will generate
// different strings for the mantisse and the exponent. If you assign
//      char *str=f_to_decimalExp(x, anz_mts, 1, 0)
// then str points to the mantisse string and str+strlen(str) points to the exponent string.
// If the pointer ExponentBasis10 passed to f_to_decimalExp() is nonzero, the function will
// assign the 10-exponent to *ExponentBasis10 ; e.g. if the decimal representation of x
// is 1.234E58 then the integer 58 is assigned to *ExponentBasis10.

char *f_to_string(float64_t x, uint8_t max_nr_chars, uint8_t max_leading_mantisse_zeros);
// f_to_decimalExp() converts the float64 to the decimal representation of the number x if x is
// a real number or to the strings "+INF", "-INF", "NaN". If x is real, f_to_decimalExp() generates
// a decimal representation without or with mantisse-exponent representation depending on
// what in more suitable or possible. If -1 < x < 1 the exponent free representation is chosen if
// there are less than 'max_leading_mantisse_zeros' zeros after the decimal point.
// In most cases f_to_string() will generate a string with maximal 'max_nr_chars' chars. If
// necessary, f_to_string() reduces the number of decimal digits in the mantisse in order to
// get a maximum string width of 'max_nr_chars' chars. If however max_nr_chars is so small that
// even a mantisse of one digit and the corresponding exponent doesn't fit into 'max_nr_chars'
// chars, the string returned will be longer than 'max_nr_chars' chars.

float64_t f_strtod(
    char *str, char **endptr); // Converts a decimal representation of a real number
                               // of "INF", "+INF", "-INF", "NaN" into the float64 representing the
                               // same number of the non-real object. The string str must be in the
                               // usual format with or without 10-exponent, e.g.
                               //      1.234   ,   -89.32   ,  .001   ,   1E100    ,  -10.8432E32
                               // If a nonzero pointer to char-pointer is passed as endptr,
                               // f_strtod() will assign the char* to *endptr which points to the
                               // char (or to zero) that terminates the scan of the string str, i.e.
                               // the first char after the decimal number string or zero.
#define f_atof(str) (f_strtod((str), 0))

float64_t f_sqrt(
    float64_t x); // Returns the square root of x if x is nonnegative and NaN otherwise.
float64_t f_exp(
    float64_t x); // Evaluates the exponential function e^x at x (where e is Euler's constant).
float64_t f_log(
    float64_t x); // Returns the logarithm of x if x is positive or NaN if x is negative.
                  // If x is zero, f_log() will return -INF if F_ONLY_NAN_NO_INFINITY is not defined
                  // and NaN otherwise.
#define f_pow(x, y) \
    (f_exp(f_mult((y), f_log(x)))) // Returns the power x^y if x is positive or NaN if x is
                                   // negative. If x is zero, f_pow(0,y) will return NaN if
                  // F_ONLY_NAN_NO_INFINITY is defined or if y is zero; If x is zero, y is nonzero
                  // and F_ONLY_NAN_NO_INFINITY is not defined, f_pow(x,y) returns zero. Special
                  // cases: pow(0, INF)=0, pow(0, -INF)=+INF, pow(1, +/-INF)=NaN pow(x, INF)={ 0 if
                  // 0<=x<1 and INF if x>1 } pow(x, -INF)={ +INF if 0<=x<1 and 0 if x>1 } pow(+INF,
                  // 0)=NaN, pow(+INF, x)=+INF if x!=0

float64_t f_berechne_sinus_cosinus_tangens(float64_t x, uint8_t fkt_nr);
#define f_sin(x) (f_berechne_sinus_cosinus_tangens((x), 0)) // Returns the sine of x.
#define f_cos(x) (f_berechne_sinus_cosinus_tangens((x), 1)) // Returns the cosine of x.
#define f_tan(x) (f_berechne_sinus_cosinus_tangens((x), 2)) // Returns the tangent of x.

float64_t f_berechne_arcsin_arccos_arctan(float64_t x, uint8_t fkt_nr);
#define f_arcsin(x)                   \
    (f_berechne_arcsin_arccos_arctan( \
        (x), 0)) // Returns the arc sine of x if -1<=x<=1 and NaN otherwise
#define f_arccos(x)                   \
    (f_berechne_arcsin_arccos_arctan( \
        (x), 1)) // Returns the arc cosine of x if -1<=x<=1 and NaN otherwise
#define f_arctan(x) (f_berechne_arcsin_arccos_arctan((x), 2)) // Returns the arc tangent of x

#ifdef USE_AVR
#define f_EULER_E                               ((float64_t)0x4005bf0a8b145769LLU) // 2.7182818284590452
#define f_NUMBER_PI                             ((float64_t)0x400921fb54442d18LLU) // 3.1415926535897932
#define float64_NUMBER_ONE                      ((float64_t)0x3ff0000000000000LLU) // 1.0
#define float64_NUMBER_PLUS_ZERO                ((float64_t)0x0000000000000000LLU) // 0.0
#define float64_ONE_POSSIBLE_NAN_REPRESENTATION ((float64_t)0x7fffffffffffffffLLU) // NaN
#define float64_PLUS_INFINITY                   ((float64_t)0x7ff0000000000000LLU) // +INF
#define float64_MINUS_INFINITY                  ((float64_t)0xfff0000000000000LLU) // -INF
#else
#define f_EULER_E                               ((float64_t)0x4005bf0a8b145769) // 2.7182818284590452
#define f_NUMBER_PI                             ((float64_t)0x400921fb54442d18) // 3.1415926535897932
#define float64_NUMBER_ONE                      ((float64_t)0x3ff0000000000000) // 1.0
#define float64_NUMBER_PLUS_ZERO                ((float64_t)0x0000000000000000) // 0.0
#define float64_ONE_POSSIBLE_NAN_REPRESENTATION ((float64_t)0x7fffffffffffffff) // NaN
#define float64_PLUS_INFINITY                   ((float64_t)0x7ff0000000000000) // +INF
#define float64_MINUS_INFINITY                  ((float64_t)0xfff0000000000000) // -INF
#endif

#endif // #ifndef avr_f64_h_included
