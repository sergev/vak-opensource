#include <gtest/gtest.h>
extern "C" {
#include "hashmap.h"
}

// Test fixture for hash map tests
class HashMapTest : public ::testing::Test {
protected:
    void SetUp() override { map = create_hashmap(16); }

    void TearDown() override { free_hashmap(map); }

    HashMap *map;
};

// Test hash map creation
TEST_F(HashMapTest, CreateHashMap)
{
    ASSERT_NE(map, nullptr);
    ASSERT_NE(map->buckets, nullptr);
    ASSERT_EQ(map->size, 16);
    for (size_t i = 0; i < map->size; i++) {
        EXPECT_EQ(map->buckets[i], nullptr);
    }
}

// Test inserting a new key-value pair
TEST_F(HashMapTest, InsertNewKey)
{
    EXPECT_TRUE(insert(map, "key1", 42));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 42);
}

// Test updating an existing key
TEST_F(HashMapTest, UpdateExistingKey)
{
    EXPECT_TRUE(insert(map, "key1", 42));
    EXPECT_TRUE(insert(map, "key1", 100));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 100);
}

// Test inserting multiple keys
TEST_F(HashMapTest, InsertMultipleKeys)
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
TEST_F(HashMapTest, GetNonExistentKey)
{
    int value;
    EXPECT_FALSE(get(map, "nonexistent", &value));
}

// Test removing a key
TEST_F(HashMapTest, RemoveKey)
{
    EXPECT_TRUE(insert(map, "key1", 42));
    EXPECT_TRUE(remove_key(map, "key1"));
    int value;
    EXPECT_FALSE(get(map, "key1", &value));
}

// Test removing non-existent key
TEST_F(HashMapTest, RemoveNonExistentKey)
{
    EXPECT_FALSE(remove_key(map, "nonexistent"));
}

// Test null inputs
TEST_F(HashMapTest, NullInputs)
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

// Test hash map with collisions
TEST_F(HashMapTest, HandleCollisions)
{
    // Assuming hash function may cause collisions for certain inputs
    EXPECT_TRUE(insert(map, "key1", 1));
    EXPECT_TRUE(insert(map, "key2", 2));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(get(map, "key2", &value));
    EXPECT_EQ(value, 2);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}