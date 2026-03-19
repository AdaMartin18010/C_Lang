/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 595
 * Language: c
 * Block ID: 79e9c6fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// malloc_usage.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// ========== 基础分配模式 ==========

// 1. 基本分配
EMSCRIPTEN_KEEPALIVE
void* basic_allocation(int size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "malloc failed for size %d\n", size);
        return NULL;
    }
    return ptr;
}

// 2. 零初始化分配
EMSCRIPTEN_KEEPALIVE
void* zero_allocation(int count, int size) {
    void* ptr = calloc(count, size);
    if (!ptr) {
        fprintf(stderr, "calloc failed\n");
        return NULL;
    }
    // calloc 已清零，无需 memset
    return ptr;
}

// 3. 重新分配
EMSCRIPTEN_KEEPALIVE
void* resize_allocation(void* ptr, int new_size) {
    void* new_ptr = realloc(ptr, new_size);
    if (!new_ptr && new_size > 0) {
        fprintf(stderr, "realloc failed\n");
        return NULL;
    }
    return new_ptr;
}

// 4. 分配并复制字符串
EMSCRIPTEN_KEEPALIVE
char* duplicate_string(const char* source) {
    if (!source) return NULL;

    char* copy = strdup(source);
    // 或: char* copy = malloc(strlen(source) + 1);
    //     strcpy(copy, source);

    return copy;
}

// ========== 常见数据结构分配 ==========

// 动态数组
typedef struct {
    int* data;
    int size;
    int capacity;
} DynamicArray;

EMSCRIPTEN_KEEPALIVE
DynamicArray* array_create(int initial_capacity) {
    DynamicArray* arr = (DynamicArray*)malloc(sizeof(DynamicArray));
    if (!arr) return NULL;

    arr->data = (int*)malloc(initial_capacity * sizeof(int));
    if (!arr->data) {
        free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

EMSCRIPTEN_KEEPALIVE
void array_destroy(DynamicArray* arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}

EMSCRIPTEN_KEEPALIVE
int array_push(DynamicArray* arr, int value) {
    if (arr->size >= arr->capacity) {
        // 扩容
        int new_capacity = arr->capacity * 2;
        int* new_data = (int*)realloc(arr->data, new_capacity * sizeof(int));
        if (!new_data) return -1;

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    arr->data[arr->size++] = value;
    return 0;
}

// 链表
typedef struct Node {
    int value;
    struct Node* next;
} Node;

EMSCRIPTEN_KEEPALIVE
Node* list_create(int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) {
        node->value = value;
        node->next = NULL;
    }
    return node;
}

EMSCRIPTEN_KEEPALIVE
void list_append(Node** head, int value) {
    Node* new_node = list_create(value);
    if (!new_node) return;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    Node* current = *head;
    while (current->next) {
        current = current->next;
    }
    current->next = new_node;
}

EMSCRIPTEN_KEEPALIVE
void list_destroy(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

// 树结构
typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

EMSCRIPTEN_KEEPALIVE
TreeNode* tree_create(int value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node) {
        node->value = value;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

EMSCRIPTEN_KEEPALIVE
void tree_destroy(TreeNode* root) {
    if (root) {
        tree_destroy(root->left);
        tree_destroy(root->right);
        free(root);
    }
}
