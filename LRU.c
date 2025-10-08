#include <stdio.h>
#include <stdlib.h>

// Node for doubly linked list
typedef struct Node {
    int key, value;
    struct Node *prev, *next;
} Node;

// LRU Cache structure
typedef struct {
    int capacity;
    int size;
    Node *head, *tail;
    Node **hash;  // array of pointers for hashmap
} LRUCache;

#define HASH_SIZE 1000

// Hash function
int hashFunc(int key) {
    return key % HASH_SIZE;
}

// Create a new node
Node* createNode(int key, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->prev = newNode->next = NULL;
    return newNode;
}

// Create an LRU Cache
LRUCache* lRUCacheCreate(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = cache->tail = NULL;
    cache->hash = (Node**)calloc(HASH_SIZE, sizeof(Node*));
    return cache;
}

// Move a node to front (most recently used)
void moveToHead(LRUCache* cache, Node* node) {
    if (cache->head == node) return;

    // Detach node
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (cache->tail == node) cache->tail = node->prev;

    // Move to front
    node->next = cache->head;
    node->prev = NULL;
    if (cache->head) cache->head->prev = node;
    cache->head = node;

    if (!cache->tail)
        cache->tail = node;
}

// Remove least recently used (tail)
void removeTail(LRUCache* cache) {
    if (!cache->tail) return;

    Node* node = cache->tail;
    int hashIndex = hashFunc(node->key);
    cache->hash[hashIndex] = NULL;

    if (node->prev)
        node->prev->next = NULL;
    else
        cache->head = NULL;

    cache->tail = node->prev;
    free(node);
    cache->size--;
}

// Get operation
int lRUCacheGet(LRUCache* cache, int key) {
    int hashIndex = hashFunc(key);
    Node* node = cache->hash[hashIndex];

    if (node && node->key == key) {
        moveToHead(cache, node);
        return node->value;
    }
    return -1;
}

// Put operation
void lRUCachePut(LRUCache* cache, int key, int value) {
    int hashIndex = hashFunc(key);
    Node* node = cache->hash[hashIndex];

    if (node && node->key == key) {
        node->value = value;
        moveToHead(cache, node);
    } else {
        Node* newNode = createNode(key, value);
        newNode->next = cache->head;
        if (cache->head) cache->head->prev = newNode;
        cache->head = newNode;
        if (!cache->tail) cache->tail = newNode;

        cache->hash[hashIndex] = newNode;
        cache->size++;

        if (cache->size > cache->capacity)
            removeTail(cache);
    }
}

// Free memory
void lRUCacheFree(LRUCache* cache) {
    Node* temp = cache->head;
    while (temp) {
        Node* next = temp->next;
        free(temp);
        temp = next;
    }
    free(cache->hash);
    free(cache);
}

// Driver code
int main() {
    LRUCache* cache = lRUCacheCreate(2);

    lRUCachePut(cache, 1, 10);
    lRUCachePut(cache, 2, 20);
    printf("Get 1: %d\n", lRUCacheGet(cache, 1)); // 10

    lRUCachePut(cache, 3, 30); // Removes key 2
    printf("Get 2: %d\n", lRUCacheGet(cache, 2)); // -1 (not found)

    lRUCachePut(cache, 4, 40); // Removes key 1
    printf("Get 1: %d\n", lRUCacheGet(cache, 1)); // -1
    printf("Get 3: %d\n", lRUCacheGet(cache, 3)); // 30
    printf("Get 4: %d\n", lRUCacheGet(cache, 4)); // 40

    lRUCacheFree(cache);
    return 0;
}
