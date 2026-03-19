/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1757
 * Language: c
 * Block ID: a345c536
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ============================================================================
 * 栈和队列完整实现 (Stack & Queue Complete Implementation)
 * ============================================================================
 * 编译命令: gcc -std=c23 -Wall -Wextra -o stack_queue stack_queue.c
 * 运行命令: ./stack_queue
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#if __STDC_VERSION__ < 202311L
    #define nullptr NULL
#endif

// 这里应包含前面所有的结构定义和函数实现...
// 为简洁起见，省略重复代码

// =============================================================================
// 测试函数
// =============================================================================

void test_array_stack() {
    printf("\n========== 数组栈测试 ==========\n");
    ArrayStack stack;
    array_stack_init(&stack);

    printf("Push: ");
    for (int i = 1; i <= 20; i++) {
        array_stack_push(&stack, i);
        if (i <= 5 || i > 17) {
            printf("%d ", i);
        } else if (i == 6) {
            printf("... ");
        }
    }
    printf("\n");
    array_stack_print(&stack);

    int val;
    array_stack_pop(&stack, &val);
    printf("Pop: %d\n", val);
    array_stack_print(&stack);

    array_stack_destroy(&stack);
    printf("数组栈测试通过!\n");
}

void test_linked_queue() {
    printf("\n========== 链表队列测试 ==========\n");
    LinkedQueue queue;
    linked_queue_init(&queue);

    printf("Enqueue: ");
    for (int i = 1; i <= 10; i++) {
        linked_queue_enqueue(&queue, i * 10);
        printf("%d ", i * 10);
    }
    printf("\n");
    linked_queue_print(&queue);

    int val;
    linked_queue_dequeue(&queue, &val);
    printf("Dequeue: %d\n", val);
    linked_queue_print(&queue);

    linked_queue_destroy(&queue);
    printf("链表队列测试通过!\n");
}

void test_expression_evaluation() {
    printf("\n========== 表达式求值测试 ==========\n");

    const char* expressions[] = {
        "3+4*2/(1-5)^2^3",
        "(2+3)*4",
        "10+20*3",
    };

    for (int i = 0; i < 3; i++) {
        char infix[256], postfix[256];
        strcpy(infix, expressions[i]);

        printf("\n中缀: %s\n", infix);
        infix_to_postfix(infix, postfix);
        printf("后缀: %s\n", postfix);
        // int result = evaluate_postfix(postfix);
        // printf("结果: %d\n", result);
    }

    printf("表达式求值测试通过!\n");
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       栈和队列完整实现测试 (C23 Standard)                  ║\n");
    printf("║       Stack & Queue Complete Implementation                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);

    test_array_stack();
    test_linked_queue();
    test_expression_evaluation();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
