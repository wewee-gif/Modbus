#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"



unsigned int hash(unsigned short key) {
    // 注意：这里只是简单的示例，可能需要一个更好的哈希函数
    return key % HASHMAP_INIT_CAPACITY; // 仅供演示，实际中应使用更复杂的哈希算法
    //我这里使用的时候限制了size不能大于8 所以这种基础的哈希算法没问题，甚至不会出现哈希碰撞  以后学习一下更好的hash算法
}

HashMap *hashmap_create() {
    HashMap *map = malloc(sizeof(HashMap));
    map->capacity = HASHMAP_INIT_CAPACITY;
    map->size = 0;
    map->entries = malloc(sizeof(KeyValue*) * map->capacity);
    memset(map->entries, 0, sizeof(KeyValue*) * map->capacity);
    return map;
}

void hashmap_resize(HashMap *map, int new_capacity) {
    KeyValue **new_entries = malloc(sizeof(KeyValue*) * new_capacity);
    memset(new_entries, 0, sizeof(KeyValue*) * new_capacity);
    for (int i = 0; i < map->capacity; ++i) {
        KeyValue *current = map->entries[i];
        while (current != NULL) {
            KeyValue *next = current->next;
            unsigned int index = hash(current->key) % new_capacity;
            current->next = new_entries[index];
            new_entries[index] = current;
            current = next;
        }
    }
    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;
}

void hashmap_put(HashMap *map, unsigned short key, unsigned short value) {
    // 检查是否需要调整容量...
    if (map->size >= map->capacity * HASHMAP_LOAD_FACTOR) {
        hashmap_resize(map, map->capacity * 2);
    }

    unsigned int index = hash(key) % map->capacity;
    KeyValue *kv = malloc(sizeof(KeyValue));
    kv->key = key;
    kv->value = value;
    kv->next = map->entries[index];
    map->entries[index] = kv;
    map->size++;
}

unsigned short hashmap_get(HashMap *map, unsigned short key,unsigned short *status) {
    unsigned int index = hash(key) % map->capacity;
    KeyValue *kv = map->entries[index];
    while (kv != NULL) {
        if (kv->key == key) {
            return kv->value;
        }
        kv = kv->next;
    }

    status=0;//状态为-1 代表没找到
    return 0; 
}


//在其他函数中尽量少使用，不然就相当于遍历了两遍
int hashmap_contain(HashMap *map, unsigned short key){
    unsigned int index = hash(key) % map->capacity;
    KeyValue *kv = map->entries[index];
    while (kv != NULL) {
        if (kv->key == key) {
            return 1;
        }
        kv = kv->next;
    }
    return -1;
}


 int hashmap_remove(HashMap *map, unsigned short key) {
    unsigned int index = hash(key) % map->capacity;
    KeyValue *kv = map->entries[index];
    KeyValue *kv_pre;

    if (kv->key == key) {
        map->entries[index]=kv->next;
        free(kv);
        map->size--;
        return 1;
    }else{
        kv_pre=kv;
        kv = kv->next;
        while (kv != NULL) {
            if (kv->key == key) {
                kv_pre->next=kv->next;
                free(kv);
                map->size--;
                return 1;
            }
            kv_pre=kv;
            kv = kv->next;
        }
    }
    return -1;
}

int hashmap_remove_all(HashMap *map){
    KeyValue *kv;
    KeyValue *kv_pre;
    for(int i=0;i<map->capacity;i++){
        kv = map->entries[i];
        while (kv != NULL) {
            kv_pre=kv;
            kv = kv->next;
            free(kv_pre);
            map->size--;
        }
    }
    if(map->size=0){
        return 1;
    }
    return -1;
}


void hashmap_free(HashMap *map) {
    for (int i = 0; i < map->capacity; ++i) {
        KeyValue *current = map->entries[i];
        while (current != NULL) {
            KeyValue *next = current->next;
            free(current);
            current = next;
        }
    }
    free(map->entries);
    free(map);
}

// int main() {
//     HashMap *map = hashmap_create();
//     hashmap_put(map, 123, 456);
//     hashmap_put(map, 789, 101);
//     unsigned short int value = hashmap_get(map, 123);
//     if (value != -1) { // 注意：这里应该使用其他方式来表示未找到的情况
//         printf("Value for key 123: %hu\n", value);
//     } else {
//         printf("Key 123 not found\n");
//     }
//     hashmap_free(map);
//     return 0;
// }