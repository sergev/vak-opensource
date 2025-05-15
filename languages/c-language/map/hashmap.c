#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for each key-value pair (node in the linked list)
typedef struct HashNode {
    char *key;
    int value;
    struct HashNode *next;
} HashNode;

// Structure for the hash map
typedef struct HashMap {
    HashNode **buckets;
    size_t size;
} HashMap;

// Simple hash function for strings
unsigned int hash(const char *key, size_t size)
{
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % size;
}

// Initialize the hash map
HashMap *create_hashmap(size_t size)
{
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (!map)
        return NULL;

    map->buckets = (HashNode **)calloc(size, sizeof(HashNode *));
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    map->size = size;
    return map;
}

// Insert or update a key-value pair
int insert(HashMap *map, const char *key, int value)
{
    if (!map || !key)
        return 0;

    unsigned int index = hash(key, map->size);

    // Check if key already exists
    HashNode *node = map->buckets[index];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value = value; // Update value if key exists
            return 1;
        }
        node = node->next;
    }

    // Create new node
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (!new_node)
        return 0;

    new_node->key = strdup(key);
    if (!new_node->key) {
        free(new_node);
        return 0;
    }

    new_node->value     = value;
    new_node->next      = map->buckets[index];
    map->buckets[index] = new_node;
    return 1;
}

// Get value by key, returns 1 if found, 0 if not
int get(HashMap *map, const char *key, int *value)
{
    if (!map || !key || !value)
        return 0;

    unsigned int index = hash(key, map->size);
    HashNode *node     = map->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            *value = node->value;
            return 1;
        }
        node = node->next;
    }
    return 0;
}

// Remove key-value pair, returns 1 if removed, 0 if not found
int remove_key(HashMap *map, const char *key)
{
    if (!map || !key)
        return 0;

    unsigned int index = hash(key, map->size);
    HashNode *node     = map->buckets[index];
    HashNode *prev     = NULL;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev) {
                prev->next = node->next;
            } else {
                map->buckets[index] = node->next;
            }
            free(node->key);
            free(node);
            return 1;
        }
        prev = node;
        node = node->next;
    }
    return 0;
}

// Free the hash map and all its nodes
void free_hashmap(HashMap *map)
{
    if (!map)
        return;

    for (size_t i = 0; i < map->size; i++) {
        HashNode *node = map->buckets[i];
        while (node) {
            HashNode *temp = node;
            node           = node->next;
            free(temp->key);
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}

// Example usage
int main()
{
    HashMap *map = create_hashmap(16);
    if (!map) {
        printf("Failed to create hashmap\n");
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

    free_hashmap(map);
    return 0;
}