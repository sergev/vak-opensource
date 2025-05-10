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

// Test error: Missing semicolon in declaration: int x
TEST_F(ParserTest, MissingSemicolonDeclaration)
{
    SetTokenSequence({ { TOKEN_INT, "int" }, { TOKEN_IDENTIFIER, "x" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int x");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Invalid type in declaration: invalid x;
TEST_F(ParserTest, InvalidTypeDeclaration)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "invalid" },
                       { TOKEN Identifier, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("invalid x;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Missing identifier in declaration: int = 42;
TEST_F(ParserTest, MissingIdentifierDeclaration)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_ASSIGN, "=" },
                       { TOKEN_I_CONSTANT, "42" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int = 42;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Unclosed struct: struct S { int x;
TEST_F(ParserTest, UnclosedStructDeclaration)
{
    SetTokenSequence({ { TOKEN_STRUCT, "struct" },
                       { TOKEN_IDENTIFIER, "S" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("struct S { int x;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Missing semicolon in statement: x = 1
TEST_F(ParserTest, MissingSemicolonStatement)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_ASSIGN, "=" },
                       { TOKEN_I_CONSTANT, "1" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x = 1");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Unclosed block: { x = 1
TEST_F(ParserTest, UnclosedBlockStatement)
{
    SetTokenSequence({ { TOKEN_LBRACE, "{" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_ASSIGN, "=" },
                       { TOKEN_I_CONSTANT, "1" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("{ x = 1");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Incomplete if: if (x
TEST_F(ParserTest, IncompleteIfStatement)
{
    SetTokenSequence({ { TOKEN_IF, "if" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("if (x");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Missing condition in while: while () x;
TEST_F(ParserTest, MissingConditionWhileStatement)
{
    SetTokenSequence({ { TOKEN_WHILE, "while" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("while () x;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Mismatched parentheses in expression: (x + y
TEST_F(ParserTest, MismatchedParenthesesExpression)
{
    SetTokenSequence({ { TOKEN_LPAREN, "(" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_PLUS, "+" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("(x + y");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Missing operand in expression: x + ;
TEST_F(ParserTest, MissingOperandExpression)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_PLUS, "+" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x + ;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Invalid operator in expression: x @ y;
TEST_F(ParserTest, InvalidOperatorExpression)
{
    SetTokenSequence({ { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_IDENTIFIER, "@" },
                       { TOKEN_IDENTIFIER, "y" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("x @ y;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Unclosed string in expression: "hello
TEST_F(ParserTest, UnclosedStringExpression)
{
    SetTokenSequence(
        { { TOKEN_STRING_LITERAL, "hello" }, { TOKEN_SEMICOLON, ";" }, { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("\"hello");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Invalid type specifier: int int x;
TEST_F(ParserTest, InvalidTypeSpecifier)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "x" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int int x;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Incomplete typedef: typedef int;
TEST_F(ParserTest, IncompleteTypedef)
{
    SetTokenSequence({ { TOKEN_TYPEDEF, "typedef" },
                       { TOKEN_INT, "int" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("typedef int;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Invalid static assert: _Static_assert(1;
TEST_F(ParserTest, InvalidStaticAssert)
{
    SetTokenSequence({ { TOKEN_STATIC_ASSERT, "_Static_assert" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_I_CONSTANT, "1" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Static_assert(1;");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Unclosed _Alignas: _Alignas(int
TEST_F(ParserTest, UnclosedAlignas)
{
    SetTokenSequence({ { TOKEN_ALIGNAS, "_Alignas" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_SEMICOLON, ";" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("_Alignas(int");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Test error: Invalid parameter in function: int f(int,) {}
TEST_F(ParserTest, InvalidFunctionParameter)
{
    SetTokenSequence({ { TOKEN_INT, "int" },
                       { TOKEN_IDENTIFIER, "f" },
                       { TOKEN_LPAREN, "(" },
                       { TOKEN_INT, "int" },
                       { TOKEN_COMMA, "," },
                       { TOKEN_RPAREN, ")" },
                       { TOKEN_LBRACE, "{" },
                       { TOKEN_RBRACE, "}" },
                       { TOKEN_EOF, "" } });

    FILE *f          = CreateTempFile("int f(int,) {}");
    Program *program = parse(f);
    fclose(f);

    EXPECT_EQ(nullptr, program);
}

// Main for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}