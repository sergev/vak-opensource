use clap::Parser;
use itertools::Itertools;
use minimizer::minimize_boolean_function;
use std::collections::HashSet;

mod minimizer;

#[derive(Clone, PartialEq, Eq)]
pub enum TruthValue {
    Zero,
    One,
    DontCare,
}

#[derive(Parser, Debug)]
#[command(author, version, about = "Boolean function minimizer using Quine-McCluskey", long_about = None)]
struct Args {
    /// Truth table as a string of 0s, 1s, and Xs (e.g., 01X010X1)
    #[arg(long, default_value = None)]
    truth_table: Option<String>,

    /// Enable printing of the truth table
    #[arg(long, default_value_t = true)]
    print_truth_table: bool,

    /// Disable printing of the truth table
    #[arg(long, conflicts_with = "print_truth_table")]
    no_print_truth_table: bool,

    /// Enable printing of the implicant table
    #[arg(long, default_value_t = true)]
    print_implicant_table: bool,

    /// Disable printing of the implicant table
    #[arg(long, conflicts_with = "print_implicant_table")]
    no_print_implicant_table: bool,

    /// Enable debug logging for Quine-McCluskey iterations
    #[arg(long, default_value_t = false)]
    debug: bool,

    /// Number of variables below which sequential processing is used
    #[arg(long, default_value_t = 5)]
    rayon_threshold: usize,
}

pub fn parse_truth_table(input: &str) -> Result<Vec<TruthValue>, String> {
    input.chars().map(|c| match c.to_ascii_uppercase() {
        '0' => Ok(TruthValue::Zero),
        '1' => Ok(TruthValue::One),
        'X' => Ok(TruthValue::DontCare),
        _ => Err(format!("Invalid character: {}", c)),
    }).collect()
}

// Pretty-print truth table
pub fn print_truth_table(truth_table: &[TruthValue], n_vars: usize, should_print: bool) {
    if !should_print {
        return;
    }

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
pub fn print_implicant_table(
    prime_implicants: &HashSet<(u64, u64)>,
    essential: &[(u64, u64)],
    minterms: &[usize],
    n_vars: usize,
    should_print: bool,
) {
    if !should_print {
        return;
    }

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

fn main() {
    let args = Args::parse();

    let truth_table = args.truth_table.unwrap_or_else(|| "0000000000X10000000000000000000000000000000000000000000000000000010X010X010X010X010X010X010X010X010X010X010X010X010X010X010X010X00000000001X0000000000000000000000000000000000001X1X1X1X1X1X1X1X0000000000000000000000000000000000000000000000000000000000000000".to_string());
    let should_print_truth_table = args.print_truth_table && !args.no_print_truth_table;
    let should_print_implicant_table = args.print_implicant_table && !args.no_print_implicant_table;
    let debug = args.debug;
    let rayon_threshold = args.rayon_threshold;

    match minimize_boolean_function(&truth_table, should_print_truth_table, should_print_implicant_table, debug, rayon_threshold) {
        Ok(result) => println!("Result: {}", result),
        Err(e) => eprintln!("Error: {}", e),
    }
}

#[cfg(test)]
mod tests {
    pub use super::*;
    pub mod tests;
}
