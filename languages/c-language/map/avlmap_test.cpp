#include <gtest/gtest.h>
extern "C" {
#include "string_map.h"
}

// Test fixture for string-to-int map tests
class StringMapTest : public ::testing::Test {
protected:
    void SetUp() override { map_init(&map); }

    void TearDown() override { map_free(&map); }

    StringMap map;

    // Helper to check if tree is balanced
    static bool is_balanced(StringNode *node)
    {
        if (!node)
            return true;
        int bf = abs(map_node_height(node->left) - map_node_height(node->right));
        if (bf > 1)
            return false;
        return is_balanced(node->left) && is_balanced(node->right);
    };
};

// Test map creation
TEST_F(StringMapTest, CreateStringMap)
{
    EXPECT_EQ(map.root, nullptr);
}

// Test inserting a new key-value pair
TEST_F(StringMapTest, InsertNewKey)
{
    EXPECT_TRUE(map_insert(&map, "key1", 42));
    int value;
    EXPECT_TRUE(map_get(&map, "key1", &value));
    EXPECT_EQ(value, 42);
}

// Test updating an existing key
TEST_F(StringMapTest, UpdateExistingKey)
{
    EXPECT_TRUE(map_insert(&map, "key1", 42));
    EXPECT_TRUE(map_insert(&map, "key1", 100));
    int value;
    EXPECT_TRUE(map_get(&map, "key1", &value));
    EXPECT_EQ(value, 100);
}

// Test inserting multiple keys
TEST_F(StringMapTest, InsertMultipleKeys)
{
    EXPECT_TRUE(map_insert(&map, "apple", 5));
    EXPECT_TRUE(map_insert(&map, "banana", 10));
    EXPECT_TRUE(map_insert(&map, "orange", 15));

    int value;
    EXPECT_TRUE(map_get(&map, "apple", &value));
    EXPECT_EQ(value, 5);
    EXPECT_TRUE(map_get(&map, "banana", &value));
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(map_get(&map, "orange", &value));
    EXPECT_EQ(value, 15);
}

// Test getting non-existent key
TEST_F(StringMapTest, GetNonExistentKey)
{
    int value;
    EXPECT_FALSE(map_get(&map, "nonexistent", &value));
}

// Test removing a key
TEST_F(StringMapTest, RemoveKey)
{
    EXPECT_TRUE(map_insert(&map, "key1", 42));
    map_remove_key(&map, "key1");
    int value;
    EXPECT_FALSE(map_get(&map, "key1", &value));
}

// Test removing non-existent key
TEST_F(StringMapTest, RemoveNonExistentKey)
{
    map_remove_key(&map, "nonexistent");
}

// Test null inputs
TEST_F(StringMapTest, NullInputs)
{
    EXPECT_FALSE(map_insert(nullptr, "key1", 42));
    EXPECT_FALSE(map_insert(&map, nullptr, 42));
    int value;
    EXPECT_FALSE(map_get(nullptr, "key1", &value));
    EXPECT_FALSE(map_get(&map, nullptr, &value));
    EXPECT_FALSE(map_get(&map, "key1", nullptr));
    map_remove_key(nullptr, "key1");
    map_remove_key(&map, nullptr);
}

// Test balance after multiple insertions
TEST_F(StringMapTest, BalanceAfterInsertions)
{
    // Insert keys in a way that may cause imbalance
    const char *keys[] = { "a", "b", "c", "d", "e" };
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(map_insert(&map, keys[i], i));
    }

    EXPECT_TRUE(is_balanced(map.root));
}

// Test balance after deletions
TEST_F(StringMapTest, BalanceAfterDeletions)
{
    // Insert multiple keys
    EXPECT_TRUE(map_insert(&map, "apple", 5));
    EXPECT_TRUE(map_insert(&map, "banana", 10));
    EXPECT_TRUE(map_insert(&map, "orange", 15));
    EXPECT_TRUE(map_insert(&map, "grape", 20));

    // Remove some keys
    map_remove_key(&map, "banana");
    int value;
    EXPECT_FALSE(map_get(&map, "banana", &value));
    map_remove_key(&map, "orange");
    EXPECT_FALSE(map_get(&map, "orange", &value));

    EXPECT_TRUE(is_balanced(map.root));
}
