use bitvec::vec::BitVec;
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
    prime_implicants: &HashSet<(BitVec, BitVec)>,
    essential: &[(BitVec, BitVec)],
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

    let to_term = |term: &BitVec, mask: &BitVec| -> String {
        let result = term.iter().zip(mask.iter()).enumerate()
            .filter(|(_, (_, mask_bit))| **mask_bit)
            .map(|(i, (term_bit, _))| {
                let var = VARS[i];
                if **term_bit {
                    var.to_string()
                } else {
                    format!("~{}", var)
                }
            })
            .collect_vec();
        if result.is_empty() { "1".to_string() } else { result.join("") }
    };

    let to_binary = |term: &BitVec, mask: &BitVec| -> String {
        term.iter().zip(mask.iter())
            .map(|(t, m)| if !**m { '-' } else if **t { '1' } else { '0' })
            .collect::<String>()
    };

    let covers_minterm = |term: &BitVec, mask: &BitVec, m: usize| -> bool {
        let m_bv = {
            let mut bv = BitVec::new();
            bv.resize(n_vars, false);
            for i in 0..n_vars {
                bv.set(i, (m & (1 << (n_vars - 1 - i))) != 0);
            }
            bv
        };
        m_bv.iter().zip(term.iter().zip(mask.iter()))
            .all(|(m_bit, (t_bit, mask_bit))| !**mask_bit || (**m_bit == **t_bit))
    };

    for (term, mask) in prime_implicants {
        let covered = minterms.iter()
            .filter(|&&m| covers_minterm(term, mask, m))
            .map(|m| m.to_string())
            .collect_vec()
            .join(", ");
        let is_essential = essential.contains(&(term.clone(), mask.clone()));
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

    // Convert minterms to BitVec
    let to_bitvec = |m: usize| -> BitVec {
        let mut bv = BitVec::new();
        bv.resize(n_vars, false);
        for i in 0..n_vars {
            bv.set(i, (m & (1 << (n_vars - 1 - i))) != 0);
        }
        bv
    };

    // Group implicants by number of 1s
    let mut groups: Vec<Vec<(Option<usize>, BitVec)>> = vec![vec![]; n_vars + 1];
    for &m in &implicants {
        let bv = to_bitvec(m);
        let ones = bv.count_ones();
        groups[ones].push((Some(m), bv));
    }

    // Quine-McCluskey: Combine implicants
    let mut prime_implicants: HashSet<(BitVec, BitVec)> = HashSet::new();
    loop {
        let mut new_groups: Vec<Vec<(Option<usize>, BitVec)>> = vec![vec![]; n_vars + 1];
        let mut combined: HashSet<(Option<usize>, BitVec)> = HashSet::new();

        // Choose sequential or parallel based on n_vars
        if n_vars < 5 {
            // Sequential processing
            for i in 0..n_vars {
                let group_i = &groups[i];
                let group_i1 = &groups[i + 1];
                for (m1, t1) in group_i {
                    for (m2, t2) in group_i1 {
                        let xor = t1.clone() ^ t2.clone();
                        if xor.count_ones() == 1 {
                            let mut new_term = t1.clone();
                            let diff_idx = xor.iter_ones().next().unwrap();
                            new_term.set(diff_idx, false);
                            let ones = new_term.count_ones();
                            new_groups[ones].push((None, new_term.clone()));
                            combined.insert((*m1, t1.clone()));
                            combined.insert((*m2, t2.clone()));
                        }
                    }
                }
            }
        } else {
            // Parallel processing
            let new_groups_mutex: Vec<Mutex<Vec<(Option<usize>, BitVec)>>> = (0..=n_vars)
                .map(|_| Mutex::new(vec![]))
                .collect();
            let combined_mutex: Mutex<HashSet<(Option<usize>, BitVec)>> = Mutex::new(HashSet::new());

            (0..n_vars).par_iter().for_each(|&i| {
                let group_i = &groups[i];
                let group_i1 = &groups[i + 1];
                for (m1, t1) in group_i {
                    for (m2, t2) in group_i1 {
                        let xor = t1.clone() ^ t2.clone();
                        if xor.count_ones() == 1 {
                            let mut new_term = t1.clone();
                            let diff_idx = xor.iter_ones().next().unwrap();
                            new_term.set(diff_idx, false);
                            let ones = new_term.count_ones();
                            new_groups_mutex[ones].lock().unwrap().push((None, new_term.clone()));
                            let mut combined_lock = combined_mutex.lock().unwrap();
                            combined_lock.insert((*m1, t1.clone()));
                            combined_lock.insert((*m2, t2.clone()));
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
            for (m, t) in group {
                if !combined.contains(&(*m, t.clone())) {
                    let mask = BitVec::repeat(true, n_vars);
                    prime_implicants.insert((t.clone(), mask));
                }
            }
        }

        groups = new_groups;
        if groups.iter().all(|g| g.is_empty()) {
            break;
        }
    }

    // Select essential prime implicants
    let mut essential: Vec<(BitVec, BitVec)> = vec![];
    let mut covered: HashSet<usize> = HashSet::new();
    for &m in &minterms {
        if dont_cares.contains(&m) {
            continue;
        }
        let m_bv = to_bitvec(m);
        let covering = prime_implicants.iter()
            .filter(|(t, mask)| {
                m_bv.iter().zip(t.iter().zip(mask.iter()))
                    .all(|(m_bit, (t_bit, mask_bit))| !**mask_bit || (**m_bit == **t_bit))
            })
            .cloned()
            .collect_vec();
        if covering.len() == 1 {
            let pi = covering[0].clone();
            if !essential.contains(&pi) {
                essential.push(pi.clone());
                let (t, mask) = &pi;
                for &m2 in &minterms {
                    let m2_bv = to_bitvec(m2);
                    if m2_bv.iter().zip(t.iter().zip(mask.iter()))
                        .all(|(m_bit, (t_bit, mask_bit))| !**mask_bit || (**m_bit == **t_bit)) {
                        covered.insert(m2);
                    }
                }
            }
        }
    }
    // Cover remaining minterms
    for &m in &minterms {
        if !covered.contains(&m) && !dont_cares.contains(&m) {
            let m_bv = to_bitvec(m);
            for pi in &prime_implicants {
                let (t, mask) = pi;
                if !essential.contains(pi) && m_bv.iter().zip(t.iter().zip(mask.iter()))
                    .all(|(m_bit, (t_bit, mask_bit))| !**mask_bit || (**m_bit == **t_bit)) {
                    essential.push(pi.clone());
                    for &m2 in &minterms {
                        let m2_bv = to_bitvec(m2);
                        if m2_bv.iter().zip(t.iter().zip(mask.iter()))
                            .all(|(m_bit, (t_bit, mask_bit))| !**mask_bit || (**m_bit == **t_bit)) {
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
    let expression = essential.iter().map(|(term, mask)| {
        let result = term.iter().zip(mask.iter()).enumerate()
            .filter(|(_, (_, mask_bit))| **mask_bit)
            .map(|(i, (term_bit, _))| {
                let var = VARS[i];
                if **term_bit {
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
    // A=1, B=0, C=1, D=1 (1011xxxx)
    for efgh in 0..16 {
        let m = (1 << 7) | (0 << 6) | (1 << 5) | (1 << 4) | efgh;
        table[m] = if efgh % 2 == 0 { '1' } else { 'X' };
    }
    // E=1, F=0, G=1 (xxxx101x)
    for abcdh in 0..32 {
        let m = (abcdh >> 4) << 7 | (1 << 3) | (0 << 2) | (1 << 1) | (abcdh & 1);
        table[m] = if abcdh % 3 == 0 { '1' } else { 'X' };
    }
    // A=0, B=1, H=1 (01xxxxx1)
    for cdefg in 0..32 {
        let m = (0 << 7) | (1 << 6) | (cdefg << 1) | 1;
        table[m] = if cdefg % 2 == 0 { '1' } else { 'X' };
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
