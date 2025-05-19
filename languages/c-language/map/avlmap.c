//
// Implementation of string-to-int map using an AVL tree, with self-balancing.
//
// Each `StringNode` includes a `height` field to track subtree height,
// used to compute balance factors.
//
// Balancing:
//  - The `balance_factor` is calculated as `height(left) - height(right)`.
//  - After insertions and deletions, the `balance` function checks
//    if the balance factor is outside [-1, 1].
//  - Four cases are handled: Left-Left, Left-Right, Right-Right,
//    Right-Left, using left and right rotations.
//  - Rotations (`rotate_left`, `rotate_right`) adjust the tree structure
//    while preserving BST properties.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_map.h"

// Get the height of a node (0 for NULL)
int map_node_height(StringNode *node)
{
    return node ? node->height : 0;
}

// Update the height of a node
static void update_height(StringNode *node)
{
    if (node) {
        int left_height  = map_node_height(node->left);
        int right_height = map_node_height(node->right);

        node->height = 1 + (left_height > right_height ? left_height : right_height);
    }
}

// Get the balance factor of a node
static int balance_factor(StringNode *node)
{
    return node ? map_node_height(node->left) - map_node_height(node->right) : 0;
}

// Right rotation
static StringNode *rotate_right(StringNode *y)
{
    StringNode *x  = y->left;
    StringNode *T2 = x->right;
    x->right       = y;
    y->left        = T2;
    update_height(y);
    update_height(x);
    return x;
}

// Left rotation
static StringNode *rotate_left(StringNode *x)
{
    StringNode *y  = x->right;
    StringNode *T2 = y->left;
    y->left        = x;
    x->right       = T2;
    update_height(x);
    update_height(y);
    return y;
}

// Re-balance a node
static StringNode *rebalance(StringNode *node)
{
    if (!node)
        return node;

    update_height(node);
    int bf = balance_factor(node);

    // Left heavy
    if (bf > 1) {
        if (balance_factor(node->left) < 0) {
            node->left = rotate_left(node->left); // Left-Right case
        }
        return rotate_right(node); // Left-Left case
    }
    // Right heavy
    if (bf < -1) {
        if (balance_factor(node->right) > 0) {
            node->right = rotate_right(node->right); // Right-Left case
        }
        return rotate_left(node); // Right-Right case
    }
    return node;
}

// Helper function to rebalance the tree
static StringNode *rebalance_tree(StringNode *node)
{
    if (!node) {
        return NULL;
    }

    // Post-order: rebalance children first
    node->left  = rebalance_tree(node->left);
    node->right = rebalance_tree(node->right);

    rebalance(node);
    return node;
}

// Initialize the AVL map
void map_init(StringMap *map)
{
    map->root = NULL;
}

// Create a new node
static StringNode *create_node(const char *key, int value)
{
    StringNode *node = (StringNode *)malloc(sizeof(StringNode) + strlen(key));
    if (!node)
        return NULL;
    strcpy(node->key, key);
    node->value  = value;
    node->left   = NULL;
    node->right  = NULL;
    node->height = 1;
    return node;
}

// Insert or update a key-value pair
static StringNode *insert_node(StringNode *node, const char *key, int value)
{
    if (!node) {
        return create_node(key, value);
    }

    int cmp = strcmp(key, node->key);
    if (cmp == 0) {
        node->value = value; // Update value
        return node;
    } else if (cmp < 0) {
        node->left = insert_node(node->left, key, value);
    } else {
        node->right = insert_node(node->right, key, value);
    }

    return rebalance(node);
}

int map_insert(StringMap *map, const char *key, int value)
{
    if (!map || !key)
        return 0;
    map->root = insert_node(map->root, key, value);
    return map->root ? 1 : 0;
}

// Get value by key, returns 1 if found, 0 if not
int map_get(StringMap *map, const char *key, int *value)
{
    if (!map || !key || !value)
        return 0;

    StringNode *current = map->root;
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

// Helper function to find node with minimum key and its parent
static StringNode *min_node_with_parent(StringNode *node, StringNode **parent)
{
    *parent             = NULL;
    StringNode *current = node;
    while (current->left) {
        *parent = current;
        current = current->left;
    }
    return current;
}

// Helper function to remove a single node without balancing
static StringNode *remove_single_node(StringNode *node)
{
    // Node with only one child or no child
    if (!node->left) {
        StringNode *temp = node->right;
        free(node);
        return temp;
    } else if (!node->right) {
        StringNode *temp = node->left;
        free(node);
        return temp;
    }

    // Node with two children
    StringNode *parent_of_successor;
    StringNode *successor = min_node_with_parent(node->right, &parent_of_successor);

    // Unlink successor from its current position
    if (parent_of_successor) {
        parent_of_successor->left = successor->right;
    } else {
        // Successor is the right child of node
        node->right = successor->right;
    }

    // Replace node with successor
    successor->left  = node->left;
    successor->right = node->right;
    update_height(successor);

    // Free the node
    free(node);
    return successor;
}

// Remove a node, returns the new root of the subtree
static StringNode *remove_node(StringNode *node, const char *key)
{
    if (!node)
        return NULL;

    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = remove_node(node->left, key);
    } else if (cmp > 0) {
        node->right = remove_node(node->right, key);
    } else {
        node = remove_single_node(node);
    }

    return rebalance(node);
}

// Helper function for conditional node removal
static StringNode *remove_node_cond(StringNode *node, bool (*cond)(const char *))
{
    if (!node) {
        return NULL;
    }

    // Post-order traversal: process left and right subtrees first
    node->left  = remove_node_cond(node->left, cond);
    node->right = remove_node_cond(node->right, cond);

    // Check condition for current node
    if (cond(node->key)) {
        node = remove_single_node(node);
        if (!node) {
            return NULL; // Node was removed and had no children
        }
    }

    // Update height but defer balancing
    update_height(node);
    return node;
}

void map_remove_key(StringMap *map, const char *key)
{
    if (map && key) {
        map->root = remove_node(map->root, key);
    }
}

void map_remove_cond(StringMap *map, bool (*cond)(const char *))
{
    if (!map || !cond) {
        return;
    }
    // Phase 1: Remove nodes without balancing
    map->root = remove_node_cond(map->root, cond);

    // Phase 2: Rebalance the entire tree
    map->root = rebalance_tree(map->root);
}

static void free_nodes(StringNode *node)
{
    if (!node)
        return;
    free_nodes(node->left);
    free_nodes(node->right);
    free(node);
}

// Free the AVL map and all its nodes
void map_free(StringMap *map)
{
    free_nodes(map->root);
}

#if 0
// Example usage
int main()
{
    StringMap map;
    map_init(&map);

    // Insert some key-value pairs
    map_insert(&map, "apple", 5);
    map_insert(&map, "banana", 10);
    map_insert(&map, "orange", 15);
    map_insert(&map, "apple", 20); // Update apple's value

    // Retrieve values
    int value;
    if (map_get(&map, "apple", &value)) {
        printf("apple: %d\n", value); // Should print 20
    }
    if (map_get(&map, "banana", &value)) {
        printf("banana: %d\n", value); // Should print 10
    }
    if (!map_get(&map, "grape", &value)) {
        printf("grape: not found\n");
    }

    // Remove a key
    if (map_remove_key(&map, "banana")) {
        printf("banana removed\n");
    }
    if (!map_get(&map, "banana", &value)) {
        printf("banana: not found\n");
    }

    map_free(&map);
    return 0;
}
#endif
