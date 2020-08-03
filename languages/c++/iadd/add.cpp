#include "catch.hpp"

//
// Add two integet numbers.
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
TEST_CASE("Check zero", "[add]")
{
    REQUIRE(add(0,         INT_MIN)   == INT_MIN);
    REQUIRE(add(0,         INT_MIN+1) == INT_MIN+1);
    REQUIRE(add(0,         -1)        == -1);
    REQUIRE(add(0,         0)         == 0);
    REQUIRE(add(0,         1)         == 1);
    REQUIRE(add(0,         INT_MAX-1) == INT_MAX-1);
    REQUIRE(add(0,         INT_MAX)   == INT_MAX);

    REQUIRE(add(INT_MIN,   0)         == INT_MIN);
    REQUIRE(add(INT_MIN+1, 0)         == INT_MIN+1);
    REQUIRE(add(-1,        0)         == -1);
    REQUIRE(add(1,         0)         == 1);
    REQUIRE(add(INT_MAX-1, 0)         == INT_MAX-1);
    REQUIRE(add(INT_MAX,   0)         == INT_MAX);
}

TEST_CASE("Check one", "[add]")
{
    REQUIRE(add(1,         INT_MIN)   == INT_MIN+1);
    REQUIRE(add(1,         INT_MIN+1) == INT_MIN+2);
    REQUIRE(add(1,         -1)        == 0);
    REQUIRE(add(1,         1)         == 2);
    REQUIRE(add(1,         INT_MAX-1) == INT_MAX);
    REQUIRE(add(-1,        INT_MIN+1) == INT_MIN);
    REQUIRE(add(-1,        -1)        == -2);
    REQUIRE(add(-1,        1)         == 0);
    REQUIRE(add(-1,        INT_MAX-1) == INT_MAX-2);
    REQUIRE(add(-1,        INT_MAX)   == INT_MAX-1);

    REQUIRE(add(INT_MAX-1, 1)         == INT_MAX);
    REQUIRE(add(INT_MIN,   1)         == INT_MIN+1);
    REQUIRE(add(INT_MIN+1, 1)         == INT_MIN+2);

    REQUIRE(add(INT_MAX,   -1)        == INT_MAX-1);
    REQUIRE(add(INT_MAX-1, -1)        == INT_MAX-2);
    REQUIRE(add(INT_MIN+1, -1)        == INT_MIN);
}

TEST_CASE("Check int_max, int_min", "[add]")
{
    REQUIRE(add(INT_MAX,   INT_MIN)   == -1);
    REQUIRE(add(INT_MAX,   INT_MIN+1) == 0);
    REQUIRE(add(INT_MAX-1, INT_MIN)   == -2);
    REQUIRE(add(INT_MAX-1, INT_MIN+1) == -1);

    REQUIRE(add(INT_MIN,   INT_MAX-1) == -2);
    REQUIRE(add(INT_MIN,   INT_MAX)   == -1);
    REQUIRE(add(INT_MIN+1, INT_MAX-1) == -1);
    REQUIRE(add(INT_MIN+1, INT_MAX)   == 0);
}

//
// Check exception on overflow.
//
TEST_CASE( "Check overflow", "[add]")
{
    REQUIRE_THROWS(add(1,         INT_MAX));
    REQUIRE_THROWS(add(-1,        INT_MIN));

    REQUIRE_THROWS(add(INT_MAX,   1));
    REQUIRE_THROWS(add(INT_MIN,   -1));

    REQUIRE_THROWS(add(INT_MAX,   INT_MAX-1));
    REQUIRE_THROWS(add(INT_MAX,   INT_MAX));
    REQUIRE_THROWS(add(INT_MAX-1, INT_MAX-1));
    REQUIRE_THROWS(add(INT_MAX-1, INT_MAX));

    REQUIRE_THROWS(add(INT_MIN,   INT_MIN));
    REQUIRE_THROWS(add(INT_MIN,   INT_MIN));
    REQUIRE_THROWS(add(INT_MIN,   INT_MIN+1));
    REQUIRE_THROWS(add(INT_MIN+1, INT_MIN));
    REQUIRE_THROWS(add(INT_MIN+1, INT_MIN+1));
}
