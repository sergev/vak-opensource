#include <algorithm>
#include <cmath>
#include <iostream>
#include <mutex>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <thread>

#include "utils.h"

static bool should_print_truth_table = true;
static bool should_print_implicant_table = true;
static bool debug = true;
size_t thread_threshold = 99;

std::string minimize_boolean_function(const std::string &truth_table)
{
    size_t n_vars = static_cast<size_t>(std::log2(truth_table.size()));
    if (truth_table.size() != (1ull << n_vars)) {
        throw std::runtime_error("Truth table length must be a power of 2");
    }
    if (n_vars > 64) {
        throw std::runtime_error("Number of variables exceeds uint64_t capacity (64)");
    }

    // Print truth table
    print_truth_table(truth_table, n_vars, should_print_truth_table);

    // Extract minterms and don't cares
    std::vector<size_t> minterms;
    std::vector<size_t> dont_cares;
    for (size_t i = 0; i < truth_table.size(); ++i) {
        if (truth_table[i] == '1') {
            minterms.push_back(i);
        } else if (truth_table[i] == 'X') {
            dont_cares.push_back(i);
        } else if (truth_table[i] != '0') {
            throw std::runtime_error("Truth table must contain only '0', '1', or 'X'");
        }
    }
    if (minterms.empty()) {
        return "0";
    }

    // Generate implicants
    std::vector<size_t> implicants;
    implicants.reserve(minterms.size() + dont_cares.size());
    implicants.insert(implicants.end(), minterms.begin(), minterms.end());
    implicants.insert(implicants.end(), dont_cares.begin(), dont_cares.end());
    if (implicants.empty()) {
        return "0";
    }

    if (debug) {
        std::cout << "Minterms: " << minterms.size() << ", Don't cares: " << dont_cares.size()
                  << std::endl;
    }

    // Group implicants by number of 1s
    std::vector<std::vector<std::tuple<std::optional<size_t>, uint64_t, uint64_t>>> groups(n_vars +
                                                                                           1);
    for (const auto &m : implicants) {
        uint64_t term = static_cast<uint64_t>(m);
        uint64_t mask = (1ull << n_vars) - 1; // All bits initially relevant
        size_t ones   = std::popcount(term);
        groups[ones].emplace_back(std::make_optional(m), term, mask);
    }

    // Quine-McCluskey: Combine implicants
    std::set<std::pair<uint64_t, uint64_t>> prime_implicants;
    size_t max_iterations = n_vars * 3;
    size_t iteration      = 0;

    while (true) {
        ++iteration;
        if (iteration > max_iterations) {
            std::cerr << "Warning: Reached max iterations (" << max_iterations
                      << ") in Quine-McCluskey loop" << std::endl;
            break;
        }

        if (debug) {
            size_t total_implicants = 0;
            size_t non_empty_groups = 0;
            for (const auto &g : groups) {
                total_implicants += g.size();
                if (!g.empty())
                    ++non_empty_groups;
            }
            std::cout << "Iteration " << iteration << ": " << non_empty_groups << " groups with "
                      << total_implicants << " implicants" << std::endl;
        }

        std::vector<std::vector<std::tuple<std::optional<size_t>, uint64_t, uint64_t>>> new_groups(
            n_vars + 1);
        std::set<std::tuple<std::optional<size_t>, uint64_t, uint64_t>> combined;

        if (n_vars < thread_threshold) {
            // Sequential processing
            for (size_t i = 0; i < n_vars; ++i) {
                const auto &group_i = groups[i];
                const auto &group_i1 =
                    (i + 1 < groups.size())
                        ? groups[i + 1]
                        : std::vector<std::tuple<std::optional<size_t>, uint64_t, uint64_t>>();
                for (const auto &[m1, t1, mask1] : group_i) {
                    for (const auto &[m2, t2, mask2] : group_i1) {
                        uint64_t xor_val = t1 ^ t2;
                        if (std::popcount(xor_val) == 1 && mask1 == mask2) {
                            uint64_t new_term = t1 & ~xor_val;
                            uint64_t new_mask = mask1 & ~xor_val;
                            size_t ones       = std::popcount(new_term);
                            new_groups[ones].emplace_back(std::nullopt, new_term, new_mask);
                            combined.emplace(m1, t1, mask1);
                            combined.emplace(m2, t2, mask2);
                        }
                    }
                }
            }
        } else {
            // Parallel processing with native threads
            std::vector<std::mutex> new_groups_mutex(n_vars + 1);
            std::mutex combined_mutex;
            std::vector<std::thread> threads;

            for (size_t i = 0; i < n_vars; ++i) {
                threads.emplace_back([i, &groups, &new_groups, &new_groups_mutex, &combined,
                                      &combined_mutex]() {
                    std::vector<std::tuple<size_t, std::optional<size_t>, uint64_t, uint64_t>>
                        local_new;
                    std::vector<std::tuple<std::optional<size_t>, uint64_t, uint64_t>>
                        local_combined;
                    const auto &group_i = groups[i];
                    const auto &group_i1 =
                        (i + 1 < groups.size())
                            ? groups[i + 1]
                            : std::vector<std::tuple<std::optional<size_t>, uint64_t, uint64_t>>();

                    for (const auto &[m1, t1, mask1] : group_i) {
                        for (const auto &[m2, t2, mask2] : group_i1) {
                            uint64_t xor_val = t1 ^ t2;
                            if (std::popcount(xor_val) == 1 && mask1 == mask2) {
                                uint64_t new_term = t1 & ~xor_val;
                                uint64_t new_mask = mask1 & ~xor_val;
                                size_t ones       = std::popcount(new_term);
                                local_new.emplace_back(ones, std::nullopt, new_term, new_mask);
                                local_combined.emplace_back(m1, t1, mask1);
                                local_combined.emplace_back(m2, t2, mask2);
                            }
                        }
                    }

                    for (const auto &[ones, m, t, mask] : local_new) {
                        std::lock_guard<std::mutex> lock(new_groups_mutex[ones]);
                        new_groups[ones].emplace_back(m, t, mask);
                    }
                    {
                        std::lock_guard<std::mutex> lock(combined_mutex);
                        combined.insert(local_combined.begin(), local_combined.end());
                    }
                });
            }

            for (auto &t : threads) {
                t.join();
            }
        }

        // Add uncombined terms as prime implicants
        for (const auto &group : groups) {
            for (const auto &[m, t, mask] : group) {
                if (combined.find(std::make_tuple(m, t, mask)) == combined.end()) {
                    prime_implicants.emplace(t, mask);
                }
            }
        }

        groups = std::move(new_groups);
        bool all_empty =
            std::all_of(groups.begin(), groups.end(), [](const auto &g) { return g.empty(); });
        if (all_empty) {
            if (debug) {
                std::cout << "Quine-McCluskey loop terminated after " << iteration << " iterations"
                          << std::endl;
            }
            break;
        }
    }

    // Select essential prime implicants
    std::vector<std::pair<uint64_t, uint64_t>> essential;
    std::set<size_t> covered;
    for (const auto &m : minterms) {
        if (std::find(dont_cares.begin(), dont_cares.end(), m) != dont_cares.end()) {
            continue;
        }
        std::vector<std::pair<uint64_t, uint64_t>> covering;
        for (const auto &[t, mask] : prime_implicants) {
            if ((static_cast<uint64_t>(m) & mask) == (t & mask)) {
                covering.emplace_back(t, mask);
            }
        }
        if (covering.empty()) {
            if (debug) {
                std::cerr << "No covering implicant for minterm " << m << std::endl;
            }
            continue;
        }
        if (covering.size() == 1) {
            auto pi = covering[0];
            if (std::find(essential.begin(), essential.end(), pi) == essential.end()) {
                essential.push_back(pi);
                auto [t, mask] = pi;
                for (const auto &m2 : minterms) {
                    if ((static_cast<uint64_t>(m2) & mask) == (t & mask)) {
                        covered.insert(m2);
                    }
                }
            }
        }
    }

    // Cover remaining minterms
    for (const auto &m : minterms) {
        if (covered.find(m) == covered.end() &&
            std::find(dont_cares.begin(), dont_cares.end(), m) == dont_cares.end()) {
            std::vector<std::pair<uint64_t, uint64_t>> covering;
            for (const auto &[t, mask] : prime_implicants) {
                if ((static_cast<uint64_t>(m) & mask) == (t & mask)) {
                    covering.emplace_back(t, mask);
                }
            }
            if (!covering.empty()) {
                auto pi = covering[0];
                if (std::find(essential.begin(), essential.end(), pi) == essential.end()) {
                    essential.push_back(pi);
                    auto [t, mask] = pi;
                    for (const auto &m2 : minterms) {
                        if ((static_cast<uint64_t>(m2) & mask) == (t & mask)) {
                            covered.insert(m2);
                        }
                    }
                }
            }
        }
    }

    // Print implicant table
    print_implicant_table(prime_implicants, essential, minterms, n_vars,
                          should_print_implicant_table);

    // Convert to Boolean expression
    static const char VARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::vector<std::string> terms;
    for (const auto &[term, mask] : essential) {
        std::vector<std::string> literals;
        for (size_t i = 0; i < n_vars; ++i) {
            if (mask & (1ull << (n_vars - 1 - i))) {
                char var = VARS[i];
                if (term & (1ull << (n_vars - 1 - i))) {
                    literals.emplace_back(1, var);
                } else {
                    literals.emplace_back("~" + std::string(1, var));
                }
            }
        }
        terms.push_back(literals.empty()
                            ? "1"
                            : std::accumulate(literals.begin(), literals.end(), std::string()));
    }
    std::string expression = terms.empty() ? "0"
                                           : terms[0] + (terms.size() > 1 ? " + " + terms[1] : "") +
                                                 (terms.size() > 2 ? " + " + terms[2] : "");
    return expression;
}
