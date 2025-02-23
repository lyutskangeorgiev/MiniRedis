Mini Redis - A Lightweight Key-Value Store

A simplified in-memory key-value store inspired by Redis, implemented in C, with support for strings and lists, as well as basic persistence.

Features
- Basic GET and PUT operations for string values
- LPUSH, RPUSH, LPOP, RPOP for list operations
- Key deletion support
- Persistence: Save and load data from a file
- Hash-based storage for efficient lookups

Installation
Prerequisites:
- GCC or Clang (for compilation)
- Make (if using a Makefile)

Build:
1. Clone the repository:
   git clone https://github.com/lyutskangeorgiev/mini-redis.git
   cd mini-redis

2. Compile the project:
   make  # If using a Makefile
   OR manually compile:
   gcc -o mini_redis main.c hash_table.c -Wall -Wextra -O2

Usage
Run the program:
   ./mini_redis

Available Commands:
1  - GET key      → Retrieves a string value
2  - PUT key value → Stores a string value
3  - DELETE key   → Deletes a key
4  - SAVE filename → Saves the current state to a file
5  - LOAD filename → Loads a saved state
6  - LPUSH key value → Pushes value to the left of a list
7  - RPUSH key value → Pushes value to the right of a list
8  - LPOP key → Removes and returns the leftmost value of a list
9  - RPOP key → Removes and returns the rightmost value of a list
10 - EXIT → Exits the program

Code Structure
- hash_table.h → Header file defining key structures & functions.
- hash_table.c → Implements hash table operations.
- main.c → Handles user input and executes commands.
- Makefile → (Optional) Simplifies the build process.

Core Data Structures:
typedef enum { STRING, LIST } ValueType;

typedef struct listNode {
    char* value;
    struct listNode* next;
} listNode;

typedef union {
    char* stringValue;
    listNode* listValue;
} Value;

typedef struct keyValuePair {
    char* key;
    ValueType type;
    Value value;
    struct keyValuePair* next;
} keyValuePair;

typedef struct hashTable {
    keyValuePair** buckets;
    int size;
} hashTable;

Future Improvements
- Add TTL (Time-to-Live) expiration for keys  
- Implement networking for remote client support  
- Optimize hash function for better performance  
- Implement support for sets and hashes  

Contributing
Contributions are welcome! Feel free to fork this repository and submit pull requests.
