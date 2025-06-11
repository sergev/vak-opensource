#ifndef MINIMIZER_H
#define MINIMIZER_H

#include <stdint.h>

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

// External variables
extern Minterm minterms[1ULL << 16];
extern int num_vars;

// Utility functions
void print_binary(uint64_t x, int bits);
int count_ones(uint64_t x);

// Function declaration
int can_merge(Implicant a, Implicant b, Implicant *result);

#endif // MINIMIZER_H
