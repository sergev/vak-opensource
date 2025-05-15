I'll implement a string-to-int mapping in C using a binary search tree (BST) instead of a hash map. In a BST, keys (strings) are ordered lexicographically, allowing efficient insertion, retrieval, and deletion without hashing. Each node will store a string key, an integer value, and pointers to left and right children. The implementation will provide similar functionality to the previous hash map: insert, get, remove, and free operations.

Below is a complete C program implementing the BST-based string-to-int mapping.

```x-csrc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for each node in the binary search tree
typedef struct BSTNode {
    char* key;
    int value;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

// Structure for the BST map
typedef struct BSTMap {
    BSTNode* root;
} BSTMap;

// Initialize the BST map
BSTMap* create_bstmap() {
    BSTMap* map = (BSTMap*)malloc(sizeof(BSTMap));
    if (!map) return NULL;
    map->root = NULL;
    return map;
}

// Create a new node
BSTNode* create_node(const char* key, int value) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (!node) return NULL;
    node->key = strdup(key);
    if (!node->key) {
        free(node);
        return NULL;
    }
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Insert or update a key-value pair
int insert(BSTMap* map, const char* key, int value) {
    if (!map || !key) return 0;

    // If tree is empty, create root
    if (!map->root) {
        map->root = create_node(key, value);
        return map->root ? 1 : 0;
    }

    BSTNode* current = map->root;
    while (current) {
        int cmp = strcmp(key, current->key);
        if (cmp == 0) {
            // Key exists, update value
            current->value = value;
            return 1;
        } else if (cmp < 0) {
            // Go left
            if (!current->left) {
                current->left = create_node(key, value);
                return current->left ? 1 : 0;
            }
            current = current->left;
        } else {
            // Go right
            if (!current->right) {
                current->right = create_node(key, value);
                return current->right ? 1 : 0;
            }
            current = current->right;
        }
    }
    return 0; // Should not reach here
}

// Get value by key, returns 1 if found, 0 if not
int get(BSTMap* map, const char* key, int* value) {
    if (!map || !key || !value) return 0;

    BSTNode* current = map->root;
    while (current) {
        int cmp = strcmp(key, current->key);
        if (cmp == 0) {
            *value = current->value;
            return 1;
        } else if (cmp < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0;
}

// Find the node with the minimum key in a subtree
BSTNode* find_min(BSTNode* node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

// Remove key-value pair, returns 1 if removed, 0 if not found
int remove_key(BSTMap* map, const char* key) {
    if (!map || !key) return 0;

    BSTNode* parent = NULL;
    BSTNode* current = map->root;

    // Find the node and its parent
    while (current) {
        int cmp = strcmp(key, current->key);
        if (cmp == 0) break;
        parent = current;
        current = cmp < 0 ? current->left : current->right;
    }

    if (!current) return 0; // Key not found

    // Case 1: Leaf node
    if (!current->left && !current->right) {
        if (current == map->root) {
            map->root = NULL;
        } else if (parent->left == current) {
            parent->left = NULL;
        } else {
            parent->right = NULL;
        }
        free(current->key);
        free(current);
        return 1;
    }

    // Case 2: Node with one child
    if (!current->left || !current->right) {
        BSTNode* child = current->left ? current->left : current->right;
        if (current == map->root) {
            map->root = child;
        } else if (parent->left == current) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        free(current->key);
        free(current);
        return 1;
    }

    // Case 3: Node with two children
    BSTNode* successor = find_min(current->right);
    char* temp_key = strdup(successor->key);
    int temp_value = successor->value;
    if (!temp_key) return 0;

    remove_key(map, successor->key); // Remove successor
    free(current->key);
    current->key = temp_key;
    current->value = temp_value;
    return 1;
}

// Free the BST and all its nodes
void free_bstmap(BSTMap* map) {
    if (!map) return;

    // Recursive helper to free nodes
    void free_nodes(BSTNode* node) {
        if (!node) return;
        free_nodes(node->left);
        free_nodes(node->right);
        free(node->key);
        free(node);
    }

    free_nodes(map->root);
    free(map);
}

// Example usage
int main() {
    BSTMap* map = create_bstmap();
    if (!map) {
        printf("Failed to create bstmap\n");
        return 1;
    }

    // Insert some key-value pairs
    insert(map, "apple", 5);
    insert(map, "banana", 10);
    insert(map, "orange", 15);
    insert(map, "apple", 20); // Update apple's value

    // Retrieve values
    int value;
    if (get(map, "apple", &value)) {
        printf("apple: %d\n", value); // Should print 20
    }
    if (get(map, "banana", &value)) {
        printf("banana: %d\n", value); // Should print 10
    }
    if (!get(map, "grape", &value)) {
        printf("grape: not found\n");
    }

    // Remove a key
    if (remove_key(map, "banana")) {
        printf("banana removed\n");
    }
    if (!get(map, "banana", &value)) {
        printf("banana: not found\n");
    }

    free_bstmap(map);
    return 0;
}
```

### Implementation Details
This BST-based string-to-int mapping provides functionality similar to C++'s `std::unordered_map<std::string, int>` but uses a binary search tree instead of hashing. Key features include:
- **Structure**: Each `BSTNode` stores a string key, an integer value, and left/right child pointers. The `BSTMap` holds a root pointer.
- **Insert**: Inserts a new key-value pair or updates the value if the key exists. Uses `strcmp` for lexicographical ordering (left for key < node, right for key > node).
- **Get**: Retrieves the value for a given key by traversing the tree based on `strcmp`. Returns 1 if found, 0 if not.
- **Remove**: Handles three cases: leaf node, node with one child, and node with two children (using the minimum key in the right subtree as the successor).
- **Free**: Recursively frees all nodes and their keys, then the map itself.
- **No Hashing**: Unlike the previous hash map, this uses tree-based ordering, with O(log n) average time complexity for balanced trees (though worst-case O(n) if unbalanced).

### Usage
To use this BST map:
1. Create a map with `create_bstmap()`.
2. Insert key-value pairs with `insert(map, key, value)`.
3. Retrieve values with `get(map, key, &value)`.
4. Remove entries with `remove_key(map, key)`.
5. Free the map with `free_bstmap(map)`.

The `main()` function demonstrates these operations with example insertions, retrievals, updates, and deletions. Compile and run with:
```bash
gcc bstmap.c -o bstmap && ./bstmap
```

### Notes
- **Performance**: The BST has O(log n) average-case time complexity for insert, get, and remove if balanced. However, it can degrade to O(n) in the worst case (e.g., if keys are inserted in sorted order). To improve this, you could add self-balancing (e.g., AVL or red-black tree), but this implementation keeps it simple.
- **Comparison to Hash Map**: Unlike the hash map, the BST doesn't require a hash function or handle collisions, but it may be slower for large datasets unless balanced. It naturally supports ordered traversal (not implemented here but possible with in-order traversal).
- **Memory**: Like the hash map, it dynamically allocates memory for keys and nodes, with proper cleanup.

If you need unit tests for this BST implementation (similar to the GoogleTest suite for the hash map) or additional features (e.g., iteration, balancing), let me know!
