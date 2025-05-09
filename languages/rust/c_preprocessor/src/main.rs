use clap::{Arg, Command};
use std::collections::HashMap;
use std::fs::File;
use std::io::{self, Read, Write};
use std::path::Path;

#[derive(Clone)]
enum Macro {
    Simple(String),
    Parameterized {
        params: Vec<String>,
        body: String,
    },
}

struct Preprocessor {
    macros: HashMap<String, Macro>,
    include_dirs: Vec<String>,
    defined: HashMap<String, bool>,
    skip_block: bool,
    skip_depth: usize,
}

impl Preprocessor {
    fn new() -> Self {
        Preprocessor {
            macros: HashMap::new(),
            include_dirs: vec![".".to_string()],
            defined: HashMap::new(),
            skip_block: false,
            skip_depth: 0,
        }
    }

    fn add_include_dir(&mut self, dir: &str) {
        self.include_dirs.push(dir.to_string());
    }

    fn define_macro(&mut self, name: &str, macro_def: Macro) {
        self.macros.insert(name.to_string(), macro_def);
        self.defined.insert(name.to_string(), true);
    }

    fn undef_macro(&mut self, name: &str) {
        self.macros.remove(name);
        self.defined.remove(name);
    }

    fn process_file(&mut self, path: &str) -> io::Result<String> {
        let mut file = File::open(path)?;
        let mut content = String::new();
        file.read_to_string(&mut content)?;
        self.process_content(&content, path)
    }

    fn process_content(&mut self, content: &str, current_path: &str) -> io::Result<String> {
        let mut output = String::new();
        let lines: Vec<&str> = content.lines().collect();
        let mut i = 0;

        while i < lines.len() {
            let line = lines[i].trim();
            if line.starts_with("#") {
                let directive = line[1..].trim().splitn(2, ' ').collect::<Vec<&str>>();
                let dir_name = directive[0].to_lowercase();

                if self.skip_block && !["endif", "else", "elif", "ifdef", "ifndef"].contains(&dir_name.as_str()) {
                    i += 1;
                    continue;
                }

                match dir_name.as_str() {
                    "include" => {
                        if directive.len() > 1 {
                            let include_file = directive[1].trim_matches(|c| c == '"' || c == '<' || c == '>');
                            let included_content = self.handle_include(include_file, current_path)?;
                            output.push_str(&included_content);
                        }
                    }
                    "define" => {
                        if directive.len() > 1 {
                            self.handle_define(directive[1]);
                        }
                    }
                    "undef" => {
                        if directive.len() > 1 {
                            self.undef_macro(directive[1].trim());
                        }
                    }
                    "ifdef" => {
                        if directive.len() > 1 {
                            self.handle_ifdef(directive[1].trim());
                        }
                    }
                    "ifndef" => {
                        if directive.len() > 1 {
                            self.handle_ifndef(directive[1].trim());
                        }
                    }
                    "else" => {
                        self.handle_else();
                    }
                    "endif" => {
                        self.handle_endif();
                    }
                    "elif" => {
                        if directive.len() > 1 {
                            self.handle_elif(directive[1].trim());
                        }
                    }
                    _ => {
                        if !self.skip_block {
                            output.push_str(line);
                            output.push('\n');
                        }
                    }
                }
            } else if !self.skip_block {
                let expanded_line = self.expand_macros(line);
                output.push_str(&expanded_line);
                output.push('\n');
            }
            i += 1;
        }
        Ok(output)
    }

    fn handle_include(&mut self, include_file: &str, current_path: &str) -> io::Result<String> {
        let path = Path::new(current_path)
            .parent()
            .unwrap_or_else(|| Path::new("."))
            .join(include_file);

        if path.exists() {
            return self.process_file(path.to_str().unwrap());
        }

        for dir in &self.include_dirs {
            let full_path = Path::new(dir).join(include_file);
            if full_path.exists() {
                return self.process_file(full_path.to_str().unwrap());
            }
        }

        Err(io::Error::new(
            io::ErrorKind::NotFound,
            format!("Include file {} not found", include_file),
        ))
    }

    fn handle_define(&mut self, def: &str) {
        let parts: Vec<&str> = def.splitn(2, ' ').collect();
        let name_part = parts[0].trim();

        if name_part.contains('(') {
            let (name, params) = name_part.split_once('(').unwrap();
            let params = params
                .trim_end_matches(')')
                .split(',')
                .map(|s| s.trim().to_string())
                .filter(|s| !s.is_empty())
                .collect::<Vec<String>>();

            let body = parts.get(1).map(|s| s.trim()).unwrap_or("").to_string();
            self.define_macro(
                name.trim(),
                Macro::Parameterized { params, body },
            );
        } else {
            let body = parts.get(1).map(|s| s.trim()).unwrap_or("").to_string();
            self.define_macro(name_part, Macro::Simple(body));
        }
    }

    fn handle_ifdef(&mut self, name: &str) {
        self.skip_depth += 1;
        if !self.skip_block {
            self.skip_block = !self.defined.contains_key(name);
        }
    }

    fn handle_ifndef(&mut self, name: &str) {
        self.skip_depth += 1;
        if !self.skip_block {
            self.skip_block = self.defined.contains_key(name);
        }
    }

    fn handle_else(&mut self) {
        if self.skip_depth == 0 {
            return; // No matching #ifdef/#ifndef
        }
        if self.skip_depth == 1 {
            self.skip_block = !self.skip_block;
        }
    }

    fn handle_elif(&mut self, name: &str) {
        if self.skip_depth == 0 {
            return; // No matching #ifdef/#ifndef
        }
        if self.skip_depth == 1 {
            self.skip_block = !self.defined.contains_key(name);
        }
    }

    fn handle_endif(&mut self) {
        if self.skip_depth > 0 {
            self.skip_depth -= 1;
            if self.skip_depth == 0 {
                self.skip_block = false;
            }
        }
    }

    fn expand_macros(&self, line: &str) -> String {
        let mut result = String::new();
        let mut chars = line.chars().enumerate().peekable();
        let mut current_word = String::new();

        while let Some((i, c)) = chars.next() {
            if c.is_alphanumeric() || c == '_' {
                current_word.push(c);
            } else {
                if !current_word.is_empty() {
                    if let Some(m) = self.macros.get(&current_word) {
                        match m {
                            Macro::Simple(body) => {
                                result.push_str(body);
                            }
                            Macro::Parameterized { params, body } => {
                                let mut next_pos = i;
                                let mut found_open_paren = false;

                                // Skip whitespace to find '('
                                while let Some((j, next_c)) = chars.peek() {
                                    if next_c.is_whitespace() {
                                        result.push(*next_c);
                                        next_pos = *j + 1;
                                        chars.next();
                                    } else if *next_c == '(' {
                                        found_open_paren = true;
                                        next_pos = *j + 1;
                                        chars.next(); // Consume '('
                                        break;
                                    } else {
                                        break;
                                    }
                                }

                                if found_open_paren {
                                    let mut args = Vec::new();
                                    let mut current_arg = String::new();
                                    let mut paren_depth = 1;
                                    let mut bracket_depth = 0;
                                    let mut brace_depth = 0;

                                    while let Some((j, arg_c)) = chars.next() {
                                        current_arg.push(arg_c);
                                        if arg_c == '(' {
                                            paren_depth += 1;
                                        } else if arg_c == ')' {
                                            paren_depth -= 1;
                                            if paren_depth == 0 && bracket_depth == 0 && brace_depth == 0 {
                                                if !current_arg.trim().is_empty() {
                                                    args.push(current_arg.trim().to_string());
                                                }
                                                next_pos = j + 1;
                                                break;
                                            }
                                        } else if arg_c == '[' {
                                            bracket_depth += 1;
                                        } else if arg_c == ']' {
                                            bracket_depth -= 1;
                                        } else if arg_c == '{' {
                                            brace_depth += 1;
                                        } else if arg_c == '}' {
                                            brace_depth -= 1;
                                        } else if arg_c == ',' && paren_depth == 1 && bracket_depth == 0 && brace_depth == 0 {
                                            if !current_arg.trim().is_empty() {
                                                // Remove the trailing comma
                                                let arg = current_arg.trim_end_matches(',').trim().to_string();
                                                if !arg.is_empty() {
                                                    args.push(arg);
                                                }
                                            }
                                            current_arg.clear();
                                            continue;
                                        }
                                    }

                                    if args.len() == params.len() {
                                        let mut expanded = body.clone();
                                        for (param, arg) in params.iter().zip(args.iter()) {
                                            expanded = expanded.replace(param, arg);
                                        }
                                        result.push_str(&expanded);
                                    } else {
                                        // Output macro call as-is if args don't match
                                        result.push_str(&current_word);
                                        result.push('(');
                                        result.push_str(&args.join(", "));
                                        if !args.is_empty() {
                                            result.push(')');
                                        }
                                    }
                                } else {
                                    result.push_str(&current_word);
                                }
                            }
                        }
                    } else {
                        result.push_str(&current_word);
                    }
                    current_word.clear();
                }
                result.push(c);
            }
        }

        // Handle any remaining word
        if !current_word.is_empty() {
            if let Some(m) = self.macros.get(&current_word) {
                if let Macro::Simple(body) = m {
                    result.push_str(body);
                } else {
                    result.push_str(&current_word);
                }
            } else {
                result.push_str(&current_word);
            }
        }

        result
    }
}

fn main() -> io::Result<()> {
    let matches = Command::new("c-preprocessor")
        .version("1.0")
        .about("A simple C preprocessor written in Rust")
        .arg(
            Arg::new("include")
                .short('I')
                .long("include")
                .value_name("PATH")
                .help("Add directory to include search path")
                .action(clap::ArgAction::Append),
        )
        .arg(
            Arg::new("input")
                .help("Input file to process")
                .required(true)
                .index(1),
        )
        .get_matches();

    let mut preprocessor = Preprocessor::new();

    if let Some(includes) = matches.get_many::<String>("include") {
        for path in includes {
            preprocessor.add_include_dir(path);
        }
    }

    let input_file = matches.get_one::<String>("input").expect("Input file required");
    let output = preprocessor.process_file(input_file)?;

    // Write to stdout
    io::stdout().write_all(output.as_bytes())?;

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_simple_macro() {
        let mut preprocessor = Preprocessor::new();
        preprocessor.define_macro("MAX", Macro::Simple("100".to_string()));
        let input = "int a = MAX;";
        let output = preprocessor.expand_macros(input);
        assert_eq!(output, "int a = 100;");
    }

    #[test]
    fn test_parameterized_macro() {
        let mut preprocessor = Preprocessor::new();
        preprocessor.define_macro(
            "ADD",
            Macro::Parameterized {
                params: vec!["x".to_string(), "y".to_string()],
                body: "((x) + (y))".to_string(),
            },
        );
        let input = "int b = ADD(5, 3);";
        let output = preprocessor.expand_macros(input);
        let expected = "int b = ((5) + (3));";
        assert_eq!(output, expected, "Expected '{}', but got '{}'", expected, output);
    }

    #[test]
    fn test_macro_two_args() {
        let mut preprocessor = Preprocessor::new();
        let input = "#define foo(a, b) a+b\nfoo((1,2),(3,4))\n";
        let output = preprocessor.process_content(input, "test.c").unwrap();
        let expected = "\n(1,2)+(3,4)\n";
        assert_eq!(output, expected, "Expected '{}', but got '{}'", expected, output);
    }

    #[test]
    fn test_ifdef() {
        let mut preprocessor = Preprocessor::new();
        preprocessor.define_macro("DEBUG", Macro::Simple("1".to_string()));
        let input = "#ifdef DEBUG\nint x = 1;\n#else\nint x = 0;\n#endif";
        let output = preprocessor.process_content(input, "test.c").unwrap();
        assert!(output.contains("int x = 1;"));
        assert!(!output.contains("int x = 0;"));
    }

    #[test]
    fn test_ifndef() {
        let mut preprocessor = Preprocessor::new();
        let input = "#ifndef DEBUG\nint x = 1;\n#else\nint x = 0;\n#endif";
        let output = preprocessor.process_content(input, "test.c").unwrap();
        assert!(output.contains("int x = 1;"));
        assert!(!output.contains("int x = 0;"));
    }

    #[test]
    fn test_undef() {
        let mut preprocessor = Preprocessor::new();
        preprocessor.define_macro("MAX", Macro::Simple("100".to_string()));
        let input = "#undef MAX\nint x = MAX;";
        let output = preprocessor.process_content(input, "test.c").unwrap();
        assert!(output.contains("int x = MAX;")); // MAX should not expand
    }
}