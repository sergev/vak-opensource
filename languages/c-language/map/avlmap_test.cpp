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

    // Helper function to count nodes
    static int count_nodes(StringNode *node)
    {
        if (!node)
            return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }

    // Helper to check if tree is balanced
    static bool is_balanced(StringNode *node)
    {
        if (!node)
            return true;
        int bf = abs(map_node_height(node->left) - map_node_height(node->right));
        if (bf > 1)
            return false;
        return is_balanced(node->left) && is_balanced(node->right);
    }

    // Helper function to check BST property
    static bool is_bst(StringNode *node, const char *min_key, const char *max_key)
    {
        if (!node)
            return true;
        if (min_key && strcmp(node->key, min_key) <= 0)
            return false;
        if (max_key && strcmp(node->key, max_key) >= 0)
            return false;
        return is_bst(node->left, min_key, node->key) && is_bst(node->right, node->key, max_key);
    }
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

static bool cond_true(const char *key)
{
    return true;
}

// Test empty map
TEST_F(StringMapTest, CondEmptyMap)
{
    map_remove_cond(&map, cond_true);
    EXPECT_EQ(map.root, nullptr);
    EXPECT_TRUE(is_balanced(map.root));
    EXPECT_TRUE(is_bst(map.root, NULL, NULL));
}

// Test null inputs
TEST_F(StringMapTest, CondNullInputs)
{
    map_remove_cond(NULL, cond_true);
    map_remove_cond(&map, NULL);
    EXPECT_EQ(map.root, nullptr);
}

static bool cond_false(const char *key)
{
    return false;
}

// Test no removals (cond always false)
TEST_F(StringMapTest, CondNoRemovals)
{
    map_insert(&map, "apple", 1);
    map_insert(&map, "banana", 2);
    map_insert(&map, "cherry", 3);
    map_remove_cond(&map, cond_false);
    EXPECT_EQ(count_nodes(map.root), 3);
    EXPECT_TRUE(is_balanced(map.root));
    EXPECT_TRUE(is_bst(map.root, NULL, NULL));
}

// Test remove all nodes (cond always true)
TEST_F(StringMapTest, CondRemoveAll)
{
    map_insert(&map, "apple", 1);
    map_insert(&map, "banana", 2);
    map_insert(&map, "cherry", 3);
    map_remove_cond(&map, cond_true);
    EXPECT_EQ(map.root, nullptr);
    EXPECT_TRUE(is_balanced(map.root));
    EXPECT_TRUE(is_bst(map.root, NULL, NULL));
}

static bool cond_less4(const char *key)
{
    return strlen(key) < 4;
}

// Test remove short keys
TEST_F(StringMapTest, CondRemoveShortKeys)
{
    map_insert(&map, "a", 1);
    map_insert(&map, "bb", 2);
    map_insert(&map, "ccc", 3);
    map_insert(&map, "dddd", 4);
    map_insert(&map, "eeeee", 5);
    map_remove_cond(&map, cond_less4);
    EXPECT_EQ(count_nodes(map.root), 2); // Should keep "dddd" and "eeeee"
    EXPECT_TRUE(is_balanced(map.root));
    EXPECT_TRUE(is_bst(map.root, NULL, NULL));

    // Verify remaining keys
    StringNode *node = map.root;
    while (node) {
        EXPECT_GE(strlen(node->key), 4);
        node = node->left ? node->left : node->right;
    }
}

static bool cond_ca(const char *key)
{
    return strncmp(key, "ca", 2) == 0;
}

// Test remove keys with prefix
TEST_F(StringMapTest, CondRemovePrefix)
{
    map_insert(&map, "cat", 1);
    map_insert(&map, "car", 2);
    map_insert(&map, "dog", 3);
    map_insert(&map, "bird", 4);
    map_remove_cond(&map, cond_ca);
    EXPECT_EQ(count_nodes(map.root), 2); // Should keep "dog" and "bird"
    EXPECT_TRUE(is_balanced(map.root));
    EXPECT_TRUE(is_bst(map.root, NULL, NULL));
    StringNode *node = map.root;
    while (node) {
        EXPECT_NE(strncmp(node->key, "ca", 2), 0);
        node = node->left ? node->left : node->right;
    }
}

static bool cond_even(const char *key)
{
    return strlen(key) % 2 == 0;
}

// Test large tree with mixed removals
TEST_F(StringMapTest, CondLargeTreeMixedRemovals)
{
    const char *keys[] = { "alpha", "beta", "gamma", "delta", "epsilon",
                           "zeta",  "eta",  "theta", "iota",  "kappa" };
    int n              = sizeof(keys) / sizeof(keys[0]);
    for (int i = 0; i < n; i++) {
        map_insert(&map, keys[i], i + 1);
    }
    map_remove_cond(&map, cond_even); // Remove keys with even length

    // Expected remaining keys: "alpha", "gamma", "delta", "epsilon",
    // "eta", "theta", "kappa" (odd lengths)
    EXPECT_EQ(count_nodes(map.root), 7);
    EXPECT_TRUE(is_balanced(map.root));
    EXPECT_TRUE(is_bst(map.root, NULL, NULL));
}
