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

// Helper to get external declaration from program
ExternalDecl *GetExternalDecl(Program *program)
{
    EXPECT_NE(nullptr, program);
    EXPECT_NE(nullptr, program->decls);
    return program->decls;
}
}
;

// Test declaration: int x;
TEST_F(ParserTest, ParseSimpleDeclaration)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, ext->kind);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    EXPECT_NE(nullptr, decl->u.var.specifiers);
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_NE(nullptr, decl->u.var.declarators);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
    EXPECT_EQ(nullptr, decl->u.var.declarators->init);
}

// Test declaration: int x = 42;
TEST_F(ParserTest, ParseInitializedDeclaration)
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

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, ext->kind);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
    EXPECT_NE(nullptr, decl->u.var.declarators->init);
    EXPECT_EQ(INITIALIZER_SINGLE, decl->u.var.declarators->init->kind);
    EXPECT_EQ(EXPR_LITERAL, decl->u.var.declarators->init->u.expr->kind);
    EXPECT_EQ(LITERAL_INT, decl->u.var.declarators->init->u.expr->u.literal->kind);
    EXPECT_EQ(42, decl->u.var.declarators->init->u.expr->u.literal->u.int_val);
}

// Test declaration: int x, y;
TEST_F(ParserTest, ParseMultipleDeclarators)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_COMMA, "," },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int x, y;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, ext->kind);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
    EXPECT_NE(nullptr, decl->u.var.declarators->next);
    EXPECT_STREQ("y", decl->u.var.declarators->next->declarator->u.named.name);
    EXPECT_EQ(nullptr, decl->u.var.declarators->next->next);
}

// Test declaration: int;
TEST_F(ParserTest, ParseEmptyDeclaration)
{
    SetTokenSequence({ { TOKEN_INT, "int" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, ext->kind);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(DECL_EMPTY, decl->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_EQ(nullptr, decl->u.var.declarators);
}

// Test static assert: _Static_assert(1, "msg");
TEST_F(ParserTest, ParseStaticAssertDeclaration)
{
    SetTokenSequence({ { TOKEN_STATIC_ASSERT, "_Static_assert" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_I_CONSTANT, "1" },
                       { TOKEN_COMMA, "," },
                       { TOKEN_STRING_LITERAL, "msg" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Static_assert(1, \"msg\");");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, ext->kind);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(DECL_STATIC_ASSERT, decl->kind);
    EXPECT_EQ(EXPR_LITERAL, decl->u.static_assert.condition->kind);
    EXPECT_EQ(LITERAL_INT, decl->u.static_assert.condition->u.literal->kind);
    EXPECT_EQ(1, decl->u.static_assert.condition->u.literal->u.int_val);
    EXPECT_STREQ("msg", decl->u.static_assert.message);
}

// Test function definition: int f() {}
TEST_F(ParserTest, ParseFunctionDefinitionNoParams)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int f() {}");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, ext->kind);
    EXPECT_EQ(TYPE_INT, ext->u.function.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("f", ext->u.function.declarator->u.named.name);
    EXPECT_NE(nullptr, ext->u.function.declarator->u.named.suffixes);
    EXPECT_EQ(SUFFIX_FUNCTION, ext->u.function.declarator->u.named.suffixes->kind);
    EXPECT_TRUE(ext->u.function.declarator->u.named.suffixes->u.function.params->is_empty);
    EXPECT_EQ(STMT_COMPOUND, ext->u.function.body->kind);
    EXPECT_EQ(nullptr, ext->u.function.body->u.compound);
}

// Test function definition: int f(int x) { return x; }
TEST_F(ParserTest, ParseFunctionDefinitionWithParams)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RETURN, "return" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int f(int x) { return x; }");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, ext->kind);
    EXPECT_EQ(TYPE_INT, ext->u.function.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("f", ext->u.function.declarator->u.named.name);
    EXPECT_EQ(SUFFIX_FUNCTION, ext->u.function.declarator->u.named.suffixes->kind);
    ParamList *params = ext->u.function.declarator->u.named.suffixes->u.function.params;
    EXPECT_FALSE(params->is_empty);
    EXPECT_NE(nullptr, params->u.params);
    EXPECT_EQ(TYPE_INT, params->u.params->type->kind);
    EXPECT_STREQ("x", params->u.params->name);
    EXPECT_EQ(STMT_COMPOUND, ext->u.function.body->kind);
    EXPECT_NE(nullptr, ext->u.function.body->u.compound);
    EXPECT_EQ(STMT_RETURN, ext->u.function.body->u.compound->u.stmt->kind);
    EXPECT_STREQ("x", ext->u.function.body->u.compound->u.stmt->u.expr->u.var);
}

// Test declaration list: int x; int y;
TEST_F(ParserTest, ParseDeclarationList)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int f() { int x; int y; }");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, ext->kind);
    EXPECT_EQ(STMT_COMPOUND, ext->u.function.body->kind);
    DeclOrStmt *items = ext->u.function.body->u.compound;
    EXPECT_NE(nullptr, items);
    EXPECT_EQ(DECL_OR_STMT_DECL, items->kind);
    EXPECT_EQ(DECL_VAR, items->u.decl->kind);
    EXPECT_EQ(TYPE_INT, items->u.decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", items->u.decl->u.var.declarators->declarator->u.named.name);
    EXPECT_NE(nullptr, items->next);
    EXPECT_EQ(DECL_OR_STMT_DECL, items->next->kind);
    EXPECT_EQ(DECL_VAR, items->next->u.decl->kind);
    EXPECT_STREQ("y", items->next->u.decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: void x;
TEST_F(ParserTest, ParseTypeVoid)
{
    SetTokenSequence({ { TOKEN_VOID, "void" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("void x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_VOID, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: char x;
TEST_F(ParserTest, ParseTypeChar)
{
    SetTokenSequence({ { TOKEN_CHAR, "char" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("char x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_CHAR, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: short x;
TEST_F(ParserTest, ParseTypeShort)
{
    SetTokenSequence({ { TOKEN_SHORT, "short" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("short x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_SHORT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: int x;
TEST_F(ParserTest, ParseTypeInt)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: long x;
TEST_F(ParserTest, ParseTypeLong)
{
    SetTokenSequence({ { TOKEN_LONG, "long" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("long x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_LONG, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: float x;
TEST_F(ParserTest, ParseTypeFloat)
{
    SetTokenSequence({ { TOKEN_FLOAT, "float" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("float x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_FLOAT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: double x;
TEST_F(ParserTest, ParseTypeDouble)
{
    SetTokenSequence({ { TOKEN_DOUBLE, "double" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("double x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_DOUBLE, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: signed x;
TEST_F(ParserTest, ParseTypeSigned)
{
    SetTokenSequence({ { TOKEN_SIGNED, "signed" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("signed x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_EQ(SIGNED_SIGNED, decl->u.var.specifiers->type_specs->u.basic->u.char_t.signedness);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: unsigned x;
TEST_F(ParserTest, ParseTypeUnsigned)
{
    SetTokenSequence({ { TOKEN_UNSIGNED, "unsigned" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("unsigned x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_EQ(SIGNED_UNSIGNED, decl->u.var.specifiers->type_specs->u.basic->u.char_t.signedness);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: _Bool x;
TEST_F(ParserTest, ParseTypeBool)
{
    SetTokenSequence({ { TOKEN_BOOL, "_Bool" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Bool x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_BOOL, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: _Complex x;
TEST_F(ParserTest, ParseTypeComplex)
{
    SetTokenSequence({ { TOKEN_COMPLEX, "_Complex" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Complex x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_COMPLEX, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: _Imaginary x;
TEST_F(ParserTest, ParseTypeImaginary)
{
    SetTokenSequence({ { TOKEN_IMAGINARY, "_Imaginary" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Imaginary x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_BASIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_IMAGINARY, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: struct S { int x; } s;
TEST_F(ParserTest, ParseTypeStruct)
{
    SetTokenSequence({ { TOKEN_STRUCT, "struct" },
                       { TOKEN_IDENTIFIER, "S" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_IDENTIFIER, "s" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("struct S { int x; } s;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_STRUCT, decl->u.var.specifiers->type_specs->kind);
    EXPECT_STREQ("S", decl->u.var.specifiers->type_specs->u.struct_spec.name);
    EXPECT_NE(nullptr, decl->u.var.specifiers->type_specs->u.struct_spec.fields);
    EXPECT_FALSE(decl->u.var.specifiers->type_specs->u.struct_spec.fields->is_anonymous);
    EXPECT_STREQ("x", decl->u.var.specifiers->type_specs->u.struct_spec.fields->u.named.name);
    EXPECT_EQ(TYPE_INT,
              decl->u.var.specifiers->type_specs->u.struct_spec.fields->u.named.type->kind);
    EXPECT_STREQ("s", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: struct { int x; } s;
TEST_F(ParserTest, ParseTypeAnonymousStruct)
{
    SetTokenSequence({ { TOKEN_STRUCT, "struct" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_IDENTIFIER, "s" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("struct { int x; } s;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_STRUCT, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(nullptr, decl->u.var.specifiers->type_specs->u.struct_spec.name);
    EXPECT_NE(nullptr, decl->u.var.specifiers->type_specs->u.struct_spec.fields);
    EXPECT_FALSE(decl->u.var.specifiers->type_specs->u.struct_spec.fields->is_anonymous);
    EXPECT_STREQ("x", decl->u.var.specifiers->type_specs->u.struct_spec.fields->u.named.name);
    EXPECT_EQ(TYPE_INT,
              decl->u.var.specifiers->type_specs->u.struct_spec.fields->u.named.type->kind);
    EXPECT_STREQ("s", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: union U { int x; } u;
TEST_F(ParserTest, ParseTypeUnion)
{
    SetTokenSequence({ { TOKEN_UNION, "union" },
                       { TOKEN_IDENTIFIER, "U" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_IDENTIFIER, "u" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("union U { int x; } u;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_UNION, decl->u.var.specifiers->type_specs->kind);
    EXPECT_STREQ("U", decl->u.var.specifiers->type_specs->u.struct_spec.name);
    EXPECT_NE(nullptr, decl->u.var.specifiers->type_specs->u.struct_spec.fields);
    EXPECT_FALSE(decl->u.var.specifiers->type_specs->u.struct_spec.fields->is_anonymous);
    EXPECT_STREQ("x", decl->u.var.specifiers->type_specs->u.struct_spec.fields->u.named.name);
    EXPECT_EQ(TYPE_INT,
              decl->u.var.specifiers->type_specs->u.struct_spec.fields->u.named.type->kind);
    EXPECT_STREQ("u", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: enum E { A };
TEST_F(ParserTest, ParseTypeEnum)
{
    SetTokenSequence({ { TOKEN_ENUM, "enum" },
                       { TOKEN_IDENTIFIER, "E" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_IDENTIFIER, "A" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("enum E { A };");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_ENUM, decl->u.var.specifiers->type_specs->kind);
    EXPECT_STREQ("E", decl->u.var.specifiers->type_specs->u.enum_spec.name);
    EXPECT_NE(nullptr, decl->u.var.specifiers->type_specs->u.enum_spec.enumerators);
    EXPECT_STREQ("A", decl->u.var.specifiers->type_specs->u.enum_spec.enumerators->name);
    EXPECT_EQ(nullptr, decl->u.var.specifiers->type_specs->u.enum_spec.enumerators->value);
}

// Test type specifier: enum { A };
TEST_F(ParserTest, ParseTypeAnonymousEnum)
{
    SetTokenSequence({ { TOKEN_ENUM, "enum" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_IDENTIFIER, "A" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("enum { A };");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_ENUM, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(nullptr, decl->u.var.specifiers->type_specs->u.enum_spec.name);
    EXPECT_NE(nullptr, decl->u.var.specifiers->type_specs->u.enum_spec.enumerators);
    EXPECT_STREQ("A", decl->u.var.specifiers->type_specs->u.enum_spec.enumerators->name);
}

// Test type specifier: typedef int T; T x;
TEST_F(ParserTest, ParseTypeTypedef)
{
    SetTokenSequence({ { TOKEN_TYPEDEF, "typedef" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "T" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_TYPEDEF_NAME, "T" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("typedef int T; T x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(STORAGE_CLASS_TYPEDEF, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("T", decl->u.var.declarators->declarator->u.named.name);
    EXPECT_NE(nullptr, ext->next);
    decl = ext->next->u.declaration;
    EXPECT_EQ(TYPE_SPEC_TYPEDEF_NAME, decl->u.var.specifiers->type_specs->kind);
    EXPECT_STREQ("T", decl->u.var.specifiers->type_specs->u.typedef_name);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type specifier: _Atomic(int) x;
TEST_F(ParserTest, ParseTypeAtomic)
{
    SetTokenSequence({ { TOKEN_ATOMIC, "_Atomic" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Atomic(int) x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(TYPE_SPEC_ATOMIC, decl->u.var.specifiers->type_specs->kind);
    EXPECT_EQ(TYPE_ATOMIC, decl->u.var.specifiers->type_specs->u.atomic->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.atomic->u.atomic.base->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type qualifier: const int x;
TEST_F(ParserTest, ParseTypeQualifierConst)
{
    SetTokenSequence({ { TOKEN_CONST, "const" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("const int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->qualifiers);
    EXPECT_EQ(TYPE_QUALIFIER_CONST, decl->u.var.specifiers->qualifiers->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type qualifier: restrict int x;
TEST_F(ParserTest, ParseTypeQualifierRestrict)
{
    SetTokenSequence({ { TOKEN_RESTRICT, "restrict" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("restrict int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->qualifiers);
    EXPECT_EQ(TYPE_QUALIFIER_RESTRICT, decl->u.var.specifiers->qualifiers->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type qualifier: volatile int x;
TEST_F(ParserTest, ParseTypeQualifierVolatile)
{
    SetTokenSequence({ { TOKEN_VOLATILE, "volatile" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("volatile int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->qualifiers);
    EXPECT_EQ(TYPE_QUALIFIER_VOLATILE, decl->u.var.specifiers->qualifiers->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type qualifier: _Atomic int x;
TEST_F(ParserTest, ParseTypeQualifierAtomic)
{
    SetTokenSequence({ { TOKEN_ATOMIC, "_Atomic" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Atomic int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->qualifiers);
    EXPECT_EQ(TYPE_QUALIFIER_ATOMIC, decl->u.var.specifiers->qualifiers->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test storage class: typedef int x;
TEST_F(ParserTest, ParseStorageClassTypedef)
{
    SetTokenSequence({ { TOKEN_TYPEDEF, "typedef" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("typedef int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->storage);
    EXPECT_EQ(STORAGE_CLASS_TYPEDEF, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test storage class: extern int x;
TEST_F(ParserTest, ParseStorageClassExtern)
{
    SetTokenSequence({ { TOKEN_EXTERN, "extern" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("extern int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->storage);
    EXPECT_EQ(STORAGE_CLASS_EXTERN, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test storage class: static int x;
TEST_F(ParserTest, ParseStorageClassStatic)
{
    SetTokenSequence({ { TOKEN_STATIC, "static" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("static int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->storage);
    EXPECT_EQ(STORAGE_CLASS_STATIC, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test storage class: _Thread_local int x;
TEST_F(ParserTest, ParseStorageClassThreadLocal)
{
    SetTokenSequence({ { TOKEN_THREAD_LOCAL, "_Thread_local" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Thread_local int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->storage);
    EXPECT_EQ(STORAGE_CLASS_THREAD_LOCAL, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test storage class: auto int x;
TEST_F(ParserTest, ParseStorageClassAuto)
{
    SetTokenSequence({ { TOKEN_AUTO, "auto" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("auto int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->storage);
    EXPECT_EQ(STORAGE_CLASS_AUTO, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test storage class: register int x;
TEST_F(ParserTest, ParseStorageClassRegister)
{
    SetTokenSequence({ { TOKEN_REGISTER, "register" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("register int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->storage);
    EXPECT_EQ(STORAGE_CLASS_REGISTER, decl->u.var.specifiers->storage->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test function specifier: inline int f() {}
TEST_F(ParserTest, ParseFunctionSpecifierInline)
{
    SetTokenSequence({ { TOKEN_INLINE, "inline" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("inline int f() {}");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, ext->kind);
    EXPECT_NE(nullptr, ext->u.function.specifiers->func_specs);
    EXPECT_EQ(FUNC_SPEC_INLINE, ext->u.function.specifiers->func_specs->kind);
    EXPECT_EQ(TYPE_INT, ext->u.function.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("f", ext->u.function.declarator->u.named.name);
}

// Test function specifier: _Noreturn void f() {}
TEST_F(ParserTest, ParseFunctionSpecifierNoreturn)
{
    SetTokenSequence({ { TOKEN_NORETURN, "_Noreturn" },
                       { TOKEN_VOID, "void" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Noreturn void f() {}");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_FUNCTION, ext->kind);
    EXPECT_NE(nullptr, ext->u.function.specifiers->func_specs);
    EXPECT_EQ(FUNC_SPEC_NORETURN, ext->u.function.specifiers->func_specs->kind);
    EXPECT_EQ(TYPE_VOID, ext->u.function.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("f", ext->u.function.declarator->u.named.name);
}

// Test alignment specifier: _Alignas(int) int x;
TEST_F(ParserTest, ParseAlignmentSpecifierType)
{
    SetTokenSequence({ { TOKEN_ALIGNAS, "_Alignas" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Alignas(int) int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->align_spec);
    EXPECT_EQ(ALIGN_SPEC_TYPE, decl->u.var.specifiers->align_spec->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->align_spec->u.type->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test alignment specifier: _Alignas(8) int x;
TEST_F(ParserTest, ParseAlignmentSpecifierExpr)
{
    SetTokenSequence({ { TOKEN_ALIGNAS, "_Alignas" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_I_CONSTANT, "8" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Alignas(8) int x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_NE(nullptr, decl->u.var.specifiers->align_spec);
    EXPECT_EQ(ALIGN_SPEC_EXPR, decl->u.var.specifiers->align_spec->kind);
    EXPECT_EQ(EXPR_LITERAL, decl->u.var.specifiers->align_spec->u.expr->kind);
    EXPECT_EQ(8, decl->u.var.specifiers->align_spec->u.expr->u.literal->u.int_val);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->declarator->u.named.name);
}

// Test type name: (int) x
TEST_F(ParserTest, ParseTypeNameSimple)
{
    SetTokenSequence({ { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("(int) x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(EXPR_CAST, decl->u.var.declarators->init->u.expr->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.declarators->init->u.expr->u.cast.type->kind);
    EXPECT_STREQ("x", decl->u.var.declarators->init->u.expr->u.cast.expr->u.var);
}

// Test type name: (const int*) x
TEST_F(ParserTest, ParseTypeNameQualified)
{
    SetTokenSequence({ { TOKEN_LPAREN, "(" },
                       { TOKEN_CONST, "const" },
                       { TOKEN_INT, "int" },
                       { TOKEN_STAR, "*" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("(const int*) x;");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(EXPR_CAST, decl->u.var.declarators->init->u.expr->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.declarators->init->u.expr->u.cast.type->kind);
    EXPECT_NE(nullptr, decl->u.var.declarators->init->u.expr->u.cast.type->qualifiers);
    EXPECT_EQ(TYPE_QUALIFIER_CONST,
              decl->u.var.declarators->init->u.expr->u.cast.type->qualifiers->kind);
    EXPECT_NE(nullptr,
              decl->u.var.declarators->init->u.expr->u.cast.type->qualifiers->next); // Pointer
    EXPECT_STREQ("x", decl->u.var.declarators->init->u.expr->u.cast.expr->u.var);
}

TEST_F(ParserTest, ParseFunctionDeclaration)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int f();");
    Program *program = parse(f);
    fclose(f);

    ExternalDecl *ext = GetExternalDecl(program);
    EXPECT_EQ(EXTERNAL_DECL_DECLARATION, ext->kind);
    Declaration *decl = ext->u.declaration;
    EXPECT_EQ(DECL_VAR, decl->kind);
    EXPECT_EQ(TYPE_INT, decl->u.var.specifiers->type_specs->u.basic->kind);
    EXPECT_STREQ("f", decl->u.var.declarators->declarator->u.named.name);
    EXPECT_NE(nullptr, decl->u.var.declarators->declarator->u.named.suffixes);
    EXPECT_EQ(SUFFIX_FUNCTION, decl->u.var.declarators->declarator->u.named.suffixes->kind);
    EXPECT_TRUE(decl->u.var.declarators->declarator->u.named.suffixes->u.function.params->is_empty);
}

// Main for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
