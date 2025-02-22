#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#define SIZE 100

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    STRING,
    LIST
} ValueType;

typedef struct listNode {
    char* value;
    struct listNode* next;
} listNode;

typedef union {
    char* stringValue;
    listNode* listValue;
} Value;

typedef struct keyValuePair{
  char* key;
  ValueType type;
    Value value;
  struct keyValuePair* next;
}keyValuePair;

typedef struct hashTable{
    keyValuePair** buckets;
    int size;
}hashTable;

hashTable* create_table(int size);
void put(hashTable* table, const char* key, const char* value);
char* get(const hashTable* table, const char* key);
char* delete_key(hashTable* table, const char* key);
void free_table(hashTable* table);
int save_to_file(hashTable* table, const char* filename);
int load_from_file(hashTable* table, const char* filename);
unsigned long hash(const char* key, int size);

void lpush(hashTable* table, const char* key, const char* value);
char* lpop(hashTable* table, const char* key);
void rpush(hashTable* table, const char* key, const char* value);
char* rpop(hashTable* table, const char* key);

#endif