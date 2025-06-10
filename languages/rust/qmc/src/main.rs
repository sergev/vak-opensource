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

fn minimize_boolean_function(truth_table_str: &str) -> Result<String, String> {
    // Parse and validate truth table
    let truth_table = parse_truth_table(truth_table_str)?;
    let n_vars = (truth_table.len() as f64).log2() as usize;
    if truth_table.len() != 1 << n_vars {
        return Err("Truth table length must be a power of 2".to_string());
    }

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
        let new_groups: Vec<Mutex<Vec<(Option<usize>, BitVec)>>> = (0..=n_vars)
            .map(|_| Mutex::new(vec![]))
            .collect();
        let combined: Mutex<HashSet<(Option<usize>, BitVec)>> = Mutex::new(HashSet::new());

        // Parallelize comparison across groups
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
                        new_groups[ones].lock().unwrap().push((None, new_term.clone()));
                        let mut combined_lock = combined.lock().unwrap();
                        combined_lock.insert((*m1, t1.clone()));
                        combined_lock.insert((*m2, t2.clone()));
                    }
                }
            }
        });

        // Add uncombined terms as prime implicants
        for group in &groups {
            for (m, t) in group {
                if !combined.lock().unwrap().contains(&(*m, t.clone())) {
                    let mask = BitVec::repeat(true, n_vars);
                    prime_implicants.insert((t.clone(), mask));
                }
            }
        }

        // Update groups
        groups = new_groups.into_iter()
            .map(|m| m.into_inner().unwrap())
            .collect();
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
                    .all(|(m_bit, (t_bit, mask_bit))| !mask_bit | (m_bit == t_bit))
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
                        .all(|(m_bit, (t_bit, mask_bit))| !mask_bit | (m_bit == t_bit)) {
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
                    .all(|(m_bit, (t_bit, mask_bit))| !mask_bit | (m_bit == t_bit)) {
                    essential.push(pi.clone());
                    for &m2 in &minterms {
                        let m2_bv = to_bitvec(m2);
                        if m2_bv.iter().zip(t.iter().zip(mask.iter()))
                            .all(|(m_bit, (t_bit, mask_bit))| !mask_bit | (m_bit == t_bit)) {
                            covered.insert(m2);
                        }
                    }
                    break;
                }
            }
        }
    }

    // Convert to Boolean expression
    static VARS: [char; 26] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];
    let expression = essential.iter().map(|(term, mask)| {
        let result = term.iter().zip(mask.iter()).enumerate()
            .filter(|(_, (_, mask_bit))| *mask_bit)
            .map(|(i, (term_bit, _))| {
                let var = VARS[i];
                if *term_bit {
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
