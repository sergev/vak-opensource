use crate::{TruthValue, print_implicant_table};
use itertools::Itertools;
use rayon::prelude::*;
use std::collections::HashSet;
use std::sync::Mutex;

pub fn minimize_boolean_function(
    truth_table_str: &str,
    should_print_truth_table: bool,
    should_print_implicant_table: bool,
    debug: bool,
    rayon_threshold: usize,
) -> Result<String, String> {
    // Parse and validate truth table
    let truth_table = crate::parse_truth_table(truth_table_str)?;
    let n_vars = (truth_table.len() as f64).log2() as usize;
    if truth_table.len() != 1 << n_vars {
        return Err("Truth table length must be a power of 2".to_string());
    }
    if n_vars > 64 {
        return Err("Number of variables exceeds u64 capacity (64)".to_string());
    }

    // Print truth table
    crate::print_truth_table(&truth_table, n_vars, should_print_truth_table);

    // Extract minterms and don't cares
    let minterms = truth_table.iter().enumerate()
        .filter(|(_, v)| **v == TruthValue::One)
        .map(|(i, _)| i)
        .collect_vec();
    let dont_cares = truth_table.iter().enumerate()
        .filter(|(_, v)| **v == TruthValue::DontCare)
        .map(|(i, _)| i)
        .collect_vec();
    if minterms.is_empty() {
        return Ok("0".to_string());
    }

    // Generate implicants
    let implicants = minterms.iter().chain(dont_cares.iter()).copied().collect_vec();
    if implicants.is_empty() {
        return Ok("0".to_string());
    }

    // Group implicants by number of 1s
    let mut groups: Vec<Vec<(Option<usize>, u64, u64)>> = vec![vec![]; n_vars + 1];
    for &m in &implicants {
        let term = m as u64;
        let mask = (1u64 << n_vars) - 1; // All bits initially relevant
        let ones = term.count_ones() as usize;
        groups[ones].push((Some(m), term, mask));
    }

    // Quine-McCluskey: Combine implicants
    let mut prime_implicants: HashSet<(u64, u64)> = HashSet::new();
    let max_iterations = n_vars * 2; // Prevent infinite loops
    let mut iteration = 0;

    loop {
        iteration += 1;
        if iteration > max_iterations {
            eprintln!("Warning: Reached max iterations ({}) in Quine-McCluskey loop", max_iterations);
            break;
        }

        if debug {
            println!("Iteration {}: {} groups with implicants", iteration, groups.iter().filter(|g| !g.is_empty()).count());
        }

        let mut new_groups: Vec<Vec<(Option<usize>, u64, u64)>> = vec![vec![]; n_vars + 1];
        let mut combined: HashSet<(Option<usize>, u64, u64)> = HashSet::new();

        // Choose sequential or parallel based on n_vars
        if n_vars < rayon_threshold {
            // Sequential processing
            for i in 0..n_vars {
                let group_i = &groups[i];
                let group_i1 = &groups[i + 1];
                for (m1, t1, mask1) in group_i {
                    for (m2, t2, mask2) in group_i1 {
                        let xor = t1 ^ t2;
                        if xor.count_ones() == 1 && mask1 == mask2 {
                            let new_term = t1 & !xor;
                            let new_mask = *mask1 & !xor;
                            let ones = new_term.count_ones() as usize;
                            new_groups[ones].push((None, new_term, new_mask));
                            combined.insert((*m1, *t1, *mask1));
                            combined.insert((*m2, *t2, *mask2));
                        }
                    }
                }
            }
        } else {
            // Parallel processing
            let new_groups_mutex: Vec<Mutex<Vec<(Option<usize>, u64, u64)>>> = (0..=n_vars)
                .map(|_| Mutex::new(vec![]))
                .collect();
            let combined_mutex: Mutex<HashSet<(Option<usize>, u64, u64)>> = Mutex::new(HashSet::new());

            (0..n_vars).into_par_iter().for_each(|i| {
                let group_i = &groups[i];
                let group_i1 = &groups[i + 1];
                for (m1, t1, mask1) in group_i {
                    for (m2, t2, mask2) in group_i1 {
                        let xor = t1 ^ t2;
                        if xor.count_ones() == 1 && mask1 == mask2 {
                            let new_term = t1 & !xor;
                            let new_mask = *mask1 & !xor;
                            let ones = new_term.count_ones() as usize;
                            new_groups_mutex[ones].lock().unwrap().push((None, new_term, new_mask));
                            let mut combined_lock = combined_mutex.lock().unwrap();
                            combined_lock.insert((*m1, *t1, *mask1));
                            combined_lock.insert((*m2, *t2, *mask2));
                        }
                    }
                }
            });

            new_groups = new_groups_mutex.into_iter()
                .map(|m| m.into_inner().unwrap())
                .collect();
            combined = combined_mutex.into_inner().unwrap();
        }

        // Add uncombined terms as prime implicants
        for group in &groups {
            for (m, t, mask) in group {
                if !combined.contains(&(*m, *t, *mask)) {
                    prime_implicants.insert((*t, *mask));
                }
            }
        }

        groups = new_groups;
        if groups.iter().all(|g| g.is_empty()) {
            if debug {
                println!("Quine-McCluskey loop terminated after {} iterations", iteration);
            }
            break;
        }
    }

    // Select essential prime implicants
    let mut essential: Vec<(u64, u64)> = vec![];
    let mut covered: HashSet<usize> = HashSet::new();
    for &m in &minterms {
        if dont_cares.contains(&m) {
            continue;
        }
        let covering = prime_implicants.iter()
            .filter(|&&(t, mask)| (m as u64 & mask) == (t & mask))
            .copied()
            .collect_vec();
        if covering.len() == 1 {
            let pi = covering[0];
            if !essential.contains(&pi) {
                essential.push(pi);
                let (t, mask) = pi;
                for &m2 in &minterms {
                    if (m2 as u64 & mask) == (t & mask) {
                        covered.insert(m2);
                    }
                }
            }
        }
    }
    // Cover remaining minterms
    for &m in &minterms {
        if !covered.contains(&m) && !dont_cares.contains(&m) {
            for &(t, mask) in &prime_implicants {
                if !essential.contains(&(t, mask)) && (m as u64 & mask) == (t & mask) {
                    essential.push((t, mask));
                    for &m2 in &minterms {
                        if (m2 as u64 & mask) == (t & mask) {
                            covered.insert(m2);
                        }
                    }
                    break;
                }
            }
        }
    }

    // Print implicant table
    print_implicant_table(&prime_implicants, &essential, &minterms, n_vars, should_print_implicant_table);

    // Convert to Boolean expression
    static VARS: [char; 26] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];
    let expression = essential.iter().map(|&(term, mask)| {
        let result = (0..n_vars)
            .filter(|&i| mask & (1 << i) != 0)
            .map(|i| {
                let var = VARS[i];
                if term & (1 << i) != 0 {
                    var.to_string()
                } else {
                    format!("~{}", var)
                }
            })
            .collect_vec();
        if result.is_empty() { "1".to_string() } else { result.join("") }
    }).collect_vec().join(" + ");
    Ok(if expression.is_empty() { "1".to_string() } else { expression })
}
