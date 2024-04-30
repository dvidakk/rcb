#ifndef HMAP_H
#define HMAP_H

// Define a node structure for the hash map
typedef struct Node {
    char *key;
    char *value;
    struct Node *next;
} Node;

// Define the hash table structure
typedef struct HashMap {
    int capacity;  // Initial capacity of the hash table
    float load_factor;  // Threshold for resizing (e.g., 0.75)
    Node **buckets;  // Array of pointers to linked lists (buckets)
    int size;  // Current number of elements in the hash table
} HashMap;

HashMap* create_hash_map(int capacity, float load_factor);
void put(HashMap* map, char* key, char* value);
char* get(HashMap* map, char* key);
void resize(HashMap* map);
void free_hash_map(HashMap* map);
char* combine_switches(char* switch1, char* switch2);


#endif // HMAP_H