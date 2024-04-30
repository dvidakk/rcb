/*TODO:
 - create a dinamic hash map for storing cli args
 - add ability to combine cli switches 
 - try keep it fast*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hmap.h"

// Hash function (replace with a more robust hashing algorithm if needed)
int hash_function(char* key, int capacity) {
    int hash = 0;
    int c;
    while ((c = *key++)) {
        hash = (hash * 31 + c) % capacity;
    }
    return hash;
}

// Combine CLI switch flags (modify based on your switch handling logic)
char* combine_switches(char* switch1, char* switch2) {
    // Example: Concatenate switches with a separator (e.g., "-")
    int combined_len = strlen(switch1) + strlen(switch2) + 2;
    char* combined = malloc(combined_len);
    snprintf(combined, combined_len, "%s-%s", switch1, switch2);
    return combined;
}

// Implementation of hash map functions

HashMap* create_hash_map(int capacity, float load_factor) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) {
        fprintf(stderr, "Failed to allocate memory for HashMap: file %s at line %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    map->capacity = capacity;
    map->load_factor = load_factor;
    map->size = 0;
    map->buckets = (Node**)calloc(capacity, sizeof(Node*));
    if (!map->buckets) {
        fprintf(stderr, "Failed to allocate memory for HashMap buckets: file %s at line %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    return map;
}

void put(HashMap* map, char* key, char* value) {
    if (!map || !key || !value) {
        fprintf(stderr, "Null pointer passed to put: file %s at line %d\n", __FILE__, __LINE__);
        return;
    }
    int index = hash_function(key, map->capacity);
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->key = key;  // Assuming keys are not dynamically allocated
    new_node->value = value;
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check for resizing
    if ((float)map->size / map->capacity >= map->load_factor) {
        resize(map);
    }
}

char* get(HashMap* map, char* key) {
    if (!map || !key) {
        fprintf(stderr, "Null pointer passed to get: file %s at line %d\n", __FILE__, __LINE__);
        return NULL;
    }
    int index = hash_function(key, map->capacity);
    Node* current = map->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void resize(HashMap* map) {
    if (!map) {
        fprintf(stderr, "Null pointer passed to resize: file %s at line %d\n", __FILE__, __LINE__);
        return;
    }
    int new_capacity = map->capacity * 2;
    Node** new_buckets = (Node**)calloc(new_capacity, sizeof(Node*));

    // Rehash all existing elements
    for (int i = 0; i < map->capacity; i++) {
        Node* current = map->buckets[i];
        while (current != NULL) {
            int new_index = hash_function(current->key, new_capacity);
            Node* temp = current;
            current = current->next;
            temp->next = new_buckets[new_index];
            new_buckets[new_index] = temp;
        }
    }

    // Free old buckets
    free(map->buckets);

    map->capacity = new_capacity;
    map->buckets = new_buckets;
}

void free_hash_map(HashMap* map) {
    if (!map) {
        fprintf(stderr, "Null pointer passed to free_hash_map: file %s at line %d\n", __FILE__, __LINE__);
        return;
    }
    for (int i = 0; i < map->capacity; i++) {
        Node* current = map->buckets[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}
