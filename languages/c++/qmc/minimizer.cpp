#include <algorithm>
#include <cmath>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "utils.h"

namespace {
//bool print_truth_table            = false;
//bool print_implicant_table        = false;
bool debug                        = false;
constexpr size_t thread_threshold = 4;
constexpr char vars[]             = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
} // namespace

struct Implicant {
    uint64_t term;                  // Binary representation
    uint64_t mask;                  // Bits that matter (1 = relevant)

    // Comparison operator for std::set
    bool operator<(const Implicant &other) const
    {
        if (term != other.term)
            return term < other.term;
        return mask < other.mask;
    }
};

std::string minimize_boolean_function(const std::string &truth_table)
{
    // Validate input
    size_t n_vars = std::log2(truth_table.size());
    if (truth_table.size() != (1ull << n_vars)) {
        throw std::runtime_error("Truth table length must be a power of 2");
    }
    if (n_vars > 64) {
        throw std::runtime_error("Too many variables (max 64)");
    }

    //if (print_truth_table) {
    //    print_truth_table(truth_table, n_vars);
    //}

    // Collect minterms and don't-cares
    std::vector<size_t> minterms, dont_cares;
    for (size_t i = 0; i < truth_table.size(); ++i) {
        switch (truth_table[i]) {
        case '1':
            minterms.push_back(i);
            break;
        case 'X':
            dont_cares.push_back(i);
            break;
        case '0':
            break;
        default:
            throw std::runtime_error("Invalid truth table character");
        }
    }
    if (minterms.empty()) {
        return "0";
    }

    if (debug) {
        std::cout << "Minterms: " << minterms.size() << ", Don't-cares: " << dont_cares.size()
                  << std::endl;
    }

    // Initialize implicants
    std::vector<Implicant> implicants;
    implicants.reserve(minterms.size() + dont_cares.size());
    uint64_t full_mask = (1ull << n_vars) - 1;
    for (size_t term : minterms) {
        implicants.push_back({ term, full_mask });
    }
    for (size_t term : dont_cares) {
        implicants.push_back({ term, full_mask });
    }

    // Group by number of 1s
    std::vector<std::vector<Implicant>> groups(n_vars + 1);
    for (const auto &imp : implicants) {
        groups[std::popcount(imp.term)].push_back(imp);
    }

    // Quine-McCluskey algorithm
    std::set<std::pair<uint64_t, uint64_t>> prime_implicants;
    size_t max_iterations = n_vars * 3;
    for (size_t iter = 0; iter < max_iterations; ++iter) {
        if (debug) {
            size_t count = 0;
            for (const auto &g : groups)
                count += g.size();
            std::cout << "Iteration " << iter + 1 << ": " << count << " implicants" << std::endl;
        }

        std::vector<std::vector<Implicant>> new_groups(n_vars + 1);
        std::set<Implicant> combined;

        auto combine_groups = [&](size_t i) {
            std::vector<Implicant> local_new;
            std::vector<Implicant> local_combined;
            for (const auto &imp1 : groups[i]) {
                for (const auto &imp2 : groups[i + 1]) {
                    uint64_t diff = imp1.term ^ imp2.term;
                    if (std::popcount(diff) == 1 && imp1.mask == imp2.mask) {
                        Implicant new_imp = { imp1.term & ~diff, imp1.mask & ~diff };
                        local_new.push_back(new_imp);
                        local_combined.push_back(imp1);
                        local_combined.push_back(imp2);
                    }
                }
            }
            return std::make_pair(local_new, local_combined);
        };

        if (n_vars < thread_threshold) {
            // Sequential processing
            for (size_t i = 0; i < n_vars; ++i) {
                auto [new_imps, comb] = combine_groups(i);
                if (!new_imps.empty()) {
                    new_groups[std::popcount(new_imps.front().term)] = std::move(new_imps);
                }
                combined.insert(comb.begin(), comb.end());
            }
        } else {
            // Parallel processing
            std::vector<std::mutex> group_mutex(n_vars + 1);
            std::mutex combined_mutex;
            std::vector<std::thread> threads;

            for (size_t i = 0; i < n_vars; ++i) {
                threads.emplace_back(
                    [&](size_t idx) {
                        auto [new_imps, comb] = combine_groups(idx);
                        if (!new_imps.empty()) {
                            std::lock_guard lock(group_mutex[std::popcount(new_imps.front().term)]);
                            new_groups[std::popcount(new_imps.front().term)] = std::move(new_imps);
                        }
                        {
                            std::lock_guard lock(combined_mutex);
                            combined.insert(comb.begin(), comb.end());
                        }
                    },
                    i);
            }
            for (auto &t : threads)
                t.join();
        }

        // Collect prime implicants
        for (const auto &group : groups) {
            for (const auto &imp : group) {
                if (!combined.contains(imp)) {
                    prime_implicants.emplace(imp.term, imp.mask);
                }
            }
        }

        groups = std::move(new_groups);
        if (std::all_of(groups.begin(), groups.end(), [](const auto &g) { return g.empty(); })) {
            if (debug) {
                std::cout << "Terminated after " << iter + 1 << " iterations" << std::endl;
            }
            break;
        }
    }

    // Select essential prime implicants
    std::vector<std::pair<uint64_t, uint64_t>> essential;
    std::set<size_t> covered;

    for (size_t m : minterms) {
        if (std::find(dont_cares.begin(), dont_cares.end(), m) != dont_cares.end()) {
            continue;
        }
        std::vector<std::pair<uint64_t, uint64_t>> covering;
        for (const auto &pi : prime_implicants) {
            if ((m & pi.second) == (pi.first & pi.second)) {
                covering.push_back(pi);
            }
        }
        if (covering.size() == 1) {
            auto pi = covering[0];
            if (std::find(essential.begin(), essential.end(), pi) == essential.end()) {
                essential.push_back(pi);
                for (size_t m2 : minterms) {
                    if ((m2 & pi.second) == (pi.first & pi.second)) {
                        covered.insert(m2);
                    }
                }
            }
        }
    }

    // Cover remaining minterms
    for (size_t m : minterms) {
        if (!covered.contains(m) &&
            std::find(dont_cares.begin(), dont_cares.end(), m) == dont_cares.end()) {
            for (const auto &pi : prime_implicants) {
                if ((m & pi.second) == (pi.first & pi.second)) {
                    if (std::find(essential.begin(), essential.end(), pi) == essential.end()) {
                        essential.push_back(pi);
                        for (size_t m2 : minterms) {
                            if ((m2 & pi.second) == (pi.first & pi.second)) {
                                covered.insert(m2);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    //if (print_implicant_table) {
    //    print_implicant_table(prime_implicants, essential, minterms, n_vars);
    //}

    // Generate boolean expression
    std::vector<std::string> terms;
    for (const auto &[term, mask] : essential) {
        std::vector<std::string> literals;
        for (size_t i = 0; i < n_vars; ++i) {
            if (mask & (1ull << (n_vars - 1 - i))) {
                char var = vars[i];
                literals.push_back((term & (1ull << (n_vars - 1 - i))) ? std::string(1, var)
                                                                       : "~" + std::string(1, var));
            }
        }
        terms.push_back(literals.empty() ? "1" : literals[0]);
        for (size_t i = 1; i < literals.size(); ++i) {
            terms.back() += literals[i];
        }
    }

    if (terms.empty())
        return "0";
    std::string result = terms[0];
    for (size_t i = 1; i < std::min<size_t>(terms.size(), 3); ++i) {
        result += " + " + terms[i];
    }
    return result;
}
