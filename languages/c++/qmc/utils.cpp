#include "utils.h"

#include <iomanip>
#include <iostream>
#include <sstream>

void print_truth_table(const std::string &truth_table, size_t n_vars, bool should_print)
{
    if (!should_print) {
        return;
    }

    static const char VARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // Print header
    std::cout << "Truth Table:\n";
    for (size_t i = 0; i < n_vars; ++i) {
        std::cout << VARS[i] << " ";
    }
    std::cout << "| Output\n";
    for (size_t i = 0; i < n_vars; ++i) {
        std::cout << "--";
    }
    std::cout << "|-------\n";

    // Print rows
    for (size_t i = 0; i < truth_table.size(); ++i) {
        // Convert index to binary
        for (size_t j = 0; j < n_vars; ++j) {
            size_t bit = n_vars - 1 - j;
            std::cout << ((i >> bit) & 1) << " ";
        }
        std::cout << "| " << truth_table[i] << "\n";
    }
    std::cout << std::endl;
}

void print_implicant_table(const std::set<std::pair<uint64_t, uint64_t>> &prime_implicants,
                           const std::vector<std::pair<uint64_t, uint64_t>> &essential,
                           const std::vector<size_t> &minterms, size_t n_vars, bool should_print)
{
    if (!should_print) {
        return;
    }

    static const char VARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::cout << "Implicant Table:\n";

    // Print header with minterms
    std::cout << std::setw(15) << "Implicant |";
    for (const auto &m : minterms) {
        std::cout << std::setw(5) << m;
    }
    std::cout << "\n" << std::string(15, '-') << "|";
    for (size_t i = 0; i < minterms.size(); ++i) {
        std::cout << "-----";
    }
    std::cout << "\n";

    // Print each prime implicant
    for (const auto &[term, mask] : prime_implicants) {
        // Convert implicant to string
        std::vector<char> term_bits(n_vars, '-');
        for (size_t i = 0; i < n_vars; ++i) {
            if (mask & (1ull << i)) {
                term_bits[n_vars - 1 - i] = (term & (1ull << i)) ? '1' : '0';
            }
        }
        std::string term_str(term_bits.begin(), term_bits.end());

        // Mark essential implicants
        bool is_essential = std::find(essential.begin(), essential.end(),
                                      std::make_pair(term, mask)) != essential.end();
        std::cout << std::setw(12) << term_str << (is_essential ? "*" : " ") << " |";

        // Print coverage
        for (const auto &m : minterms) {
            if ((static_cast<uint64_t>(m) & mask) == (term & mask)) {
                std::cout << std::setw(5) << "X";
            } else {
                std::cout << std::setw(5) << " ";
            }
        }
        std::cout << "\n";
    }

    // Print essential implicants as Boolean terms
    std::cout << "\nEssential Prime Implicants:\n";
    for (const auto &[term, mask] : essential) {
        std::vector<std::string> literals;
        for (size_t i = 0; i < n_vars; ++i) {
            if (mask & (1ull << i)) {
                char var = VARS[i];
                if (term & (1ull << i)) {
                    literals.emplace_back(1, var);
                } else {
                    literals.emplace_back("~" + std::string(1, var));
                }
            }
        }
        std::string term_str = literals.empty()
                                   ? "1"
                                   : literals[0] + literals[1] +
                                         (literals.size() > 2 ? literals[2] + literals[3] : "");
        std::cout << term_str << "\n";
    }
    std::cout << std::endl;
}
