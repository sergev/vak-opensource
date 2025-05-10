#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "scanner.h"

// Test fixture for scanner tests
class ScannerTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        // Reset scanner state
        init_scanner(nullptr);
    }

    // Helper function to set scanner input from a string
    void SetInput(const std::string &input)
    {
        // Create a temporary file with the input
        temp_file = tmpfile();
        fwrite(input.c_str(), 1, input.size(), temp_file);
        rewind(temp_file);
        init_scanner(temp_file);
    }

    // Helper function to get next token
    int GetNextToken() { return yylex(); }

    // Helper function to get current lexeme
    std::string GetLexeme() { return std::string(get_yytext()); }

    void TearDown() override
    {
        if (temp_file) {
            fclose(temp_file);
        }
        init_scanner(nullptr);
    }

private:
    FILE *temp_file = nullptr;
};

// Test keywords
TEST_F(ScannerTest, HandlesKeywords)
{
    SetInput("int char double if else while for");
    EXPECT_EQ(GetNextToken(), TOKEN_INT);
    EXPECT_EQ(GetLexeme(), "int");
    EXPECT_EQ(GetNextToken(), TOKEN_CHAR);
    EXPECT_EQ(GetLexeme(), "char");
    EXPECT_EQ(GetNextToken(), TOKEN_DOUBLE);
    EXPECT_EQ(GetLexeme(), "double");
    EXPECT_EQ(GetNextToken(), TOKEN_IF);
    EXPECT_EQ(GetLexeme(), "if");
    EXPECT_EQ(GetNextToken(), TOKEN_ELSE);
    EXPECT_EQ(GetLexeme(), "else");
    EXPECT_EQ(GetNextToken(), TOKEN_WHILE);
    EXPECT_EQ(GetLexeme(), "while");
    EXPECT_EQ(GetNextToken(), TOKEN_FOR);
    EXPECT_EQ(GetLexeme(), "for");
}

// Test identifiers
TEST_F(ScannerTest, HandlesIdentifiers)
{
    SetInput("variable my_function _hidden123");
    EXPECT_EQ(GetNextToken(), TOKEN_IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "variable");
    EXPECT_EQ(GetNextToken(), TOKEN_IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "my_function");
    EXPECT_EQ(GetNextToken(), TOKEN_IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "_hidden123");
}

// Test integer constants
TEST_F(ScannerTest, HandlesIntegerConstants)
{
    SetInput("123 0x1a 0777 42u");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "123");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "0x1a");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "0777");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "42u");
}

// Test floating-point constants
TEST_F(ScannerTest, HandlesFloatingPointConstants)
{
    SetInput("3.14 1e-10 0x1.2p3");
    EXPECT_EQ(GetNextToken(), TOKEN_F_CONSTANT);
    EXPECT_EQ(GetLexeme(), "3.14");
    EXPECT_EQ(GetNextToken(), TOKEN_F_CONSTANT);
    EXPECT_EQ(GetLexeme(), "1e-10");
    EXPECT_EQ(GetNextToken(), TOKEN_F_CONSTANT);
    EXPECT_EQ(GetLexeme(), "0x1.2p3");
}

// Test string literals
TEST_F(ScannerTest, HandlesStringLiterals)
{
    SetInput("\"hello\" L\"world\" u8\"utf8\" \"escaped\\nchar\"");
    EXPECT_EQ(GetNextToken(), TOKEN_STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "\"hello\"");
    EXPECT_EQ(GetNextToken(), TOKEN_STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "L\"world\"");
    EXPECT_EQ(GetNextToken(), TOKEN_STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "u8\"utf8\"");
    EXPECT_EQ(GetNextToken(), TOKEN_STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "\"escaped\\nchar\"");
}

// Test char literals
TEST_F(ScannerTest, HandlesCharLiterals)
{
    SetInput("'h' L'w' u8'u' '\\n'");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "'h'");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "L'w'");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "u8'u'");
    EXPECT_EQ(GetNextToken(), TOKEN_I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "'\\n'");
}

// Test operators
TEST_F(ScannerTest, HandlesOperators)
{
    SetInput("+= -= *= /= %= >>= <<= && || == != <= >=");
    EXPECT_EQ(GetNextToken(), TOKEN_ADD_ASSIGN);
    EXPECT_EQ(GetLexeme(), "+=");
    EXPECT_EQ(GetNextToken(), TOKEN_SUB_ASSIGN);
    EXPECT_EQ(GetLexeme(), "-=");
    EXPECT_EQ(GetNextToken(), TOKEN_MUL_ASSIGN);
    EXPECT_EQ(GetLexeme(), "*=");
    EXPECT_EQ(GetNextToken(), TOKEN_DIV_ASSIGN);
    EXPECT_EQ(GetLexeme(), "/=");
    EXPECT_EQ(GetNextToken(), TOKEN_MOD_ASSIGN);
    EXPECT_EQ(GetLexeme(), "%=");
    EXPECT_EQ(GetNextToken(), TOKEN_RIGHT_ASSIGN);
    EXPECT_EQ(GetLexeme(), ">>=");
    EXPECT_EQ(GetNextToken(), TOKEN_LEFT_ASSIGN);
    EXPECT_EQ(GetLexeme(), "<<=");
    EXPECT_EQ(GetNextToken(), TOKEN_AND_OP);
    EXPECT_EQ(GetLexeme(), "&&");
    EXPECT_EQ(GetNextToken(), TOKEN_OR_OP);
    EXPECT_EQ(GetLexeme(), "||");
    EXPECT_EQ(GetNextToken(), TOKEN_EQ_OP);
    EXPECT_EQ(GetLexeme(), "==");
    EXPECT_EQ(GetNextToken(), TOKEN_NE_OP);
    EXPECT_EQ(GetLexeme(), "!=");
    EXPECT_EQ(GetNextToken(), TOKEN_LE_OP);
    EXPECT_EQ(GetLexeme(), "<=");
    EXPECT_EQ(GetNextToken(), TOKEN_GE_OP);
    EXPECT_EQ(GetLexeme(), ">=");
}

// Test single-character tokens
TEST_F(ScannerTest, HandlesSingleCharacterTokens)
{
    SetInput("; , : = ( ) { } [ ] . & * + -");
    EXPECT_EQ(GetNextToken(), TOKEN_SEMICOLON);
    EXPECT_EQ(GetLexeme(), ";");
    EXPECT_EQ(GetNextToken(), TOKEN_COMMA);
    EXPECT_EQ(GetLexeme(), ",");
    EXPECT_EQ(GetNextToken(), TOKEN_COLON);
    EXPECT_EQ(GetLexeme(), ":");
    EXPECT_EQ(GetNextToken(), TOKEN_ASSIGN);
    EXPECT_EQ(GetLexeme(), "=");
    EXPECT_EQ(GetNextToken(), TOKEN_LPAREN);
    EXPECT_EQ(GetLexeme(), "(");
    EXPECT_EQ(GetNextToken(), TOKEN_RPAREN);
    EXPECT_EQ(GetLexeme(), ")");
    EXPECT_EQ(GetNextToken(), TOKEN_LBRACE);
    EXPECT_EQ(GetLexeme(), "{");
    EXPECT_EQ(GetNextToken(), TOKEN_RBRACE);
    EXPECT_EQ(GetLexeme(), "}");
    EXPECT_EQ(GetNextToken(), TOKEN_LBRACKET);
    EXPECT_EQ(GetLexeme(), "[");
    EXPECT_EQ(GetNextToken(), TOKEN_RBRACKET);
    EXPECT_EQ(GetLexeme(), "]");
    EXPECT_EQ(GetNextToken(), TOKEN_DOT);
    EXPECT_EQ(GetLexeme(), ".");
    EXPECT_EQ(GetNextToken(), TOKEN_AMPERSAND);
    EXPECT_EQ(GetLexeme(), "&");
    EXPECT_EQ(GetNextToken(), TOKEN_STAR);
    EXPECT_EQ(GetLexeme(), "*");
    EXPECT_EQ(GetNextToken(), TOKEN_PLUS);
    EXPECT_EQ(GetLexeme(), "+");
    EXPECT_EQ(GetNextToken(), TOKEN_MINUS);
    EXPECT_EQ(GetLexeme(), "-");
}

// Test comments
TEST_F(ScannerTest, HandlesComments)
{
    SetInput("/* comment */ // line comment\n int");
    // Comments are consumed, so we should get the next token
    EXPECT_EQ(GetNextToken(), TOKEN_INT);
    EXPECT_EQ(GetLexeme(), "int");
}

// Test whitespace
TEST_F(ScannerTest, HandlesWhitespace)
{
    SetInput("  \t\n int \t var");
    EXPECT_EQ(GetNextToken(), TOKEN_INT);
    EXPECT_EQ(GetLexeme(), "int");
    EXPECT_EQ(GetNextToken(), TOKEN_IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "var");
}

// Test end of input
TEST_F(ScannerTest, HandlesEndOfInput)
{
    SetInput("int");
    EXPECT_EQ(GetNextToken(), TOKEN_INT);
    EXPECT_EQ(GetNextToken(), TOKEN_EOF);
}

// Main function for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
