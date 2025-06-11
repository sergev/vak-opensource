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
        printf("Error: Truth table length %zu is not a power of 2\n", len);
        exit(1);
    }
    if (num_vars > 64) {
        printf("Error: Too many variables (%d, max 64)\n", num_vars);
        exit(1);
    }

    printf("[DEBUG] Parsing truth table with %d variables, length %zu\n", num_vars, len);
    for (uint64_t i = 0; i < len; i++) {
        if (truth_table[i] != '0' && truth_table[i] != '1' && truth_table[i] != 'X') {
            printf("Error: Invalid character '%c' in truth table\n", truth_table[i]);
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
    uint64_t diff          = a.value ^ b.value;
    uint64_t relevant_bits = a.mask & b.mask;
    diff &= relevant_bits;
    if (count_ones(diff) != 1) {
        // Allow merge if one is a don't care
        if (a.mask == 0 || b.mask == 0) {
            diff                = a.value ^ b.value;
            uint64_t check_bits = a.mask | b.mask;
            diff &= check_bits;
            if (count_ones(diff) != 1)
                return 0;
        } else {
            return 0;
        }
    }

    // Determine the differing bit position
    uint64_t diff_bit = diff;
    result->mask      = (a.mask & b.mask) & ~diff;
    if (a.mask == 0) {
        result->value = b.value;
        result->mask  = b.mask & ~diff;
    } else if (b.mask == 0) {
        result->value = a.value;
        result->mask  = a.mask & ~diff;
    } else {
        result->value = a.value & ~diff_bit; // Clear differing bit
    }
    if (result->mask == 0 && (a.mask != 0 || b.mask != 0))
        return 0;

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
        printf("Error: Memory allocation failed\n");
        exit(1);
    }

    Implicant *temp = malloc(src_count * sizeof(Implicant));
    if (!temp) {
        free(used);
        printf("Error: Memory allocation failed\n");
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
            printf("Error: Memory allocation failed\n");
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
        printf("Error: Memory allocation failed\n");
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
        printf("Error: Memory allocation failed\n");
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

    // Cover remaining uncovered minterms
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

// Test main function
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
