use itertools::Itertools;
use rayon::prelude::*;
use std::collections::HashSet;
use std::sync::Mutex;

#[derive(Clone, PartialEq, Eq)]
enum TruthValue {
    Zero,
    One,
    DontCare,
}

fn parse_truth_table(input: &str) -> Result<Vec<TruthValue>, String> {
    input.chars().map(|c| match c.to_ascii_uppercase() {
        '0' => Ok(TruthValue::Zero),
        '1' => Ok(TruthValue::One),
        'X' => Ok(TruthValue::DontCare),
        _ => Err(format!("Invalid character: {}", c)),
    }).collect()
}

// Pretty-print truth table
fn print_truth_table(truth_table: &[TruthValue], n_vars: usize) {
    static VARS: [char; 26] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];

    println!("\nTruth Table:");
    let header = VARS[..n_vars].iter().map(|v| v.to_string()).collect_vec().join(" | ") + " | Minterm | Output";
    let width = header.len() + (n_vars + 2) * 3;
    println!("{:-<width$}", "");
    println!("| {} |", header);
        println!("{:-<width$}", "");

    let max_rows = if n_vars >= 5 { 5 } else { truth_table.len() };
    let total_rows = truth_table.len();

    for i in 0..max_rows {
        let bits = (0..n_vars).rev()
            .map(|j| if (i & (1 << j)) != 0 { "1" } else { "0" })
            .collect_vec()
            .join(" | ");
        let output = match &truth_table[i] {
            TruthValue::Zero => "0",
            TruthValue::One => "1",
            TruthValue::DontCare => "X",
        };
        println!("| {} | {:<7} | {:<6} |", bits, i, output);
    }

    if max_rows < total_rows {
        println!("| {:^width$} |", "...");
        for i in (total_rows - max_rows)..total_rows {
            let bits = (0..n_vars).rev()
                .map(|j| if (i & (1 << j)) != 0 { "1" } else { "0" })
            .collect_vec()
            .join(" | ");
            let output = match &truth_table[i] {
                TruthValue::Zero => "0",
                TruthValue::One => "1",
                TruthValue::DontCare => "X",
            };
            println!("| {} | {:<7} | {:<6} |", bits, i, output);
        }
    }

    println!("{:-<width$}", "");
}

// Pretty-print implicant table
fn print_implicant_table(
    prime_implicants: &HashSet<(u64, u64)>,
    essential: &[(u64, u64)],
    minterms: &[usize],
    n_vars: usize,
) {
    static VARS: [char; 26] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];

    println!("\nImplicant Table:");
    println!("{:-<80}", "");
    println!(
        "| {:<20} | {:<15} | {:<40} | {:<5} |",
        "Implicant", "Binary", "Minterms Covered", "Essential"
    );
    println!("{:-<80}", "");

    let to_term = |term: u64, mask: u64| -> String {
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
    };

    let to_binary = |term: u64, mask: u64| -> String {
        (0..n_vars)
            .rev()
            .map(|i| {
                if mask & (1 << i) == 0 {
                    '-'
                } else if term & (1 << i) != 0 {
                    '1'
                } else {
                    '0'
                }
            })
            .collect::<String>()
    };

    let covers_minterm = |term: u64, mask: u64, m: usize| -> bool {
        (m as u64 & mask) == (term & mask)
    };

    for &(term, mask) in prime_implicants {
        let covered = minterms.iter()
            .filter(|&&m| covers_minterm(term, mask, m))
            .map(|m| m.to_string())
            .collect_vec()
            .join(", ");
        let is_essential = essential.contains(&(term, mask));
        println!(
            "| {:<20} | {:<15} | {:<40} | {:<5} |",
            to_term(term, mask),
            to_binary(term, mask),
            covered,
            if is_essential { "Yes" } else { "No" }
        );
    }

    println!("{:-<80}", "");
}

fn minimize_boolean_function(truth_table_str: &str) -> Result<String, String> {
    // Parse and validate truth table
    let truth_table = parse_truth_table(truth_table_str)?;
    let n_vars = (truth_table.len() as f64).log2() as usize;
    if truth_table.len() != 1 << n_vars {
        return Err("Truth table length must be a power of 2".to_string());
    }
    if n_vars > 64 {
        return Err("Number of variables exceeds u64 capacity (64)".to_string());
    }

    // Print truth table
    print_truth_table(&truth_table, n_vars);

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

        println!("Iteration {}: {} groups with implicants", iteration, groups.iter().filter(|g| !g.is_empty()).count());

        let mut new_groups: Vec<Vec<(Option<usize>, u64, u64)>> = vec![vec![]; n_vars + 1];
        let mut combined: HashSet<(Option<usize>, u64, u64)> = HashSet::new();

        // Choose sequential or parallel based on n_vars
        if n_vars < 5 {
            // Sequential processing
            for i in 0..n_vars {
                let group_i = &groups[i];
                let group_i1 = &groups[i + 1];
                for (m1, t1, mask1) in group_i {
                    for (m2, t2, mask2) in group_i1 {
                        let xor = t1 ^ t2;
                        if xor.count_ones() == 1 && mask1 == mask2 {
                            let new_term = t1 & !xor;
                            let new_mask = *mask1 & !xor; // Update mask for differing bit
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
            println!("Quine-McCluskey loop terminated after {} iterations", iteration);
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
    print_implicant_table(&prime_implicants, &essential, &minterms, n_vars);

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

// 8-variable truth table: A~BCD + E~FG + ~ABH
fn generate_8var_truth_table() -> String {
    let mut table = vec!['0'; 256];
    // A=1, B=0, C=1, D=1 (1011xxxx): only 1s, no Xs
    for efgh in [0, 2, 4, 6, 8, 10, 12, 14] { // Even for 1s
        let m = (1 << 7) | (0 << 6) | (1 << 5) | (1 << 4) | efgh;
        table[m] = '1';
    }
    // E=1, F=0, G=1 (xxxx101x): only 1s, no Xs
    for abcdh in [0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30] { // Divisible by 3
        let m = (abcdh >> 4) << 7 | (1 << 3) | (0 << 2) | (1 << 1) | (abcdh & 1);
        table[m] = '1';
    }
    // A=0, B=1, H=1 (01xxxxx1): only 1s, no Xs
    for cdefg in [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30] { // Even
        let m = (0 << 7) | (1 << 6) | (cdefg << 1) | 1;
        table[m] = '1';
    }
    table.into_iter().collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_3_variables() {
        let truth_table = "01X010X1";
        let result = minimize_boolean_function(truth_table).unwrap();
        assert!(!result.is_empty());
        println!("3-variable result: {}", result);
    }

    #[test]
    fn test_4_variables() {
        let truth_table = "01X01001X00101X1";
        let result = minimize_boolean_function(truth_table).unwrap();
        assert!(!result.is_empty());
        println!("4-variable result: {}", result);
    }

    #[test]
    fn test_8_variables() {
        let truth_table = generate_8var_truth_table();
        let result = minimize_boolean_function(&truth_table).unwrap();
        assert!(!result.is_empty());
        println!("8-variable result: {}", result);
    }
}

fn main() {
    let truth_table_8 = generate_8var_truth_table();
    match minimize_boolean_function(&truth_table_8) {
        Ok(result) => println!("8-variable result: {}", result),
        Err(e) => println!("Error: {}", e),
    }
}
