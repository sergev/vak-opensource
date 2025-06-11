#include <algorithm>
#include <cstdint>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

// Represents an implicant in Quine-McCluskey
struct Implicant {
    uint64_t bits;                       // Bits that are set (1s)
    uint64_t mask;                       // Bits that are defined (1 or 0, not X)
    std::set<uint64_t> covered_minterms; // Minterms this implicant covers

    Implicant(uint64_t minterm, uint64_t mask = UINT64_MAX) : bits(minterm), mask(mask)
    {
        covered_minterms.insert(minterm);
    }

    bool can_combine(const Implicant &other) const
    {
        if (mask != other.mask)
            return false;
        int diff_count = 0;
        // uint64_t diff_bit = 0;
        for (int i = 0; i < 64; ++i) {
            if ((mask & (1ULL << i)) && ((bits >> i) & 1) != ((other.bits >> i) & 1)) {
                diff_count++;
                // diff_bit = 1ULL << i;
                if (diff_count > 1)
                    return false;
            }
        }
        return diff_count == 1;
    }

    Implicant combine(const Implicant &other) const
    {
        Implicant result(*this);
        result.mask &= ~(bits ^ other.bits); // Remove differing bit from mask
        result.covered_minterms.insert(other.covered_minterms.begin(),
                                       other.covered_minterms.end());
        return result;
    }

    bool operator<(const Implicant &other) const
    {
        return std::tie(bits, mask) < std::tie(other.bits, other.mask);
    }

    bool covers(uint64_t minterm) const { return (minterm & mask) == (bits & mask); }
};

// Convert implicant to string (e.g., "~AC" for 3 variables where A=0, C=1, B=X)
std::string implicant_to_string(const Implicant &imp, int var_count)
{
    std::string result;
    for (int i = 0; i < var_count; ++i) {
        if (imp.mask & (1ULL << i)) {
            char var = 'A' + i;
            if ((imp.bits >> i) & 1) {
                result += var;
            } else {
                result += '~';
                result += var;
            }
        }
    }
    return result.empty() ? "1" : result;
}

std::string minimize_boolean_function(const std::string &truth_table)
{
    if (truth_table.empty())
        return "";

    // Determine number of variables (log2 of truth table length)
    int var_count = 0;
    uint64_t len  = truth_table.size();
    while (len > 1) {
        len >>= 1;
        ++var_count;
    }
    if ((1ULL << var_count) != truth_table.size()) {
        return ""; // Invalid truth table length
    }
    if (var_count > 64)
        return ""; // Too many variables

    // Collect minterms and don't cares
    std::vector<Implicant> implicants;
    for (uint64_t i = 0; i < truth_table.size(); ++i) {
        if (truth_table[i] == '1') {
            implicants.emplace_back(i);
        } else if (truth_table[i] == 'X') {
            implicants.emplace_back(i);
        }
    }

    // Quine-McCluskey: Generate prime implicants
    std::set<Implicant> prime_implicants;
    std::vector<Implicant> current = implicants;
    while (!current.empty()) {
        std::vector<Implicant> next;
        std::set<Implicant> used;
        bool combined = false;

        for (size_t i = 0; i < current.size(); ++i) {
            // bool was_combined = false;
            for (size_t j = i + 1; j < current.size(); ++j) {
                if (current[i].can_combine(current[j])) {
                    next.push_back(current[i].combine(current[j]));
                    used.insert(current[i]);
                    used.insert(current[j]);
                    combined = true;
                }
            }
            if (!used.count(current[i])) {
                prime_implicants.insert(current[i]);
            }
        }

        if (!combined)
            break;
        current = std::move(next);
    }

    // Select essential prime implicants
    std::set<uint64_t> minterms;
    for (const auto &imp : implicants) {
        if (truth_table[imp.bits] == '1') {
            minterms.insert(imp.bits);
        }
    }

    std::vector<Implicant> essential;
    std::set<uint64_t> covered_minterms;
#if 0
    while (!minterms.empty()) {
        // Find implicant that covers the most uncovered minterms
        Implicant *best    = nullptr;
        size_t max_covered = 0;
        for (const auto &imp : prime_implicants) {
            size_t count = 0;
            for (uint64_t m : minterms) {
                if (imp.covers(m))
                    count++;
            }
            if (count > max_covered) {
                max_covered = count;
                best        = const_cast<Implicant *>(&imp);
            }
        }

        if (!best)
            break; // No more coverage possible
        essential.push_back(*best);
        for (uint64_t m : best->covered_minterms) {
            minterms.erase(m);
            covered_minterms.insert(m);
        }
    }
#else
    while (!minterms.empty()) {
        Implicant *best    = nullptr;
        size_t max_covered = 0;
        for (const auto &imp : prime_implicants) {
            size_t count = 0;
            for (uint64_t m : minterms) {
                if (imp.covers(m))
                    count++;
            }
            if (count > max_covered && count > 0) {
                max_covered = count;
                best        = const_cast<Implicant *>(&imp);
            }
        }
        if (!best)
            break;
        essential.push_back(*best);
        for (uint64_t m : best->covered_minterms) {
            if (minterms.count(m)) {
                minterms.erase(m);
                covered_minterms.insert(m);
            }
        }
    }
#endif

    // Format result
    std::stringstream result;
    for (size_t i = 0; i < essential.size(); ++i) {
        if (i > 0)
            result << " + ";
        result << implicant_to_string(essential[i], var_count);
    }

    return result.str().empty() ? "0" : result.str();
}
