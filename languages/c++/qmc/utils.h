#include <set>
#include <string>
#include <vector>

// Declarations for functions in utils.cpp
void print_truth_table(const std::string &truth_table, size_t n_vars, bool should_print);
void print_implicant_table(const std::set<std::pair<uint64_t, uint64_t>> &prime_implicants,
                           const std::vector<std::pair<uint64_t, uint64_t>> &essential,
                           const std::vector<size_t> &minterms, size_t n_vars, bool should_print);
