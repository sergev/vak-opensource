I'll create unit tests for the C hash map implementation using GoogleTest. The tests will cover key functionality: creating the hash map, inserting key-value pairs, retrieving values, updating existing keys, removing keys, and handling edge cases like null inputs or non-existent keys. Since the hash map is written in C, we'll use GoogleTest's C++ framework but interface with the C code via an extern "C" block.

Below is a GoogleTest suite that assumes the hash map implementation is in `hashmap.h` and `hashmap.c`. I'll provide the test file and a brief setup guide.

```x-c++src
#include <gtest/gtest.h>
extern "C" {
#include "hashmap.h"
}

// Test fixture for hash map tests
class HashMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        map = create_hashmap(16);
    }

    void TearDown() override {
        free_hashmap(map);
    }

    HashMap* map;
};

// Test hash map creation
TEST_F(HashMapTest, CreateHashMap) {
    ASSERT_NE(map, nullptr);
    ASSERT_NE(map->buckets, nullptr);
    ASSERT_EQ(map->size, 16);
    for (size_t i = 0; i < map->size; i++) {
        EXPECT_EQ(map->buckets[i], nullptr);
    }
}

// Test inserting a new key-value pair
TEST_F(HashMapTest, InsertNewKey) {
    EXPECT_TRUE(insert(map, "key1", 42));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 42);
}

// Test updating an existing key
TEST_F(HashMapTest, UpdateExistingKey) {
    EXPECT_TRUE(insert(map, "key1", 42));
    EXPECT_TRUE(insert(map, "key1", 100));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 100);
}

// Test inserting multiple keys
TEST_F(HashMapTest, InsertMultipleKeys) {
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
TEST_F(HashMapTest, GetNonExistentKey) {
    int value;
    EXPECT_FALSE(get(map, "nonexistent", &value));
}

// Test removing a key
TEST_F(HashMapTest, RemoveKey) {
    EXPECT_TRUE(insert(map, "key1", 42));
    EXPECT_TRUE(remove_key(map, "key1"));
    int value;
    EXPECT_FALSE(get(map, "key1", &value));
}

// Test removing non-existent key
TEST_F(HashMapTest, RemoveNonExistentKey) {
    EXPECT_FALSE(remove_key(map, "nonexistent"));
}

// Test null inputs
TEST_F(HashMapTest, NullInputs) {
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
TEST_F(HashMapTest, HandleCollisions) {
    // Assuming hash function may cause collisions for certain inputs
    EXPECT_TRUE(insert(map, "key1", 1));
    EXPECT_TRUE(insert(map, "key2", 2));
    int value;
    EXPECT_TRUE(get(map, "key1", &value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(get(map, "key2", &value));
    EXPECT_EQ(value, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

### Setup and Usage Instructions
1. **Prerequisites**: Ensure GoogleTest is installed. On Ubuntu, you can install it with:
   ```bash
   sudo apt-get install libgtest-dev
   ```
   Then compile GoogleTest:
   ```bash
   cd /usr/src/gtest
   sudo cmake CMakeLists.txt
   sudo make
   sudo cp lib/*.a /usr/lib
   ```

2. **File Structure**:
   - `hashmap.h`: Header file with the hash map declarations (structs and function prototypes).
   - `hashmap.c`: The hash map implementation (from the previous response).
   - `hashmap_test.cpp`: The GoogleTest file above.

   Example `hashmap.h`:
   ```c
   #ifndef HASHMAP_H
   #define HASHMAP_H

   #include <stddef.h>

   typedef struct HashNode {
       char* key;
       int value;
       struct HashNode* next;
   } HashNode;

   typedef struct HashMap {
       HashNode** buckets;
       size_t size;
   } HashMap;

   HashMap* create_hashmap(size_t size);
   int insert(HashMap* map, const char* key, int value);
   int get(HashMap* map, const char* key, int* value);
   int remove_key(HashMap* map, const char* key);
   void free_hashmap(HashMap* map);

   #endif
   ```

3. **Compile and Run**:
   Compile the test program with:
   ```bash
   g++ -std=c++11 hashmap_test.cpp hashmap.c -lgtest -lgtest_main -pthread -o hashmap_test
   ```
   Run the tests:
   ```bash
   ./hashmap_test
   ```

### Test Coverage
The test suite includes:
- **CreateHashMap**: Verifies hash map initialization.
- **InsertNewKey**: Tests inserting a new key-value pair.
- **UpdateExistingKey**: Tests updating an existing key's value.
- **InsertMultipleKeys**: Tests inserting and retrieving multiple keys.
- **GetNonExistentKey**: Ensures non-existent keys return false.
- **RemoveKey**: Tests removing an existing key.
- **RemoveNonExistentKey**: Tests removing a non-existent key.
- **NullInputs**: Tests handling of null inputs for all functions.
- **HandleCollisions**: Tests basic collision handling (though specific collision cases depend on the hash function).

These tests ensure the hash map behaves correctly for common use cases and edge cases, providing a robust check of the C implementation. If you need additional tests or specific scenarios (e.g., stress testing with many keys), let me know!
