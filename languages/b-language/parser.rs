use std::iter::Peekable;
use std::vec::IntoIter;

// Token types for the B language
#[derive(Debug, PartialEq, Clone)]
enum Token {
    Ident(String),
    Literal(i32),
    LBrace, RBrace, LBracket, RBracket, LParen, RParen,
    Semicolon, Comma, Equal,
    Plus, Minus, Star, Slash, Percent, Less, Greater, And, Or, Tilde, Caret,
    PlusPlus, MinusMinus, LessEq, GreaterEq, EqEq, NotEq, LShift, RShift, Question, Colon,
    PlusEq, MinusEq, StarEq, SlashEq, PercentEq, LShiftEq, RShiftEq, AndEq, CaretEq, OrEq,
    Ampersand, // & is both bitwise AND and address-of, context determines usage
    If, While, Return, Auto,
    EOF,
}

// AST types based on ASDL
#[derive(Debug)]
struct Program {
    declarations: Vec<Declaration>,
}

#[derive(Debug)]
enum Declaration {
    VarDecl(String, Option<Expr>),
    VectorDecl(String, Expr, Option<Vec<Expr>>),
    FuncDecl(String, Vec<String>, Box<Stmt>),
}

#[derive(Debug)]
enum Stmt {
    ExprStmt(Expr),
    AssignStmt(String, Expr),                    // Simple assignment to a variable (e.g., x = 5)
    VectorAssignStmt(Box<Expr>, Box<Expr>, Expr), // Assignment to a vector element (e.g., v[0] = 5)
    CompoundAssignStmt(String, CompoundOp, Expr), // Compound assignment (e.g., x += 5)
    IfStmt(Expr, Box<Stmt>, Option<Box<Stmt>>),
    WhileStmt(Expr, Box<Stmt>),
    ReturnStmt(Option<Expr>),
    BlockStmt(Vec<Stmt>),
    AutoStmt(String, Expr),
    AutoVectorStmt(String, Expr),
}

#[derive(Debug)]
enum Expr {
    Literal(i32),
    Ident(String),
    BinaryOp(Box<Expr>, BinOp, Box<Expr>),
    UnaryOp(UnaryOp, Box<Expr>),
    Call(Box<Expr>, Vec<Expr>),
    Dereference(Box<Expr>),
    AddressOf(String),
    IncDec(String, IncDec),
    VectorAccess(Box<Expr>, Box<Expr>),
    Ternary(Box<Expr>, Box<Expr>, Box<Expr>),
}

#[derive(Debug)]
enum BinOp {
    Add, Subtract, Multiply, Divide, Modulo,
    Less, LessEq, Greater, GreaterEq, Equal, NotEqual,
    BitAnd, BitOr, BitXor, LShift, RShift,
}

#[derive(Debug)]
enum UnaryOp {
    Negate, Deref, Addr, BitNot,
}

#[derive(Debug)]
enum IncDec {
    Inc, Dec,
}

#[derive(Debug)]
enum CompoundOp {
    Add, Subtract, Multiply, Divide, Modulo, LShift, RShift, And, Xor, Or,
}

// Lexer struct (unchanged)
struct Lexer<'a> {
    input: &'a str,
    pos: usize,
}

impl<'a> Lexer<'a> {
    fn new(input: &'a str) -> Self {
        Lexer { input, pos: 0 }
    }

    fn next_token(&mut self) -> Token {
        self.skip_whitespace_and_comments();
        if self.pos >= self.input.len() {
            return Token::EOF;
        }

        let ch = self.input[self.pos..].chars().next().unwrap();
        self.pos += 1;

        match ch {
            '{' => Token::LBrace,
            '}' => Token::RBrace,
            '[' => Token::LBracket,
            ']' => Token::RBracket,
            '(' => Token::LParen,
            ')' => Token::RParen,
            ';' => Token::Semicolon,
            ',' => Token::Comma,
            '=' => match self.peek() {
                Some('=') => { self.pos += 1; Token::EqEq },
                Some('+') => { self.pos += 1; Token::PlusEq },
                Some('-') => { self.pos += 1; Token::MinusEq },
                Some('*') => { self.pos += 1; Token::StarEq },
                Some('/') => { self.pos += 1; Token::SlashEq },
                Some('%') => { self.pos += 1; Token::PercentEq },
                Some('<') if self.peek_next() == Some('<') => { self.pos += 2; Token::LShiftEq },
                Some('>') if self.peek_next() == Some('>') => { self.pos += 2; Token::RShiftEq },
                Some('&') => { self.pos += 1; Token::AndEq },
                Some('^') => { self.pos += 1; Token::CaretEq },
                Some('|') => { self.pos += 1; Token::OrEq },
                _ => Token::Equal,
            },
            '+' => match self.peek() {
                Some('+') => { self.pos += 1; Token::PlusPlus },
                _ => Token::Plus,
            },
            '-' => match self.peek() {
                Some('-') => { self.pos += 1; Token::MinusMinus },
                _ => Token::Minus,
            },
            '*' => Token::Star,
            '/' => {
                if self.peek() == Some('*') {
                    self.skip_comment();
                    self.next_token()
                } else {
                    Token::Slash
                }
            },
            '%' => Token::Percent,
            '<' => match self.peek() {
                Some('=') => { self.pos += 1; Token::LessEq },
                Some('<') => { self.pos += 1; Token::LShift },
                Some('>') => { self.pos += 1; Token::NotEq },
                _ => Token::Less,
            },
            '>' => match self.peek() {
                Some('=') => { self.pos += 1; Token::GreaterEq },
                Some('>') => { self.pos += 1; Token::RShift },
                _ => Token::Greater,
            },
            '&' => Token::Ampersand,
            '|' => Token::Or,
            '~' => Token::Tilde,
            '^' => Token::Caret,
            '?' => Token::Question,
            ':' => Token::Colon,
            'a'..='z' | 'A'..='Z' | '_' => self.parse_identifier(ch),
            '0'..='9' => self.parse_literal(ch),
            _ => panic!("Unexpected character: {}", ch),
        }
    }

    fn peek(&self) -> Option<char> {
        self.input[self.pos..].chars().next()
    }

    fn peek_next(&self) -> Option<char> {
        if self.pos + 1 < self.input.len() {
            self.input[self.pos + 1..].chars().next()
        } else {
            None
        }
    }

    fn skip_whitespace_and_comments(&mut self) {
        loop {
            self.skip_whitespace();
            if self.pos + 1 < self.input.len() && &self.input[self.pos..self.pos + 2] == "/*" {
                self.skip_comment();
            } else {
                break;
            }
        }
    }

    fn skip_whitespace(&mut self) {
        while self.pos < self.input.len() && self.input[self.pos..].chars().next().unwrap().is_whitespace() {
            self.pos += 1;
        }
    }

    fn skip_comment(&mut self) {
        self.pos += 2; // Skip /*
        while self.pos + 1 < self.input.len() && &self.input[self.pos..self.pos + 2] != "*/" {
            self.pos += 1;
        }
        if self.pos + 1 >= self.input.len() {
            panic!("Unterminated comment");
        }
        self.pos += 2; // Skip */
    }

    fn parse_identifier(&mut self, first: char) -> Token {
        let mut ident = String::new();
        ident.push(first);
        while let Some(ch) = self.peek() {
            if ch.is_alphanumeric() || ch == '_' {
                ident.push(ch);
                self.pos += 1;
            } else {
                break;
            }
        }
        match ident.as_str() {
            "if" => Token::If,
            "while" => Token::While,
            "return" => Token::Return,
            "auto" => Token::Auto,
            _ => Token::Ident(ident),
        }
    }

    fn parse_literal(&mut self, first: char) -> Token {
        let mut num = String::new();
        num.push(first);
        while let Some(ch) = self.peek() {
            if ch.is_digit(10) {
                num.push(ch);
                self.pos += 1;
            } else {
                break;
            }
        }
        Token::Literal(num.parse::<i32>().unwrap())
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Token;
    fn next(&mut self) -> Option<Self::Item> {
        let token = self.next_token();
        if token == Token::EOF && self.pos >= self.input.len() {
            None
        } else {
            Some(token)
        }
    }
}

// Parser struct
struct Parser {
    tokens: Peekable<IntoIter<Token>>,
}

impl Parser {
    fn new(tokens: Vec<Token>) -> Self {
        Parser { tokens: tokens.into_iter().peekable() }
    }

    fn next_token(&mut self) -> Token {
        self.tokens.next().unwrap_or(Token::EOF)
    }

    fn peek(&mut self) -> &Token {
        self.tokens.peek().unwrap_or(&Token::EOF)
    }

    fn parse_program(&mut self) -> Program {
        let mut declarations = Vec::new();
        while *self.peek() != Token::EOF {
            declarations.push(self.parse_declaration());
        }
        Program { declarations }
    }

    fn parse_declaration(&mut self) -> Declaration {
        let token = self.next_token();
        match token {
            Token::Ident(name) => {
                match self.peek() {
                    Token::LBracket => {
                        self.next_token(); // Consume [
                        let size = self.parse_expr(0);
                        self.expect(Token::RBracket);
                        let init = if *self.peek() != Token::Semicolon {
                            let mut inits = Vec::new();
                            while *self.peek() != Token::Semicolon {
                                inits.push(self.parse_expr(0));
                                if *self.peek() == Token::Comma {
                                    self.next_token();
                                }
                            }
                            Some(inits)
                        } else {
                            None
                        };
                        self.expect(Token::Semicolon);
                        Declaration::VectorDecl(name, size, init)
                    }
                    Token::LParen => {
                        self.next_token(); // Consume (
                        let mut params = Vec::new();
                        while *self.peek() != Token::RParen {
                            if let Token::Ident(param) = self.next_token() {
                                params.push(param);
                            }
                            if *self.peek() == Token::Comma {
                                self.next_token();
                            }
                        }
                        self.expect(Token::RParen);
                        let body = self.parse_stmt();
                        Declaration::FuncDecl(name, params, Box::new(body))
                    }
                    _ => {
                        let init = if *self.peek() != Token::Semicolon {
                            Some(self.parse_expr(0))
                        } else {
                            None
                        };
                        self.expect(Token::Semicolon);
                        Declaration::VarDecl(name, init)
                    }
                }
            }
            _ => panic!("Expected identifier at start of declaration, got {:?}", token),
        }
    }

    fn parse_stmt(&mut self) -> Stmt {
        match self.peek() {
            Token::LBrace => {
                self.next_token(); // Consume {
                let mut statements = Vec::new();
                while *self.peek() != Token::RBrace && *self.peek() != Token::EOF {
                    statements.push(self.parse_stmt());
                }
                self.expect(Token::RBrace);
                Stmt::BlockStmt(statements)
            }
            Token::Auto => {
                self.next_token(); // Consume auto
                if let Token::Ident(name) = self.next_token() {
                    match self.peek() {
                        Token::LBracket => {
                            self.next_token(); // Consume [
                            let size = self.parse_expr(0);
                            self.expect(Token::RBracket);
                            self.expect(Token::Semicolon);
                            Stmt::AutoVectorStmt(name, size)
                        }
                        _ => {
                            let init = self.parse_expr(0);
                            self.expect(Token::Semicolon);
                            Stmt::AutoStmt(name, init)
                        }
                    }
                } else {
                    panic!("Expected identifier after auto");
                }
            }
            Token::If => {
                self.next_token(); // Consume if
                self.expect(Token::LParen);
                let condition = self.parse_expr(0);
                self.expect(Token::RParen);
                let then_body = Box::new(self.parse_stmt());
                let else_body = if *self.peek() == Token::If { // B uses 'if' for else
                    self.next_token(); // Consume else (if)
                    Some(Box::new(self.parse_stmt()))
                } else {
                    None
                };
                Stmt::IfStmt(condition, then_body, else_body)
            }
            Token::While => {
                self.next_token(); // Consume while
                self.expect(Token::LParen);
                let condition = self.parse_expr(0);
                self.expect(Token::RParen);
                let body = Box::new(self.parse_stmt());
                Stmt::WhileStmt(condition, body)
            }
            Token::Return => {
                self.next_token(); // Consume return
                let value = if *self.peek() != Token::Semicolon {
                    Some(self.parse_expr(0))
                } else {
                    None
                };
                self.expect(Token::Semicolon);
                Stmt::ReturnStmt(value)
            }
            _ => {
                let expr = self.parse_expr(0);
                match self.peek() {
                    Token::Equal => {
                        self.next_token(); // Consume =
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        match expr {
                            Expr::Ident(name) => Stmt::AssignStmt(name, value),
                            Expr::VectorAccess(vec, idx) => Stmt::VectorAssignStmt(vec, idx, value),
                            _ => panic!("Invalid assignment target"),
                        }
                    }
                    Token::PlusEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Add, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::MinusEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Subtract, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::StarEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Multiply, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::SlashEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Divide, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::PercentEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Modulo, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::LShiftEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::LShift, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::RShiftEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::RShift, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::AndEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::And, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::CaretEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Xor, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    Token::OrEq => {
                        self.next_token();
                        let value = self.parse_expr(0);
                        self.expect(Token::Semicolon);
                        if let Expr::Ident(name) = expr {
                            Stmt::CompoundAssignStmt(name, CompoundOp::Or, value)
                        } else {
                            panic!("Invalid compound assignment target");
                        }
                    }
                    _ => {
                        self.expect(Token::Semicolon);
                        Stmt::ExprStmt(expr)
                    }
                }
            }
        }
    }

    fn parse_expr(&mut self, min_prec: u8) -> Expr {
        let mut lhs = self.parse_term();
        while let Some((op, prec, assoc)) = self.peek_binop() {
            if prec < min_prec {
                break;
            }
            self.next_token(); // Consume operator
            if op == BinOp::TernaryStart {
                let then_expr = self.parse_expr(0);
                self.expect(Token::Colon);
                let else_expr = self.parse_expr(0);
                return Expr::Ternary(Box::new(lhs), Box::new(then_expr), Box::new(else_expr));
            }
            let next_prec = if assoc == Assoc::Left { prec + 1 } else { prec };
            let rhs = self.parse_expr(next_prec);
            lhs = Expr::BinaryOp(Box::new(lhs), op, Box::new(rhs));
        }
        lhs
    }

    fn parse_term(&mut self) -> Expr {
        let token = self.next_token();
        match token {
            Token::Literal(val) => Expr::Literal(val),
            Token::Ident(name) => {
                match self.peek() {
                    Token::LParen => {
                        self.next_token(); // Consume (
                        let mut args = Vec::new();
                        while *self.peek() != Token::RParen {
                            args.push(self.parse_expr(0));
                            if *self.peek() == Token::Comma {
                                self.next_token();
                            }
                        }
                        self.expect(Token::RParen);
                        Expr::Call(Box::new(Expr::Ident(name)), args)
                    }
                    Token::LBracket => {
                        self.next_token(); // Consume [
                        let index = self.parse_expr(0);
                        self.expect(Token::RBracket);
                        Expr::VectorAccess(Box::new(Expr::Ident(name)), Box::new(index))
                    }
                    Token::PlusPlus => {
                        self.next_token(); // Consume ++
                        Expr::IncDec(name, IncDec::Inc)
                    }
                    Token::MinusMinus => {
                        self.next_token(); // Consume --
                        Expr::IncDec(name, IncDec::Dec)
                    }
                    _ => Expr::Ident(name),
                }
            }
            Token::Minus => Expr::UnaryOp(UnaryOp::Negate, Box::new(self.parse_term())),
            Token::Star => Expr::UnaryOp(UnaryOp::Deref, Box::new(self.parse_term())),
            Token::Ampersand => {
                if let Token::Ident(name) = self.next_token() {
                    Expr::AddressOf(name)
                } else {
                    panic!("Expected identifier after &");
                }
            }
            Token::Tilde => Expr::UnaryOp(UnaryOp::BitNot, Box::new(self.parse_term())),
            Token::LParen => {
                let expr = self.parse_expr(0);
                self.expect(Token::RParen);
                expr
            }
            _ => panic!("Unexpected token in expression: {:?}", token),
        }
    }

    fn peek_binop(&mut self) -> Option<(BinOp, u8, Assoc)> {
        match self.peek() {
            Token::Or => Some((BinOp::Or, 1, Assoc::Left)),
            Token::Caret => Some((BinOp::BitXor, 2, Assoc::Left)),
            Token::And => Some((BinOp::And, 3, Assoc::Left)),
            Token::EqEq => Some((BinOp::Equal, 4, Assoc::Left)),
            Token::NotEq => Some((BinOp::NotEqual, 4, Assoc::Left)),
            Token::Less => Some((BinOp::Less, 5, Assoc::Left)),
            Token::LessEq => Some((BinOp::LessEq, 5, Assoc::Left)),
            Token::Greater => Some((BinOp::Greater, 5, Assoc::Left)),
            Token::GreaterEq => Some((BinOp::GreaterEq, 5, Assoc::Left)),
            Token::LShift => Some((BinOp::LShift, 6, Assoc::Left)),
            Token::RShift => Some((BinOp::RShift, 6, Assoc::Left)),
            Token::Plus => Some((BinOp::Add, 7, Assoc::Left)),
            Token::Minus => Some((BinOp::Subtract, 7, Assoc::Left)),
            Token::Star => Some((BinOp::Multiply, 8, Assoc::Left)),
            Token::Slash => Some((BinOp::Divide, 8, Assoc::Left)),
            Token::Percent => Some((BinOp::Modulo, 8, Assoc::Left)),
            Token::Question => Some((BinOp::TernaryStart, 9, Assoc::Right)),
            _ => None,
        }
    }

    fn expect(&mut self, expected: Token) {
        let token = self.next_token();
        if token != expected {
            panic!("Expected {:?}, got {:?}", expected, token);
        }
    }
}

#[derive(Debug, PartialEq)]
enum Assoc {
    Left,
    Right,
}

#[derive(Debug, PartialEq)]
enum BinOp {
    Add, Subtract, Multiply, Divide, Modulo,
    Less, LessEq, Greater, GreaterEq, Equal, NotEqual,
    BitAnd, BitOr, BitXor, LShift, RShift,
    TernaryStart,
}

// Example usage with lexer
fn main() {
    let source = r#"
        /* Global vector declaration */
        v[2] 1, 2;
        foo(x) {
            auto y 5;   /* Local scalar */
            auto w[2];  /* Local vector */
            w[0] = y;   /* Vector assignment */
            x += 1;     /* Compound assignment */
            y = x > 0 ? x : -x; /* Ternary */
            return v[1] & y; /* Bitwise AND */
        }
    "#;

    let lexer = Lexer::new(source);
    let tokens: Vec<Token> = lexer.collect();
    let mut parser = Parser::new(tokens);
    let ast = parser.parse_program();
    println!("{:#?}", ast);
}
