#include <stdio.h>

#include "hash_table.h"

int main(void) {
    hashTable* table = create_table(SIZE);

    while (1) {
        int choice;
        char key[256];
        char value[256];
        char* result;
        char file[256];
        char input[256];
        keyValuePair* entry;

        printf("Enter a command: (1 = GET, 2 = PUT, 3 = DELETE, 4 = SAVE, 5 = LOAD, 6 = LPUSH, 7 = RPUSH, 8 = LPOP, 9 = RPOP, 10 = EXIT)\n");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input\n");
            continue;
        }
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number between 1 and 10.\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("Using GET\n");
                printf("Give the key (string) of the element you search for\n");
                scanf("%s", key);
                result = get(table, key);

                if (result == NULL) {
                    printf("ERROR: Key not found\n");
                } else {
                    printf("Value: %s\n", result);
                }
                break;
            case 2:
                printf("Using PUT\n");
                printf("Give the key (string) of the element you want to put in\n");
                scanf("%s", key);
                getchar();
                printf("Give the value (string) of the element you want to put in\n");
                fgets(value, sizeof(value), stdin);
                value[strcspn(value, "\n")] = 0;
                put(table, key, value);
                break;
            case 3:
                printf("Using DELETE\n");
                printf("Give the key (string) of the element you want to delete\n");
                scanf("%s", key);
                char* deleted = delete_key(table, key);
                if (deleted == NULL) {
                    printf("Key deletion was not successful\n");
                }
                else {
                    printf("Successfully deleted element with value: %s\n", deleted);
                }
                break;
            case 4:
                printf("Using SAVE\n");
                printf("Give the name of the file you want to write in\n");
                scanf("%s", file);
                if (save_to_file(table, file) == 0) {
                    printf("Successfully saved in file\n");
                }
                else {
                    printf("Saving in file unsuccessful\n");
                }
                break;
            case 5:
                printf("Using LOAD\n");
                printf("Give the name of the file you want to read from\n");
                if (!fgets(file, sizeof(file), stdin)) {
                    printf("Error reading the file name\n");
                    continue;
                }
                file[strcspn(file, "\n")] = 0;
                if (load_from_file(table, file) == 0) {
                    printf("Successfully loading from file\n");
                }
                else {
                    printf("Loading from file unsuccessful\n");
                }
                break;
            case 6:
                printf("Using LPUSH\n");
                printf("Give the key (string) of the element you want to put in\n");
                scanf("%s", key);

                entry = table->buckets[hash(key, table->size)];
                while (entry != NULL && strcmp(entry->key, key) != 0) {
                    entry = entry->next;
                }
                if (entry != NULL && entry->type != LIST) {
                    printf("ERROR: the element with that key is not a LIST\n");
                    break;
                }

                printf("Enter values (type STOP to end): ");
                while (scanf("%s", value) == 1 && strcmp(value, "STOP") != 0) {
                    lpush(table, key, value);
                }
                break;
            case 7:
                printf("Using RPUSH\n");
                printf("Give the key (string) of the element you want to put in\n");
                scanf("%s", key);

                entry = table->buckets[hash(key, table->size)];
                while (entry != NULL && strcmp(entry->key, key) != 0) {
                    entry = entry->next;
                }
                if (entry != NULL && entry->type != LIST) {
                    printf("ERROR: The element with that key is not a LIST\n");
                    break;
                }

                printf("Enter values (type STOP to end): ");
                while (scanf("%s", value) == 1 && strcmp(value, "STOP") != 0) {
                    rpush(table, key, value);
                }
                break;
            case 8:
                printf("Using LPOP\n");
                printf("Give the key (string) of the element you want to remove\n");
                scanf("%s", key);
                while (1) {
                    char response[10];
                    printf("Do you want to pop an element (type STOP to stop): ");
                    scanf("%s", response);
                    if (strcmp(response, "STOP") == 0) {
                        break;
                    }
                    char* poppedVal = lpop(table, key);
                    if (poppedVal == NULL) {
                        printf("List is empty\n");
                        break;
                    }
                    printf("Popped value is %s\n", poppedVal);
                    free(poppedVal);
                }
                break;
            case 9:
                printf("Using RPOP\n");
                printf("Give the key (string) of the element you want to remove\n");
                scanf("%s", key);
                while (1) {
                    char response[10];
                    printf("Do you want to pop an element (type STOP to stop): ");
                    scanf("%s", response);
                    if (strcmp(response, "STOP") == 0) {
                        break;
                    }
                    char* poppedVal = rpop(table, key);
                    if (poppedVal == NULL) {
                        printf("List is empty\n");
                        break;
                    }
                    printf("Popped value is %s\n", poppedVal);
                    free(poppedVal);
                }
                break;
            case 10:
                printf("Using EXIT\n");
                printf("Enter filename to save the data: ");
                scanf("%s", file);
                if (save_to_file(table, file) == 0) {
                    printf("Data saved successfully\n");
                }else {
                    printf("Warning: Data saving failed\n");
                }
                free_table(table);
                exit(0);
            default:
                printf("Invalid choice, pick a number between 1 and 10\n");
                break;
        }
    }
}
