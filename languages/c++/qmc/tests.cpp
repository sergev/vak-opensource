#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

// Function from minimizer.cpp
std::string minimize_boolean_function(const std::string &truth_table_str);

//                                      bool should_print_truth_table,
//                                      bool should_print_implicant_table, bool debug,
//                                      size_t thread_threshold);

// Test fixture for setting common parameters
class MinimizerTest : public ::testing::Test {
protected:
    bool should_print_truth_table     = false;
    bool should_print_implicant_table = false;
    bool debug                        = false;
    size_t thread_threshold           = 10;
};

// 2-variable function test
TEST_F(MinimizerTest, TwoVariables)
{
    std::string truth_table = "0101";
    std::string result      = minimize_boolean_function(truth_table);
    EXPECT_EQ(result, "B");
}

// 3-variable function test
TEST_F(MinimizerTest, ThreeVariables)
{
    // Truth table for f(A,B,C) = Σ(1,3,5) + d(4,6)
    // ABC: 000 001 010 011 100 101 110 111
    //      0   1   0   1   X   1   X   0
    std::string truth_table = "0101X1X0";
    std::string result      = minimize_boolean_function(truth_table);
    EXPECT_EQ(result, "~AC + ~BC");
}

// 4-variable function test
TEST_F(MinimizerTest, FourVariables)
{
    // Truth table for f(A,B,C,D) = Σ(0,5,10,15) + d(2,7,12)
    // ABCD: 0000 0001 0010 0011 0100 0101 0110 0111
    //       1    0    X    0    0    1    0    X
    //       1000 1001 1010 1011 1100 1101 1110 1111
    //       0    0    1    0    X    0    0    1
    std::string truth_table = "10X0010X0010X001";
    std::string result      = minimize_boolean_function(truth_table);
    EXPECT_EQ(result, "BCD + ~BC~D + ~ABD + ~A~B~D");
}

// 5-variable function test
TEST_F(MinimizerTest, FiveVariables)
{
    // Truth table for f(A,B,C,D,E) = Σ(3,9,17,25,31) + d(0,10,20,30)
    // ABCDE: 00000 00001 00010 00011 ... 11111
    //        X     0     0     1    ... 1
    std::string truth_table = "X001000001X0000001000000100000X1";
    std::string result      = minimize_boolean_function(truth_table);
    // Expected: ~A~BCD + A~BCD + ABC~D + ABCD (covers 3,9,17,25,31)
    EXPECT_EQ(result, "A~C~DE + ~A~B~CDE + ABCD + B~C~DE");
}

// 6-variable function test
TEST_F(MinimizerTest, SixVariables)
{
    // Truth table with many don't cares
    // f(A,B,C,D,E,F) = Σ(1,63) + d(0,2,4,8,16,32)
    //                                   1         2         3         4         5         6
    //                         0123456789012345678901234567890123456789012345678901234567890123
    std::string truth_table = "X1X0X0X0X0000000X000000000000000X0000000000000000000000000000001";
    std::string result      = minimize_boolean_function(truth_table);
    EXPECT_EQ(result, "~A~B~C~D~E + ABCDEF");
}

// 7-variable function test
TEST_F(MinimizerTest, SevenVariables)
{
    // Truth table for f(A,B,C,D,E,F,G) = Σ(5,21,85,127) + d(1,3,64,65)
    //                                   1         2         3         4         5         6
    //                         0123456789012345678901234567890123456789012345678901234567890123
    std::string truth_table = "0X0X010000000000000001000000000000000000000000000000000000000000"
    //                               7         8         9        10        11        12
    //                         4567890123456789012345678901234567890123456789012345678901234567
                              "XX00000000000000000001000000000000000000000000000000000000000001";
    std::string result = minimize_boolean_function(truth_table);
    EXPECT_EQ(result, "~BC~DE~FG + ABCDEFG + ~A~B~C~D~FG");
}

// 8-variable function test
TEST_F(MinimizerTest, EightVariables)
{
    // Truth table for f(A,B,C,D,E,F,G,H) = Σ(10,100,255) + d(0,1,2,3,4)
    std::string truth_table(256, '0'); // Initialize with zeros
    truth_table[0]     = 'X';
    truth_table[1]     = 'X';
    truth_table[2]     = 'X';
    truth_table[3]     = 'X';
    truth_table[4]     = 'X';
    truth_table[10]    = '1'; // Minterm 10
    truth_table[100]   = '1'; // Minterm 100
    truth_table[255]   = '1'; // Minterm 255
    std::string result = minimize_boolean_function(truth_table);
    EXPECT_EQ(result, "~A~B~C~D~FG~H + ~ABC~D~EF~G~H + ABCDEFGH");
}
