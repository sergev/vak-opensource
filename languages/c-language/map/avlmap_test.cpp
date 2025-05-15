#include <gtest/gtest.h>
extern "C" {
#include "avlmap.h"
}

// Test fixture for AVL map tests
class AVLMapTest : public ::testing::Test {
protected:
    void SetUp() override { map = create_avlmap(); }

    void TearDown() override { free_avlmap(map); }

    AVLMap *map;
};

// Test AVL map creation
TEST_F(AVLMapTest, CreateAVLMap)
{
    ASSERT_NE(map, nullptr);
    EXPECT_EQ(map->root, nullptr);
}

// Test inserting a new key-value pair
TEST_F(AVLMapTest, InsertNewKey)
{
    EXPECT_TRUE(insert(map, "key1", 42));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 42);
}

// Test updating an existing key
TEST_F(AVLMapTest, UpdateExistingKey)
{
    EXPECT_TRUE(insert(map, "key1", 42));
    EXPECT_TRUE(insert(map, "key1", 100));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 100);
}

// Test inserting multiple keys
TEST_F(AVLMapTest, InsertMultipleKeys)
{
    EXPECT_TRUE(insert(map, "apple", 5));
    EXPECT_TRUE(insert(map, "banana", 10));
    EXPECT_TRUE(insert(map, "orange", 15));

    int value;
    EXPECT_TRUE(get(map, "apple", &value));
    EXPECT_EQ(value, 5);
    EXPECT_TRUE(get(map, "banana", &value));
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(get(map, "orange", &value));
    EXPECT_EQ(value, 15);
}

// Test getting non-existent key
TEST_F(AVLMapTest, GetNonExistentKey)
{
    int value;
    EXPECT_FALSE(get(map, "nonexistent", &value));
}

// Test removing a key
TEST_F(AVLMapTest, RemoveKey)
{
    EXPECT_TRUE(insert(map, "key1", 42));
    EXPECT_TRUE(remove_key(map, "key1"));
    int value;
    EXPECT_FALSE(get(map, "key1", &value));
}

// Test removing non-existent key
TEST_F(AVLMapTest, RemoveNonExistentKey)
{
    EXPECT_FALSE(remove_key(map, "nonexistent"));
}

// Test null inputs
TEST_F(AVLMapTest, NullInputs)
{
    EXPECT_FALSE(insert(nullptr, "key1", 42));
    EXPECT_FALSE(insert(map, nullptr, 42));
    int value;
    EXPECT_FALSE(get(nullptr, "key1", &value));
    EXPECT_FALSE(get(map, nullptr, &value));
    EXPECT_FALSE(get(map, "key1", nullptr));
    EXPECT_FALSE(remove_key(nullptr, "key1"));
    EXPECT_FALSE(remove_key(map, nullptr));
}

// Test AVL balance after multiple insertions
TEST_F(AVLMapTest, BalanceAfterInsertions)
{
    // Insert keys in a way that may cause imbalance
    const char *keys[] = { "a", "b", "c", "d", "e" };
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(insert(map, keys[i], i));
    }

    // Helper to check if tree is balanced
    auto is_balanced = [](AVLNode *node) -> bool {
        if (!node)
            return true;
        int bf = abs(height(node->left) - height(node->right));
        if (bf > 1)
            return false;
        return is_balanced(node->left) && is_balanced(node->right);
    };

    EXPECT_TRUE(is_balanced(map->root));
}

// Test AVL balance after deletions
TEST_F(AVLMapTest, BalanceAfterDeletions)
{
    // Insert multiple keys
    EXPECT_TRUE(insert(map, "apple", 5));
    EXPECT_TRUE(insert(map, "banana", 10));
    EXPECT_TRUE(insert(map, "orange", 15));
    EXPECT_TRUE(insert(map, "grape", 20));

    // Remove some keys
    EXPECT_TRUE(remove_key(map, "banana"));
    EXPECT_TRUE(remove_key(map, "orange"));

    // Helper to check if tree is balanced
    auto is_balanced = [](AVLNode *node) -> bool {
        if (!node)
            return true;
        int bf = abs(height(node->left) - height(node->right));
        if (bf > 1)
            return false;
        return is_balanced(node->left) && is_balanced(node->right);
    };

    EXPECT_TRUE(is_balanced(map->root));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}