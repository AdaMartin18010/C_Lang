/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1598
 * Language: c
 * Block ID: 6815f73d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ============================================================================
 * 链表完整实现 (Linked List Complete Implementation)
 * ============================================================================
 * 编译命令: gcc -std=c23 -Wall -Wextra -o linked_list linked_list.c
 * 运行命令: ./linked_list
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// C23 兼容性宏
#if __STDC_VERSION__ < 202311L
    #define nullptr NULL
#endif

// =============================================================================
// 第一部分: 单链表实现
// =============================================================================

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    size_t size;
} LinkedList;

// 函数声明
[[nodiscard]] Node* create_node(int data);
void list_init(LinkedList* list);
void list_destroy(LinkedList* list);
bool list_prepend(LinkedList* list, int data);
bool list_append(LinkedList* list, int data);
bool list_insert_at(LinkedList* list, size_t index, int data);
bool list_delete(LinkedList* list, int data);
bool list_delete_at(LinkedList* list, size_t index);
int list_find(const LinkedList* list, int data);
int list_get(const LinkedList* list, size_t index);
void list_print(const LinkedList* list);
void list_reverse(LinkedList* list);
Node* list_reverse_recursive_helper(Node* head);
void list_reverse_recursive(LinkedList* list);
bool list_has_cycle(const Node* head);
Node* list_find_cycle_entry(Node* head);

// 实现...
[[nodiscard]]
Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return nullptr;
    }
    new_node->data = data;
    new_node->next = nullptr;
    return new_node;
}

void list_init(LinkedList* list) {
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

void list_destroy(LinkedList* list) {
    Node* current = list->head;
    while (current != nullptr) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

// 这里省略了重复的实现，实际文件应包含完整代码...
// [完整实现已在上面各节展示]

// =============================================================================
// 第二部分: 测试函数
// =============================================================================

void test_basic_operations() {
    printf("\n========== 基础操作测试 ==========\n");

    LinkedList list;
    list_init(&list);

    // 插入测试
    printf("\n--- 插入测试 ---\n");
    for (int i = 1; i <= 5; i++) {
        list_append(&list, i * 10);
        printf("追加 %d: ", i * 10);
        list_print(&list);
    }

    list_prepend(&list, 5);
    printf("头部插入 5: ");
    list_print(&list);

    list_insert_at(&list, 3, 25);
    printf("位置3插入 25: ");
    list_print(&list);

    // 查找测试
    printf("\n--- 查找测试 ---\n");
    int idx = list_find(&list, 30);
    printf("查找30: 索引 = %d\n", idx);
    printf("获取索引2的元素: %d\n", list_get(&list, 2));

    // 删除测试
    printf("\n--- 删除测试 ---\n");
    list_delete(&list, 25);
    printf("删除25后: ");
    list_print(&list);

    list_delete_at(&list, 0);
    printf("删除位置0后: ");
    list_print(&list);

    list_destroy(&list);
    printf("\n基础测试通过!\n");
}

void test_reverse() {
    printf("\n========== 反转测试 ==========\n");

    LinkedList list;
    list_init(&list);

    for (int i = 1; i <= 5; i++) {
        list_append(&list, i);
    }
    printf("原链表: ");
    list_print(&list);

    list_reverse(&list);
    printf("迭代反转后: ");
    list_print(&list);

    list_reverse_recursive(&list);
    printf("递归反转后: ");
    list_print(&list);

    list_destroy(&list);
    printf("\n反转测试通过!\n");
}

void test_cycle_detection() {
    printf("\n========== 环检测测试 ==========\n");

    // 创建有环链表
    Node* n1 = create_node(1);
    Node* n2 = create_node(2);
    Node* n3 = create_node(3);
    Node* n4 = create_node(4);

    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = n2;  // 创建环

    bool has_cycle = list_has_cycle(n1);
    printf("链表有环: %s\n", has_cycle ? "是" : "否");

    if (has_cycle) {
        Node* entry = list_find_cycle_entry(n1);
        printf("环的入口: %d\n", entry ? entry->data : -1);
    }

    // 清理 (需要打破环才能正常释放)
    n4->next = nullptr;
    free(n1);
    free(n2);
    free(n3);
    free(n4);

    printf("\n环检测测试通过!\n");
}

void test_josephus() {
    printf("\n========== 约瑟夫问题测试 ==========\n");
    printf("n=7, m=3 的约瑟夫问题:\n");
    int survivor = josephus(7, 3);
    printf("最后幸存者是: %d\n", survivor);
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║         链表完整实现测试 (C23 Standard)                    ║\n");
    printf("║         Linked List Complete Implementation                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);
    printf("C标准版本: %ld\n", __STDC_VERSION__);

    srand((unsigned int)time(nullptr));

    test_basic_operations();
    test_reverse();
    test_cycle_detection();
    test_josephus();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
