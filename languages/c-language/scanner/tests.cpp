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
    EXPECT_EQ(GetNextToken(), INT);
    EXPECT_EQ(GetLexeme(), "int");
    EXPECT_EQ(GetNextToken(), CHAR);
    EXPECT_EQ(GetLexeme(), "char");
    EXPECT_EQ(GetNextToken(), DOUBLE);
    EXPECT_EQ(GetLexeme(), "double");
    EXPECT_EQ(GetNextToken(), IF);
    EXPECT_EQ(GetLexeme(), "if");
    EXPECT_EQ(GetNextToken(), ELSE);
    EXPECT_EQ(GetLexeme(), "else");
    EXPECT_EQ(GetNextToken(), WHILE);
    EXPECT_EQ(GetLexeme(), "while");
    EXPECT_EQ(GetNextToken(), FOR);
    EXPECT_EQ(GetLexeme(), "for");
}

// Test identifiers
TEST_F(ScannerTest, HandlesIdentifiers)
{
    SetInput("variable my_function _hidden123");
    EXPECT_EQ(GetNextToken(), IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "variable");
    EXPECT_EQ(GetNextToken(), IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "my_function");
    EXPECT_EQ(GetNextToken(), IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "_hidden123");
}

// Test integer constants
TEST_F(ScannerTest, HandlesIntegerConstants)
{
    SetInput("123 0x1a 0777 42u");
    EXPECT_EQ(GetNextToken(), I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "123");
    EXPECT_EQ(GetNextToken(), I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "0x1a");
    EXPECT_EQ(GetNextToken(), I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "0777");
    EXPECT_EQ(GetNextToken(), I_CONSTANT);
    EXPECT_EQ(GetLexeme(), "42u");
}

// Test floating-point constants
TEST_F(ScannerTest, HandlesFloatingPointConstants)
{
    SetInput("3.14 1e-10 0x1.2p3");
    EXPECT_EQ(GetNextToken(), F_CONSTANT);
    EXPECT_EQ(GetLexeme(), "3.14");
    EXPECT_EQ(GetNextToken(), F_CONSTANT);
    EXPECT_EQ(GetLexeme(), "1e-10");
    EXPECT_EQ(GetNextToken(), F_CONSTANT);
    EXPECT_EQ(GetLexeme(), "0x1.2p3");
}

// Test string literals
TEST_F(ScannerTest, HandlesStringLiterals)
{
    SetInput("\"hello\" L\"world\" \"escaped\\nchar\"");
    EXPECT_EQ(GetNextToken(), STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "\"hello\"");
    EXPECT_EQ(GetNextToken(), STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "L\"world\"");
    EXPECT_EQ(GetNextToken(), STRING_LITERAL);
    EXPECT_EQ(GetLexeme(), "\"escaped\\nchar\"");
}

// Test operators
TEST_F(ScannerTest, HandlesOperators)
{
    SetInput("+= -= *= /= %= >>= <<= && || == != <= >=");
    EXPECT_EQ(GetNextToken(), ADD_ASSIGN);
    EXPECT_EQ(GetLexeme(), "+=");
    EXPECT_EQ(GetNextToken(), SUB_ASSIGN);
    EXPECT_EQ(GetLexeme(), "-=");
    EXPECT_EQ(GetNextToken(), MUL_ASSIGN);
    EXPECT_EQ(GetLexeme(), "*=");
    EXPECT_EQ(GetNextToken(), DIV_ASSIGN);
    EXPECT_EQ(GetLexeme(), "/=");
    EXPECT_EQ(GetNextToken(), MOD_ASSIGN);
    EXPECT_EQ(GetLexeme(), "%=");
    EXPECT_EQ(GetNextToken(), RIGHT_ASSIGN);
    EXPECT_EQ(GetLexeme(), ">>=");
    EXPECT_EQ(GetNextToken(), LEFT_ASSIGN);
    EXPECT_EQ(GetLexeme(), "<<=");
    EXPECT_EQ(GetNextToken(), AND_OP);
    EXPECT_EQ(GetLexeme(), "&&");
    EXPECT_EQ(GetNextToken(), OR_OP);
    EXPECT_EQ(GetLexeme(), "||");
    EXPECT_EQ(GetNextToken(), EQ_OP);
    EXPECT_EQ(GetLexeme(), "==");
    EXPECT_EQ(GetNextToken(), NE_OP);
    EXPECT_EQ(GetLexeme(), "!=");
    EXPECT_EQ(GetNextToken(), LE_OP);
    EXPECT_EQ(GetLexeme(), "<=");
    EXPECT_EQ(GetNextToken(), GE_OP);
    EXPECT_EQ(GetLexeme(), ">=");
}

// Test single-character tokens
TEST_F(ScannerTest, HandlesSingleCharacterTokens)
{
    SetInput("; , ( ) { } [ ] . & * + -");
    EXPECT_EQ(GetNextToken(), ';');
    EXPECT_EQ(GetLexeme(), ";");
    EXPECT_EQ(GetNextToken(), ',');
    EXPECT_EQ(GetLexeme(), ",");
    EXPECT_EQ(GetNextToken(), '(');
    EXPECT_EQ(GetLexeme(), "(");
    EXPECT_EQ(GetNextToken(), ')');
    EXPECT_EQ(GetLexeme(), ")");
    EXPECT_EQ(GetNextToken(), '{');
    EXPECT_EQ(GetLexeme(), "{");
    EXPECT_EQ(GetNextToken(), '}');
    EXPECT_EQ(GetLexeme(), "}");
    EXPECT_EQ(GetNextToken(), '[');
    EXPECT_EQ(GetLexeme(), "[");
    EXPECT_EQ(GetNextToken(), ']');
    EXPECT_EQ(GetLexeme(), "]");
    EXPECT_EQ(GetNextToken(), '.');
    EXPECT_EQ(GetLexeme(), ".");
    EXPECT_EQ(GetNextToken(), '&');
    EXPECT_EQ(GetLexeme(), "&");
    EXPECT_EQ(GetNextToken(), '*');
    EXPECT_EQ(GetLexeme(), "*");
    EXPECT_EQ(GetNextToken(), '+');
    EXPECT_EQ(GetLexeme(), "+");
    EXPECT_EQ(GetNextToken(), '-');
    EXPECT_EQ(GetLexeme(), "-");
}

// Test comments
TEST_F(ScannerTest, HandlesComments)
{
    SetInput("/* comment */ // line comment\n int");
    // Comments are consumed, so we should get the next token
    EXPECT_EQ(GetNextToken(), INT);
    EXPECT_EQ(GetLexeme(), "int");
}

// Test whitespace
TEST_F(ScannerTest, HandlesWhitespace)
{
    SetInput("  \t\n int \t var");
    EXPECT_EQ(GetNextToken(), INT);
    EXPECT_EQ(GetLexeme(), "int");
    EXPECT_EQ(GetNextToken(), IDENTIFIER);
    EXPECT_EQ(GetLexeme(), "var");
}

// Test end of input
TEST_F(ScannerTest, HandlesEndOfInput)
{
    SetInput("int");
    EXPECT_EQ(GetNextToken(), INT);
    EXPECT_EQ(GetNextToken(), 0); // EOF
}

// Main function for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
