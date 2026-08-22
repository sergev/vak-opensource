/*
 * divbug.c -- demonstrates the bug in Algorithm D of Knuth's "The Art of
 * Computer Programming", Vol. 2, section 4.3.1 (3rd edition, printings 1..52).
 *
 * Step D3 corrects the trial quotient q^ at most twice, then D4 multiplies it
 * back in as "a one-place number", i.e. uses only q^ mod b.  Theorem B allows
 * q^ = q+3, so q^ can reach b+2; two decrements leave b, still two limbs, whose
 * low limb is 0.  D4 then subtracts nothing and the quotient digit comes out 0.
 *
 * Build:  cc -O2 -std=c99 -Wall -Wextra -o divbug divbug.c
 * Run:    ./divbug [odd base b, default 3]
 *
 * See README.md and A-long-division-story.md.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint64_t limb;          /* one limb: a value < b */

#define MAXL 32                 /* max limbs in any operand */

/* Limbs are stored little-endian: x[0] is least significant.  The base b is
   passed explicitly and is at most 2^31, so b*b still fits in a uint64_t. */

/* ------------------------------------------------------------------ */
/* multiprecision helpers                                             */
/* ------------------------------------------------------------------ */

static int mp_cmp(const limb *x, const limb *y, int n)
{
    for (int i = n - 1; i >= 0; i--)
        if (x[i] != y[i])
            return x[i] < y[i] ? -1 : 1;
    return 0;
}

/* dst[0..n] = x[0..n-1] * m */
static void mp_mul_small(limb *dst, const limb *x, int n, uint64_t m, uint64_t b)
{
    uint64_t carry = 0;
    for (int i = 0; i < n; i++) {
        uint64_t p = x[i] * m + carry;
        dst[i] = p % b;
        carry = p / b;
    }
    dst[n] = carry;
}

/* value of x as a plain integer; 0 if it does not fit in unsigned long long */
static int mp_value(const limb *x, int n, uint64_t b, unsigned long long *out)
{
    unsigned long long v = 0;
    for (int i = n - 1; i >= 0; i--) {
        if (v > (~0ULL - x[i]) / b)
            return 0;
        v = v * b + x[i];
    }
    *out = v;
    return 1;
}

static void mp_print(const char *name, const limb *x, int n, uint64_t b)
{
    unsigned long long val;
    printf("  %-4s = (", name);
    for (int i = n - 1; i >= 0; i--)
        printf("%s%llu", i == n - 1 ? "" : ",", (unsigned long long)x[i]);
    printf(")_%llu", (unsigned long long)b);
    if (mp_value(x, n, b, &val))
        printf(" = %llu", val);
    printf("\n");
}

/* u[j..j+n] -= q * v[0..n-1];  returns 1 if the result went negative */
static int mul_sub(limb *u, int j, const limb *v, int n, uint64_t q, uint64_t b)
{
    uint64_t borrow = 0, carry = 0;
    for (int i = 0; i <= n; i++) {
        uint64_t d;
        if (i < n) {
            uint64_t p = q * v[i] + carry;
            carry = p / b;
            d = p % b;
        } else {
            d = carry;
        }
        uint64_t s = u[j + i];
        if (s < d + borrow) {
            u[j + i] = s + b - d - borrow;
            borrow = 1;
        } else {
            u[j + i] = s - d - borrow;
            borrow = 0;
        }
    }
    return (int)borrow;
}

/* u[j..j+n] += v[0..n-1];  the final carry cancels the borrow, so drop it */
static void add_back(limb *u, int j, const limb *v, int n, uint64_t b)
{
    uint64_t carry = 0;
    for (int i = 0; i < n; i++) {
        uint64_t s = u[j + i] + v[i] + carry;
        u[j + i] = s % b;
        carry = s / b;
    }
    u[j + n] = (u[j + n] + carry) % b;
}

/* ------------------------------------------------------------------ */
/* correctness certificate: q*v + r == u  and  r < v                  */
/* ------------------------------------------------------------------ */

#define BAD_IDENTITY 1          /* q*v + r != u          */
#define BAD_REMAINDER 2         /* r >= v                */

/* returns 0 if correct, else a mask of the above; acc gets q*v + r */
static int verify(const limb *u, int ulen, const limb *v, int n,
                  const limb *q, int qlen, const limb *r, uint64_t b, limb *acc)
{
    int alen = qlen + n + 1;
    int bad = 0;

    memset(acc, 0, (size_t)(2 * MAXL + 2) * sizeof *acc);
    for (int i = 0; i < qlen; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < n; j++) {
            uint64_t t = acc[i + j] + q[i] * v[j] + carry;
            acc[i + j] = t % b;
            carry = t / b;
        }
        for (int k = i + n; carry; k++) {
            uint64_t t = acc[k] + carry;
            acc[k] = t % b;
            carry = t / b;
        }
    }
    uint64_t carry = 0;
    for (int j = 0; j < n; j++) {
        uint64_t t = acc[j] + r[j] + carry;
        acc[j] = t % b;
        carry = t / b;
    }
    for (int k = n; carry; k++) {
        uint64_t t = acc[k] + carry;
        acc[k] = t % b;
        carry = t / b;
    }

    for (int i = 0; i < alen; i++)
        if (acc[i] != (i < ulen ? u[i] : 0))
            bad |= BAD_IDENTITY;
    if (mp_cmp(r, v, n) >= 0)
        bad |= BAD_REMAINDER;
    return bad;
}

static int check(const limb *u, int ulen, const limb *v, int n,
                 const limb *q, int qlen, const limb *r, uint64_t b)
{
    limb acc[2 * MAXL + 2];
    return verify(u, ulen, v, n, q, qlen, r, b, acc) == 0;
}

/* ------------------------------------------------------------------ */
/* ground truth: schoolbook division, quotient digit by binary search */
/* ------------------------------------------------------------------ */

static void ref_div(const limb *u, int ulen, const limb *v, int n, uint64_t b,
                    limb *q, limb *r)
{
    limb rem[MAXL + 2], prod[MAXL + 2];

    memset(rem, 0, sizeof rem);
    memset(q, 0, (size_t)ulen * sizeof *q);

    for (int i = ulen - 1; i >= 0; i--) {
        for (int k = n; k > 0; k--)         /* rem = rem*b + u[i] */
            rem[k] = rem[k - 1];
        rem[0] = u[i];

        uint64_t lo = 0, hi = b - 1, d = 0;
        while (lo <= hi) {                  /* largest d with d*v <= rem */
            uint64_t mid = lo + (hi - lo) / 2;
            mp_mul_small(prod, v, n, mid, b);
            if (mp_cmp(prod, rem, n + 1) <= 0) {
                d = mid;
                lo = mid + 1;
            } else {
                if (mid == 0)
                    break;
                hi = mid - 1;
            }
        }
        q[i] = d;

        mp_mul_small(prod, v, n, d, b);
        uint64_t borrow = 0;
        for (int k = 0; k <= n; k++) {
            uint64_t s = rem[k], t = prod[k] + borrow;
            if (s < t) {
                rem[k] = s + b - t;
                borrow = 1;
            } else {
                rem[k] = s - t;
                borrow = 0;
            }
        }
    }
    memcpy(r, rem, (size_t)n * sizeof *r);
}

/* ------------------------------------------------------------------ */
/* Algorithm 4.3.1D                                                   */
/* ------------------------------------------------------------------ */

#define AS_PRINTED 0    /* D3's test runs at most twice, as the book words it */
#define AS_CODED   1    /* D3's test is a while loop, as everyone implements it */

static void algo_d(const limb *u_in, int ulen, const limb *v_in, int n, uint64_t b,
                   int variant, limb *q, limb *r, int trace)
{
    limb u[MAXL + 2], v[MAXL + 2];
    int m = ulen - n - 1;

    /* D1. Normalize. */
    uint64_t d = b / (v_in[n - 1] + 1);
    memset(u, 0, sizeof u);
    mp_mul_small(u, u_in, ulen, d, b);
    mp_mul_small(v, v_in, n, d, b);
    if (u[ulen] != 0 || v[n] != 0) {        /* cannot happen: top n limbs of u < v */
        fprintf(stderr, "internal error: normalisation overflowed\n");
        exit(1);
    }
    memset(q, 0, (size_t)ulen * sizeof *q);

    /* D2/D7. Loop on j. */
    for (int j = m; j >= 0; j--) {
        /* D3. Calculate q^. */
        uint64_t num = u[j + n] * b + u[j + n - 1];
        uint64_t qhat = num / v[n - 1];
        uint64_t rhat = num % v[n - 1];
        int corr = 0;

        if (trace)
            printf("    D3: q^ = %llu, r^ = %llu\n",
                   (unsigned long long)qhat, (unsigned long long)rhat);
        for (;;) {
            if (!(qhat >= b || qhat * v[n - 2] > b * rhat + u[j + n - 2]))
                break;
            qhat--;
            rhat += v[n - 1];
            corr++;
            if (trace)
                printf("        correction %d -> q^ = %llu, r^ = %llu\n",
                       corr, (unsigned long long)qhat, (unsigned long long)rhat);
            if (rhat >= b)
                break;
            if (variant == AS_PRINTED && corr == 2) {
                if (trace)
                    printf("        (as printed: the test is not repeated a third time)\n");
                break;
            }
        }

        /* D4. Multiply and subtract -- by "a one-place number", so only the
           low limb of q^ takes part.  This is where the bug bites. */
        uint64_t qlow = qhat % b;
        if (trace && qhat >= b)
            printf("        q^ = %llu = (%llu,%llu)_%llu does not fit in one limb;"
                   " D4 multiplies by its low limb %llu\n",
                   (unsigned long long)qhat, (unsigned long long)(qhat / b),
                   (unsigned long long)qlow, (unsigned long long)b,
                   (unsigned long long)qlow);
        int borrow = mul_sub(u, j, v, n, qlow, b);

        /* D5. Test remainder.  D6. Add back. */
        q[j] = qlow;
        if (borrow) {
            q[j] = qlow - 1;
            add_back(u, j, v, n, b);
        }
    }

    /* D8. Unnormalize. */
    uint64_t rest = 0;
    for (int i = n - 1; i >= 0; i--) {
        uint64_t cur = rest * b + u[i];
        r[i] = cur / d;
        rest = cur % d;
    }
}

/* ------------------------------------------------------------------ */
/* the counterexample                                                 */
/* ------------------------------------------------------------------ */

/*
 * For any odd b >= 3, with t = (b-1)/2:
 *
 *     u = (t, b-1, 0, 0)_b      v = (t, b-1, 1)_b
 *     q = b-1                   r = (t+1)b^2 - 2b + 1
 *
 * v is already normalised (v_{n-1} = t = floor(b/2)) and the top three limbs of
 * u are exactly v-1, so the quotient is a single limb.  The trial quotient is
 * q^ = floor((t*b + b-1)/t) = b+2, the largest Theorem B allows.
 */
static void counterexample(uint64_t b, limb *u, limb *v)
{
    uint64_t t = (b - 1) / 2;
    u[3] = t; u[2] = b - 1; u[1] = 0; u[0] = 0;
    v[2] = t; v[1] = b - 1; v[0] = 1;
}

static void report(const limb *u, int ulen, const limb *v, int n,
                   const limb *q, const limb *r, uint64_t b)
{
    limb acc[2 * MAXL + 2];
    unsigned long long qv, rv, av, uv, vv;
    int bad = verify(u, ulen, v, n, q, ulen, r, b, acc);

    mp_value(q, ulen, b, &qv);
    mp_value(r, n, b, &rv);
    printf("    -> q = %llu, r = %llu    %s\n",
           qv, rv, bad ? "*** WRONG ***" : "correct");
    if (bad & BAD_IDENTITY) {
        mp_value(acc, ulen + n + 1, b, &av);
        mp_value(u, ulen, b, &uv);
        printf("       q*v + r = %llu, but u = %llu\n", av, uv);
    }
    if (bad & BAD_REMAINDER) {
        mp_value(v, n, b, &vv);
        printf("       remainder %llu is not less than v = %llu\n", rv, vv);
    }
}

/* does the counterexample for this base break the given variant? */
static int fails(uint64_t b, int variant)
{
    limb u[8], v[8], q[8], r[8];

    counterexample(b, u, v);
    algo_d(u, 4, v, 3, b, variant, q, r, 0);
    return !check(u, 4, v, 3, q, 4, r, b);
}

/* ------------------------------------------------------------------ */
/* every legal 4-by-3-limb division in a small base                   */
/* ------------------------------------------------------------------ */

static void exhaustive(uint64_t b)
{
    const int n = 3, ulen = 4;
    limb u[8], v[8], q[8], r[8];
    long bad_printed = 0, bad_coded = 0, cases = 0;

    for (v[2] = b / 2; v[2] < b; v[2]++)            /* normalised divisor */
     for (v[1] = 0; v[1] < b; v[1]++)
      for (v[0] = 0; v[0] < b; v[0]++)
       for (u[3] = 0; u[3] < b; u[3]++)
        for (u[2] = 0; u[2] < b; u[2]++)
         for (u[1] = 0; u[1] < b; u[1]++) {
            if (mp_cmp(u + 1, v, n) >= 0)           /* top n limbs of u < v */
                continue;
            for (u[0] = 0; u[0] < b; u[0]++) {
                cases++;
                algo_d(u, ulen, v, n, b, AS_PRINTED, q, r, 0);
                if (!check(u, ulen, v, n, q, ulen, r, b))
                    bad_printed++;
                algo_d(u, ulen, v, n, b, AS_CODED, q, r, 0);
                if (!check(u, ulen, v, n, q, ulen, r, b))
                    bad_coded++;
            }
         }
    printf("  b = %-3llu %-7s %9ld divisions   as printed: %5ld wrong   "
           "while loop: %ld wrong\n",
           (unsigned long long)b, (b & 1) ? "(odd)" : "(even)",
           cases, bad_printed, bad_coded);
}

/* ------------------------------------------------------------------ */

int main(int argc, char **argv)
{
    const int n = 3, ulen = 4;
    limb u[8], v[8], q[8], r[8];
    uint64_t b = 3;

    if (argc > 1) {
        b = strtoull(argv[1], NULL, 0);
        if (b < 3 || b > (1ULL << 31) || (b & 1) == 0) {
            fprintf(stderr, "usage: %s [odd base b, 3 <= b <= 2^31]\n", argv[0]);
            fprintf(stderr, "the failure needs an odd base; even bases are safe\n");
            return 1;
        }
    }

    printf("Knuth, TAOCP vol. 2, Algorithm 4.3.1D -- smallest failing input for base %llu\n\n",
           (unsigned long long)b);

    counterexample(b, u, v);
    mp_print("u", u, ulen, b);
    mp_print("v", v, n, b);
    printf("\n");

    printf("  schoolbook reference:\n");
    ref_div(u, ulen, v, n, b, q, r);
    report(u, ulen, v, n, q, r, b);
    printf("\n");

    printf("  Algorithm D as printed in the book:\n");
    algo_d(u, ulen, v, n, b, AS_PRINTED, q, r, 1);
    report(u, ulen, v, n, q, r, b);
    printf("\n");

    printf("  Algorithm D with D3 as a while loop (how it is usually coded):\n");
    algo_d(u, ulen, v, n, b, AS_CODED, q, r, 1);
    report(u, ulen, v, n, q, r, b);
    printf("\n");

    printf("The same counterexample in other odd bases:\n");
    static const uint64_t bases[] = { 3, 5, 7, 65, 2147483647 };
    for (size_t i = 0; i < sizeof bases / sizeof *bases; i++)
        printf("  b = %-12llu as printed: %-9s while loop: %s\n",
               (unsigned long long)bases[i],
               fails(bases[i], AS_PRINTED) ? "WRONG" : "correct",
               fails(bases[i], AS_CODED) ? "WRONG" : "correct");
    printf("\n");

    printf("Every legal 4-by-3-limb division, exhaustively:\n");
    exhaustive(3);
    exhaustive(4);
    exhaustive(5);
    exhaustive(7);
    exhaustive(8);
    exhaustive(9);
    printf("\nThe failure needs q^ = q+3 = b+2, which requires an odd base:\n"
           "v_{n-1} = u_n = (b-1)/2 and u_{n-1} = b-1.  Real machines use\n"
           "b = 2^32 or 2^64, so the bug cannot be reached there.\n");
    return 0;
}
