use std::collections::{HashSet, HashMap};
use std::char;

#[derive(Clone, PartialEq, Eq, Hash)]
enum TruthValue {
    Zero,
    One,
    DontCare,
}

fn parse_truth_table(input: &str) -> Result<Vec<TruthValue>, String> {
    input.chars().map(|c| match c {
        '0' => Ok(TruthValue::Zero),
        '1' => Ok(TruthValue::One),
        'X' | 'x' => Ok(TruthValue::DontCare),
        _ => Err(format!("Invalid character in truth table: {}", c)),
    }).collect()
}

fn minimize_boolean_function(truth_table_str: &str) -> Result<String, String> {
    // Step 1: Parse truth table string
    let truth_table = parse_truth_table(truth_table_str)?;

    // Step 2: Determine number of variables
    let n_vars = (truth_table.len() as f64).log2() as usize;
    if truth_table.len() != 1 << n_vars {
        return Err("Truth table length must be a power of 2".to_string());
    }

    // Step 3: Extract minterms (1s) and don't cares (Xs)
    let minterms: Vec<usize> = truth_table.iter().enumerate()
        .filter(|(_, v)| **v == TruthValue::One)
        .map(|(i, _)| i)
        .collect();
    let dont_cares: Vec<usize> = truth_table.iter().enumerate()
        .filter(|(_, v)| **v == TruthValue::DontCare)
        .map(|(i, _)| i)
        .collect();
    if minterms.is_empty() {
        return Ok("0".to_string());
    }

    // Step 4: Generate implicants (minterms + don't cares)
    let mut implicants = minterms.clone();
    implicants.extend(dont_cares.iter());
    if implicants.is_empty() {
        return Ok("0".to_string());
    }

    // Step 5: Group implicants by number of 1s
    fn count_ones(n: &str) -> usize {
        n.chars().filter(|&c| c == '1').count()
    }

    fn count_ones_int(n: usize) -> usize {
        n.count_ones() as usize
    }

    let mut groups: HashMap<usize, Vec<(Option<usize>, String)>> = HashMap::new();
    for i in 0..=n_vars {
        groups.insert(i, vec![]);
    }
    for &m in &implicants {
        let binary = format!("{:0width$b}", m, width=n_vars);
        groups.get_mut(&count_ones_int(m)).unwrap().push((Some(m), binary));
    }

    // Step 6: Quine-McCluskey: Combine implicants differing by one bit
    fn differ_by_one(s1: &str, s2: &str) -> bool {
        s1.chars().zip(s2.chars()).filter(|&(a, b)| a != b).count() == 1
    }

    fn combine_terms(s1: &str, s2: &str) -> String {
        s1.chars().zip(s2.chars())
            .map(|(a, b)| if a != b { '-' } else { a })
            .collect()
    }

    let mut prime_implicants: HashSet<String> = HashSet::new();
    let mut used: HashSet<usize> = HashSet::new();
    loop {
        let mut new_groups: HashMap<usize, Vec<(Option<usize>, String)>> = HashMap::new();
        for i in 0..=n_vars {
            new_groups.insert(i, vec![]);
        }
        let mut combined: HashSet<(Option<usize>, String)> = HashSet::new();
        for i in 0..n_vars {
            for (m1, s1) in groups.get(&i).unwrap_or(&vec![]) {
                for (m2, s2) in groups.get(&(i + 1)).unwrap_or(&vec![]) {
                    if differ_by_one(s1, s2) {
                        let new_term = combine_terms(s1, s2);
                        new_groups.get_mut(&count_ones(&new_term)).unwrap()
                            .push((None, new_term.clone()));
                        combined.insert((m1.clone(), s1.clone()));
                        combined.insert((m2.clone(), s2.clone()));
                        if let Some(m1_val) = m1 {
                            used.insert(*m1_val);
                        }
                        if let Some(m2_val) = m2 {
                            used.insert(*m2_val);
                        }
                    }
                }
            }
        }
        // Add uncombined terms as prime implicants
        for i in 0..=n_vars {
            for (m, s) in groups.get(&i).unwrap_or(&vec![]) {
                if !combined.contains(&(*m, s.clone())) {
                    prime_implicants.insert(s.clone());
                }
            }
        }
        // Update groups
        groups = new_groups;
        if groups.values().all(|v| v.is_empty()) {
            break;
        }
    }

    // Step 7: Select essential prime implicants
    fn covers_implicant(term: &str, minterm: usize, n_vars: usize) -> bool {
        let minterm_bits = format!("{:0width$b}", minterm, width=n_vars);
        term.chars().zip(minterm_bits.chars())
            .all(|(t, m)| t == '-' || t == m)
    }

    let mut essential: Vec<String> = vec![];
    let mut covered: HashSet<usize> = HashSet::new();
    for &m in &minterms {
        if dont_cares.contains(&m) {
            continue;
        }
        let covering: Vec<String> = prime_implicants.iter()
            .filter(|pi| covers_implicant(pi, m, n_vars))
            .cloned()
            .collect();
        if covering.len() == 1 {
            let pi = covering[0].clone();
            if !essential.contains(&pi) {
                essential.push(pi.clone());
            }
            for m2 in minterms.iter().filter(|m2| covers_implicant(&pi, **m2, n_vars)) {
                covered.insert(*m2);
            }
        }
    }
    // Add additional implicants if needed
    for &m in &minterms {
        if !covered.contains(&m) && !dont_cares.contains(&m) {
            for pi in &prime_implicants {
                if !essential.contains(pi) && covers_implicant(pi, m, n_vars) {
                    essential.push(pi.clone());
                    for m2 in minterms.iter().filter(|m2| covers_implicant(pi, **m2, n_vars)) {
                        covered.insert(*m2);
                    }
                    break;
                }
            }
        }
    }

    // Step 8: Convert prime implicants to Boolean expression
    fn term_to_expression(term: &str, n_vars: usize) -> String {
        let vars: Vec<char> = (0..n_vars).map(|i| char::from_u32('A' as u32 + i as u32).unwrap()).collect();
        let mut result: Vec<String> = vec![];
        for (v, t) in vars.iter().zip(term.chars()) {
            match t {
                '1' => result.push(v.to_string()),
                '0' => result.push(format!("~{}", v)),
                _ => {},
            }
        }
        if result.is_empty() { "1".to_string() } else { result.join("") }
    }

    if essential.is_empty() {
        return Ok("0".to_string());
    }
    let expression = essential.iter()
        .map(|pi| term_to_expression(pi, n_vars))
        .collect::<Vec<String>>()
        .join(" + ");
    Ok(if expression.is_empty() { "1".to_string() } else { expression })
}

// Helper function to generate 8-variable truth table
fn generate_8var_truth_table() -> String {
    let mut table = vec!['0'; 256];
    // Pattern 1: A=1, B=0, C=1, D=1 (E,F,G,H any) -> 1 or X
    for efgh in 0..16 {
        let minterm = (1 << 7) | (0 << 6) | (1 << 5) | (1 << 4) | efgh; // 1011xxxx
        table[minterm] = if efgh % 2 == 0 { '1' } else { 'X' };
    }
    // Pattern 2: E=1, F=0, G=1 (A,B,C,D,H any) -> 1 or X
    for abcdh in 0..32 {
        let minterm = (abcdh >> 4) << 7 | (1 << 3) | (0 << 2) | (1 << 1) | (abcdh & 1); // xxxx101x
        table[minterm] = if abcdh % 3 == 0 { '1' } else { 'X' };
    }
    // Pattern 3: A=0, B=1, H=1 (C,D,E,F,G any) -> 1 or X
    for cdefg in 0..32 {
        let minterm = (0 << 7) | (1 << 6) | (cdefg << 1) | 1; // 01xxxxx1
        table[minterm] = if cdefg % 2 == 0 { '1' } else { 'X' };
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
        println!("3-variable result: {}", result);
    }

    #[test]
    fn test_4_variables() {
        let truth_table = "01X01001X00101X1";
        let result = minimize_boolean_function(truth_table).unwrap();
        println!("4-variable result: {}", result);
    }

    #[test]
    fn test_8_variables() {
        let truth_table = generate_8var_truth_table();
        let result = minimize_boolean_function(&truth_table).unwrap();
        println!("8-variable result: {}", result);
    }
}

fn main() {
    // Example usage for 8-variable function
    let truth_table_8 = generate_8var_truth_table();
    println!("Truth table: {}", truth_table_8);
    match minimize_boolean_function(&truth_table_8) {
        Ok(result) => println!("8-variable result: {}", result),
        Err(e) => println!("Error: {}", e),
    }
}
