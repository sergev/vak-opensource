#include <climits>
#include "gtest/gtest.h"

//
// Add two integer numbers.
// Throw an exception in case of overflow.
//
int add(int a, int b)
{
    int sum;

    if (a >= 0) {
        if (b >= 0) {
            // Both non-negative.
            sum = (a - INT_MAX) + b;
            if (sum > 0)
                throw std::overflow_error("Integer overflow");
            sum += INT_MAX;
        } else {
            // Opposite signs.
            sum = a + b;
        }
    } else {
        if (b >= 0) {
            // Opposite signs.
            sum = a + b;
        } else {
            // Both negative.
            sum = (a - INT_MIN) + b;
            if (sum < 0)
                throw std::overflow_error("Integer overflow");
            sum += INT_MIN;
        }
    }
    return sum;
}

//
// Check valid inputs.
//
TEST(add, check_zero)
{
    EXPECT_EQ(add(0,         INT_MIN),   INT_MIN);
    EXPECT_EQ(add(0,         INT_MIN+1), INT_MIN+1);
    EXPECT_EQ(add(0,         -1),        -1);
    EXPECT_EQ(add(0,         0),         0);
    EXPECT_EQ(add(0,         1),         1);
    EXPECT_EQ(add(0,         INT_MAX-1), INT_MAX-1);
    EXPECT_EQ(add(0,         INT_MAX),   INT_MAX);

    EXPECT_EQ(add(INT_MIN,   0),         INT_MIN);
    EXPECT_EQ(add(INT_MIN+1, 0),         INT_MIN+1);
    EXPECT_EQ(add(-1,        0),         -1);
    EXPECT_EQ(add(1,         0),         1);
    EXPECT_EQ(add(INT_MAX-1, 0),         INT_MAX-1);
    EXPECT_EQ(add(INT_MAX,   0),         INT_MAX);
}

TEST(add, check_one)
{
    EXPECT_EQ(add(1,         INT_MIN),   INT_MIN+1);
    EXPECT_EQ(add(1,         INT_MIN+1), INT_MIN+2);
    EXPECT_EQ(add(1,         -1),        0);
    EXPECT_EQ(add(1,         1),         2);
    EXPECT_EQ(add(1,         INT_MAX-1), INT_MAX);
    EXPECT_EQ(add(-1,        INT_MIN+1), INT_MIN);
    EXPECT_EQ(add(-1,        -1),        -2);
    EXPECT_EQ(add(-1,        1),         0);
    EXPECT_EQ(add(-1,        INT_MAX-1), INT_MAX-2);
    EXPECT_EQ(add(-1,        INT_MAX),   INT_MAX-1);

    EXPECT_EQ(add(INT_MAX-1, 1),         INT_MAX);
    EXPECT_EQ(add(INT_MIN,   1),         INT_MIN+1);
    EXPECT_EQ(add(INT_MIN+1, 1),         INT_MIN+2);

    EXPECT_EQ(add(INT_MAX,   -1),        INT_MAX-1);
    EXPECT_EQ(add(INT_MAX-1, -1),        INT_MAX-2);
    EXPECT_EQ(add(INT_MIN+1, -1),        INT_MIN);
}

TEST(add, check_max_mix)
{
    EXPECT_EQ(add(INT_MAX,   INT_MIN),   -1);
    EXPECT_EQ(add(INT_MAX,   INT_MIN+1), 0);
    EXPECT_EQ(add(INT_MAX-1, INT_MIN),   -2);
    EXPECT_EQ(add(INT_MAX-1, INT_MIN+1), -1);

    EXPECT_EQ(add(INT_MIN,   INT_MAX-1), -2);
    EXPECT_EQ(add(INT_MIN,   INT_MAX),   -1);
    EXPECT_EQ(add(INT_MIN+1, INT_MAX-1), -1);
    EXPECT_EQ(add(INT_MIN+1, INT_MAX),   0);
}

//
// Check exception on overflow.
//
TEST(add, check_overflow)
{
    EXPECT_ANY_THROW(add(1,         INT_MAX));
    EXPECT_ANY_THROW(add(-1,        INT_MIN));

    EXPECT_ANY_THROW(add(INT_MAX,   1));
    EXPECT_ANY_THROW(add(INT_MIN,   -1));

    EXPECT_ANY_THROW(add(INT_MAX,   INT_MAX-1));
    EXPECT_ANY_THROW(add(INT_MAX,   INT_MAX));
    EXPECT_ANY_THROW(add(INT_MAX-1, INT_MAX-1));
    EXPECT_ANY_THROW(add(INT_MAX-1, INT_MAX));

    EXPECT_ANY_THROW(add(INT_MIN,   INT_MIN));
    EXPECT_ANY_THROW(add(INT_MIN,   INT_MIN));
    EXPECT_ANY_THROW(add(INT_MIN,   INT_MIN+1));
    EXPECT_ANY_THROW(add(INT_MIN+1, INT_MIN));
    EXPECT_ANY_THROW(add(INT_MIN+1, INT_MIN+1));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
