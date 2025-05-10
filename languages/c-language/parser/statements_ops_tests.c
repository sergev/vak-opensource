#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "ast.h"
#include "c-scanner.h"

// Mock lexer
class MockScanner
{
public:
    MOCK_METHOD0(yylex, int());
    MOCK_METHOD0(get_yytext, char *());
    MOCK_METHOD1(init_scanner, void(FILE *));
};

// Global mock instance
static MockScanner *mock_scanner = nullptr;

// Token sequence for mocking
static std::vector<std::pair<int, std::string>> token_sequence;
static size_t token_index = 0;

// Mock implementations
extern "C" {
void init_scanner(FILE *input)
{
    if (mock_scanner) {
        mock_scanner->init_scanner(input);
    }
    token_index = 0;
}

int yylex()
{
    if (!mock_scanner || token_index >= token_sequence.size()) {
        return TOKEN_EOF;
    }
    return token_sequence[token_index++].first;
}

char *get_yytext()
{
    if (!mock_scanner || token_index == 0 || token_index > token_sequence.size()) {
        return nullptr;
    }
    return const_cast<char *>(token_sequence[token_index - 1].second.c_str());
}
}

// Test fixture
class ParserTest : public ::testing::
                   Test{ protected : void SetUp() override{ mock_scanner = new MockScanner();
token_sequence.clear();
token_index = 0;
}

void TearDown() override
{
    delete mock_scanner;
    mock_scanner = nullptr;
}

// Helper to set token sequence
void SetTokenSequence(const std::vector<std::pair<int, std::string>> &tokens)
{
    token_sequence = tokens;
    token_index    = 0;
}

// Helper to create a temporary file with content
FILE *CreateTempFile(const char *content)
{
    FILE *f = tmpfile();
    fwrite(content, 1, strlen(content), f);
    rewind(f);
    return f;
}

// Helper to get statement from program
Stmt *GetStatement(Program *program)
{
    EXPECT_NE(nullptr, program);
    EXPECT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    Expr *expr = id->init->u.expr;
    EXPECT_EQ(EXPR_CALL, expr->kind); // Simplified AST structure
    return expr->u.call.func;         // Assuming statement is wrapped
}
}
;

// Test labeled statement: label: x;
TEST_F(ParserTest, ParseLabeledStatement)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "label" },
                       { TOKEN_COLON, ":" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("label: x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_LABELED, stmt->kind);
    EXPECT_STREQ("label", stmt->u.labeled.label);
    EXPECT_EQ(STMT_EXPR, stmt->u.labeled.stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.labeled.stmt->u.expr->kind);
    EXPECT_STREQ("x", stmt->u.labeled.stmt->u.expr->u.var);
}

// Test case statement: case 42: x;
TEST_F(ParserTest, ParseCaseStatement)
{
    SetTokenSequence({ { TOKEN_CASE, "case" },
                       { TOKEN_I_CONSTANT, "42" },
                       { TOKEN_COLON, ":" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("case 42: x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_CASE, stmt->kind);
    EXPECT_EQ(EXPR_LITERAL, stmt->u.case_stmt.expr->kind);
    EXPECT_EQ(LITERAL_INT, stmt->u.case_stmt.expr->u.literal->kind);
    EXPECT_EQ(42, stmt->u.case_stmt.expr->u.literal->u.int_val);
    EXPECT_EQ(STMT_EXPR, stmt->u.case_stmt.stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.case_stmt.stmt->u.expr->kind);
    EXPECT_STREQ("x", stmt->u.case_stmt.stmt->u.expr->u.var);
}

// Test default statement: default: x;
TEST_F(ParserTest, ParseDefaultStatement)
{
    SetTokenSequence({ { TOKEN_DEFAULT, "default" },
                       { TOKEN_COLON, ":" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("default: x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_DEFAULT, stmt->kind);
    EXPECT_EQ(STMT_EXPR, stmt->u.default_stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.default_stmt->u.expr->kind);
    EXPECT_STREQ("x", stmt->u.default_stmt->u.expr->u.var);
}

// Test compound statement: { x; y; }
TEST_F(ParserTest, ParseCompoundStatement)
{
    SetTokenSequence({ { TOKEN_LBRACE, "{" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("{ x; y; }");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_COMPOUND, stmt->kind);
    DeclOrStmt *items = stmt->u.compound;
    EXPECT_NE(nullptr, items);
    EXPECT_EQ(DECL_OR_STMT_STMT, items->kind);
    EXPECT_EQ(STMT_EXPR, items->u.stmt->kind);
    EXPECT_EQ(EXPR_VAR, items->u.stmt->u.expr->kind);
    EXPECT_STREQ("x", items->u.stmt->u.expr->u.var);
    EXPECT_NE(nullptr, items->next);
    EXPECT_EQ(DECL_OR_STMT_STMT, items->next->kind);
    EXPECT_EQ(STMT_EXPR, items->next->u.stmt->kind);
    EXPECT_EQ(EXPR_VAR, items->next->u.stmt->u.expr->kind);
    EXPECT_STREQ("y", items->next->u.stmt->u.expr->u.var);
    EXPECT_EQ(nullptr, items->next->next);
}

// Test empty expression statement: ;
TEST_F(ParserTest, ParseEmptyExpressionStatement)
{
    SetTokenSequence({ { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile(";");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(nullptr, stmt->u.expr);
}

// Test expression statement: x;
TEST_F(ParserTest, ParseExpressionStatement)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.var);
}

// Test if statement: if (x) y;
TEST_F(ParserTest, ParseIfStatement)
{
    SetTokenSequence({ { TOKEN_IF, "if" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("if (x) y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_IF, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.condition->kind);
    EXPECT_STREQ("x", stmt->u.if_stmt.condition->u.var);
    EXPECT_EQ(STMT_EXPR, stmt->u.if_stmt.then_stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.then_stmt->u.expr->kind);
    EXPECT_STREQ("y", stmt->u.if_stmt.then_stmt->u.expr->u.var);
    EXPECT_EQ(nullptr, stmt->u.if_stmt.else_stmt);
}

// Test if-else statement: if (x) y; else z;
TEST_F(ParserTest, ParseIfElseStatement)
{
    SetTokenSequence({ { TOKEN_IF, "if" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_ELSE, "else" },
                       { TOKEN_IDENTIFIER, "z" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("if (x) y; else z;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_IF, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.condition->kind);
    EXPECT_STREQ("x", stmt->u.if_stmt.condition->u.var);
    EXPECT_EQ(STMT_EXPR, stmt->u.if_stmt.then_stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.then_stmt->u.expr->kind);
    EXPECT_STREQ("y", stmt->u.if_stmt.then_stmt->u.expr->u.var);
    EXPECT_NE(nullptr, stmt->u.if_stmt.else_stmt);
    EXPECT_EQ(STMT_EXPR, stmt->u.if_stmt.else_stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.else_stmt->u.expr->kind);
    EXPECT_STREQ("z", stmt->u.if_stmt.else_stmt->u.expr->u.var);
}

// Test switch statement: switch (x) { case 1: y; }
TEST_F(ParserTest, ParseSwitchStatement)
{
    SetTokenSequence({ { TOKEN_SWITCH, "switch" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_CASE, "case" },
                       { TOKEN_I_CONSTANT, "1" },
                       { TOKEN_COLON, ":" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("switch (x) { case 1: y; }");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_SWITCH, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.switch_stmt.expr->kind);
    EXPECT_STREQ("x", stmt->u.switch_stmt.expr->u.var);
    EXPECT_EQ(STMT_COMPOUND, stmt->u.switch_stmt.body->kind);
    EXPECT_NE(nullptr, stmt->u.switch_stmt.body->u.compound);
    EXPECT_EQ(DECL_OR_STMT_STMT, stmt->u.switch_stmt.body->u.compound->kind);
    EXPECT_EQ(STMT_CASE, stmt->u.switch_stmt.body->u.compound->u.stmt->kind);
    EXPECT_EQ(EXPR_LITERAL, stmt->u.switch_stmt.body->u.compound->u.stmt->u.case_stmt.expr->kind);
    EXPECT_EQ(1,
              stmt->u.switch_stmt.body->u.compound->u.stmt->u.case_stmt.expr->u.literal->u.int_val);
    EXPECT_EQ(STMT_EXPR, stmt->u.switch_stmt.body->u.compound->u.stmt->u.case_stmt.stmt->kind);
    EXPECT_STREQ("y",
                 stmt->u.switch_stmt.body->u.compound->u.stmt->u.case_stmt.stmt->u.expr->u.var);
}

// Test while statement: while (x) y;
TEST_F(ParserTest, ParseWhileStatement)
{
    SetTokenSequence({ { TOKEN_WHILE, "while" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("while (x) y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_WHILE, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.while_stmt.condition->kind);
    EXPECT_STREQ("x", stmt->u.while_stmt.condition->u.var);
    EXPECT_EQ(STMT_EXPR, stmt->u.while_stmt.body->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.while_stmt.body->u.expr->kind);
    EXPECT_STREQ("y", stmt->u.while_stmt.body->u.expr->u.var);
}

// Test do-while statement: do x; while (y);
TEST_F(ParserTest, ParseDoWhileStatement)
{
    SetTokenSequence({ { TOKEN_DO, "do" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_WHILE, "while" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("do x; while (y);");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_DO_WHILE, stmt->kind);
    EXPECT_EQ(STMT_EXPR, stmt->u.do_while.body->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.do_while.body->u.expr->kind);
    EXPECT_STREQ("x", stmt->u.do_while.body->u.expr->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.do_while.condition->kind);
    EXPECT_STREQ("y", stmt->u.do_while.condition->u.var);
}

// Test for statement: for (i = 0; i < 10; i++) x;
TEST_F(ParserTest, ParseForStatement)
{
    SetTokenSequence({ { TOKEN_FOR, "for" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "i" },
                       { TOKEN_ASSIGN, "=" },
                       { TOKEN_I_CONSTANT, "0" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_IDENTIFIER, "i" },
                       { TOKEN_LT, "<" },
                       { TOKEN_I_CONSTANT, "10" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_IDENTIFIER, "i" },
                       { TOKEN_INC_OP, "++" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("for (i = 0; i < 10; i++) x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_FOR, stmt->kind);
    EXPECT_EQ(FOR_INIT_EXPR, stmt->u.for_stmt.init->kind);
    EXPECT_EQ(EXPR_ASSIGN, stmt->u.for_stmt.init->u.expr->kind);
    EXPECT_STREQ("i", stmt->u.for_stmt.init->u.expr->u.assign.target->u.var);
    EXPECT_EQ(EXPR_LITERAL, stmt->u.for_stmt.init->u.expr->u.assign.value->kind);
    EXPECT_EQ(0, stmt->u.for_stmt.init->u.expr->u.assign.value->u.literal->u.int_val);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.for_stmt.condition->kind);
    EXPECT_EQ(BINARY_LT, stmt->u.for_stmt.condition->u.binary_op.op->kind);
    EXPECT_STREQ("i", stmt->u.for_stmt.condition->u.binary_op.left->u.var);
    EXPECT_EQ(10, stmt->u.for_stmt.condition->u.binary_op.right->u.literal->u.int_val);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.for_stmt.update->kind);
    EXPECT_EQ(UNARY_POST_INC, stmt->u.for_stmt.update->u.unary_op.op->kind);
    EXPECT_STREQ("i", stmt->u.for_stmt.update->u.unary_op.expr->u.var);
    EXPECT_EQ(STMT_EXPR, stmt->u.for_stmt.body->kind);
    EXPECT_STREQ("x", stmt->u.for_stmt.body->u.expr->u.var);
}

// Test for statement with declaration: for (int i = 0; i < 10; i++) x;
TEST_F(ParserTest, ParseForDeclStatement)
{
    SetTokenSequence({ { TOKEN_FOR, "for" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "i" },
                       { TOKEN_ASSIGN, "=" },
                       { TOKEN_I_CONSTANT, "0" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_IDENTIFIER, "i" },
                       { TOKEN_LT, "<" },
                       { TOKEN_I_CONSTANT, "10" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_IDENTIFIER, "i" },
                       { TOKEN_INC_OP, "++" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("for (int i = 0; i < 10; i++) x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_FOR, stmt->kind);
    EXPECT_EQ(FOR_INIT_DECL, stmt->u.for_stmt.init->kind);
    EXPECT_EQ(DECL_VAR, stmt->u.for_stmt.init->u.decl->kind);
    EXPECT_EQ(TYPE_INT, stmt->u.for_stmt.init->u.decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("i", stmt->u.for_stmt.init->u.decl->u.var.declarators->declarator->u.named.name);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.for_stmt.condition->kind);
    EXPECT_EQ(BINARY_LT, stmt->u.for_stmt.condition->u.binary_op.op->kind);
    EXPECT_STREQ("i", stmt->u.for_stmt.condition->u.binary_op.left->u.var);
    EXPECT_EQ(10, stmt->u.for_stmt.condition->u.binary_op.right->u.literal->u.int_val);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.for_stmt.update->kind);
    EXPECT_EQ(UNARY_POST_INC, stmt->u.for_stmt.update->u.unary_op.op->kind);
    EXPECT_STREQ("i", stmt->u.for_stmt.update->u.unary_op.expr->u.var);
    EXPECT_EQ(STMT_EXPR, stmt->u.for_stmt.body->kind);
    EXPECT_STREQ("x", stmt->u.for_stmt.body->u.expr->u.var);
}

// Test goto statement: goto label;
TEST_F(ParserTest, ParseGotoStatement)
{
    SetTokenSequence({ { TOKEN_GOTO, "goto" },
                       { TOKEN_IDENTIFIER, "label" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("goto label;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_GOTO, stmt->kind);
    EXPECT_STREQ("label", stmt->u.goto_label);
}

// Test continue statement: continue;
TEST_F(ParserTest, ParseContinueStatement)
{
    SetTokenSequence(
        { { TOKEN_CONTINUE, "continue" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("continue;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_CONTINUE, stmt->kind);
}

// Test break statement: break;
TEST_F(ParserTest, ParseBreakStatement)
{
    SetTokenSequence({ { TOKEN_BREAK, "break" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("break;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_BREAK, stmt->kind);
}

// Test return statement: return x;
TEST_F(ParserTest, ParseReturnStatement)
{
    SetTokenSequence({ { TOKEN_RETURN, "return" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("return x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_RETURN, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.var);
}

// Test return statement: return;
TEST_F(ParserTest, ParseEmptyReturnStatement)
{
    SetTokenSequence({ { TOKEN_RETURN, "return" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("return;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_RETURN, stmt->kind);
    EXPECT_EQ(nullptr, stmt->u.expr);
}

// Test unary operator: &x
TEST_F(ParserTest, ParseUnaryAddress)
{
    SetTokenSequence({ { TOKEN_AMPERSAND, "&" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("&x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_ADDRESS, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: *x
TEST_F(ParserTest, ParseUnaryDeref)
{
    SetTokenSequence({ { TOKEN_STAR, "*" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("*x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_DEREF, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: +x
TEST_F(ParserTest, ParseUnaryPlus)
{
    SetTokenSequence({ { TOKEN_PLUS, "+" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("+x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_PLUS, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: -x
TEST_F(ParserTest, ParseUnaryNeg)
{
    SetTokenSequence({ { TOKEN_MINUS, "-" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("-x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_NEG, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: ~x
TEST_F(ParserTest, ParseUnaryBitNot)
{
    SetTokenSequence({ { TOKEN_TILDE, "~" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("~x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_BIT_NOT, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: !x
TEST_F(ParserTest, ParseUnaryLogNot)
{
    SetTokenSequence({ { TOKEN_NOT, "!" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("!x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_LOG_NOT, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: ++x
TEST_F(ParserTest, ParseUnaryPreInc)
{
    SetTokenSequence({ { TOKEN_INC_OP, "++" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("++x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_PRE_INC, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: --x
TEST_F(ParserTest, ParseUnaryPreDec)
{
    SetTokenSequence({ { TOKEN_DEC_OP, "--" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("--x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_UNARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(UNARY_PRE_DEC, stmt->u.expr->u.unary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.unary_op.expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.unary_op.expr->u.var);
}

// Test unary operator: x++
TEST_F(ParserTest, ParseUnaryPostInc)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_INC_OP, "++" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x++;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_POST_INC, stmt->u.expr->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.post_inc->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.post_inc->u.var);
}

// Test unary operator: x--
TEST_F(ParserTest, ParseUnaryPostDec)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_DEC_OP, "--" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x--;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_POST_DEC, stmt->u.expr->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.post_dec->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.post_dec->u.var);
}

// Test unary operator: sizeof x
TEST_F(ParserTest, ParseSizeofExpr)
{
    SetTokenSequence({ { TOKEN_SIZEOF, "sizeof" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("sizeof x;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_SIZEOF_EXPR, stmt->u.expr->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.sizeof_expr->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.sizeof_expr->u.var);
}

// Test unary operator: sizeof(int)
TEST_F(ParserTest, ParseSizeofType)
{
    SetTokenSequence({ { TOKEN_SIZEOF, "sizeof" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("sizeof(int);");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_SIZEOF_TYPE, stmt->u.expr->kind);
    EXPECT_EQ(TYPE_INT, stmt->u.expr->u.sizeof_type->kind);
}

// Test unary operator: _Alignof(int)
TEST_F(ParserTest, ParseAlignof)
{
    SetTokenSequence({ { TOKEN_ALIGNOF, "_Alignof" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Alignof(int);");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_ALIGNOF, stmt->u.expr->kind);
    EXPECT_EQ(TYPE_INT, stmt->u.expr->u.alignof->kind);
}

// Test binary operator: x * y
TEST_F(ParserTest, ParseBinaryMul)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_STAR, "*" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x * y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_MUL, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x / y
TEST_F(ParserTest, ParseBinaryDiv)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SLASH, "/" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x / y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_DIV, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x % y
TEST_F(ParserTest, ParseBinaryMod)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_PERCENT, "%" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x % y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_MOD, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x + y
TEST_F(ParserTest, ParseBinaryAdd)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_PLUS, "+" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x + y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_ADD, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x - y
TEST_F(ParserTest, ParseBinarySub)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_MINUS, "-" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x - y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_SUB, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x << y
TEST_F(ParserTest, ParseBinaryLeftShift)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_LEFT_OP, "<<" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x << y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_LEFT_SHIFT, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x >> y
TEST_F(ParserTest, ParseBinaryRightShift)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RIGHT_OP, ">>" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x >> y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_RIGHT_SHIFT, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x < y
TEST_F(ParserTest, ParseBinaryLessThan)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_LT, "<" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x < y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_LT, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x > y
TEST_F(ParserTest, ParseBinaryGreaterThan)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_GT, ">" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x > y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_GT, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x <= y
TEST_F(ParserTest, ParseBinaryLessEqual)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_LE_OP, "<=" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x <= y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_LE, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x >= y
TEST_F(ParserTest, ParseBinaryGreaterEqual)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_GE_OP, ">=" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x >= y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_GE, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x == y
TEST_F(ParserTest, ParseBinaryEqual)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_EQ_OP, "==" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x == y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_EQ, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x != y
TEST_F(ParserTest, ParseBinaryNotEqual)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_NE_OP, "!=" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x != y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_NE, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x & y
TEST_F(ParserTest, ParseBinaryBitAnd)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_AMPERSAND, "&" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x & y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_BIT_AND, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x ^ y
TEST_F(ParserTest, ParseBinaryBitXor)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_CARET, "^" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x ^ y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_BIT_XOR, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x | y
TEST_F(ParserTest, ParseBinaryBitOr)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_PIPE, "|" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x | y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_BIT_OR, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x && y
TEST_F(ParserTest, ParseBinaryLogAnd)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_AND_OP, "&&" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x && y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_LOG_AND, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Test binary operator: x || y
TEST_F(ParserTest, ParseBinaryLogOr)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_OR_OP, "||" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x || y;");
    Program *program = parse(f);
    fclose(f);

    Stmt *stmt = GetStatement(program);
    EXPECT_EQ(STMT_EXPR, stmt->kind);
    EXPECT_EQ(EXPR_BINARY_OP, stmt->u.expr->kind);
    EXPECT_EQ(BINARY_LOG_OR, stmt->u.expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", stmt->u.expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, stmt->u.expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", stmt->u.expr->u.binary_op.right->u.var);
}

// Main for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}