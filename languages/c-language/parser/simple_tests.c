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
}
;

// Test primary expression: identifier
TEST_F(ParserTest, ParseIdentifier)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    EXPECT_STREQ("x", id->declarator->u.named.name);
}

// Test primary expression: integer constant
TEST_F(ParserTest, ParseIntegerConstant)
{
    SetTokenSequence({ { TOKEN_I_CONSTANT, "42" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("42;");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    Expr *expr = id->init->u.expr;
    EXPECT_EQ(EXPR_LITERAL, expr->kind);
    EXPECT_EQ(LITERAL_INT, expr->u.literal->kind);
    EXPECT_EQ(42, expr->u.literal->u.int_val);
}

// Test binary expression: x + y
TEST_F(ParserTest, ParseBinaryExpression)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_PLUS, "+" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x + y;");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    Expr *expr = id->init->u.expr;
    EXPECT_EQ(EXPR_BINARY_OP, expr->kind);
    EXPECT_EQ(BINARY_ADD, expr->u.binary_op.op->kind);
    EXPECT_EQ(EXPR_VAR, expr->u.binary_op.left->kind);
    EXPECT_STREQ("x", expr->u.binary_op.left->u.var);
    EXPECT_EQ(EXPR_VAR, expr->u.binary_op.right->kind);
    EXPECT_STREQ("y", expr->u.binary_op.right->u.var);
}

// Test function call: f(x, y)
TEST_F(ParserTest, ParseFunctionCall)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_COMMA, "," },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("f(x, y);");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    Expr *expr = id->init->u.expr;
    EXPECT_EQ(EXPR_CALL, expr->kind);
    EXPECT_EQ(EXPR_VAR, expr->u.call.func->kind);
    EXPECT_STREQ("f", expr->u.call.func->u.var);
    Expr *args = expr->u.call.args;
    EXPECT_NE(nullptr, args);
    EXPECT_EQ(EXPR_VAR, args->kind);
    EXPECT_STREQ("x", args->u.var);
    EXPECT_NE(nullptr, args->next);
    EXPECT_EQ(EXPR_VAR, args->next->kind);
    EXPECT_STREQ("y", args->next->u.var);
    EXPECT_EQ(nullptr, args->next->next);
}

// Test if statement: if (x) return y;
TEST_F(ParserTest, ParseIfStatement)
{
    SetTokenSequence({ { TOKEN_IF, "if" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_RETURN, "return" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("if (x) return y;");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    Stmt *stmt = id->init->u.expr->u.call.func; /* Simplified */
    EXPECT_EQ(STMT_IF, stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.condition->kind);
    EXPECT_STREQ("x", stmt->u.if_stmt.condition->u.var);
    EXPECT_EQ(STMT_RETURN, stmt->u.if_stmt.then_stmt->kind);
    EXPECT_EQ(EXPR_VAR, stmt->u.if_stmt.then_stmt->u.expr->kind);
    EXPECT_STREQ("y", stmt->u.if_stmt.then_stmt->u.expr->u.var);
    EXPECT_EQ(nullptr, stmt->u.if_stmt.else_stmt);
}

// Test variable declaration: int x = 42;
TEST_F(ParserTest, ParseVariableDeclaration)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_ASSIGN, "=" },
                       { TOKEN_I_CONSTANT, "42" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int x = 42;");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    EXPECT_NE(nullptr, decl->u.var.specifiers);
    EXPECT_NE(nullptr, decl->u.var.specifiers->type_specs);
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    InitDeclarator *id = decl->u.var.declarators;
    EXPECT_NE(nullptr, id);
    EXPECT_STREQ("x", id->declarator->u.named.name);
    EXPECT_NE(nullptr, id->init);
    EXPECT_EQ(INITIALIZER_SINGLE, id->init->kind);
    EXPECT_EQ(EXPR_LITERAL, id->init->u.expr->kind);
    EXPECT_EQ(LITERAL_INT, id->init->u.expr->u.literal->kind);
    EXPECT_EQ(42, id->init->u.expr->u.literal->u.int_val);
}

// Test function definition: int main() { return 0; }
TEST_F(ParserTest, ParseFunctionDefinition)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "main" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RETURN, "return" },
                       { TOKEN_I_CONSTANT, "0" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int main() { return 0; }");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, program->decls->kind);
    DeclSpec *spec = program->decls->u.function.specifiers;
    EXPECT_NE(nullptr, spec);
    EXPECT_NE(nullptr, spec->type_specs);
    EXPECT_EQ(TYPE_SPEC_BASIC, spec->type_specs->kind);
    EXPECT_EQ(TYPE_INT, spec->type_specs->u.basic->kind);
    Declarator *decl = program->decls->u.function.declarator;
    EXPECT_NE(nullptr, decl);
    EXPECT_STREQ("main", decl->u.named.name);
    EXPECT_NE(nullptr, decl->u.named.suffixes);
    EXPECT_EQ(SUFFIX_FUNCTION, decl->u.named.suffixes->kind);
    EXPECT_TRUE(decl->u.named.suffixes->u.function.params->is_empty);
    Stmt *body = program->decls->u.function.body;
    EXPECT_NE(nullptr, body);
    EXPECT_EQ(STMT_COMPOUND, body->kind);
    EXPECT_NE(nullptr, body->u.compound);
    EXPECT_EQ(DECL_OR_STMT_STMT, body->u.compound->kind);
    EXPECT_EQ(STMT_RETURN, body->u.compound->u.stmt->kind);
    EXPECT_EQ(EXPR_LITERAL, body->u.compound->u.stmt->u.expr->kind);
    EXPECT_EQ(LITERAL_INT, body->u.compound->u.stmt->u.expr->u.literal->kind);
    EXPECT_EQ(0, body->u.compound->u.stmt->u.expr->u.literal->u.int_val);
}

// Test translation unit: int x; void f() {}
TEST_F(ParserTest, ParseTranslationUnit)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_VOID, "void" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int x; void f() {}");
    Program *program = parse(f);
    fclose(f);

    ASSERT_NE(nullptr, program);
    ASSERT_NE(nullptr, program->decls);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, program->decls->kind);
    Declaration *decl = program->decls->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    EXPECT_NE(nullptr, decl->u.var.specifiers);
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);

    ASSERT_NE(nullptr, program->decls->next);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, program->decls->next->kind);
    DeclSpec *spec = program->decls->next->u.function.specifiers;
    EXPECT_EQ(TYPE_SPEC_BASIC, spec->type_specs->kind);
    EXPECT_EQ(TYPE_VOID, spec->type_specs->u.basic->kind);
    Declarator *func = program->decls->next->u.function.declarator;
    EXPECT_STREQ("f", func->u.named.name);
    EXPECT_EQ(SUFFIX_FUNCTION, func->u.named.suffixes->kind);
    EXPECT_TRUE(func->u.named.suffixes->u.function.params->is_empty);
    EXPECT_EQ(STMT_COMPOUND, program->decls->next->u.function.body->kind);
    EXPECT_EQ(nullptr, program->decls->next->u.function.body->u.compound);
}

// Main for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}