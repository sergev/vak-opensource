#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for each node in the AVL tree
typedef struct AVLNode {
    char *key;
    int value;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

// Structure for the AVL map
typedef struct AVLMap {
    AVLNode *root;
} AVLMap;

// Get the height of a node (0 for NULL)
int height(AVLNode *node)
{
    return node ? node->height : 0;
}

// Update the height of a node
void update_height(AVLNode *node)
{
    if (node) {
        node->height = 1 + (height(node->left) > height(node->right) ? height(node->left)
                                                                     : height(node->right));
    }
}

// Get the balance factor of a node
int balance_factor(AVLNode *node)
{
    return node ? height(node->left) - height(node->right) : 0;
}

// Right rotation
AVLNode *rotate_right(AVLNode *y)
{
    AVLNode *x  = y->left;
    AVLNode *T2 = x->right;
    x->right    = y;
    y->left     = T2;
    update_height(y);
    update_height(x);
    return x;
}

// Left rotation
AVLNode *rotate_left(AVLNode *x)
{
    AVLNode *y  = x->right;
    AVLNode *T2 = y->left;
    y->left     = x;
    x->right    = T2;
    update_height(x);
    update_height(y);
    return y;
}

// Balance a node
AVLNode *balance(AVLNode *node)
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

// Initialize the AVL map
AVLMap *create_avlmap()
{
    AVLMap *map = (AVLMap *)malloc(sizeof(AVLMap));
    if (!map)
        return NULL;
    map->root = NULL;
    return map;
}

// Create a new node
AVLNode *create_node(const char *key, int value)
{
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    if (!node)
        return NULL;
    node->key = strdup(key);
    if (!node->key) {
        free(node);
        return NULL;
    }
    node->value  = value;
    node->left   = NULL;
    node->right  = NULL;
    node->height = 1;
    return node;
}

// Insert or update a key-value pair
AVLNode *insert_node(AVLNode *node, const char *key, int value)
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

    return balance(node);
}

int insert(AVLMap *map, const char *key, int value)
{
    if (!map || !key)
        return 0;
    map->root = insert_node(map->root, key, value);
    return map->root ? 1 : 0;
}

// Get value by key, returns 1 if found, 0 if not
int get(AVLMap *map, const char *key, int *value)
{
    if (!map || !key || !value)
        return 0;

    AVLNode *current = map->root;
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
AVLNode *find_min(AVLNode *node)
{
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

// Remove a node, returns the new root of the subtree
AVLNode *remove_node(AVLNode *node, const char *key)
{
    if (!node)
        return NULL;

    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = remove_node(node->left, key);
    } else if (cmp > 0) {
        node->right = remove_node(node->right, key);
    } else {
        // Node found
        if (!node->left || !node->right) {
            AVLNode *temp = node->left ? node->left : node->right;
            free(node->key);
            free(node);
            return temp;
        }

        // Node with two children
        AVLNode *successor = find_min(node->right);
        node->key          = strdup(successor->key);
        node->value        = successor->value;
        if (!node->key) {
            free(node->key);
            free(node);
            return NULL;
        }
        node->right = remove_node(node->right, successor->key);
    }

    return balance(node);
}

int remove_key(AVLMap *map, const char *key)
{
    if (!map || !key)
        return 0;
    AVLNode *new_root = remove_node(map->root, key);
    if (map->root == new_root)
        return 0; // Key not found
    map->root = new_root;
    return 1;
}

// Free the AVL map and all its nodes
void free_avlmap(AVLMap *map)
{
    if (!map)
        return;

    void free_nodes(AVLNode * node)
    {
        if (!node)
            return;
        free_nodes(node->left);
        free_nodes(node->right);
        free(node->key);
        free(node);
    }

    free_nodes(map->root);
    free(map);
}

// Example usage
int main()
{
    AVLMap *map = create_avlmap();
    if (!map) {
        printf("Failed to create avlmap\n");
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

    free_avlmap(map);
    return 0;
}