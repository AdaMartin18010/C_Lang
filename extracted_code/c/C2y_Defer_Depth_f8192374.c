/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1839
 * Language: c
 * Block ID: f8192374
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: scope_guard.c
// compile: clang -std=c2y scope_guard.c -o scope_guard

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 可撤销的defer模式
// 使用一个标志变量来控制是否执行清理

typedef struct {
    bool committed;
    const char *name;
} Transaction;

int database_transaction_pattern(void) {
    Transaction tx = {false, "main_transaction"};

    defer {
        if (!tx.committed) {
            printf("[ROLLBACK] Transaction '%s' aborted, rolling back...\n", tx.name);
            // 执行实际的回滚操作
        } else {
            printf("[COMMIT] Transaction '%s' already committed\n", tx.name);
        }
    }

    printf("Starting transaction '%s'\n", tx.name);

    // 执行操作1
    printf("Executing operation 1...\n");
    if (rand() % 10 == 0) {
        printf("Operation 1 failed!\n");
        return -1;  // 自动回滚
    }

    // 执行操作2
    printf("Executing operation 2...\n");
    if (rand() % 10 == 0) {
        printf("Operation 2 failed!\n");
        return -1;  // 自动回滚
    }

    // 执行操作3
    printf("Executing operation 3...\n");
    if (rand() % 10 == 0) {
        printf("Operation 3 failed!\n");
        return -1;  // 自动回滚
    }

    // 所有操作成功，提交事务
    tx.committed = true;
    printf("All operations successful, committing transaction...\n");

    return 0;  // defer看到committed=true，不会回滚
}

// 资源池管理
typedef struct {
    void **items;
    size_t capacity;
    size_t count;
    bool initialized;
} ResourcePool;

int resource_pool_example(void) {
    ResourcePool pool = {0};

    // 分配资源池
    pool.capacity = 10;
    pool.items = calloc(pool.capacity, sizeof(void*));
    if (!pool.items) return -1;
    defer {
        printf("Cleaning up resource pool (%zu items allocated)\n", pool.count);
        for (size_t i = 0; i < pool.count; i++) {
            if (pool.items[i]) {
                printf("  Releasing item %zu\n", i);
                free(pool.items[i]);
            }
        }
        free(pool.items);
    }

    // 分配资源
    for (size_t i = 0; i < 5; i++) {
        pool.items[i] = malloc(100);
        if (!pool.items[i]) return -1;
        pool.count++;
        printf("Allocated item %zu\n", i);
    }

    // 模拟某些操作可能失败
    if (rand() % 2 == 0) {
        printf("Simulated failure!\n");
        return -1;  // 自动清理所有已分配的资源
    }

    printf("Operations completed successfully\n");
    return 0;  // 自动清理
}

int main(void) {
    srand(42);

    printf("=== Transaction Pattern ===\n");
    for (int i = 0; i < 5; i++) {
        printf("\n--- Run %d ---\n", i + 1);
        database_transaction_pattern();
    }

    printf("\n=== Resource Pool Pattern ===\n");
    resource_pool_example();

    return 0;
}
