use std::collections::VecDeque;

// Token definitions (unchanged)
#[derive(Debug, Clone, PartialEq)]
enum Token {
    Identifier(String),
    Number(i32),
    String(String),
    Int, Float, Char, Void,
    Plus, Minus, Star, Slash,
    Equal, EqualEqual, BangEqual,
    Less, Greater, LessEqual, GreaterEqual,
    Bang,
    LParen, RParen, LBrace, RBrace,
    Semicolon, Comma,
    If, Else, While, Return,
}

// AST nodes
#[derive(Debug)]
struct TranslationUnit {
    declarations: Vec<Declaration>,
}

#[derive(Debug)]
enum Declaration {
    Variable(Type, String, Option<Expr>),
    Function(Type, String, Vec<Parameter>, CompoundStmt),
}

#[derive(Debug)]
struct Parameter(Type, String);

#[derive(Debug)]
enum Type {
    Int, Float, Char, Void,
}

#[derive(Debug)]
struct CompoundStmt {
    declarations: Vec<Declaration>,
    statements: Vec<Stmt>,
}

#[derive(Debug)]
enum Stmt {
    Compound(CompoundStmt),
    Expr(Expr),
    If(Expr, Box<Stmt>, Option<Box<Stmt>>),
    While(Expr, Box<Stmt>),
    Return(Option<Expr>),
}

#[derive(Debug)]
enum Expr {
    Number(i32),
    Identifier(String),
    Binary(Box<Expr>, BinOp, Box<Expr>),
    Unary(UnOp, Box<Expr>),
    Assign(Box<Expr>, Box<Expr>),
}

#[derive(Debug)]
enum BinOp {
    Add, Sub, Mul, Div,
    Equal, NotEqual,
    Less, Greater, LessEqual, GreaterEqual,
    Assign,
}

#[derive(Debug)]
enum UnOp {
    Neg, Not,
}

// Parser implementation
struct Parser {
    tokens: VecDeque<Token>,
}

impl Parser {
    fn new(tokens: Vec<Token>) -> Self {
        Parser { tokens: tokens.into() }
    }

    fn peek(&self) -> Option<&Token> {
        self.tokens.front()
    }

    fn advance(&mut self) -> Option<Token> {
        self.tokens.pop_front()
    }

    fn expect(&mut self, expected: Token) -> Result<(), String> {
        if self.peek() == Some(&expected) {
            self.advance();
            Ok(())
        } else {
            Err(format!("Expected {:?}", expected))
        }
    }

    fn parse_translation_unit(&mut self) -> Result<TranslationUnit, String> {
        let mut declarations = Vec::new();
        while self.peek().is_some() {
            declarations.push(self.parse_declaration()?);
        }
        Ok(TranslationUnit { declarations })
    }

    fn parse_declaration(&mut self) -> Result<Declaration, String> {
        let typ = self.parse_type()?;
        let ident = match self.advance() {
            Some(Token::Identifier(id)) => id,
            _ => return Err("Expected identifier".to_string()),
        };
        match self.peek() {
            Some(Token::LParen) => {
                self.advance(); // consume '('
                let params = self.parse_parameter_list()?;
                self.expect(Token::RParen)?;
                let body = self.parse_compound_stmt()?;
                Ok(Declaration::Function(typ, ident, params, body))
            }
            Some(Token::Equal) => {
                self.advance(); // consume '='
                let init = self.parse_expr()?;
                self.expect(Token::Semicolon)?;
                Ok(Declaration::Variable(typ, ident, Some(init)))
            }
            Some(Token::Semicolon) => {
                self.advance(); // consume ';'
                Ok(Declaration::Variable(typ, ident, None))
            }
            _ => Err("Expected '(', '=', or ';'".to_string()),
        }
    }

    fn parse_type(&mut self) -> Result<Type, String> {
        match self.advance() {
            Some(Token::Int) => Ok(Type::Int),
            Some(Token::Float) => Ok(Type::Float),
            Some(Token::Char) => Ok(Type::Char),
            Some(Token::Void) => Ok(Type::Void),
            _ => Err("Expected type".to_string()),
        }
    }

    fn parse_parameter_list(&mut self) -> Result<Vec<Parameter>, String> {
        let mut params = Vec::new();
        if self.peek() != Some(&Token::RParen) {
            loop {
                let typ = self.parse_type()?;
                let ident = match self.advance() {
                    Some(Token::Identifier(id)) => id,
                    _ => return Err("Expected identifier".to_string()),
                };
                params.push(Parameter(typ, ident));
                match self.peek() {
                    Some(Token::Comma) => { self.advance(); }
                    Some(Token::RParen) => break,
                    _ => return Err("Expected ',' or ')'".to_string()),
                }
            }
        }
        Ok(params)
    }

    fn parse_stmt(&mut self) -> Result<Stmt, String> {
        match self.peek() {
            Some(Token::LBrace) => Ok(Stmt::Compound(self.parse_compound_stmt()?)),
            Some(Token::If) => self.parse_if_stmt(),
            Some(Token::While) => self.parse_while_stmt(),
            Some(Token::Return) => self.parse_return_stmt(),
            _ => Ok(Stmt::Expr(self.parse_expr_stmt()?)),
        }
    }

    fn parse_compound_stmt(&mut self) -> Result<CompoundStmt, String> {
        self.expect(Token::LBrace)?;
        let mut decls = Vec::new();
        let mut stmts = Vec::new();
        while self.peek() != Some(&Token::RBrace) {
            if matches!(self.peek(), Some(Token::Int) | Some(Token::Float) | Some(Token::Char) | Some(Token::Void)) {
                decls.push(self.parse_declaration()?);
            } else {
                stmts.push(self.parse_stmt()?);
            }
        }
        self.expect(Token::RBrace)?;
        Ok(CompoundStmt { declarations: decls, statements: stmts })
    }

    fn parse_if_stmt(&mut self) -> Result<Stmt, String> {
        self.advance(); // consume 'if'
        self.expect(Token::LParen)?;
        let cond = self.parse_expr()?;
        self.expect(Token::RParen)?;
        let then = Box::new(self.parse_stmt()?);
        let els = if self.peek() == Some(&Token::Else) {
            self.advance(); // consume 'else'
            Some(Box::new(self.parse_stmt()?))
        } else {
            None
        };
        Ok(Stmt::If(cond, then, els))
    }

    fn parse_while_stmt(&mut self) -> Result<Stmt, String> {
        self.advance(); // consume 'while'
        self.expect(Token::LParen)?;
        let cond = self.parse_expr()?;
        self.expect(Token::RParen)?;
        let body = Box::new(self.parse_stmt()?);
        Ok(Stmt::While(cond, body))
    }

    fn parse_return_stmt(&mut self) -> Result<Stmt, String> {
        self.advance(); // consume 'return'
        let expr = if self.peek() != Some(&Token::Semicolon) {
            Some(self.parse_expr()?)
        } else {
            None
        };
        self.expect(Token::Semicolon)?;
        Ok(Stmt::Return(expr))
    }

    fn parse_expr_stmt(&mut self) -> Result<Expr, String> {
        let expr = self.parse_expr()?;
        self.expect(Token::Semicolon)?;
        Ok(expr)
    }

    fn parse_expr(&mut self) -> Result<Expr, String> {
        self.parse_assignment_expr()
    }

    fn parse_assignment_expr(&mut self) -> Result<Expr, String> {
        let left = self.parse_equality_expr()?;
        if self.peek() == Some(&Token::Equal) {
            self.advance(); // consume '='
            let right = self.parse_assignment_expr()?;
            Ok(Expr::Assign(Box::new(left), Box::new(right)))
        } else {
            Ok(left)
        }
    }

    fn parse_equality_expr(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_relational_expr()?;
        while let Some(op) = self.peek() {
            let bin_op = match op {
                Token::EqualEqual => BinOp::Equal,
                Token::BangEqual => BinOp::NotEqual,
                _ => break,
            };
            self.advance();
            let right = self.parse_relational_expr()?;
            expr = Expr::Binary(Box::new(expr), bin_op, Box::new(right));
        }
        Ok(expr)
    }

    fn parse_relational_expr(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_add_expr()?;
        while let Some(op) = self.peek() {
            let bin_op = match op {
                Token::Less => BinOp::Less,
                Token::Greater => BinOp::Greater,
                Token::LessEqual => BinOp::LessEqual,
                Token::GreaterEqual => BinOp::GreaterEqual,
                _ => break,
            };
            self.advance();
            let right = self.parse_add_expr()?;
            expr = Expr::Binary(Box::new(expr), bin_op, Box::new(right));
        }
        Ok(expr)
    }

    fn parse_add_expr(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_mult_expr()?;
        while let Some(op) = self.peek() {
            let bin_op = match op {
                Token::Plus => BinOp::Add,
                Token::Minus => BinOp::Sub,
                _ => break,
            };
            self.advance();
            let right = self.parse_mult_expr()?;
            expr = Expr::Binary(Box::new(expr), bin_op, Box::new(right));
        }
        Ok(expr)
    }

    fn parse_mult_expr(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_unary_expr()?;
        while let Some(op) = self.peek() {
            let bin_op = match op {
                Token::Star => BinOp::Mul,
                Token::Slash => BinOp::Div,
                _ => break,
            };
            self.advance();
            let right = self.parse_unary_expr()?;
            expr = Expr::Binary(Box::new(expr), bin_op, Box::new(right));
        }
        Ok(expr)
    }

    fn parse_unary_expr(&mut self) -> Result<Expr, String> {
        if let Some(op) = self.peek() {
            let un_op = match op {
                Token::Minus => UnOp::Neg,
                Token::Bang => UnOp::Not,
                _ => return self.parse_primary_expr(),
            };
            self.advance();
            let expr = self.parse_unary_expr()?;
            return Ok(Expr::Unary(un_op, Box::new(expr)));
        }
        self.parse_primary_expr()
    }

    fn parse_primary_expr(&mut self) -> Result<Expr, String> {
        match self.advance() {
            Some(Token::Identifier(id)) => Ok(Expr::Identifier(id)),
            Some(Token::Number(n)) => Ok(Expr::Number(n)),
            Some(Token::LParen) => {
                let expr = self.parse_expr()?;
                self.expect(Token::RParen)?;
                Ok(expr)
            }
            _ => Err("Expected identifier, number, or '('".to_string()),
        }
    }
}

// Lexer (unchanged)
fn tokenize(input: &str) -> Vec<Token> {
    let mut tokens = Vec::new();
    let mut chars = input.chars().peekable();

    while let Some(c) = chars.next() {
        match c {
            ' ' | '\n' | '\t' => continue,
            '(' => tokens.push(Token::LParen),
            ')' => tokens.push(Token::RParen),
            '{' => tokens.push(Token::LBrace),
            '}' => tokens.push(Token::RBrace),
            ';' => tokens.push(Token::Semicolon),
            ',' => tokens.push(Token::Comma),
            '+' => tokens.push(Token::Plus),
            '-' => tokens.push(Token::Minus),
            '*' => tokens.push(Token::Star),
            '/' => tokens.push(Token::Slash),
            '=' => {
                if chars.peek() == Some(&'=') {
                    chars.next();
                    tokens.push(Token::EqualEqual);
                } else {
                    tokens.push(Token::Equal);
                }
            }
            '!' => {
                if chars.peek() == Some(&'=') {
                    chars.next();
                    tokens.push(Token::BangEqual);
                } else {
                    tokens.push(Token::Bang);
                }
            }
            '<' => {
                if chars.peek() == Some(&'=') {
                    chars.next();
                    tokens.push(Token::LessEqual);
                } else {
                    tokens.push(Token::Less);
                }
            }
            '>' => {
                if chars.peek() == Some(&'=') {
                    chars.next();
                    tokens.push(Token::GreaterEqual);
                } else {
                    tokens.push(Token::Greater);
                }
            }
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
            'a'..='z' | 'A'..='Z' | '_' => {
                let mut id = String::new();
                id.push(c);
                while let Some(&next) = chars.peek() {
                    if next.is_alphanumeric() || next == '_' {
                        id.push(next);
                        chars.next();
                    } else {
                        break;
                    }
                }
                match id.as_str() {
                    "int" => tokens.push(Token::Int),
                    "float" => tokens.push(Token::Float),
                    "char" => tokens.push(Token::Char),
                    "void" => tokens.push(Token::Void),
                    "if" => tokens.push(Token::If),
                    "else" => tokens.push(Token::Else),
                    "while" => tokens.push(Token::While),
                    "return" => tokens.push(Token::Return),
                    _ => tokens.push(Token::Identifier(id)),
                }
            }
            _ => {}
        }
    }
    tokens
}

// Main function (unchanged)
fn main() {
    let input = r#"
        int x = 42;
        int main(int argc) {
            int y = 3 + 5 * 2;
            if (y > x) {
                return 1;
            } else {
                return 0;
            }
        }
    "#;

    let tokens = tokenize(input);
    let mut parser = Parser::new(tokens);
    match parser.parse_translation_unit() {
        Ok(ast) => println!("{:#?}", ast),
        Err(e) => println!("Error: {}", e),
    }
}
