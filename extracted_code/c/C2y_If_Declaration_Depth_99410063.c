/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 1468
 * Language: c
 * Block ID: 99410063
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 6: 内存分配检查
 * 演示使用 if 声明进行安全的内存分配
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 安全的 malloc 包装
#define SAFE_MALLOC(ptr, size) \
    if ((ptr) = malloc(size); (ptr) == NULL) { \
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__); \
    } else

// 动态数组结构
typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} IntVector;

IntVector* int_vector_create(size_t initial_capacity) {
    if (IntVector* vec = malloc(sizeof(IntVector)); vec == NULL) {
        return NULL;
    } else {
        if (int* data = malloc(initial_capacity * sizeof(int)); data == NULL) {
            free(vec);
            return NULL;
        } else {
            vec->data = data;
            vec->size = 0;
            vec->capacity = initial_capacity;
            return vec;
        }
    }
}

bool int_vector_push(IntVector* vec, int value) {
    if (vec->size >= vec->capacity) {
        // 需要扩容
        size_t new_capacity = vec->capacity * 2;
        if (new_capacity == 0) new_capacity = 4;

        if (int* new_data = realloc(vec->data, new_capacity * sizeof(int));
            new_data == NULL) {
            return false;
        } else {
            vec->data = new_data;
            vec->capacity = new_capacity;
        }
    }

    vec->data[vec->size++] = value;
    return true;
}

void int_vector_free(IntVector* vec) {
    if (vec != NULL) {
        free(vec->data);
        free(vec);
    }
}

// 安全的字符串复制
char* safe_strdup(const char* str) {
    if (str == NULL) return NULL;

    size_t len = strlen(str) + 1;
    if (char* copy = malloc(len); copy == NULL) {
        return NULL;
    } else {
        memcpy(copy, str, len);
        return copy;
    }
}

// 安全的字符串连接
char* safe_strcat(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL) return NULL;

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    if (char* result = malloc(len1 + len2 + 1); result == NULL) {
        return NULL;
    } else {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2 + 1);
        return result;
    }
}

// 多维度数组分配
int** allocate_matrix(size_t rows, size_t cols) {
    if (int** matrix = malloc(rows * sizeof(int*)); matrix == NULL) {
        return NULL;
    } else {
        bool allocation_failed = false;

        for (size_t i = 0; i < rows; i++) {
            if (int* row = malloc(cols * sizeof(int)); row == NULL) {
                // 清理已分配的内存
                for (size_t j = 0; j < i; j++) {
                    free(matrix[j]);
                }
                free(matrix);
                allocation_failed = true;
                break;
            } else {
                matrix[i] = row;
            }
        }

        if (allocation_failed) {
            return NULL;
        }

        return matrix;
    }
}

void free_matrix(int** matrix, size_t rows) {
    if (matrix != NULL) {
        for (size_t i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
}

// 复杂数据结构分配
typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    size_t size;
} LinkedList;

LinkedList* linked_list_create(void) {
    if (LinkedList* list = malloc(sizeof(LinkedList)); list == NULL) {
        return NULL;
    } else {
        list->head = NULL;
        list->size = 0;
        return list;
    }
}

bool linked_list_push_front(LinkedList* list, int value) {
    if (Node* node = malloc(sizeof(Node)); node == NULL) {
        return false;
    } else {
        node->value = value;
        node->next = list->head;
        list->head = node;
        list->size++;
        return true;
    }
}

void linked_list_free(LinkedList* list) {
    if (list != NULL) {
        Node* current = list->head;
        while (current != NULL) {
            Node* next = current->next;
            free(current);
            current = next;
        }
        free(list);
    }
}

int main(void) {
    printf("=== Memory Allocation Demo ===\n\n");

    // 动态数组演示
    printf("1. Dynamic array:\n");
    if (IntVector* vec = int_vector_create(4); vec == NULL) {
        printf("Failed to create vector\n");
    } else {
        for (int i = 0; i < 10; i++) {
            if (!int_vector_push(vec, i * 10)) {
                printf("Failed to push %d\n", i);
                break;
            }
        }

        printf("Vector contents: ");
        for (size_t i = 0; i < vec->size; i++) {
            printf("%d ", vec->data[i]);
        }
        printf("\n");

        int_vector_free(vec);
    }

    // 字符串操作演示
    printf("\n2. String operations:\n");
    if (char* copy = safe_strdup("Hello, C2y!"); copy != NULL) {
        printf("Copied: %s\n", copy);

        if (char* combined = safe_strcat(copy, " Welcome!"); combined != NULL) {
            printf("Combined: %s\n", combined);
            free(combined);
        }

        free(copy);
    }

    // 矩阵分配演示
    printf("\n3. Matrix allocation:\n");
    if (int** matrix = allocate_matrix(3, 4); matrix == NULL) {
        printf("Failed to allocate matrix\n");
    } else {
        // 初始化矩阵
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 4; j++) {
                matrix[i][j] = (int)(i * 4 + j);
            }
        }

        // 打印矩阵
        printf("Matrix:\n");
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 4; j++) {
                printf("%3d ", matrix[i][j]);
            }
            printf("\n");
        }

        free_matrix(matrix, 3);
    }

    // 链表演示
    printf("\n4. Linked list:\n");
    if (LinkedList* list = linked_list_create(); list == NULL) {
        printf("Failed to create list\n");
    } else {
        for (int i = 0; i < 5; i++) {
            if (!linked_list_push_front(list, i * 10)) {
                printf("Failed to push %d\n", i);
                break;
            }
        }

        printf("List contents (front to back): ");
        for (Node* n = list->head; n != NULL; n = n->next) {
            printf("%d ", n->value);
        }
        printf("\n");

        linked_list_free(list);
    }

    return 0;
}
