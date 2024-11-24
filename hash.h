#ifndef HASH_H
#define HASH_H

#define HASHMAP_INIT_CAPACITY 8
#define HASHMAP_LOAD_FACTOR 0.75

typedef struct KeyValue {
    unsigned short key;
    unsigned short value;
    struct KeyValue *next;
} KeyValue;

typedef struct HashMap {
    KeyValue **entries;
    int capacity;
    int size;
} HashMap;


unsigned int hash(unsigned short key);
HashMap *hashmap_create();
void hashmap_resize(HashMap *map, int new_capacity);
void hashmap_put(HashMap *map, unsigned short key, unsigned short value);
unsigned short hashmap_get(HashMap *map, unsigned short key,unsigned short *status) ;
int hashmap_remove(HashMap *map, unsigned short key);
int hashmap_contain(HashMap *map, unsigned short key);
int hashmap_remove_all(HashMap *map);
void hashmap_free(HashMap *map);
#endif