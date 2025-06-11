#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "minimizer.h"

// External variables defined here
Minterm minterms[1ULL << 16];
int num_vars = 0;

// Print binary representation of a 64-bit number (up to num_vars bits)
void print_binary(uint64_t x, int bits)
{
    for (int i = bits - 1; i >= 0; i--) {
        printf("%lld", (x >> i) & 1);
    }
}

// Count 1s in a 64-bit number
int count_ones(uint64_t x)
{
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

// Compare two implicants for merging
int can_merge(Implicant a, Implicant b, Implicant *result)
{
    printf("[DEBUG] can_merge: a(value=");
    print_binary(a.value, num_vars);
    printf(", mask=");
    print_binary(a.mask, num_vars);
    printf("), b(value=");
    print_binary(b.value, num_vars);
    printf(", mask=");
    print_binary(b.mask, num_vars);
    printf(")\n");

    uint64_t diff = a.value ^ b.value; // Bits that differ
    printf("[DEBUG] diff=");
    print_binary(diff, num_vars);
    printf("\n");

    uint64_t relevant_bits;
    uint64_t diff_bits;

    // Regular merge: both implicants have defined masks
    if (a.mask != 0 && b.mask != 0) {
        printf("[DEBUG] Entering regular merge path\n");
        relevant_bits = a.mask & b.mask; // Bits defined in both
        diff_bits     = diff & relevant_bits;
        printf("[DEBUG] relevant_bits=");
        print_binary(relevant_bits, num_vars);
        printf(", diff_bits=");
        print_binary(diff_bits, num_vars);
        printf(", count_ones(diff_bits)=%d\n", count_ones(diff_bits));
        if (count_ones(diff_bits) != 1) {
            printf("[DEBUG] Regular merge failed: not exactly one bit difference\n");
            return 0;
        }

        result->value = a.value & b.value;            // Common bits
        result->mask  = a.mask & b.mask & ~diff_bits; // Clear differing bit
        printf("[DEBUG] Regular merge: tentative result(value=");
        print_binary(result->value, num_vars);
        printf(", mask=");
        print_binary(result->mask, num_vars);
        printf(")\n");
    }
    // Don't-care merge: one or both implicants are don't cares
    else {
        printf("[DEBUG] Entering don't-care merge path\n");
        relevant_bits = a.mask | b.mask; // Bits defined in either
        diff_bits     = diff & relevant_bits;
        printf("[DEBUG] relevant_bits=");
        print_binary(relevant_bits, num_vars);
        printf(", diff_bits=");
        print_binary(diff_bits, num_vars);
        printf("\n");
        if (diff_bits == 0) {
            printf("[DEBUG] Don't-care merge failed: identical in defined bits\n");
            return 0;
        }

        // Try each differing bit to find a valid merge
        for (int i = num_vars - 1; i >= 0; i--) {
            uint64_t diff_bit = 1ULL << i;
            if (!(diff_bits & diff_bit))
                continue;

            printf("[DEBUG] Testing diff_bit=");
            print_binary(diff_bit, num_vars);
            printf(" (bit %d)\n", i);

            // Compute tentative result
            if (a.mask == 0 && b.mask == 0) {
                result->value = a.value & b.value;
                result->mask  = 0;
            } else if (a.mask == 0) {
                result->value = b.value;
                result->mask  = b.mask & ~diff_bit;
            } else {
                result->value = a.value;
                result->mask  = a.mask & ~diff_bit;
            }

            printf("[DEBUG] Tentative result: value=");
            print_binary(result->value, num_vars);
            printf(", mask=");
            print_binary(result->mask, num_vars);
            printf("\n");

            // Validate minterm consistency
            int valid = 1;
            for (int j = 0; j < a.num_minterms; j++) {
                for (int k = 0; k < b.num_minterms; k++) {
                    uint64_t minterm_a = minterms[a.minterms[j]].value;
                    uint64_t minterm_b = minterms[b.minterms[k]].value;
                    uint64_t masked_a  = minterm_a & result->mask;
                    uint64_t masked_b  = minterm_b & result->mask;
                    printf("[DEBUG] Minterm check: minterm_a=");
                    print_binary(minterm_a, num_vars);
                    printf(", minterm_b=");
                    print_binary(minterm_b, num_vars);
                    printf(", masked_a=");
                    print_binary(masked_a, num_vars);
                    printf(", masked_b=");
                    print_binary(masked_b, num_vars);
                    printf("\n");
                    if (masked_a != masked_b) {
                        printf("[DEBUG] Minterm consistency failed for bit %d\n", i);
                        valid = 0;
                        break;
                    }
                }
                if (!valid)
                    break;
            }

            if (valid) {
                printf("[DEBUG] Minterm consistency passed for bit %d\n", i);
                break;
            } else if (i == 0) {
                printf("[DEBUG] Don't-care merge failed: no valid merge found\n");
                return 0;
            }
        }
    }

    // Prevent invalid implicants
    if (result->mask == 0 && (a.mask != 0 || b.mask != 0)) {
        printf("[DEBUG] Merge failed: invalid implicant (mask=0)\n");
        return 0;
    }

    // Populate minterms
    result->num_minterms = 0;
    for (int i = 0; i < a.num_minterms; i++) {
        result->minterms[result->num_minterms++] = a.minterms[i];
    }
    for (int i = 0; i < b.num_minterms; i++) {
        result->minterms[result->num_minterms++] = b.minterms[i];
    }

    printf("[DEBUG] Merge successful: result(value=");
    print_binary(result->value, num_vars);
    printf(", mask=");
    print_binary(result->mask, num_vars);
    printf(", minterms=");
    for (int i = 0; i < result->num_minterms; i++) {
        printf("%d ", result->minterms[i]);
    }
    printf(")\n");

    return 1;
}

// Unit tests for can_merge
static void run_can_merge_tests(void)
{
    printf("Running can_merge unit tests...\n");
    num_vars = 3; // Set for 3 variables (A, B, C)

    // Test 1: Regular merge (001 and 011 -> ~BC)
    printf("Test 1: Regular merge (001 and 011 -> ~BC)\n");
    minterms[0].value = 0b001;
    minterms[0].mask  = 0b111;
    minterms[1].value = 0b011;
    minterms[1].mask  = 0b111;
    Implicant a1      = { .value = 0b001, .mask = 0b111, .minterms = { 0 }, .num_minterms = 1 };
    Implicant b1      = { .value = 0b011, .mask = 0b111, .minterms = { 1 }, .num_minterms = 1 };
    Implicant r1;
    assert(can_merge(a1, b1, &r1) == 1);
    assert(r1.value == 0b001);
    assert(r1.mask == 0b101);
    assert(r1.num_minterms == 2);
    assert(r1.minterms[0] == 0 && r1.minterms[1] == 1);
    printf("Test 1 passed\n");

    // Test 2: Regular merge (001 and 101 -> ~AC)
    printf("Test 2: Regular merge (001 and 101 -> ~AC)\n");
    minterms[2].value = 0b101;
    minterms[2].mask  = 0b111;
    Implicant a2      = { .value = 0b001, .mask = 0b111, .minterms = { 0 }, .num_minterms = 1 };
    Implicant b2      = { .value = 0b101, .mask = 0b111, .minterms = { 2 }, .num_minterms = 1 };
    Implicant r2;
    assert(can_merge(a2, b2, &r2) == 1);
    assert(r2.value == 0b001);
    assert(r2.mask == 0b011);
    assert(r2.num_minterms == 2);
    assert(r2.minterms[0] == 0 && r2.minterms[1] == 2);
    printf("Test 2 passed\n");

    // Test 3: Don't-care merge (011 and 110 -> BC)
    printf("Test 3: Don't-care merge (011 and 110 -> BC)\n");
    minterms[3].value = 0b110;
    minterms[3].mask  = 0b000;
    Implicant a3      = { .value = 0b011, .mask = 0b111, .minterms = { 1 }, .num_minterms = 1 };
    Implicant b3      = { .value = 0b110, .mask = 0b000, .minterms = { 3 }, .num_minterms = 1 };
    Implicant r3;
    assert(can_merge(a3, b3, &r3) == 1);
    assert(r3.value == 0b011);
    assert(r3.mask == 0b110);
    assert(r3.num_minterms == 2);
    assert(r3.minterms[0] == 1 && r3.minterms[1] == 3);
    printf("Test 3 passed\n");

    // Test 4: Don't-care merge (100 and 101 -> ~B~C)
    printf("Test 4: Don't-care merge (100 and 101 -> ~B~C)\n");
    minterms[4].value = 0b100;
    minterms[4].mask  = 0b000;
    Implicant a4      = { .value = 0b100, .mask = 0b000, .minterms = { 4 }, .num_minterms = 1 };
    Implicant b4      = { .value = 0b101, .mask = 0b111, .minterms = { 2 }, .num_minterms = 1 };
    Implicant r4;
    assert(can_merge(a4, b4, &r4) == 1);
    assert(r4.value == 0b101);
    assert(r4.mask == 0b110);
    assert(r4.num_minterms == 2);
    assert(r4.minterms[0] == 4 && r4.minterms[1] == 2);
    printf("Test 4 passed\n");

    // Test 5: Invalid merge (multiple bit differences)
    printf("Test 5: Invalid merge (001 and 110)\n");
    Implicant a5 = { .value = 0b001, .mask = 0b111, .minterms = { 0 }, .num_minterms = 1 };
    Implicant b5 = { .value = 0b110, .mask = 0b111, .minterms = { 3 }, .num_minterms = 1 };
    Implicant r5;
    assert(can_merge(a5, b5, &r5) == 0);
    printf("Test 5 passed\n");

    // Test 6: Invalid merge (inconsistent minterms)
    printf("Test 6: Invalid merge (~AC and ~B~C)\n");
    minterms[5].value = 0b001;
    minterms[5].mask  = 0b011;
    minterms[6].value = 0b101;
    minterms[6].mask  = 0b110;
    Implicant a6      = { .value = 0b001, .mask = 0b011, .minterms = { 5 }, .num_minterms = 1 };
    Implicant b6      = { .value = 0b101, .mask = 0b110, .minterms = { 6 }, .num_minterms = 1 };
    Implicant r6;
    assert(can_merge(a6, b6, &r6) == 0);
    printf("Test 6 passed\n");

    // Test 7: Both don't cares
    printf("Test 7: Both don't cares (100 and 110)\n");
    minterms[7].value = 0b110;
    minterms[7].mask  = 0b000;
    Implicant a7      = { .value = 0b100, .mask = 0b000, .minterms = { 4 }, .num_minterms = 1 };
    Implicant b7      = { .value = 0b110, .mask = 0b000, .minterms = { 7 }, .num_minterms = 1 };
    Implicant r7;
    assert(can_merge(a7, b7, &r7) == 1);
    assert(r7.value == 0b100);
    assert(r7.mask == 0b000);
    assert(r7.num_minterms == 2);
    assert(r7.minterms[0] == 4 && r7.minterms[1] == 7);
    printf("Test 7 passed\n");

    // Test 8: Identical implicants
    printf("Test 8: Identical implicants (001 and 001)\n");
    Implicant a8 = { .value = 0b001, .mask = 0b111, .minterms = { 0 }, .num_minterms = 1 };
    Implicant b8 = { .value = 0b001, .mask = 0b111, .minterms = { 0 }, .num_minterms = 1 };
    Implicant r8;
    assert(can_merge(a8, b8, &r8) == 0);
    printf("Test 8 passed\n");

    // Test 9: Invalid mask result
    printf("Test 9: Invalid mask result (001 and 000 with single-bit mask)\n");
    minterms[8].value = 0b001;
    minterms[8].mask  = 0b001;
    minterms[9].value = 0b000;
    minterms[9].mask  = 0b001;
    Implicant a9      = { .value = 0b001, .mask = 0b001, .minterms = { 8 }, .num_minterms = 1 };
    Implicant b9      = { .value = 0b000, .mask = 0b001, .minterms = { 9 }, .num_minterms = 1 };
    Implicant r9;
    assert(can_merge(a9, b9, &r9) == 1);
    assert(r9.value == 0b000);
    assert(r9.mask == 0b000);
    assert(r9.num_minterms == 2);
    assert(r9.minterms[0] == 8 && r9.minterms[1] == 9);
    printf("Test 9 passed\n");

    printf("All can_merge tests passed!\n");
}

int main(void)
{
    run_can_merge_tests();
    return 0;
}
