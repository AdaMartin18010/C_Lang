/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\README.md
 * Line: 585
 * Language: c
 * Block ID: 06602165
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 1. 链表实现 */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    size_t size;
} LinkedList;

LinkedList* list_create(void);
void list_push_front(LinkedList *list, int value);
void list_push_back(LinkedList *list, int value);
int list_pop_front(LinkedList *list);
void list_reverse(LinkedList *list);
void list_destroy(LinkedList *list);

/* 2. 栈的实现 */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Stack;

Stack* stack_create(size_t capacity);
void stack_push(Stack *s, int value);
int stack_pop(Stack *s);
int stack_peek(const Stack *s);
int stack_is_empty(const Stack *s);
void stack_destroy(Stack *s);

/* 3. 队列的实现 */
typedef struct {
    int *data;
    size_t front;
    size_t rear;
    size_t size;
    size_t capacity;
} Queue;

Queue* queue_create(size_t capacity);
void queue_enqueue(Queue *q, int value);
int queue_dequeue(Queue *q);
int queue_is_empty(const Queue *q);
void queue_destroy(Queue *q);

/* 4. 二叉搜索树 */
typedef struct TreeNode {
    int key;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode* bst_insert(TreeNode *root, int key);
TreeNode* bst_search(TreeNode *root, int key);
TreeNode* bst_delete(TreeNode *root, int key);
void bst_inorder(TreeNode *root);   // 中序遍历
void bst_destroy(TreeNode *root);

/* 5. 哈希表 */
typedef struct HashEntry {
    char *key;
    int value;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry **buckets;
    size_t size;
    size_t count;
} HashTable;

HashTable* ht_create(size_t size);
void ht_insert(HashTable *ht, const char *key, int value);
int ht_get(HashTable *ht, const char *key, int *value);
void ht_remove(HashTable *ht, const char *key);
void ht_destroy(HashTable *ht);
