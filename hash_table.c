#include "hash_table.h"

unsigned long hash(const char* key, int size) {
    // 1. (hash << 5) shifts the bits of 'hash' 5 positions to the left.
    //    = hash * 32.
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % size;
}
hashTable* create_table(int size) {
    hashTable* table = malloc(sizeof(hashTable));
    if (table == NULL) {
        printf("Allocation of memory was unsuccessful\n");
        exit(1);
    }
    table->size = size;
    table->buckets = calloc(size, sizeof(keyValuePair*));
    if (table->buckets == NULL) {
        free(table);
        printf("Allocation of memory was unsuccessful\n");
        exit(1);
    }
    return table;
}
void put(hashTable* table, const char* key, const char* value) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0){
            free(curr->value.stringValue);
            curr->value.stringValue = strdup(value);
            if (curr->value.stringValue == NULL) {
                printf("Unsuccessful memory allocation\n");
                exit(1);
            }
            curr->type = STRING;
            return;
        }

        curr = curr->next;
    }
    keyValuePair* newNode = malloc(sizeof(keyValuePair));
    if (newNode == NULL) {
        printf("Unsuccessful memory alocation\n");
        exit(1);
    }
    newNode->type = STRING;
    newNode->value.stringValue = strdup(value);
    if (newNode->value.stringValue == NULL) {
        printf("Unsuccessful memory alocation\n");
        exit(1);
    }
    newNode->key = strdup(key);
    if (newNode->key == NULL) {
        printf("Unsuccessful memory alocation\n");
        exit(1);
    }
    newNode->next = table->buckets[index];
    table->buckets[index] = newNode;
    printf("Successfully added key\n");
}

char* get(const hashTable* table, const char* key) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            return curr->value.stringValue;
        }
        curr = curr->next;
    }
    printf("Key %s not found\n", key);
    return NULL;
}

char* delete_key(hashTable* table, const char* key) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    char* return_value = NULL;
    if (curr!=NULL && strcmp(table->buckets[index]->key, key) == 0) {
        table->buckets[index] = table->buckets[index]->next;
        return_value = curr->value.stringValue;
        free(curr->key);
        free(curr);
        return return_value;
    }
    while (curr!=NULL && curr->next != NULL) {
        if (strcmp(curr->next->key, key) == 0) {
            keyValuePair* temp = curr->next;
            curr->next = curr->next->next;
            return_value = temp->value.stringValue;
            free(temp->key);
            free(temp);
            return return_value;
        }
        curr = curr->next;
    }
    printf("No node with that key to delete\n");
    exit(1);
}

void free_table(hashTable* table) {
    for (int i = 0; i < table->size; i++) {
        keyValuePair* curr = table->buckets[i];
        while (curr != NULL) {
            keyValuePair* temp = curr;
            curr = curr->next;
            free(temp->key);
            if (temp->type == STRING) {
                if (temp->value.stringValue != NULL) {
                    free(temp->value.stringValue);
                }
            }
            else if (temp->type == LIST) {
                listNode* node = temp->value.listValue;
                while (node != NULL) {
                    listNode* tempNode = node;
                    node = node->next;
                    free(tempNode->value);
                    free(tempNode);
                }
            }
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}

int save_to_file(hashTable* table, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    for (int i = 0; i < table->size; i++) {
        keyValuePair* curr = table->buckets[i];
        while (curr != NULL) {
            fprintf(file, "%s ", curr->key);
            if (curr->type == STRING) {
                fprintf(file, "STRING %s\n", curr->value.stringValue);
            }
            else if (curr->type == LIST) {
                int count = 0;
                listNode* node = curr->value.listValue;
                while (node != NULL) {
                    count++;
                    node = node->next;
                }
                fprintf(file, "LIST %d ", count);
                node = curr->value.listValue;
                while (node != NULL) {
                    fprintf(file, "%s", node->value);
                    if (node->next != NULL) {
                        fprintf(file, ",");
                    }
                    node = node->next;
                }
                fprintf(file, "\n");
            }
            curr = curr->next;
        }
    }
    fclose(file);
    return 0;
}

int load_from_file(hashTable* table, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    char key[256];
    char value[256];
    char type_str[10];
    char list_item[256];
    ValueType type;
    while (fscanf(file, "%s %s", key, type_str) == 2){
        if (strcmp(type_str, "STRING") == 0) {
            type = STRING;
            if (fscanf(file, " %[^\n]", value) != 1) {
                perror("Error reading string value");
                fclose(file);
                return -1;
            }
            put(table, key, value);
        }
        else if (strcmp(type_str, "LIST") == 0){
            type = LIST;
            int count;
            if (fscanf(file, "%d", &count) != 1) {
                perror("Error reading list count");
                fclose(file);
                return -1;
            }
            for (int i = 0; i < count; i++) {
                if (fscanf(file, "%s", list_item) != 1) {
                    perror("Error reading list item");
                    fclose(file);
                    return -1;
                }
                lpush(table, key, list_item);
            }
        }
    }
    fclose(file);
    return 0;
}

void lpush(hashTable* table, const char* key, const char* value) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (curr->type == LIST) {
                listNode* node = malloc(sizeof(listNode));
                if (node == NULL) {
                    printf("Memory allocation failed\n");
                   return;
                }
                node->value = strdup(value);
                if (node->value == NULL) {
                    printf("Memory allocation failed\n");
                    free(node);
                    return;
                }
                node->next = curr->value.listValue;
                curr->value.listValue = node;
                return;
            }
            printf("ERROR: the element with that key is not a LIST\n");
            return;
        }
        curr = curr->next;
    }
    keyValuePair* newNode = malloc(sizeof(keyValuePair));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->key = strdup(key);
    if (newNode->key == NULL) {
        printf("Memory allocation failed\n");
        free(newNode);
        return;
    }
    newNode->type = LIST;
    newNode->value.listValue = malloc(sizeof(listNode));
    if (newNode->value.listValue == NULL) {
        printf("Memory allocation failed\n");
        free(newNode->key);
        free(newNode);
        return;
    }
    newNode->value.listValue->value = strdup(value);
    if (newNode->value.listValue->value == NULL) {
        printf("Memory allocation failed\n");
        free(newNode->value.listValue);
        free(newNode->key);
        free(newNode);
        return;
    };
    newNode->value.listValue->next = NULL;
    newNode->next = table->buckets[index];
    table->buckets[index] = newNode;
}

//Remove the returned string headVal when calling!
char* lpop(hashTable* table, const char* key) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    char* headVal;
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (curr->type == LIST) {
                if (curr->value.listValue == NULL) {
                    return NULL;
                }
                listNode* head = curr->value.listValue;
                headVal = head->value;
                curr->value.listValue = head->next;
                free(head);
                if (curr->value.listValue == NULL) {
                    if (table->buckets[index] == curr) {
                        table->buckets[index] = curr->next;
                    }
                    else {
                        keyValuePair* prev = table->buckets[index];
                        while (prev->next != NULL && prev->next != curr) {
                            prev = prev->next;
                        }
                        if (prev->next != NULL) {
                            prev->next = curr->next;
                        }
                    }
                    free(curr->key);
                    free(curr);
                }
                return headVal;
            }
            printf("ERROR: The key is of type STRING\n");
            return NULL;
        }
        curr = curr->next;
    }
    printf("ERROR: Key not found\n");
    return NULL;
}

void rpush(hashTable* table, const char* key, const char* value) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (curr->type == LIST) {
                listNode* node = malloc(sizeof(listNode));
                listNode* temp = curr->value.listValue;
                if (node == NULL) {
                    printf("Memory allocation failed\n");
                    return;
                }
                node->next = NULL;
                node->value = strdup(value);
                if (node->value == NULL) {
                    printf("Memory allocation failed\n");
                    free(node);
                    return;
                }
                if (curr->value.listValue == NULL) {
                    curr->value.listValue = node;
                    return;
                }
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = node;
                return;
            }
            printf("ERROR: Key '%s' exists but is not a LIST.\n", key);
            return;
        }
        curr = curr->next;
    }
    keyValuePair* newNode = malloc(sizeof(keyValuePair));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->key = strdup(key);
    if (newNode->key == NULL) {
        printf("Memory allocation failed\n");
        free(newNode);
        return;
    }
    newNode->type = LIST;
    newNode->value.listValue = malloc(sizeof(listNode));
    if (newNode->value.listValue == NULL) {
        printf("Memory allocation failed\n");
        free(newNode->key);
        free(newNode);
        return;
    }
    newNode->value.listValue->value = strdup(value);
    if (newNode->value.listValue->value == NULL) {
        printf("Memory allocation failed\n");
        free(newNode->value.listValue);
        free(newNode->key);
        free(newNode);
        return;
    };
    newNode->value.listValue->next = NULL;
    newNode->next = table->buckets[index];
    table->buckets[index] = newNode;
    return;
}

//Remove the returned string headVal when calling!
char* rpop(hashTable* table, const char* key) {
    int index = hash(key, table->size);
    keyValuePair* curr = table->buckets[index];
    char* lastElement = NULL;
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (curr->type == LIST) {
                if (curr->value.listValue == NULL) {
                    return NULL;
                }
                listNode* temp = curr->value.listValue;
                if (curr->value.listValue->next == NULL) {
                    lastElement = curr->value.listValue->value;
                    free(curr->value.listValue);
                    curr->value.listValue = NULL;

                    if (table->buckets[index] == curr) {
                        table->buckets[index] = curr->next;
                    } else {
                        keyValuePair* prev = table->buckets[index];
                        while (prev->next != NULL && prev->next != curr) {
                            prev = prev->next;
                        }
                        if (prev->next != NULL) {
                            prev->next = curr->next;
                        }
                    }
                    free(curr->key);
                    free(curr);
                }
                else {
                    while (temp->next != NULL && temp->next->next != NULL) {
                        temp = temp->next;
                    }
                    lastElement = temp->next->value;
                    free(temp->next);
                    temp->next = NULL;
                }
                return lastElement;

            }
            printf("ERROR: The key is of type STRING\n");
            return NULL;
        }
        curr = curr->next;
    }
    printf("ERROR: Key not found\n");
    return NULL;
}