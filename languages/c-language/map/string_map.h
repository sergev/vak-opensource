//
// Implementation of string-to-int map using an AVL tree, with self-balancing.
//
// Time Complexity: All operations (insert, get, remove) are O(log n)
// due to the height balance guarantee (tree height is at most 1.44 log(n)).
//
// Memory: It dynamically allocates memory for keys and nodes, with proper cleanup.
//
// To use this string-to-int map:
//  0. Allocate a map with `StringMap map;`.
//  1. Initialize with `map_init(&map)`.
//  2. Insert key-value pairs with `map_insert(&map, key, value)`.
//  3. Retrieve values with `map_get(&map, key, &value)`.
//  4. Remove entries with `map_remove_key(&map, key)`.
//  5. Free the map with `map_free(&map)`.
//
// Copyright (c) 2025 Serge Vakulenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <stdbool.h>

// Structure for each node in the AVL tree
typedef struct StringNode {
    struct StringNode *left;
    struct StringNode *right;
    int height;
    int value;
    char key[1]; // Dynamically sized
} StringNode;

typedef struct StringMap {
    StringNode *root;
} StringMap;

//
// Initialize the string-to-int map
//
void map_init(StringMap *);

//
// Insert: Recursively inserts or updates a key-value pair,
// then balances the tree on the way back up the recursion stack.
//
int map_insert(StringMap *map, const char *key, int value);

//
// Get: Retrieves the value for a given key by traversing the tree
// based on `strcmp`. Returns 1 if found, 0 if not.
//
int map_get(StringMap *map, const char *key, int *value);

//
// Remove: Handles three cases: leaf node, node with one child,
// and node with two children (using the minimum key in the right
// subtree as the successor), and balances the tree after deletion.
//
void map_remove_key(StringMap *map, const char *key);

//
// Remove by condition: Remove nodes where cond(key) returns true.
//
void map_remove_cond(StringMap *map, bool (*cond)(const char *));

//
// Free: Recursively frees all nodes and their keys.
//
void map_free(StringMap *map);

//
// Get the height of a node (0 for NULL)
//
int map_node_height(StringNode *node);
