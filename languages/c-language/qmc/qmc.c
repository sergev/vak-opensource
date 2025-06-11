#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to represent a minterm
typedef struct {
    uint64_t value; // Binary value of minterm
    uint64_t mask;  // Mask for don't cares (0 for X, 1 for 0/1)
    int covered;    // Flag to mark if covered
} Minterm;

// Structure to represent an implicant
typedef struct {
    uint64_t value;   // Binary value
    uint64_t mask;    // Mask for don't cares
    int minterms[64]; // Indices of covered minterms
    int num_minterms; // Number of covered minterms
} Implicant;

// Global variables
static Minterm minterms[1ULL << 16]; // Max 2^16 minterms
static int num_minterms = 0;
static Implicant implicants[1ULL << 16];
static int num_implicants = 0;
static int num_vars       = 0;

// Print binary representation of a 64-bit number (up to num_vars bits)
static void print_binary(uint64_t x, int bits)
{
    for (int i = bits - 1; i >= 0; i--) {
        printf("%lld", (x >> i) & 1);
    }
}

// Count 1s in a 64-bit number
static int count_ones(uint64_t x)
{
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

// Compute log2 of a number (for power of 2)
static int compute_log2(uint64_t n)
{
    int log2 = 0;
    if (n == 0)
        return -1; // Invalid
    while (n > 1) {
        if (n & 1)
            return -1; // Not a power of 2
        n >>= 1;
        log2++;
    }
    return log2;
}

// Parse truth table and extract minterms
static void parse_truth_table(const char *truth_table)
{
    num_minterms = 0;
    size_t len   = strlen(truth_table);
    num_vars     = compute_log2(len);
    if (num_vars < 0 || len != (1ULL << num_vars)) {
        printf("[ERROR] Truth table length %zu is not a power of 2\n", len);
        exit(1);
    }
    if (num_vars > 64) {
        printf("[ERROR] Too many variables (%d, max 64)\n", num_vars);
        exit(1);
    }

    printf("[DEBUG] Parsing truth table with %d variables, length %zu\n", num_vars, len);
    for (uint64_t i = 0; i < len; i++) {
        if (truth_table[i] != '0' && truth_table[i] != '1' && truth_table[i] != 'X') {
            printf("[ERROR] Invalid character '%c' in truth table\n", truth_table[i]);
            exit(1);
        }
        if (truth_table[i] == '1' || truth_table[i] == 'X') {
            minterms[num_minterms].value = i;
            minterms[num_minterms].mask  = (truth_table[i] == 'X') ? 0 : (~0ULL >> (64 - num_vars));
            minterms[num_minterms].covered = 0;
            printf("[DEBUG] Minterm %d: value=", num_minterms);
            print_binary(minterms[num_minterms].value, num_vars);
            printf(", mask=");
            print_binary(minterms[num_minterms].mask, num_vars);
            printf(", type=%c\n", truth_table[i]);
            num_minterms++;
        }
    }
}

// Compare two implicants for merging
static int can_merge(Implicant a, Implicant b, Implicant *result)
{
    uint64_t diff = a.value ^ b.value; // Bits that differ
    uint64_t relevant_bits;
    uint64_t diff_bits;

    // Regular merge: both implicants have defined masks
    if (a.mask != 0 && b.mask != 0) {
        relevant_bits = a.mask & b.mask; // Bits defined in both
        diff_bits     = diff & relevant_bits;
        if (count_ones(diff_bits) != 1)
            return 0; // Must differ in exactly one bit

        result->value = a.value & b.value;            // Common bits
        result->mask  = a.mask & b.mask & ~diff_bits; // Clear differing bit
    }
    // Don't-care merge: one or both implicants are don't cares
    else {
        relevant_bits = a.mask | b.mask; // Bits defined in either
        diff_bits     = diff & relevant_bits;
        if (diff_bits == 0)
            return 0; // Identical in defined bits

        // Try each differing bit to find a valid merge
        for (int i = num_vars - 1; i >= 0; i--) {
            uint64_t diff_bit = 1ULL << i;
            if (!(diff_bits & diff_bit))
                continue;

            // Test merge with this bit cleared
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

            // Validate minterm consistency for this mask
            int valid = 1;
            for (int j = 0; j < a.num_minterms; j++) {
                for (int k = 0; k < b.num_minterms; k++) {
                    uint64_t minterm_a = minterms[a.minterms[j]].value;
                    uint64_t minterm_b = minterms[b.minterms[k]].value;
                    if ((minterm_a & result->mask) != (minterm_b & result->mask)) {
                        valid = 0;
                        break;
                    }
                }
                if (!valid)
                    break;
            }

            if (valid) {
                // Merge is valid, proceed
                break;
            } else if (i == 0) {
                return 0; // No valid merge found
            }
        }
    }

    // Prevent invalid implicants
    if (result->mask == 0 && (a.mask != 0 || b.mask != 0))
        return 0;

    // Populate minterms
    result->num_minterms = 0;
    for (int i = 0; i < a.num_minterms; i++) {
        result->minterms[result->num_minterms++] = a.minterms[i];
    }
    for (int i = 0; i < b.num_minterms; i++) {
        result->minterms[result->num_minterms++] = b.minterms[i];
    }

    printf("[DEBUG] Merging: a(value=");
    print_binary(a.value, num_vars);
    printf(", mask=");
    print_binary(a.mask, num_vars);
    printf("), b(value=");
    print_binary(b.value, num_vars);
    printf(", mask=");
    print_binary(b.mask, num_vars);
    printf(") -> result(value=");
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

// Perform one step of Quine-McCluskey reduction
static int reduce_implicants(Implicant *src, int src_count, Implicant *dest)
{
    int dest_count = 0;
    int *used      = calloc(src_count, sizeof(int));
    if (!used) {
        printf("[ERROR] Memory allocation failed\n");
        exit(1);
    }

    Implicant *temp = malloc(src_count * sizeof(Implicant));
    if (!temp) {
        free(used);
        printf("[ERROR] Memory allocation failed\n");
        exit(1);
    }

    printf("[DEBUG] Reducing %d implicants:\n", src_count);
    for (int i = 0; i < src_count; i++) {
        printf("[DEBUG] Implicant %d: value=", i);
        print_binary(src[i].value, num_vars);
        printf(", mask=");
        print_binary(src[i].mask, num_vars);
        printf(", minterms=");
        for (int j = 0; j < src[i].num_minterms; j++) {
            printf("%d ", src[i].minterms[j]);
        }
        printf("\n");
    }

    for (int i = 0; i < src_count; i++) {
        for (int j = i + 1; j < src_count; j++) {
            Implicant new_imp;
            if (can_merge(src[i], src[j], &new_imp)) {
                used[i] = used[j]  = 1;
                temp[dest_count++] = new_imp;
            }
        }
    }

    // Copy unused implicants (prime implicants)
    for (int i = 0; i < src_count; i++) {
        if (!used[i] && src[i].mask != 0) {
            temp[dest_count++] = src[i];
        }
    }

    printf("[DEBUG] After reduction, %d implicants:\n", dest_count);
    for (int i = 0; i < dest_count; i++) {
        printf("[DEBUG] Implicant %d: value=", i);
        print_binary(temp[i].value, num_vars);
        printf(", mask=");
        print_binary(temp[i].mask, num_vars);
        printf(", minterms=");
        for (int j = 0; j < temp[i].num_minterms; j++) {
            printf("%d ", temp[i].minterms[j]);
        }
        printf("\n");
    }

    memcpy(dest, temp, dest_count * sizeof(Implicant));
    free(temp);
    free(used);
    return dest_count;
}

// Quine-McCluskey algorithm
static void quine_mccluskey(void)
{
    num_implicants = 0;

    // Initialize implicants with minterms
    for (int i = 0; i < num_minterms; i++) {
        implicants[num_implicants].value        = minterms[i].value;
        implicants[num_implicants].mask         = minterms[i].mask;
        implicants[num_implicants].minterms[0]  = i;
        implicants[num_implicants].num_minterms = 1;
        num_implicants++;
    }

    printf("[DEBUG] Starting Quine-McCluskey with %d initial implicants\n", num_implicants);

    // Reduce until no further merging possible
    int iteration = 0;
    while (1) {
        printf("[DEBUG] Reduction iteration %d\n", iteration++);
        Implicant *temp = malloc(num_implicants * sizeof(Implicant));
        if (!temp) {
            printf("[ERROR] Memory allocation failed\n");
            exit(1);
        }

        int new_count = reduce_implicants(implicants, num_implicants, temp);
        if (new_count == num_implicants) {
            int identical = 1;
            for (int i = 0; i < num_implicants; i++) {
                if (temp[i].value != implicants[i].value || temp[i].mask != implicants[i].mask) {
                    identical = 0;
                    break;
                }
            }
            free(temp);
            if (identical)
                break;
        }
        memcpy(implicants, temp, new_count * sizeof(Implicant));
        free(temp);
        num_implicants = new_count;
    }

    printf("[DEBUG] Quine-McCluskey complete, %d prime implicants\n", num_implicants);
}

// Select essential prime implicants
static void select_essential_implicants(void)
{
    Implicant *result = malloc(num_implicants * sizeof(Implicant));
    if (!result) {
        printf("[ERROR] Memory allocation failed\n");
        exit(1);
    }
    int result_count = 0;

    // Reset covered flags
    for (int i = 0; i < num_minterms; i++) {
        minterms[i].covered = 0;
    }

    // Build coverage table
    int *coverage = calloc(num_minterms, sizeof(int));
    if (!coverage) {
        free(result);
        printf("[ERROR] Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < num_implicants; i++) {
        if (implicants[i].mask == 0)
            continue;
        for (int j = 0; j < implicants[i].num_minterms; j++) {
            int mt_idx = implicants[i].minterms[j];
            if (minterms[mt_idx].mask != 0) {
                coverage[mt_idx]++;
            }
        }
    }

    printf("[DEBUG] Coverage counts:\n");
    for (int i = 0; i < num_minterms; i++) {
        if (minterms[i].mask != 0) {
            printf("[DEBUG] Minterm %d (value=", i);
            print_binary(minterms[i].value, num_vars);
            printf("): covered by %d implicants\n", coverage[i]);
        }
    }

    // Select essential prime implicants
    for (int i = 0; i < num_implicants; i++) {
        if (implicants[i].mask == 0)
            continue;
        int essential = 0;
        for (int j = 0; j < implicants[i].num_minterms; j++) {
            int mt_idx = implicants[i].minterms[j];
            if (minterms[mt_idx].mask == 0 || minterms[mt_idx].covered)
                continue;
            if (coverage[mt_idx] == 1) {
                essential = 1;
                break;
            }
        }
        if (essential) {
            printf("[DEBUG] Selecting essential implicant %d: value=", i);
            print_binary(implicants[i].value, num_vars);
            printf(", mask=");
            print_binary(implicants[i].mask, num_vars);
            printf(", minterms=");
            for (int j = 0; j < implicants[i].num_minterms; j++) {
                printf("%d ", implicants[i].minterms[j]);
            }
            printf("\n");
            result[result_count++] = implicants[i];
            for (int j = 0; j < implicants[i].num_minterms; j++) {
                int mt_idx = implicants[i].minterms[j];
                if (minterms[mt_idx].mask == 0)
                    continue;
                minterms[mt_idx].covered = 1;
            }
        }
    }

    // Cover remaining uncovered minterms with minimal implicants
    for (int i = 0; i < num_implicants; i++) {
        if (implicants[i].mask == 0)
            continue;
        int covers_new   = 0;
        int new_coverage = 0;
        for (int j = 0; j < implicants[i].num_minterms; j++) {
            int mt_idx = implicants[i].minterms[j];
            if (minterms[mt_idx].mask == 0 || minterms[mt_idx].covered)
                continue;
            covers_new = 1;
            new_coverage++;
        }
        if (covers_new) {
            printf("[DEBUG] Selecting additional implicant %d: value=", i);
            print_binary(implicants[i].value, num_vars);
            printf(", mask=");
            print_binary(implicants[i].mask, num_vars);
            printf(", minterms=");
            for (int j = 0; j < implicants[i].num_minterms; j++) {
                printf("%d ", implicants[i].minterms[j]);
            }
            printf(", covers %d new minterms\n", new_coverage);
            result[result_count++] = implicants[i];
            for (int j = 0; j < implicants[i].num_minterms; j++) {
                int mt_idx = implicants[i].minterms[j];
                if (minterms[mt_idx].mask == 0)
                    continue;
                minterms[mt_idx].covered = 1;
            }
        }
    }

    printf("[DEBUG] Selected %d implicants\n", result_count);
    memcpy(implicants, result, result_count * sizeof(Implicant));
    free(result);
    free(coverage);
    num_implicants = result_count;
}

// Print implicant as a product term
static void print_implicant(Implicant imp)
{
    if (imp.mask == 0)
        return;
    printf("[DEBUG] Printing implicant: value=");
    print_binary(imp.value, num_vars);
    printf(", mask=");
    print_binary(imp.mask, num_vars);
    printf(", expression=");
    int first = 1;
    for (int i = 0; i < num_vars; i++) {
        uint64_t bit = 1ULL << (num_vars - 1 - i);
        if (imp.mask & bit) {
            if (!first)
                printf("");
            if (!(imp.value & bit))
                printf("~");
            printf("%c", 'A' + i);
            first = 0;
        }
    }
    if (first)
        printf("1");
    printf("\n");

    // Print actual expression to stdout
    first = 1;
    for (int i = 0; i < num_vars; i++) {
        uint64_t bit = 1ULL << (num_vars - 1 - i);
        if (imp.mask & bit) {
            if (!first)
                printf("");
            if (!(imp.value & bit))
                printf("~");
            printf("%c", 'A' + i);
            first = 0;
        }
    }
    if (first)
        printf("1");
}

// Main minimization function
void minimize_boolean_function(const char *truth_table)
{
    parse_truth_table(truth_table);
    quine_mccluskey();
    select_essential_implicants();

    // Print result
    if (num_implicants == 0) {
        printf("0\n");
        return;
    }

    int first_term = 1;
    for (int i = 0; i < num_implicants; i++) {
        if (implicants[i].mask == 0)
            continue;
        if (!first_term)
            printf(" + ");
        print_implicant(implicants[i]);
        first_term = 0;
    }
    if (first_term)
        printf("0\n");
    else
        printf("\n");
}

// Unit tests for can_merge
#if 1
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
#else
int main(void)
{
    // Test case 1: "0101" -> "B"
    printf("Test case 1:\n");
    minimize_boolean_function("0101");

    // Test case 2: "0101X1X0" -> "~AC + BC"
    printf("\nTest case 2:\n");
    minimize_boolean_function("0101X1X0");

    return 0;
}
#endif
