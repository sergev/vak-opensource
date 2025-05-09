use std::collections::HashMap;
use std::fs;
use std::io::{self, BufRead, Write};
use std::path::{Path, PathBuf};
use std::env;

// Represents a macro definition
#[derive(Clone, Debug)]
enum MacroDefinition {
    ObjectLike(String),  // Simple replacement text
    FunctionLike {
        params: Vec<String>,
        replacement: String,
    },
}

// Represents the preprocessor state
struct Preprocessor {
    // Defined macros
    macros: HashMap<String, MacroDefinition>,
    // Include paths to search
    include_paths: Vec<PathBuf>,
    // Keep track of conditional compilation state
    if_stack: Vec<bool>,
    // Currently active condition
    currently_active: bool,
    // Files already included (to prevent recursive inclusion)
    included_files: Vec<PathBuf>,
}

impl Preprocessor {
    fn new() -> Self {
        Preprocessor {
            macros: HashMap::new(),
            include_paths: vec![PathBuf::from(".")],
            if_stack: Vec::new(),
            currently_active: true,
            included_files: Vec::new(),
        }
    }

    // Add standard predefined macros
    fn add_predefined_macros(&mut self) {
        // Add standard predefined macros
        self.macros.insert(
            "__FILE__".to_string(),
            MacroDefinition::ObjectLike("\"current_file\"".to_string()),
        );
        self.macros.insert(
            "__LINE__".to_string(),
            MacroDefinition::ObjectLike("0".to_string()),
        );
        self.macros.insert(
            "__DATE__".to_string(),
            MacroDefinition::ObjectLike("\"May 8 2025\"".to_string()),
        );
        self.macros.insert(
            "__TIME__".to_string(),
            MacroDefinition::ObjectLike("\"00:00:00\"".to_string()),
        );
    }

    // Add include path
    fn add_include_path(&mut self, path: PathBuf) {
        self.include_paths.push(path);
    }

    // Process a single line
    fn process_line(&mut self, line: &str, output: &mut dyn Write, current_file: &Path, line_number: usize) -> io::Result<()> {
        // Skip processing if in inactive branch of conditional compilation
        if !self.currently_active && !line.trim().starts_with("#") {
            return Ok(());
        }

        // Update __FILE__ and __LINE__ macros
        self.macros.insert(
            "__FILE__".to_string(),
            MacroDefinition::ObjectLike(format!("\"{}\"", current_file.display())),
        );
        self.macros.insert(
            "__LINE__".to_string(),
            MacroDefinition::ObjectLike(line_number.to_string()),
        );

        // Trim leading whitespace for directive detection
        let trimmed = line.trim_start();

        // Handle preprocessor directives
        if trimmed.starts_with("#") {
            let directive = &trimmed[1..].trim_start();

            // Handle different preprocessor directives
            if directive.starts_with("define") {
                self.handle_define(directive)?;
            } else if directive.starts_with("include") {
                self.handle_include(directive, output, current_file)?;
            } else if directive.starts_with("ifdef") || directive.starts_with("ifndef") {
                self.handle_conditional(directive)?;
            } else if directive.starts_with("else") {
                self.handle_else()?;
            } else if directive.starts_with("endif") {
                self.handle_endif()?;
            } else if directive.starts_with("undef") {
                self.handle_undef(directive)?;
            } else if !directive.is_empty() && self.currently_active {
                // Unknown directive, pass through as-is
                writeln!(output, "{}", line)?;
            }
        } else if self.currently_active {
            // Regular line, expand macros if active
            let expanded = self.expand_macros(line);
            writeln!(output, "{}", expanded)?;
        }

        Ok(())
    }

    // Handle #define directive
    fn handle_define(&mut self, directive: &str) -> io::Result<()> {
        if !self.currently_active {
            return Ok(());
        }

        // Skip "define " prefix
        let define_content = directive[6..].trim_start();
        
        // Check if it's a function-like macro
        if define_content.contains('(') {
            let name_end = define_content.find('(').unwrap();
            let name = define_content[..name_end].trim().to_string();
            
            // Extract parameters
            let params_end = define_content.find(')').unwrap();
            let params_str = &define_content[name_end + 1..params_end];
            let params: Vec<String> = params_str
                .split(',')
                .map(|s| s.trim().to_string())
                .filter(|s| !s.is_empty())
                .collect();
            
            // Extract replacement text
            let replacement = if params_end + 1 < define_content.len() {
                define_content[params_end + 1..].trim().to_string()
            } else {
                String::new()
            };
            
            self.macros.insert(
                name,
                MacroDefinition::FunctionLike {
                    params,
                    replacement,
                },
            );
        } else {
            // Object-like macro
            let parts: Vec<&str> = define_content.splitn(2, char::is_whitespace).collect();
            let name = parts[0].to_string();
            let value = if parts.len() > 1 { parts[1].to_string() } else { String::new() };
            
            self.macros.insert(name, MacroDefinition::ObjectLike(value));
        }
        
        Ok(())
    }

    // Handle #include directive
    fn handle_include(&mut self, directive: &str, output: &mut dyn Write, current_file: &Path) -> io::Result<()> {
        if !self.currently_active {
            return Ok(());
        }

        // Skip "include " prefix
        let include_path = directive[7..].trim_start();
        
        // Extract path from quotes or angle brackets
        let (path, search_system) = if include_path.starts_with('"') && include_path.ends_with('"') {
            (include_path[1..include_path.len() - 1].to_string(), false)
        } else if include_path.starts_with('<') && include_path.ends_with('>') {
            (include_path[1..include_path.len() - 1].to_string(), true)
        } else {
            return Ok(());  // Malformed include, ignore
        };
        
        // Find file
        let file_path = self.find_include_file(&path, search_system, current_file)?;
        
        // Check for circular inclusion
        if self.included_files.contains(&file_path) {
            return Ok(());
        }
        
        self.included_files.push(file_path.clone());
        
        // Process included file
        self.preprocess_file(&file_path, output)?;
        
        self.included_files.pop();
        
        Ok(())
    }

    // Find include file based on include paths
    fn find_include_file(&self, filename: &str, search_system: bool, current_file: &Path) -> io::Result<PathBuf> {
        // First try relative to current file for quoted includes
        if !search_system {
            let relative_to_current = current_file.parent().unwrap_or(Path::new(".")).join(filename);
            if relative_to_current.exists() {
                return Ok(relative_to_current);
            }
        }
        
        // Search in include paths
        for path in &self.include_paths {
            let full_path = path.join(filename);
            if full_path.exists() {
                return Ok(full_path);
            }
        }
        
        // File not found, return error
        Err(io::Error::new(
            io::ErrorKind::NotFound,
            format!("Include file not found: {}", filename),
        ))
    }

    // Handle conditional compilation directives (#ifdef, #ifndef)
    fn handle_conditional(&mut self, directive: &str) -> io::Result<()> {
        let parts: Vec<&str> = directive.splitn(2, char::is_whitespace).collect();
        if parts.len() < 2 {
            return Ok(());
        }
        
        let directive_type = parts[0];
        let macro_name = parts[1].trim();
        
        let condition = match directive_type {
            "ifdef" => self.macros.contains_key(macro_name),
            "ifndef" => !self.macros.contains_key(macro_name),
            _ => return Ok(()), // Unknown conditional
        };
        
        // Push current condition considering parent conditions
        let new_condition = self.currently_active && condition;
        self.if_stack.push(self.currently_active);
        self.currently_active = new_condition;
        
        Ok(())
    }

    // Handle #else directive
    fn handle_else(&mut self) -> io::Result<()> {
        if let Some(&parent_active) = self.if_stack.last() {
            self.currently_active = parent_active && !self.currently_active;
        }
        Ok(())
    }

    // Handle #endif directive
    fn handle_endif(&mut self) -> io::Result<()> {
        if let Some(parent_active) = self.if_stack.pop() {
            self.currently_active = parent_active;
        }
        Ok(())
    }

    // Handle #undef directive
    fn handle_undef(&mut self, directive: &str) -> io::Result<()> {
        if !self.currently_active {
            return Ok(());
        }

        let parts: Vec<&str> = directive.splitn(2, char::is_whitespace).collect();
        if parts.len() < 2 {
            return Ok(());
        }
        
        let macro_name = parts[1].trim();
        self.macros.remove(macro_name);
        
        Ok(())
    }

    // Expand macros in a line
    fn expand_macros(&self, line: &str) -> String {
        let mut result = line.to_string();
        
        // Very simple macro expansion (a more sophisticated implementation would need
        // proper tokenization and handling of nested macros)
        for (name, definition) in &self.macros {
            match definition {
                MacroDefinition::ObjectLike(replacement) => {
                    // Simple replacement
                    result = result.replace(name, replacement);
                }
                MacroDefinition::FunctionLike { .. } => {
                    // Function-like macros require more complex parsing
                    // Simplified implementation - this would need to be much more sophisticated
                    // in a real preprocessor
                }
            }
        }
        
        result
    }

    // Process entire file
    fn preprocess_file(&mut self, file_path: &Path, output: &mut dyn Write) -> io::Result<()> {
        let file = fs::File::open(file_path)?;
        let reader = io::BufReader::new(file);
        
        for (line_number, line) in reader.lines().enumerate() {
            let line = line?;
            self.process_line(&line, output, file_path, line_number + 1)?;
        }
        
        Ok(())
    }

    // Main preprocessing entry point
    pub fn preprocess(&mut self, input_file: &Path, output: &mut dyn Write) -> io::Result<()> {
        self.add_predefined_macros();
        self.preprocess_file(input_file, output)
    }
}

fn main() -> io::Result<()> {
    // Process command line arguments
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <input_file> [output_file] [-I<include_path> ...]", args[0]);
        return Ok(());
    }

    let input_file = PathBuf::from(&args[1]);
    
    // Determine output destination
    let output_path = if args.len() > 2 && !args[2].starts_with("-") {
        Some(PathBuf::from(&args[2]))
    } else {
        None
    };

    // Create preprocessor
    let mut preprocessor = Preprocessor::new();
    
    // Add include paths from command line
    for arg in &args[2..] {
        if arg.starts_with("-I") {
            let path = &arg[2..];
            preprocessor.add_include_path(PathBuf::from(path));
        }
    }
    
    // Process file
    match output_path {
        Some(path) => {
            let mut output_file = fs::File::create(path)?;
            preprocessor.preprocess(&input_file, &mut output_file)?;
        }
        None => {
            preprocessor.preprocess(&input_file, &mut io::stdout())?;
        }
    }
    
    Ok(())
}
