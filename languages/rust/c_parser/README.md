# Parser in Rust for a simplified C grammar

### Grammar
A subset of C language is defined by [ungrammar](https://rust-analyzer.github.io/blog/2020/10/24/introducing-ungrammar.html):
```
// Top-level program structure
TranslationUnit = Declaration*

// Declarations
Declaration = Type Identifier ';'
            | Type Identifier '(' ParameterList ')' CompoundStmt
ParameterList = (Type Identifier (',' Type Identifier)*)?

// Types
Type = 'int'
     | 'float'
     | 'char'
     | 'void'

// Statements
Stmt = CompoundStmt
     | ExprStmt
     | IfStmt
     | WhileStmt
     | ReturnStmt

CompoundStmt = '{' Declaration* Stmt* '}'
ExprStmt = Expr ';'
IfStmt = 'if' '(' Expr ')' Stmt ('else' Stmt)?
WhileStmt = 'while' '(' Expr ')' Stmt
ReturnStmt = 'return' Expr? ';'

// Expressions
Expr = AssignmentExpr
AssignmentExpr = EqualityExpr ('=' EqualityExpr)?
EqualityExpr = RelationalExpr (('==' | '!=') RelationalExpr)?
RelationalExpr = AddExpr (('<' | '>' | '<=' | '>=') AddExpr)?
AddExpr = MultExpr (('+' | '-') MultExpr)*
MultExpr = UnaryExpr (('*' | '/') UnaryExpr)*
UnaryExpr = PrimaryExpr
          | '-' UnaryExpr
          | '!' UnaryExpr
PrimaryExpr = Identifier
            | Number
            | String
            | '(' Expr ')'

// Basic tokens
Identifier = <token>
Number = <token>
String = <token>
```

### Input
```
int x = 42;
int main(int argc) {
    int y = 3 + 5 * 2;
    if (y > x) {
        return 1;
    } else {
        return 0;
    }
}
```

### Result
```
TranslationUnit {
    declarations: [
        Variable(
            Int,
            "x",
            Some(
                Number(
                    42,
                ),
            ),
        ),
        Function(
            Int,
            "main",
            [
                Parameter(
                    Int,
                    "argc",
                ),
            ],
            CompoundStmt {
                declarations: [
                    Variable(
                        Int,
                        "y",
                        Some(
                            Binary(
                                Number(
                                    3,
                                ),
                                Add,
                                Binary(
                                    Number(
                                        5,
                                    ),
                                    Mul,
                                    Number(
                                        2,
                                    ),
                                ),
                            ),
                        ),
                    ),
                ],
                statements: [
                    If(
                        Binary(
                            Identifier(
                                "y",
                            ),
                            Greater,
                            Identifier(
                                "x",
                            ),
                        ),
                        Compound(
                            CompoundStmt {
                                declarations: [],
                                statements: [
                                    Return(
                                        Some(
                                            Number(
                                                1,
                                            ),
                                        ),
                                    ),
                                ],
                            },
                        ),
                        Some(
                            Compound(
                                CompoundStmt {
                                    declarations: [],
                                    statements: [
                                        Return(
                                            Some(
                                                Number(
                                                    0,
                                                ),
                                            ),
                                        ),
                                    ],
                                },
                            ),
                        ),
                    ),
                ],
            },
        ),
    ],
}
```
