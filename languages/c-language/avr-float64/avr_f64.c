// avr_f64.c

// Original Code by Detlef_a (Nickname in the www.mikrocontroller.net forum).
// Extensions (trigonometric functions et al.) und changes by Florian Königstein, mail@virgusta.eu .

// The following copyright information refers to the code developed by Florian Königstein.

//***********************************************************************************************************
//***********************************************************************************************************
//
// avr_f64.c and avr_f64.h : Contains ANSI C Funktions for handling of 64 bit IEEE 754 double
// precision floating point numbers. float64_t represents an IEEE 64 bit double precision
// floating point number.
//
// Compiler: avr-gcc
//
// Copyright:
//      You may use this program free of charge for any purpose you wish provided you agree to the
//      following License: Warranty of Provenance and Disclaimer of Warranty. Licensor
//      warrants that the copyright in and to the Original Work and the patent
//      rights granted herein by Licensor are owned by the Licensor or are sublicensed to
//      You under the terms of this License with the permission of the contributor(s) of
//      those copyrights and patent rights. Except as expressly stated in the immediately
//      preceding sentence, the Original Work is provided under this License on an "AS IS" BASIS and
//      WITHOUT WARRANTY, either express or implied, including, without limitation, the
//      warranties of non-infringement, merchantability or fitness for a
//      particular purpose. THE ENTIRE RISK AS TO THE QUALITY OF THE ORIGINAL WORK IS WITH
//      YOU. This DISCLAIMER OF WARRANTY constitutes an essential part of this License. No
//      license to the Original Work is granted by this License except under this
//      disclaimer.
//
//      Limitation of Liability. Under no circumstances and under no legal theory, whether in tort
//      (including negligence), contract, or otherwise, shall the Licensor be liable to anyone
//      for any indirect, special, incidental, or consequential damages of any character
//      arising as a result of this License or the use of the Original Work including,
//      without limitation, damages for loss of goodwill, work stoppage, computer failure or
//      malfunction, or any and all other commercial damages or losses. This limitation of
//      liability shall not apply to the extent applicable law prohibits such limitation.
//
//***********************************************************************************************************
//***********************************************************************************************************

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

// If this Library shall be compiled as C++ with avr-gcc (after renaming the file to avr_f64.cpp),
// all integer constants larger than 32 Bits must get the ending LLU .

#include "avr_f64.h"

#ifdef USE_AVR
#define LANGEEINS (1ull)
#else
#define LANGEEINS (1l)
#endif

static char TemporaryMemory[128];

#ifdef USE_AVR
#define FLASHMEM_IF_AVR PROGMEM
#else
#define FLASHMEM_IF_AVR
#endif

#if defined(USE_AVR) && (defined(F_WITH_sin) || defined(F_WITH_cos) || defined(F_WITH_tan) ||    \
                         defined(F_WITH_exp) || defined(F_WITH_log) || defined(F_WITH_arcsin) || \
                         defined(F_WITH_arccos) || defined(F_WITH_arctan))
static void copy_from_flash_to_tempmem(void *address_in_flash, uint8_t nr_bytes)
{
    uint8_t i;
    for (i = 0; i < nr_bytes; i++, address_in_flash = ((char *)address_in_flash) + 1)
        TemporaryMemory[i] = pgm_read_byte(address_in_flash);
}
#endif

static void f_split64(float64_t *x, uint8_t *f_sign, int16_t *f_ex, uint64_t *frac, uint8_t lshift)
{
    // Liefert *frac=0, wenn x die Zahl Null oder +/-INF darstellt.
    // Liefert *frac!=0, wenn x NaN ist.
    // Ansonsten wird in *frac auch das führende 1-Bit der Mantisse ergänzt, welches in *x nicht
    // vorhanden ist (und implizit als 1 gilt).
    *frac = (*x) & 0xfffffffffffff;
    if (0 == (*f_ex = (((*x) >> 52) & 2047)))
        *frac = 0;
    else if (2047 != (*f_ex))
        *frac |= 0x10000000000000;
    *frac <<= lshift;
    *f_sign = ((*x) >> 63) & 1;
}

#if defined(F_WITH_float64_to_long) || defined(F_WITH_exp) || defined(F_WITH_sin) || \
    defined(F_WITH_cos) || defined(F_WITH_tan) || defined(F_WITH_arcsin) ||          \
    defined(F_WITH_arccos) || defined(F_WITH_arctan)
static void f_split_to_fixpoint(float64_t *x, uint8_t *f_sign, int16_t *f_ex, uint64_t *frac,
                                int16_t point_bitnr)
{
    // Liefert *frac=0, wenn x die Zahl Null darstellt
    f_split64(x, f_sign, f_ex, frac, 0);
    if (0 != (*f_ex) && 2047 != (*f_ex)) {
        point_bitnr = (1023 + 52) - (*f_ex) - point_bitnr;
        if (point_bitnr <= -64 || point_bitnr >= 64)
            *frac = 0;
        else if (point_bitnr < 0)
            *frac <<= -point_bitnr;
        else
            *frac >>= point_bitnr;
    }
}
#endif

static void f_combi_from_fixpoint(float64_t *x, uint8_t f_sign, int16_t f_ex, uint64_t *frac)
{
    // Achtung: NaN kann mit dieser Funktion nicht erzeugt werden.
    // Wenn *frac==0 übergeben wird, wird Null erzeugt, falls f_ex<2047 ist, und sonst +/-INF.
    uint8_t round = 0;
    uint64_t w    = *frac;
    if (0 != w) {
        while (0 == (w & 0xffffe00000000000)) {
            w <<= 8;
            f_ex -= 8;
        }
        while (0 == (w & 0xfff0000000000000)) {
            w <<= 1;
            --f_ex;
        }
        while (0 != (w & 0xff00000000000000)) {
            round = 0 != (w & (1 << 3));
            w >>= 4;
            f_ex += 4;
        }
        while (0 != (w & 0xffe0000000000000)) {
            round = 0 != (w & 1);
            w >>= 1;
            ++f_ex;
        }
        if (round) {
            ++w;
            if (0 != (w & 0xffe0000000000000)) {
                w >>= 1;
                ++f_ex;
            }
        }
        if (f_ex <= 0) {
            // Falls f_ex==0: Es werden keine unnormalisierten Zahlen außer Null unterstützt
            f_ex = 0;
            w    = 0;
        } // +0 oder -0
    } else if (f_ex < 2047)
        f_ex = 0;
    if (f_ex >= 2047) {
        f_ex = 2047;
        w    = 0;
    } // +INF oder -INF
    *((uint64_t *)x) = ((uint64_t)f_sign << 63) | ((uint64_t)f_ex << 52) | (w & 0xfffffffffffff);
}

#if defined(F_WITH_fmod) || defined(F_WITH_exp) || defined(F_WITH_sin) || defined(F_WITH_cos) || \
    defined(F_WITH_tan) || defined(F_WITH_to_decimalExp) || defined(F_WITH_to_string) ||         \
    defined(F_WITH_strtod) || defined(F_WITH_atof)
static int8_t f_shift_left_until_bit63_set(uint64_t *w)
{
    // Falls *w=0 ist oder falls das Bit mit der Nummer 63 nicht durch
    // mehrmaliges Linksschieben von *w gesetzt werden kann, wird *w=0 gesetzt und 64
    // zurückgeliefert. Ansonsten wird *w so oft nach links geshiftet, bis das Bit mit der Nummer 63
    // gesetzt ist. Die Anzahl der erfolgten Links-Shifts wird zurückgeliefert.
    register int8_t count = 0;
    register uint64_t mask;
    for (mask = ((uint64_t)255LU) << (63 - 7); 0 == (mask & (*w)) && count < 64; count += 8)
        (*w) <<= 8;
    for (mask = ((uint64_t)1LU) << 63; 0 == (mask & (*w)) && count < 64; ++count)
        (*w) <<= 1;
    return count;
}
#endif

static uint64_t approx_high_uint64_word_of_uint64_mult_uint64(uint64_t *x, uint64_t *y,
                                                              uint8_t signed_mult)
{
    // Berechnet eine Näherung von floor(x * y / (2 hoch 64)). Wenn signed_mult==0 übergeben wird,
    // ist der zurückgegebene Wert ist niemals größer und maximal um 3 kleiner als der tatsächliche
    // Wert. 0!=signed_mult & 1 : *x ist vorzeichenbehaftet 0!=signed_mult & 2 : *y ist
    // vorzeichenbehaftet
    uint64_t r = ((*x) >> 32) * ((*y) >> 32) + ((((*x) >> 32) * ((*y) & 0xffffffff)) >> 32) +
                 ((((*y) >> 32) * ((*x) & 0xffffffff)) >> 32);
    if (0 != (signed_mult & 1) && ((int64_t)(*x)) < 0)
        r -= (*y);
    if (0 != (signed_mult & 2) && ((int64_t)(*y)) < 0)
        r -= (*x);
    return r;
}

static uint64_t approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(uint64_t *x, uint64_t y,
                                                                    uint8_t signed_mult)
{
    return approx_high_uint64_word_of_uint64_mult_uint64(x, &y, signed_mult);
}

#if defined(F_PREFER_SMALL_BUT_SLOWER_CODE)
static uint64_t approx_inverse_of_fixpoint_uint64(uint64_t *y)
{
    // Es muss 0!=*y & 0x8000000000000000 sein !
    // Unter dieser Voraussetzung wird (2 hoch 126) / *y berechnet und ggf. auf die nächste
    // ganze Zahl gerundet.
    uint64_t zL = 0, dH = *y, dL = 0, aH = 0x4000000000000000, aL = 0;
    uint8_t i = 65;
    while (1) {
        dL >>= 1;
        if (dH & 1)
            dL |= ((uint64_t)1LU) << 63;
        dH >>= 1;
        if (0 == --i)
            break;
        zL <<= 1;
        if (aH > dH || (aH == dH && aL >= dL)) {
            zL |= 1;
            aH -= dH;
            if (dL > aL)
                --aH;
            aL -= dL;
        }
    }
    if (aL >= dL)
        ++zL;
    return zL;
}
#else
static uint64_t approx_inverse_of_fixpoint_uint64(uint64_t *y)
{
    // Es muss 0!=*y & 0x8000000000000000 sein !
    // Unter dieser Voraussetzung wird eine Näherung für (2 hoch 126) / *y berechnet.
    // Theoretische Überlegungen ergeben, dass nahezu alle 64 Bits des Ergebnisses signifkant sind.
    // Numerische Experimente lassen vermuten, dass der zurückgegebene Wert um maximal +/- 1
    // gegenüber dem auf die nächste ganze Zahl gerundeten Wert von (2 hoch 126) / *y abweicht.
    uint64_t yv = *y, z8, k5;
    uint16_t z3;
    uint32_t z5 = (yv >> 32), z6, k;
    uint8_t c;

    if (0 != (((unsigned long)yv) & 0x80000000) && 0 == ++z5)
        z6 = 0;
    else {
        z3 = (z5 >> 16);

        if (0 == (z3 & 0x7fff)) {
            if (0 == (z5 & 0xffff))
                return (0xffffffffffffffff + 1) - yv;
            z6 = -((z5 & 0xffff) << 2);
        } else if (0xffff == z3)
            z6 = -z5;
        else {
            z3 = (uint16_t)(1 + (0xffffffff - (z3 >> 1)) / z3);
            z6 = (((uint32_t)z3) << 17) -
                 (z5 + ((z3 * ((0x20000 + (uint32_t)z3) * (uint64_t)z5)) >> 32)) - 1;
        }
    }

    for (k = (uint32_t)(k5 = (0x100000000 | ((uint64_t)z6)) * z5 + (z5 >> 1)), c = k5 >> 32;
         0 != c;) {
        if (k + z5 < k)
            ++c; // Nach einem 8-Bit-Überlauf von c wird die Schleife abgebrochen
        k += z5;
        ++z6;
    }

    z8 = (((uint64_t)1LU) << 63) | ((uint64_t)z6) << 31;

    // In der folgenden Anweisung wird ein Integer-Überlauf bewusst ausgenutzt.
    // Es wird a=(2 hoch 64) -z -approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(&z, yv<<1, 0)
    // berechnet.
    k5 = (0xffffffffffffffff + 1) - z8 -
         approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
             &z8, yv << 1, 0); // Bei yv<<1 geht das höchstwertigste Bit verloren
    z8 = approx_high_uint64_word_of_uint64_mult_uint64(&z8, &k5, 0) +
         ((0 == (k5 & 0x8000000000000000)) ? z8 : 0);
    return (z8 + 1) >> 1;
}
#endif

#ifdef F_WITH_long_to_float64
float64_t f_long_to_float64(long n)
{
    float64_t r;
    uint64_t w = ((uint64_t)(n < 0 ? -n : n)) << 20;
    f_combi_from_fixpoint(&r, n < 0 ? 1 : 0, 1023 + 32, &w);
    return r;
}
#endif

#if defined(F_WITH_abs) || defined(F_WITH_exp) || defined(F_WITH_sin) || defined(F_WITH_cos) || \
    defined(F_WITH_tan)
float64_t f_abs(float64_t x)
{
    return x & 0x7fffffffffffffff;
}
#endif

#ifdef F_WITH_sd
/***********************************************************/
float64_t f_sd(float32_t fx)
/***********************************************************/
{
    int32_t *i;
    uint8_t f_sign;
    int16_t f_ex;
    uint64_t w;
    float64_t f64;
    // float32_t x;

    i      = &fx;
    w      = ((*i) & 0x7fffffl);
    f_ex   = (*i >> 23) & 0xff;
    f_sign = (*i >> 31) & 1;

    if (0 == f_ex && 0 != w) // Denormalisierte float (32 Bits) Zahl
        f_ex += 29 + 0x3ff - 0x7e;
    else if (255 == f_ex) // +/-INF oder NaN
        return 0 == w ? (f_sign ? float64_MINUS_INFINITY : float64_PLUS_INFINITY)
                      : float64_ONE_POSSIBLE_NAN_REPRESENTATION;
    else {
        w |= 0x800000; // Falls KEINE denormalisierte float-Zahl (32 Bits) vorliegt, wird das
                       // implizite führende 1-Bit in w ergänzt.
        if (f_ex)
            f_ex += 29 + 0x3ff - 0x7f;
        // Für ==> FLOAT (32 Bits) <== gilt: NICHT ALLE denormalisierten FLOAT-Zahlen werden als
        // Null interpretiert.
    }
    f_combi_from_fixpoint(&f64, f_sign, f_ex, &w);

    return (f64);
}
#endif

#ifdef F_WITH_ds
/***********************************************************/
float32_t f_ds(float64_t fx)
/***********************************************************/
{
    int8_t f_sign;
    int16_t f_ex;
    uint32_t ui32;
    float32_t f32;
    uint64_t w;

    f_split64(&fx, &f_sign, &f_ex, &w, 0);

    if (f_ex >= 1023 - 149) {
        if (f_ex > 1023 + 127) // +/-INF oder NaN
        {
            if (f_ex == 2047 && 0 != w) // NaN
                ui32 = 0xfffff;
            else
                ui32 = 0;
            f_ex = 255;
        } else {
            ui32 = (w >> (52 - 23)) & 0x7fffff; // Es ist 0x10000000000000 | w == w
            if (f_ex < 1023 - 126) // Es wird eine denormalisierte float-Zahl (32 Bits) erzeugt.
            {
                ui32 = (ui32 | 0x800000) >> (1023 - 126 - f_ex);
                f_ex = 0;
            } else
                f_ex = (f_ex - 0x3ff + 0x7f) & 0xff;
        }
    } else // geändert von Florian Königstein: Alle denormalisierten float64-Zahlen werden als Null
           // interpretiert.
        ui32 = 0;

    ui32 |= ((uint32_t)f_sign << 31) | ((uint32_t)f_ex << 23);
    f32 = *((float32_t *)&ui32);

    return (f32);
}
#endif

/***********************************************************/
static void f_addsub2(float64_t *x, float64_t a, float64_t b, uint8_t flagadd, uint8_t *flagexd)
/***********************************************************/
{
    // add positive doubles
    uint8_t sig;
    int16_t aex, bex;
    uint64_t wa, wb;

    f_split64(&a, &sig, &aex, &wa, 10);
    f_split64(&b, &sig, &bex, &wb, 10);

    *flagexd = 0;

#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == aex || 2047 == bex) {
        *x = float64_ONE_POSSIBLE_NAN_REPRESENTATION;
        return;
    }
#else
    if (2047 == aex) // a ist ein NaN oder +INF oder -INF
    {
        if (0 == wa && (2047 != bex || (0 == wb && flagadd)))
            *x = a;
        else
            *x = float64_ONE_POSSIBLE_NAN_REPRESENTATION;
        return;
    } else if (2047 == bex) {
        *x = b;
        return;
    }
#endif

    if (!aex || aex + 64 <= bex) {
        *x       = b;
        *flagexd = 1;
        return;
    } // Alle denormalisierten Zahlen werden als Null interpretiert.
    if (!bex || bex + 64 <= aex) {
        *x = a;
        return;
    } // Alle denormalisierten Zahlen werden als Null interpretiert.

    if (flagadd) {
        if (aex >= bex)
            wa += wb >> (aex - bex); // aex-bex<64 wurde schon durch die obige Abfrage if(!bex ||
                                     // bex+64<=aex) sichergestellt
        else {
            wa = wb + (wa >> (bex - aex)); // bex-aex<64 wurde schon durch die obige Abfrage if(!aex
                                           // || aex+64<=bex) sichergestellt
            aex = bex;
        }
    } else {
        if (aex > bex || (aex == bex && wa >= wb))
            wa -= wb >> (aex - bex); // aex-bex<64 wurde schon durch die obige Abfrage if(!bex ||
                                     // bex+64<=aex) sichergestellt
        else {
            wa = wb - (wa >> (bex - aex)); // bex-aex<64 wurde schon durch die obige Abfrage if(!aex
                                           // || aex+64<=bex) sichergestellt
            *flagexd = 1;
            aex      = bex;
        }
    }
    f_combi_from_fixpoint(x, 0, aex - 10, &wa);
}

/***********************************************************/
static void f_setsign(float64_t *x, int8_t sign)
/***********************************************************/
{
    if (sign)
        *x |= 0x8000000000000000;
    else
        *x &= 0x7fffffffffffffff;
}

/***********************************************************/
static uint8_t f_getsign(float64_t x)
/***********************************************************/
{
    uint64_t *px = &x;
    return ((uint8_t)(((*px) >> 63) & 1));
}

/***********************************************************/
float64_t f_add(float64_t a, float64_t b)
/***********************************************************/
{
    uint8_t signa, signb, signerg;
    uint8_t flagexd;
    uint64_t i64;
    float64_t *x = &i64;

    signa = f_getsign(a);
    signb = f_getsign(b);
    if (signa ^ signb) {                 // diff. signs
        f_addsub2(x, a, b, 0, &flagexd); // calc a-b
        signerg = ((flagexd ^ signa)) & 1;
    } else {                             // eq.   signs
        f_addsub2(x, a, b, 1, &flagexd); // calc a+b
        signerg = signa;
    }
    f_setsign(x, signerg);

    return (i64);
}

/***********************************************************/
float64_t f_sub(float64_t a, float64_t b)
/***********************************************************/
{
    uint8_t signb;
    float64_t bloc = b;
    uint64_t i64;

    signb = f_getsign(bloc);
    signb ^= 1;
    f_setsign(&bloc, signb);
    i64 = f_add(a, bloc);
    return (i64);
}

/***********************************************************/
float64_t f_mult(float64_t fa, float64_t fb)
/***********************************************************/
{
    // multiply doubles
    uint8_t asig, bsig;
    int16_t aex, bex;
    uint64_t am, bm;

    f_split64(&fa, &asig, &aex, &am, 11);
    f_split64(&fb, &bsig, &bex, &bm, 11);

#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == aex || 2047 == bex)
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
#else
    if (2047 == aex) // a ist ein NaN oder +INF oder -INF
    {
        if (0 != am || (2047 == bex && 0 != bm) || 0 == bex)
            return float64_ONE_POSSIBLE_NAN_REPRESENTATION; // Auch +/-INF * Null ergibt NaN
        // (+/- INF) * (+/- INF) oder (+/- INF) * endliche Zahl außer Null ergibt +/-INF
    } else if (2047 == bex) {
        if (0 != bm || 0 == aex)
            return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
        aex = bex;
        am  = bm;
    }
#endif
    else if (!aex || !bex) // geändert von Florian Königstein: Alle denormalisierten Zahlen werden
                           // als Null interpretiert.
    {                      // 0
        return float64_NUMBER_PLUS_ZERO;
    } else {
        aex = aex + bex - (0x3ff + 10);
        am  = approx_high_uint64_word_of_uint64_mult_uint64(&am, &bm, 0);
    }
    asig ^= bsig;
    f_combi_from_fixpoint(&fa, asig, aex,
                          &am); // Hier darf von aex nichts subtrahiert werden, damit
                                // auch aex==2047 (INF, NaN) nicht verfälscht wird.
    return fa;
}

/***********************************************************/
float64_t f_div(float64_t x, float64_t y)
/***********************************************************/
{
    uint8_t xsig, ysig;
    int16_t xex, yex;
    uint64_t xm, ym, i64;

    f_split64(&x, &xsig, &xex, &xm, 11);
    f_split64(&y, &ysig, &yex, &ym, 11);
#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == xex || 2047 == yex || 0 == yex)
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
#else
    if (0 == yex) { // divide by 0 wont work  . Geändert von Florian Königstein: Alle
                    // denormalisierten Zahlen werden als Null interpretiert.
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
    } else if (2047 == xex) // x ist ein NaN oder +INF oder -INF
    {
        if (0 != xm || 2047 == yex)
            return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
        // Ansonsten: (+/- INF) / (endliche Zahl außer Null)
        return (xsig ^ ysig) ? float64_MINUS_INFINITY : float64_PLUS_INFINITY;
    } else if (2047 == yex) // x ist ein NaN oder +INF oder -INF
    {
        if (0 == ym) // endliche Zahl / INF = Null
            return float64_NUMBER_PLUS_ZERO;
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
    }
#endif
    else {
        i64 = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
            &xm, approx_inverse_of_fixpoint_uint64(&ym), 0);
        xex += 1023 - yex;
    }
    f_combi_from_fixpoint(&x, xsig ^ ysig, xex - 10, &i64);
    return x;
}

#ifdef F_WITH_cut_noninteger_fraction
float64_t f_cut_noninteger_fraction(float64_t x)
{
    int16_t f_ex = (((x) >> 52) & 2047);
    if (0 == f_ex ||
        f_ex >= 1023 + 52) // Alle denormalisierten Zahlen werden als Null interpretiert.
        return x;          // schließt auch die Fälle x=NaN, x=+/-INF ein.
    if (f_ex < 1023)
        return float64_NUMBER_PLUS_ZERO;
    return x & (0xffffffffffffffff << ((1023 + 52) - f_ex));
}
#endif

#if defined(F_WITH_float64_to_long) || defined(F_WITH_exp)
long f_float64_to_long(float64_t x)
{
    // Wenn f_abs(x) größer als 1LU<<31 ist, wird Null geliefert.
    uint8_t f_sign;
    int16_t f_ex;
    uint64_t w;
    f_split_to_fixpoint(&x, &f_sign, &f_ex, &w, 0);
    return (f_ex >= 1023 + 32)
               ? 0
               : (f_sign ? -((long)w) : ((long)w)); // Hiermit werden auch die Fälle
                                                    // x=+/-INF und x=NaN abgedeckt (Lieferung von
                                                    // 0). Im Fall x=0 ist w=0.
}
#endif

#ifdef F_WITH_isnan
int8_t f_isnan(float64_t x)
{ // Returns nonzero if x is an IEEE 754 "Not A Number" and otherwise zero.
    return 0x7ff0000000000000 == (((uint64_t)x) & 0x7ff0000000000000) &&
           0 != (((uint64_t)x) & 0xfffffffffffff);
}
#endif

#ifdef F_WITH_finite
int8_t f_finite(float64_t x)
{ // Returns nonzero if x represents a real number and zero otherwise i.e. if x is +INF, -INF or
  // NaN.
    return 0x7ff0000000000000 != (((uint64_t)x) & 0x7ff0000000000000);
}
#endif

#ifdef F_WITH_compare
int8_t f_compare(float64_t x, float64_t y)
{ // If both x and y represent real numbers
    // (or +/-INF if F_ONLY_NAN_NO_INFINITY is not defined) f_compare returns
    // zero if x is equal to y, positive nonzero if x > y and negative nonzero if x < y.
    // If x or y are NaN, f_compare returns zero.
    uint8_t asig, bsig;
    int16_t xex, yex;
    uint64_t wx, wy;
    f_split64(&x, &asig, &xex, &wx, 0);
    f_split64(&y, &bsig, &yex, &wy, 0);
#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == xex || 2047 == yex)
        return 0;
#else
    if ((2047 == xex && 0 != wx) || (2047 == yex && 0 != wy)) // x=NaN oder y=NaN
        return 0;
    if (2047 == xex) // x = +INF oder -INF (oder NaN, aber schon oben abgefangen)
    {
        if (2047 == yex) // y = +INF oder -INF
            return asig == bsig ? 0 : (asig ? -1 : 1);
        else
            return asig ? -1 : 1;
    }
    if (2047 == yex)          // y = +INF oder -INF (oder NaN, aber schon oben abgefangen)
        return bsig ? 1 : -1; // x kann wegen den obigen Abfragen weder +/-INF noch NaN sein
#endif
    if (0 == xex) // x = 0 (Alle denormalisierten Zahlen werden als Null interpretiert)
        return (0 == yex && 0 == wy) ? 0 : (bsig ? 1 : -1);
    if (0 == yex || asig != bsig ||
        xex > yex) // Alle denormalisierten Zahlen werden als Null interpretiert
        return asig ? -1 : 1;
    if (xex < yex)
        return asig ? 1 : -1;
    return wx == wy ? 0 : ((wx > wy && !asig) || (wx < wy && asig) ? 1 : -1);
}
#endif

#if defined(F_WITH_exp) || defined(F_WITH_log) || defined(F_WITH_sin) || defined(F_WITH_cos) || \
    defined(F_WITH_tan) || defined(F_WITH_arcsin) || defined(F_WITH_arccos) ||                  \
    defined(F_WITH_arctan)
static uint64_t f_eval_function_by_rational_approximation_fixpoint(uint64_t x, uint8_t anz_zaehler,
                                                                   uint8_t anz_nenner,
                                                                   uint64_t *koeffs,
                                                                   uint8_t signed_mult)
{ // 0!=(signed_mult&1) : koeffs[] oder x sind vorzeichenbehaftet.
    // 0!=(signed_mult&2) : x wird als negativ interpretiert, darf aber NICHT als Zweierkomplement
    // vorliegen (es muss dann also der Betrag abs(x) von x übergeben werden). Evaluation einer
    // rationalen Funktion mit den Koeffizienten koeffs[] mittels Festkommaarithmetik. Durch x wird
    // die rationale Zahl xr = x / (2 hoch 64) dargestellt (==> Es kann nur 0<=xr<1 übergeben werden
    // oder aber -1<x<=0, falls 0!=(signed_mult&2)).
    // Der rationale Funktionswert muss zwischen 0 und kleiner als 4 liegen, falls
    // 0==(signed_mult&1), und zwischen größer als -2 und kleiner als 2, falls 0!=(signed_mult&1).
    // Er wird vor der Rückgabe mit 2 hoch 62 multipliziert, so dass 62 binäre Nachkommastellen zur
    // Verfügung stehen. Ein negativer Rückgabewert wird im Zweierkomplement dargestellt. Durch
    // koeffs[n] wird jeweils eine rationale Zahl koeffs[n] / (2 hoch 62) dargestellt. Wenn das
    // höchstwertigste Bit von koeffs[n] gesetzt ist und 0!=(signed_mult&1) übergeben wird, wird ein
    // negativer Koeffizient im Zweierkomplement dargestellt. Durch zz und nn wird jeweils eine
    // rationale Zahl zz / (2 hoch 62) bzw. nn / (2 hoch 62) dargestellt.

    uint64_t zz, nn;
    uint8_t sh;

    if (0 == anz_zaehler)
        zz = 0x800000000000000;
    else
        for (zz = 0; (((0 == (signed_mult & 2) || 0 != (anz_zaehler & 1)) ? (zz += *koeffs)
                                                                          : (zz -= *koeffs)),
                      koeffs++, 0 != --anz_zaehler);)
            zz = approx_high_uint64_word_of_uint64_mult_uint64(&zz, &x, signed_mult & 1);

    if (0 == anz_nenner)
        return zz;
    for (nn = 0;
         (((0 == (signed_mult & 2) || 0 != (anz_nenner & 1)) ? (nn += *koeffs) : (nn -= *koeffs)),
          koeffs++, 0 != --anz_nenner);)
        nn = approx_high_uint64_word_of_uint64_mult_uint64(&nn, &x, signed_mult & 1);

    sh = 0;
    while (0 == (0xff00000000000000 & nn)) { // Für approx_inverse_of_fixpoint_uint64() muss das
                                             // höchstwertigste Bit gesetzt sein !
        nn <<= 8;
        if (0 == (0xff00000000000000 & zz))
            zz <<= 8;
        else
            sh += 8;
    }
    while (0 == (0x8000000000000000 & nn)) { // Für approx_inverse_of_fixpoint_uint64() muss das
                                             // höchstwertigste Bit gesetzt sein !
        nn <<= 1;
        if (0 == (0x8000000000000000 & zz))
            zz <<= 1;
        else
            ++sh;
    }
    nn = approx_inverse_of_fixpoint_uint64(&nn);
    return approx_high_uint64_word_of_uint64_mult_uint64(&zz, &nn, 0) << sh;
}
#endif

#if defined(F_WITH_fmod) || defined(F_WITH_exp) || defined(F_WITH_sin) || defined(F_WITH_cos) || \
    defined(F_WITH_tan)
static float64_t f_mod_intern(float64_t x, uint8_t ysig, int16_t yex, uint64_t *ymts,
                              float64_t *ganz)
{
    uint8_t xsig, count;
    int16_t xex, zex;
    uint64_t xm;
    float64_t g;
    uint64_t q;

    f_split64(&x, &xsig, &xex, &xm, 11);
    if (ganz)
        *ganz = float64_NUMBER_PLUS_ZERO;
    if (0 == xex)
        return float64_NUMBER_PLUS_ZERO;
    if (2047 == xex || 2047 == yex || 0 == (yex | (*ymts))) {
        if (ganz)
            *ganz = float64_ONE_POSSIBLE_NAN_REPRESENTATION;
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
    }

    for (count = 10;
         0 != (xex | xm) && (xex > yex || (xex == yex && xm >= (*ymts))) &&
         0 != --count;) { // In vielen Fällen wird diese Schleife kein oder ein Mal durchlaufen.
        q = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
                &xm, (approx_inverse_of_fixpoint_uint64(ymts) - 5) << 1, 0) -
            5;
        // approx_inverse_of_fixpoint_uint64(&ym)-5 ist nicht größer als der tatsächliche Wert (ohne
        // Rundungsfehler). Außerdem ist (2 hoch 62) <= approx_inverse_of_fixpoint_uint64(&ym)-5 <
        // (2 hoch 63).
        if (xex == yex && 0 == (0x8000000000000000 & q)) // Rundungsfehler
            q = 0x8000000000000000;                      // entspricht der Zahl 1
        zex = xex - yex;
        q &= (0xffffffffffffffff << (63 - zex));
        xm -= approx_high_uint64_word_of_uint64_mult_uint64(&q, ymts, 0) << 1;

        if (ganz) {
            f_combi_from_fixpoint(&g, xsig ^ ysig, (1023 - 11) + zex, &q);
            *ganz = f_add(*ganz, g);
        }
        xex -= f_shift_left_until_bit63_set(&xm);
    }
    f_combi_from_fixpoint(&x, xsig, xex - 11, &xm);
    *ymts = xm;
    if (0 == count)
        return float64_NUMBER_PLUS_ZERO; // Keine exakte Berechnung erfolgt. Sicherstellen, dass
                                         // der Rückgabewert innerhalb des Bereichs von 0 bis
                                         // ausschließlich y liegt.
    return x;
}
#endif

#ifdef F_WITH_fmod
float64_t f_mod(float64_t x, float64_t y, float64_t *ganz)
{ // The fmod function calculates the floating-point remainder f of x / y such that x = i * y + f,
    // where i is an integer, f has the same sign as x, and the absolute value of f is less than the
    // absolute value of y.
    uint8_t ysig;
    int16_t yex;
    uint64_t ym;
    f_split64(&y, &ysig, &yex, &ym, 11);
    return f_mod_intern(x, ysig, yex, &ym, ganz);
}
#endif

#if defined(F_WITH_sqrt) || defined(F_WITH_sin) || defined(F_WITH_cos) || defined(F_WITH_tan) || \
    defined(F_WITH_arcsin) || defined(F_WITH_arccos) || defined(F_WITH_arctan)
static uint64_t rounded_sqrt_of_integer128(uint64_t x_high, uint64_t x_low)
{ // Liefert die auf die nächste ganze Zahl gerundete Quadratwurzel aus (2 hoch 64)*x_high + x_low .
    // Falls die Wurzel nach Rundung gleich (2 hoch 64) sein müsste, wird (2 hoch 64)-1
    // zurückgegeben.
    uint64_t rL = 0, rH = 0, sH, sL;
    uint8_t i;
    for (i = 2 * 64; 0 != i;) {
        i -= 2;
        sH = rH;
        sL = rL;
        if (i >= 64)
            sH += ((uint64_t)1LU) << (i - 64);
        else {
            if (sL >= (uint64_t)(-(((uint64_t)1LU) << i))) // sL >= (2 hoch 64) - (2 hoch i)
                sH++;
            sL += ((uint64_t)1LU) << i;
        }

        rL >>= 1;
        if (rH & 1)
            rL |= ((uint64_t)1LU) << 63;
        rH >>= 1;

        if (sH < x_high || (sH == x_high && sL <= x_low)) {
            x_high -= sH;
            if (sL > x_low)
                --x_high;
            x_low -= sL;
            if (i >= 64)
                rH |= ((uint64_t)1LU) << (i - 64);
            else
                rL |= ((uint64_t)1LU) << i;
        }
    }
    return (0 != x_high || x_low >= rL) && 0xffffffffffffffff != rL ? rL + 1 : rL;
}
#endif

#ifdef F_WITH_sqrt
float64_t f_sqrt(float64_t x)
{
    uint8_t xsig;
    int16_t xex;
    uint64_t w, wL;

    f_split64(&x, &xsig, &xex, &w, 11);
    if (0 == xex) // Null
        return float64_NUMBER_PLUS_ZERO;
#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == xex || 0 != xsig)
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
#else
    if (2047 == xex) // NaN, +INF oder -INF
    {
        if (0 == w && 0 == xsig) // +INF
            return x;
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
    }
    if (xsig)
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
#endif

    wL = 0;
    if (xex & 1) {
        if (w & 1)
            wL = ((uint64_t)1LU) << 63;
        w >>= 1;
    }

    w = rounded_sqrt_of_integer128(w, wL);
    f_combi_from_fixpoint(&x, 0, (xex + 1) / 2 + 500, &w);
    return x;
}
#endif

#ifdef F_WITH_exp
float64_t f_exp(float64_t x)
{
    uint8_t f_sign, f_sign2;
    int16_t f_ex;
    float64_t g, rest;
    int16_t gi;
    uint64_t w, w2;
    static uint64_t interp[] FLASHMEM_IF_AVR = {
        // Interpolation von exp(x) im Bereich von x=0 ... log(2.0)
        // Zähler: 12 Koeffizienten, Nenner: 0 Koeffizienten
        0x00000025f42ff241, 0x00000112e79387f1, 0x00000ba525a70ff3, 0x000067f7ce0d91cb,
        0x0003403a635c5891, 0x0016c16a4128d5ee, 0x00888888e10886ed, 0x02aaaaaaa0251d3e,
        0x0aaaaaaaab66a1c0, 0x1ffffffffff9434d, 0x400000000000180b, 0x3ffffffffffffff2
    };

    f_split64(&x, &f_sign, &f_ex, &w, 0);

    if (0 == f_ex)
        return float64_NUMBER_ONE;
#ifdef F_ONLY_NAN_NO_INFINITY
    // Ohne die folgende Abfrage würde die unten stehende Funktion f_float64_to_long() einen
    // falschen Wert liefern. Damit werden auch die Fälle erfasst, dass x=NaN oder x=+/-INF ist.
    if (f_ex >= 1023 + 10)
        return (f_sign && f_ex != 2047) ? float64_NUMBER_PLUS_ZERO
                                        : float64_ONE_POSSIBLE_NAN_REPRESENTATION;
#else
    if (2047 == f_ex) // NaN, +INF oder -INF
    {
        if (0 != w || (0 == w && 0 == f_sign)) // NaN oder +INF
            return x;
        return float64_NUMBER_PLUS_ZERO;
    }
    if (f_ex >= 1023 + 10) // Ohne diese Abfrage würde die unten stehende Funktion
                           // f_float64_to_long() einen falschen Wert liefern.
        return f_sign ? float64_NUMBER_PLUS_ZERO : float64_PLUS_INFINITY;
#endif

    w    = 0xb17217f7d1cf79ac;                      // Mantisse von log(2)
    rest = f_mod_intern(f_abs(x), 0, 1022, &w, &g); // Modulo log(2)
    // Von f_mod_intern() wurde an w nun die 64-Bit-Mantisse von x zugewiesen.
    f_split_to_fixpoint(&rest, &f_sign2, &f_ex, &w2, 64); // w2, nicht w übergeben !
    w >>= 1022 - f_ex;
    gi = f_float64_to_long(g);

    if (f_sign2)
        w = 0;
    else if (f_ex >= 1023) // Rundungsfehler bei f_mod
        w = 0xffffffffffffffff;

#ifdef USE_AVR
    copy_from_flash_to_tempmem(&interp[0], 12 * 8);
    w = f_eval_function_by_rational_approximation_fixpoint(w, 12, 0,
                                                           (uint64_t *)(&TemporaryMemory[0]), 0);
#else
    w = f_eval_function_by_rational_approximation_fixpoint(w, 12, 0, interp, 0);
#endif
    if (f_sign) {
        gi -= f_shift_left_until_bit63_set(&w);
        w = approx_inverse_of_fixpoint_uint64(&w);
    }
    f_combi_from_fixpoint(&g, 0, f_sign ? (1021 - 10) - gi : (1023 - 10) + gi, &w);

    return g;
}
#endif

#ifdef F_WITH_log
float64_t f_log(float64_t x)
{
    uint8_t f_sign;
    int8_t sgnw2 = 0; // Initialisierung sgnw2=0 wichtig !
    int16_t f_ex, f_ex2;
    uint64_t w, w2;
    static uint64_t interp[] FLASHMEM_IF_AVR = {
        // Interpolation von log(1+x)/x im Bereich von x= -0.125 ... 1
        // Zähler: 7 Koeffizienten, Nenner: 8 Koeffizienten
        0x00020306b29459c0, 0x0059c8a974126d6a, 0x03f628260c2db8e4, 0x11dfa5d2d89584a5,
        0x255cf1f4fada2dcc, 0x243348927d4a6bdf, 0x0d22e971210e91e5, 0x00005e30ad9c1880,
        0x001a68f2d08c85a8, 0x01b09953e747fe7e, 0x0af24b95490068ee, 0x2215f7d0824727db,
        0x365e581f76be278e, 0x2ac4bd4b0dd1b53c, 0x0d22e971210e91e6
    };

    f_split64(&x, &f_sign, &f_ex, &w, 11);

#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == f_ex || 0 == f_ex || 0 != f_sign)
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
#else
    if (0 == f_ex) // Null
        return float64_MINUS_INFINITY;
    if (2047 == f_ex || 0 != f_sign) // NaN, +INF oder -INF
    {
        if (0 == w && 0 == f_sign) // +INF
            return x;
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION; // -INF
    }
#endif

    if (1022 == f_ex && w >= 0xe000000000000000) { // Hiermit wird erreicht, dass auch im Fall, dass
                                                   // x kleiner als 1, aber dicht bei 1 liegt, alle
        // Mantissebits des berechneten Logarithmus signifikant sind (wenn man davon absieht, dass
        // in seltenen Fällen in die "falsche" Richtung gerundet wird).
        w = (0xffffffffffffffff + 1) -
            w; // 0xffffffffffffffff+1 ergibt gewollt 0 (warning: integral constant overflow).
        sgnw2 = -1;
    } else {
        w2 = f_ex - 1023; // vor dem Überschreiben von f_ex !
        if (f_ex < 1023) {
            w2    = -w2;
            sgnw2 = -2;
        } else if (f_ex > 1023)
            sgnw2 = 1;

        w <<= 1; // Das höchstwertigste Bit hinausschieben: Entspricht der Subtraktion von 1 von der
                 // Mantisse.
    }

    f_ex = (1022 - 9) - f_shift_left_until_bit63_set(&w);

#ifdef USE_AVR
    copy_from_flash_to_tempmem(&interp[0], 15 * 8);
    if (0 != w)
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
            &w,
            f_eval_function_by_rational_approximation_fixpoint(w >> ((1022 - 9) - f_ex), 7, 8,
                                                               (uint64_t *)(&TemporaryMemory[0]),
                                                               -1 == sgnw2 ? 3 : 0),
            0);
#else
    if (0 != w)
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
            &w,
            f_eval_function_by_rational_approximation_fixpoint(w >> ((1022 - 9) - f_ex), 7, 8,
                                                               interp, -1 == sgnw2 ? 3 : 0),
            0);
#endif

    if (0 == sgnw2 || -1 == sgnw2)
        f_ex2 = f_ex;
    else {
        f_ex2 = (1023 + 63 - 11) - f_shift_left_until_bit63_set(&w2);
        w2    = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(&w2, 0xb17217f7d1cf79ac, 0);
        if (f_ex2 >= f_ex)
            (f_ex2 - f_ex >= 64) ? (w = 0) : (w >>= (f_ex2 - f_ex));
        else
            w <<= (f_ex - f_ex2);
        if (sgnw2 >= 0)
            w += w2;
        else
            w = w2 - w;
    }

    f_combi_from_fixpoint(&x, sgnw2 < 0 ? 1 : 0, f_ex2, &w);
    return x;
}
#endif

#if defined(F_WITH_sin) || defined(F_WITH_cos) || defined(F_WITH_tan)
float64_t f_berechne_sinus_cosinus_tangens(float64_t x, uint8_t fkt_nr)
{ // fkt_nr=0 ==> Sinus, fkt_nr=1 ==> Cosinus, fkt_nr=2 ==> Tangens
    uint8_t f_sign;
    int16_t f_ex;
    uint64_t w, w2;
    uint8_t neg, tneg = 0, mir45 = (1 == fkt_nr) ? 1 : 0;
    static uint64_t interp[] FLASHMEM_IF_AVR = {
        // Interpolation von sin(sqrt(x))/sqrt(x) im Bereich von x=0 ... (Pi/4)*(Pi/4)
        // Zähler: 7 Koeffizienten (vorzeichenbehaftet), Nenner: 0 Koeffizienten
        0x000000002bb08115, 0xffffffe51a1d4a39, 0x00000b8ef1a8f972, 0xfffcbfcbfcc8da4a,
        0x0088888888879853, 0xf555555555555eb1, 0x3ffffffffffffff0
    };

    f_split64(&x, &f_sign, &f_ex, &w, 0);

    if (0 == f_ex) // Null
        return 1 == fkt_nr ? float64_NUMBER_ONE : float64_NUMBER_PLUS_ZERO;
    if (2047 == f_ex) // NaN, +INF oder -INF
        return float64_ONE_POSSIBLE_NAN_REPRESENTATION;

    tneg = neg = (0 != f_sign && 1 != fkt_nr) ? 1 : 0;
    w2         = 0xc90fdaa22168c235;                          // Mantisse von 2*PI
    x          = f_mod_intern(f_abs(x), 0, 1023 + 2, &w2, 0); // Modulo 2*PI
    // Von f_mod_intern() wurde an w2 nun die 64-Bit-Mantisse von x zugewiesen.
    f_split_to_fixpoint(&x, &f_sign, &f_ex, &w,
                        61); // &w, nicht &w2 übergeben! w2 hat 64 Bits, w nur 53 Bits Mantisse

    w = w2; // bleibt im Fall f_ex<1022 unten erhalten
    if (1025 - 64 >= f_ex)
        w2 = 0;
    else
        w2 >>= 1025 - f_ex;

    if (w2 >= 0x6487ed5110b4611a) // x>=PI
    {
        neg ^= 1;
        w2 -= 0x6487ed5110b4611a; // x -= PI
    }

    if (w2 >= 0x3243f6a8885a308d) // x>=PI/2
    {
        tneg ^= 1;
        if (1 == fkt_nr)
            neg ^= 1;
        w2 = 0x6487ed5110b4611a - w2; // x = PI-x
    }

    if (w2 >= 0x1921fb54442d1846) // x>=PI/4
    {
        mir45 ^= 1;
        w2 = 0x3243f6a8885a308d - w2; // x = PI/2-x
    }

    w2 <<= 3;
    if (f_ex >= 1022) {
        w    = w2;
        f_ex = 0;
    } else
        f_ex -= 1022;

    f_ex -= f_shift_left_until_bit63_set(&w); // Im Fall w=0 ist f_ex im Folgenden irrelevant

#ifdef USE_AVR
    copy_from_flash_to_tempmem(&interp[0], 7 * 8);
    w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
        &w,
        f_eval_function_by_rational_approximation_fixpoint(
            approx_high_uint64_word_of_uint64_mult_uint64(&w2, &w2, 0), 7, 0,
            (uint64_t *)(&TemporaryMemory[0]), 1),
        0);
#else
    w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
        &w,
        f_eval_function_by_rational_approximation_fixpoint(
            approx_high_uint64_word_of_uint64_mult_uint64(&w2, &w2, 0), 7, 0, interp, 1),
        0);
#endif
    if (0 != (0x8000000000000000 & w)) // negativ durch Rundungsfehler
        w = 0;
    // Durch die vorangehende Abfrage ist sichergestellt, dass durch Rundungsfehler kein negatives w
    // geliefert wird. Das maximale w an dieser Stelle entspricht der reellen Zahl 1/sqrt(2) als
    // Funktionswert von sin(Pi/4).

    if (mir45 || 2 == fkt_nr) {
        w2 = w;
        if (f_ex <= -64)
            w = 0;
        else
            w >>= (-f_ex);
        w = rounded_sqrt_of_integer128(
            0xffffffffffffffff - (approx_high_uint64_word_of_uint64_mult_uint64(&w, &w, 0) << 4),
            0xffffffffffffffff);
        // Hier darf nicht (0xffffffffffffffff+1) statt 0xffffffffffffffff stehen
        // (Integer-Überlauf), weil die zu subtrahierende Zahl
        // (approx_high_uint64_word_of_uint64_mult_uint64(&w, &w, 0)<<4)) auch Null sein kann. Es
        // ist sichergestellt, dass nicht durch Rundungsfehler ein w > 2 hoch 32 geliefert wird,
        // welches einem Funktionswert von sin bzw. cos von betraglich größer als 1 entsprechen
        // würde.
        if (2 == fkt_nr) {
            uint64_t *ps, *pc;
            f_ex += 2;
            if (mir45) {
                ps   = &w;
                pc   = &w2;
                f_ex = -f_ex;
            } else {
                ps = &w2;
                pc = &w;
            }
            if (0 == *pc) // z.B. bei tan(PI/2)
                return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
            f_ex -= f_shift_left_until_bit63_set(ps);
            f_ex += f_shift_left_until_bit63_set(pc);
            w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
                ps, approx_inverse_of_fixpoint_uint64(pc), 0);
            neg = tneg;
        } else
            f_ex = -2;
    }

    f_combi_from_fixpoint(&x, neg, 1013 + f_ex, &w);
    return x;
}
#endif

#if defined(F_WITH_arcsin) || defined(F_WITH_arccos) || defined(F_WITH_arctan)
float64_t f_berechne_arcsin_arccos_arctan(float64_t x, uint8_t fkt_nr)
{
    uint8_t f_sign, mir45 = 0, nr_shift;
    int16_t f_ex;
    uint64_t w, w2;
    static uint64_t interp[] FLASHMEM_IF_AVR = {
        // Interpolation von arctan(sqrt(x))/sqrt(x) im Bereich von x=0 ... 1
        // Zähler 7 Koeffizienten (alle positiv), Nenner 7 Koeffizienten (alle positiv)
        0x000051f590de71e7, 0x00252120c9d0cdc1, 0x0277897981ffc250, 0x0e45a25bb3bfe210,
        0x2360ae1e927608ad, 0x26d3fc5ec2d01988, 0x0f7f3182c822dbc8, 0x000347839b4bdad9,
        0x0082e7b5e08e6b4d, 0x056b67d2062eb49d, 0x1755b06e64aa19e5, 0x2ef15e8d8e6c5b98,
        0x2bfe6235058656ae, 0x0f7f3182c822dbce
    };

    f_split64(&x, &f_sign, &f_ex, &w, 11);

    if (0 == f_ex) // Null
        return 1 == fkt_nr ? ((float64_t)0x3ff921fb54442d18) : 0;
#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == f_ex)
        return x; // NaN
#else
    if (2047 == f_ex) // NaN, +INF oder -INF
    {
        if (0 != w) // NaN
            return x;
        if (f_sign) // -INF
            return 2 == fkt_nr ? ((float64_t)0xbff921fb54442d18)
                               : float64_ONE_POSSIBLE_NAN_REPRESENTATION;
        else
            return 2 == fkt_nr ? ((float64_t)0x3ff921fb54442d18)
                               : float64_ONE_POSSIBLE_NAN_REPRESENTATION;
    }
#endif

    if (2 != fkt_nr) {
        if (f_ex >= 1023 ||
            (f_ex == 1022 &&
             w >= 0xb504f333f9de6484 - 0x0000000100000000)) // fabs(x) >= sqrt(2)/2 - (2 hoch (-32))
        { // Es ist wichtig, dass auf w>=0xb504f333f9de6484-0x0000000100000000 statt auf
          // w>=0xb504f333f9de6484 getestet wird, damit w2 im else-Teil vor Ausführung von
          // w2=approx_inverse_of_fixpoint_uint64(&w2) mindestens gleich 0x8000000000000000 ist (wie
          // von approx_inverse_of_fixpoint_uint64() gefordert).
            if (f_ex > 1023 || (f_ex == 1023 && w > 0x8000000000000000))
                return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
            if (f_ex == 1023) // fabs(x)=1 . Dieser Fall würde im else-Code nicht korrekt behandelt.
            {
                w    = 0;
                f_ex = 1021;
            } else {
                // Die folgende Rechnung entspricht y=sqrt(2*(1-x) - (1-x)*(1-x)) / x .
                // Theoretisch (wenn ohne Rundungsfehler) liefert y=sqrt(1-x*x) / x das gleiche
                // Ergebnis; aber wenn x nahe bei 1 ist, muss zur Verringerung von Rundungsfehlern
                // die erste Rechnung benutzt werden.
                w2 = (0xffffffffffffffff + 1) - w; // 0xffffffffffffffff+1 ergibt gewollt 0
                                                   // (warning: integral constant overflow).
                for (nr_shift = 0; 0 == (0x6000000000000000 & w2); ++nr_shift)
                    w2 <<= 2;
                w = approx_inverse_of_fixpoint_uint64(&w);
                w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
                    &w,
                    rounded_sqrt_of_integer128(
                        (w2 << 1) - (approx_high_uint64_word_of_uint64_mult_uint64(&w2, &w2, 0) >>
                                     (2 * nr_shift)),
                        0),
                    0);
                f_ex = 1024 - nr_shift;
            }
            mir45 = (0 == fkt_nr) ? 1 : 0;
        } else // fabs(x) < sqrt(2)/2 - (2 hoch (-32))
        {
            f_split_to_fixpoint(&x, &f_sign, &f_ex, &w2, 64);
            w2 = 0xffffffffffffffff - approx_high_uint64_word_of_uint64_mult_uint64(&w2, &w2, 0);
            // Obwohl w2=0 oben abgefangen wird, kann
            // approx_high_uint64_word_of_uint64_mult_uint64(&w2,&w2,0)==0 sein. Daher darf dieses
            // Produkt nicht von (0xffffffffffffffff+1) abgezogen werden (Integer-Überlauf).
            // Wegen fabs(x) < sqrt(2)/2 - (2 hoch (-32)) ist an dieser Stelle
            // w2>=0x8000000000000000
            w2 = approx_inverse_of_fixpoint_uint64(&w2);
            w2 = rounded_sqrt_of_integer128(w2, 0);
            w  = approx_high_uint64_word_of_uint64_mult_uint64(
                &w, &w2, 0); // Das höchstwertigste Bit kann gesetzt sein !
            ++f_ex;
            mir45 = (1 == fkt_nr) ? 1 : 0;
        }
    }

    if (0 != w) // Wenn fkt_nr!=2 und x=+1.0 oder x=-1.0 ist, wird oben w=0 gesetzt.
        f_ex -= f_shift_left_until_bit63_set(&w);
    --f_ex;

    if (f_ex >= 1022) {
        w     = approx_inverse_of_fixpoint_uint64(&w);
        f_ex  = 2044 - f_ex - f_shift_left_until_bit63_set(&w);
        mir45 = 1;
    }

    // Im Folgenden muss der Fall 1022==f_ex gesondert behandelt werden, weil der dem Wert 1
    // entsprechende Wert nicht an f_eval_function_by_rational_approximation_fixpoint() übergeben
    // werden kann. Außerdem würden sich sonst negative Anzahl an Shifts ergeben ( w>>(1021-f_ex) ).
    // Der Fall 1022==f_ex tritt bei arctan(1) und arctan(-1) und bei bestimmten Werten von arccos
    // und arcsin auf.
    if (1022 == f_ex)
        w = 0x1921fb54442d1847; // ergibt x=Pi/4 oder x=-Pi/4
    else {
#ifdef USE_AVR
        copy_from_flash_to_tempmem(&interp[0], 14 * 8);
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
            &w,
            f_eval_function_by_rational_approximation_fixpoint(
                f_ex <= 1021 - 32 ? 0
                                  : (approx_high_uint64_word_of_uint64_mult_uint64(&w, &w, 0) >>
                                     (2 * (1021 - f_ex))),
                7, 7, (uint64_t *)(&TemporaryMemory[0]), 0),
            0);
#else
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(
            &w,
            f_eval_function_by_rational_approximation_fixpoint(
                f_ex <= 1021 - 32 ? 0
                                  : (approx_high_uint64_word_of_uint64_mult_uint64(&w, &w, 0) >>
                                     (2 * (1021 - f_ex))),
                7, 7, interp, 0),
            0);
#endif
        if (mir45) {
            w = 0x6487ed5110b4611a -
                (f_ex <= 1021 - 64 ? 0 : (w >> (1021 - f_ex))); // entspricht x = PI/2 - x
            f_ex = 1021;
        }
        if (1 == fkt_nr && f_sign) {
            w = 0xc90fdaa22168c235 -
                (f_ex <= 1021 - 64 ? 0 : (w >> (1021 - f_ex))); // entspricht x = PI - x
            f_ex   = 1021;
            f_sign = 0;
        }
    }

    f_combi_from_fixpoint(&x, f_sign, f_ex - 8, &w);

    return x;
}
#endif

#if defined(F_WITH_to_decimalExp) || defined(F_WITH_to_string) || defined(F_WITH_strtod) || \
    defined(F_WITH_atof)
static int16_t f_10HochN(int64_t n, uint64_t *res)
{
    uint64_t pot = ((uint64_t)10) << 60;
    int16_t exp2 = 0, pot_exp2 = 3;
    uint8_t neg = 0;
    *res        = ((uint64_t)1) << 63;
    if (n < 0) {
        neg = 1;
        n   = -n;
    }
    while (0 != n) {
        if (0 != (n & 1)) {
            *res = approx_high_uint64_word_of_uint64_mult_uint64(res, &pot, 0);
            exp2 += pot_exp2 + 1 - f_shift_left_until_bit63_set(res);
        }
        pot      = approx_high_uint64_word_of_uint64_mult_uint64(&pot, &pot, 0);
        pot_exp2 = (pot_exp2 << 1) + 1 - f_shift_left_until_bit63_set(&pot);
        n >>= 1;
    }
    if (neg) {
        *res = approx_inverse_of_fixpoint_uint64(res);
        exp2 = -exp2 - f_shift_left_until_bit63_set(res);
    }
    return exp2;
}
#endif

#if defined(F_WITH_to_decimalExp) || defined(F_WITH_to_string)
char *f_to_decimalExp(float64_t x, uint8_t anz_dezimal_mantisse,
                      uint8_t MantisseUndExponentGetrennt, int16_t *ExponentBasis10)
{ // f_to_decimalExp() converts the float64 to the decimal representation of the number x if x is
    // a real number or to the strings "+INF", "-INF", "NaN". If x is real, f_to_decimalExp()
    // generates a mantisse-exponent decimal representation of x using anz_dezimal_mantisse decimal
    // digits for the mantisse. If MantisseUndExponentGetrennt!=0 is passed f_to_decimalExp() will
    // generate different strings for the mantisse and the exponent. If you assign
    //      char *str=f_to_decimalExp(x, anz_mts, 1, 0)
    // then str points to the mantisse string and str+strlen(str) points to the exponent string.
    // If the pointer ExponentBasis10 passed to f_to_decimalExp() is nonzero, the function will
    // assign the 10-exponent to *ExponentBasis10 ; e.g. if the decimal representation of x
    // is 1.234E58 then the integer 58 is assigned to *ExponentBasis10.

    // *** ATTENTION *** : The functions f_to_decimalExp() and f_to_string() return pointers to
    // static memory containing the decimal representation of the float64 passed to these functions.
    // The string contained in this memory will become invalid if one of the functions
    // f_to_decimalExp(), f_to_string(), f_exp(), f_log(), f_sin(), f_cos(), f_tan(), f_arcsin(),
    // f_arccos(), f_arctan() is called as these functions will overwrite the memory.

    uint8_t f_sign;
    uint8_t len, posm, i;
    int16_t f_ex;
    uint64_t w, w2;
    int16_t Exp10;

    if (anz_dezimal_mantisse > 17)
        anz_dezimal_mantisse = 17;
    if (anz_dezimal_mantisse < 1)
        anz_dezimal_mantisse = 1;
    f_split64(&x, &f_sign, &f_ex, &w, 11);
    if (0 == f_ex) // Alle denormalisierten Zahlen werden als Null interpretiert.
    {
        TemporaryMemory[0] = '0';
        TemporaryMemory[1] = 0;
        return TemporaryMemory;
    }
#ifdef F_ONLY_NAN_NO_INFINITY
    if (2047 == f_ex) {
        strcpy(TemporaryMemory, "NaN");
        return TemporaryMemory;
    }
#else
    if (2047 == f_ex) {
        if (0 != w) {
            strcpy(TemporaryMemory, "NaN");
            return TemporaryMemory;
        }
        TemporaryMemory[0] = f_sign ? '-' : '+';
        strcpy(TemporaryMemory + 1, "INF");
        return TemporaryMemory;
    }
#endif
    f_ex -= 1023; // Nach der Abfrage auf 0==f_ex und 2047==f_ex !
    len = 0;
    if (f_sign)
        TemporaryMemory[len++] = '-';

    if (f_ex >= 0)
        Exp10 = (uint16_t)((((uint16_t)f_ex) * 10 + 31) >> 5);
    else
        Exp10 = (int16_t)(-((((uint16_t)(-f_ex)) * 9) >> 5));

    f_ex += f_10HochN(-Exp10, &w2);
    w = approx_high_uint64_word_of_uint64_mult_uint64(&w, &w2, 0);
    f_ex += 1 - f_shift_left_until_bit63_set(&w);

    while (f_ex < 0) {
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(&w, ((uint64_t)10) << 60, 0);
        f_ex += 4 - f_shift_left_until_bit63_set(&w);
        Exp10--;
    }
    while (f_ex >= 4 || (f_ex == 3 && (w & 0xf000000000000000) >= 0xa000000000000000)) {
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(&w, 0xcccccccccccccccc, 0);
        f_ex -= 3 + f_shift_left_until_bit63_set(&w);
        Exp10++;
    }
    posm = len;
    ++len; // Platz für .
    while (0 != (anz_dezimal_mantisse--)) {
        TemporaryMemory[len] = '0';
        if (f_ex >= 0) {
            TemporaryMemory[len] += (w >> (63 - f_ex));
            w <<= 1 + f_ex;
            f_ex = -1;
        }
        ++len;
        w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(&w, ((uint64_t)10) << 60, 0);
        f_ex += 4 - f_shift_left_until_bit63_set(&w);
    }
    if (f_ex >= 0 && (w >> (63 - f_ex)) >= 5) {
        for (i = len; --i > posm;)
            if (TemporaryMemory[i] == '9')
                TemporaryMemory[i] = '0';
            else {
                ++TemporaryMemory[i];
                break;
            }
        if (i == posm) {
            ++Exp10;
            TemporaryMemory[++i] = '1';
            while (++i < len)
                TemporaryMemory[i] = '0';
        }
    }
    TemporaryMemory[posm]     = TemporaryMemory[posm + 1];
    TemporaryMemory[posm + 1] = '.';
    if (MantisseUndExponentGetrennt)
        TemporaryMemory[len++] = 0;
    TemporaryMemory[len++] = 'E';
    if (Exp10 > 0)
        TemporaryMemory[len++] = '+';
    itoa(Exp10, &TemporaryMemory[len], 10);
    if (0 != ExponentBasis10)
        *ExponentBasis10 = Exp10;
    return TemporaryMemory;
}
#endif

#ifdef F_WITH_to_string
char *f_to_string(float64_t x, uint8_t max_nr_chars, uint8_t max_leading_mantisse_zeros)
{ // f_to_decimalExp() converts the float64 to the decimal representation of the number x if x is
    // a real number or to the strings "+INF", "-INF", "NaN". If x is real, f_to_decimalExp()
    // generates a decimal representation without or with mantisse-exponent representation depending
    // on what in more suitable or possible. If -1 < x < 1 the non-exponent representation is chosen
    // if there are less than max_leading_mantisse_zeros zeros after the decimal point. In most
    // cases f_to_string() will generate a string with maximal 'max_nr_chars' chars. If necessary,
    // f_to_string() reduces the number of decimal digits in the mantisse in order to get a maximum
    // string width of 'max_nr_chars' chars. If however max_nr_chars is so small that even a
    // mantisse of one digit and the corresponding exponent doesn't fit into 'max_nr_chars' chars,
    // the string returned will be longer than 'max_nr_chars' chars.

    // *** ATTENTION *** : The functions f_to_decimalExp() and f_to_string() return pointers to
    // static memory containing the decimal representation of the float64 passed to these functions.
    // The string contained in this memory will become invalid if one of the functions
    // f_to_decimalExp(), f_to_string(), f_exp(), f_log(), f_sin(), f_cos(), f_tan(), f_arcsin(),
    // f_arccos(), f_arctan() is called as these functions will overwrite the memory.

    int16_t exp10;
    int8_t nrd     = (0 != (x & 0x8000000000000000))
                         ? (max_nr_chars - 1)
                         : max_nr_chars; // nrd: Zahl der zu berechnenden dezimalen Mantissestellen.
    int8_t nrd_vor = nrd + 1;
    char *r        = 0;
    uint8_t j, k;
    for (j = 0; j < 3 && nrd != nrd_vor; j++) {
        if (nrd > nrd_vor) {
            r = f_to_decimalExp(x, nrd_vor, 1, &exp10); // Nur exp10 berechnen
            break;
        }
        r = f_to_decimalExp(x, nrd, 1, &exp10); // Nur exp10 berechnen
        if (((x >> 52) & 2047) == 2047 || 0 == (x & 0x7ff0000000000000))
            return r;
        if (exp10 < -max_leading_mantisse_zeros - 1)
            break;
        nrd_vor = nrd;

        nrd = f_getsign(x) ? max_nr_chars - 1 : max_nr_chars;
        // Diese Variable nrd muss sowohl hier als oben vor Beginn der Schleife initialisiert
        // werden.
        if (exp10 < 0)
            nrd += exp10 - 1;     // exp10-1 Zeichen durch 0.00 .... verbraucht.
        else if (exp10 + 1 < nrd) // && exp10>=0 .
            --nrd;                // Ein Zeichen durch Dezimalpunkt verbraucht.
                   // Beispiel für den Fall exp10==max_nr_chars-2 : Es sei max_nr_chars=4, exp10=2,
                   // Zahl=683.79426
                   // ==> Es wird 684 dargestellt
                   // (3 statt 4 Zeichen belegt, weil 683.8 zu viele Zeichen belegen würde).
    }

    if (0 != (x & 0x7ff0000000000000) && exp10 >= -max_leading_mantisse_zeros &&
        nrd > exp10) { // Darstellung ohne 10er-Exponent (E)
        if (f_getsign(x))
            ++r; // - Zeichen
        if (exp10 < 0) {
            for (j = strlen(r); (j--) > 0 && '0' == r[j];)
                r[j] = 0;
            r[1] = r[0];
            for (++j; j > 0; j--)
                r[j - exp10] = r[j];
            r[0] = '0';
            r[1] = '.';
            for (j = 2; ++exp10 < 0;)
                r[j++] = '0';
        } else {
            for (j = 1; j <= exp10; j++)
                if (0 == (r[j] = r[j + 1])) {
                    while (j <= exp10)
                        r[j++] = '0';
                    r[j] = 0;
                    break;
                }
            if (j + 1 < max_nr_chars && 0 != r[j]) {
                r[j] = '.';
                for (j = strlen(r); (j--) > 0 && '0' == r[j];)
                    r[j] = 0;
                if ('.' == r[j])
                    r[j] = 0;
            } else
                r[j] = 0; // Notwendig für den Fall j+1>=max_nr_chars
        }
        if (f_getsign(x))
            --r;
    } else { // Darstellung mit 10er-Exponent (E)
        j = max_nr_chars - 4;
        if (f_getsign(x))
            --j;
        exp10 =
            (x >> 52) & 2047; // exp10 wird hier zur Aufnahme des binären Exponenten "missbraucht"
        if (exp10 < 1023)
            --j;
        if (exp10 > 1023 + 34 || exp10 < 1023 - 34)
            --j;
        if (j < 1)
            j = 1;
        while ((r = f_to_decimalExp(x, j, 0, 0)), strlen(r) > (uint8_t)max_nr_chars)
            if (--j < 1)
                break;
        for (j = 2; 0 != r[j] && 'E' != r[j] && 'e' != r[j]; j++)
            ;
        k = j;
        while ((--j >= 4 || (!f_getsign(x) && j >= 3)) && '0' == r[j])
            ;
        while (0 != (r[++j] = r[k++]))
            ;
    }
    return r;
}
#endif

#if defined(F_WITH_strtod) || defined(F_WITH_atof)
float64_t f_strtod(char *str, char **endptr)
{
    char *s        = str;
    uint8_t f_sign = 0, point = 0;
    int16_t f_ex;
    uint64_t w, w2;
    int16_t Exp10;
    float64_t x;
    while (*s && (*s == ' ' || *s == '\t'))
        s++;

    if ((s[0] == 'n' || s[0] == 'N') && (s[1] == 'a' || s[1] == 'A') &&
        (s[2] == 'n' || s[2] == 'N')) {
        s += 3;
        goto return_NaN;
    }

    if (*s == '-') {
        f_sign = 1;
        ++s;
    } else if (*s == '+')
        ++s;

#ifndef F_ONLY_NAN_NO_INFINITY
    if ((s[0] == 'i' || s[0] == 'I') && (s[1] == 'n' || s[1] == 'N') &&
        (s[2] == 'f' || s[2] == 'F')) {
        if (endptr)
            *endptr = s + 3;
        return f_sign ? float64_MINUS_INFINITY : float64_PLUS_INFINITY;
    }
#endif

    Exp10 = 0;
    w     = 0;
    f_ex  = 3;
    while (1) {
        if (*s == '.') {
            ++s;
            if (point)
                goto return_NaN;
            point = 1;
        }
        if (!(*s >= '0' && *s <= '9'))
            break;
        if (f_ex <= 63 - 4) {
            if (point)
                --Exp10;
            w = approx_high_uint64_word_of_uint64_mult_uint64_pbv_y(&w, ((uint64_t)10) << 60, 0);
            if (w)
                f_ex += 4;
            w += (((uint64_t)(*s)) - '0') << (63 - f_ex);
            if (w)
                f_ex -= f_shift_left_until_bit63_set(&w);
        } else if (!point)
            ++Exp10;
        ++s;
    }
    if (*s == 'E' || *s == 'e') {
        Exp10 += atoi(++s);
        if (*s == '-' || *s == '+')
            ++s;
        while (*s >= '0' && *s <= '9')
            ++s;
    }

    f_ex += 1 + f_10HochN(Exp10, &w2);
    w = approx_high_uint64_word_of_uint64_mult_uint64(&w, &w2, 0);

    f_combi_from_fixpoint(&x, f_sign, 1023 - 11 + f_ex, &w);
    if (endptr)
        *endptr = s;
    return x;

return_NaN:
    if (endptr)
        *endptr = s;
    return float64_ONE_POSSIBLE_NAN_REPRESENTATION;
}
#endif
