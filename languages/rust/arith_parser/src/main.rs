// 1. First, let's define our token types
#[derive(Debug, Clone, PartialEq)]
enum Token {
    Number(i32),
    Plus,
    Minus,
    Multiply,
    Divide,
    LParen,
    RParen,
}

// 2. Define AST nodes based on our grammar
#[derive(Debug)]
enum Expr {
    Number(i32),
    Binary(Box<Expr>, Op, Box<Expr>),
}

#[derive(Debug)]
enum Op {
    Plus,
    Minus,
    Multiply,
    Divide,
}

// 3. The parser struct with position tracking
struct Parser {
    tokens: Vec<Token>,
    position: usize,
}

impl Parser {
    fn new(tokens: Vec<Token>) -> Self {
        Parser {
            tokens,
            position: 0,
        }
    }

    // Check if we're at the end of input
    fn is_at_end(&self) -> bool {
        self.position >= self.tokens.len()
    }

    // Peek at the current token without consuming it
    fn peek(&self) -> Option<&Token> {
        self.tokens.get(self.position)
    }

    // Consume and return the current token
    fn advance(&mut self) -> Option<&Token> {
        if !self.is_at_end() {
            self.position += 1;
            Some(&self.tokens[self.position - 1])
        } else {
            None
        }
    }

    // 4. Parse an expression based on our grammar
    fn parse_expr(&mut self) -> Result<Expr, String> {
        if let Some(Token::Number(n)) = self.peek() {
            let n = *n;
            self.advance();
            Ok(Expr::Number(n))
        } else if let Some(Token::LParen) = self.peek() {
            self.advance(); // consume '('

            let left = self.parse_expr()?;
            let op = self.parse_op()?;
            let right = self.parse_expr()?;

            if self.advance() != Some(&Token::RParen) {
                return Err("Expected ')'".to_string());
            }

            Ok(Expr::Binary(Box::new(left), op, Box::new(right)))
        } else {
            Err("Expected number or '('".to_string())
        }
    }

    // 5. Parse an operator
    fn parse_op(&mut self) -> Result<Op, String> {
        match self.advance() {
            Some(Token::Plus) => Ok(Op::Plus),
            Some(Token::Minus) => Ok(Op::Minus),
            Some(Token::Multiply) => Ok(Op::Multiply),
            Some(Token::Divide) => Ok(Op::Divide),
            _ => Err("Expected operator".to_string()),
        }
    }
}

// 6. Simple tokenizer (lexer) for our demo
fn tokenize(input: &str) -> Vec<Token> {
    let mut tokens = Vec::new();
    let mut chars = input.chars().peekable();

    while let Some(c) = chars.next() {
        match c {
            ' ' => continue,
            '(' => tokens.push(Token::LParen),
            ')' => tokens.push(Token::RParen),
            '+' => tokens.push(Token::Plus),
            '-' => tokens.push(Token::Minus),
            '*' => tokens.push(Token::Multiply),
            '/' => tokens.push(Token::Divide),
            '0'..='9' => {
                let mut num = c.to_digit(10).unwrap() as i32;
                while let Some(&next) = chars.peek() {
                    if next.is_digit(10) {
                        num = num * 10 + next.to_digit(10).unwrap() as i32;
                        chars.next();
                    } else {
                        break;
                    }
                }
                tokens.push(Token::Number(num));
            }
            _ => {} // Ignore other characters for this demo
        }
    }

    tokens
}

// 7. Main function with example usage
fn main() {
    let input = "42";  // Simple number
    let tokens = tokenize(input);
    let mut parser = Parser::new(tokens);
    match parser.parse_expr() {
        Ok(expr) => println!("Parsed simple number: {:?}", expr),
        Err(e) => println!("Error: {}", e),
    }

    let input = "(1 + 23)";  // Binary expression
    let tokens = tokenize(input);
    let mut parser = Parser::new(tokens);
    match parser.parse_expr() {
        Ok(expr) => println!("Parsed expression: {:?}", expr),
        Err(e) => println!("Error: {}", e),
    }
}
