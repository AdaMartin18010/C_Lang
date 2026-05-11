/*
 * C11 原子操作与内存序示例
 * 编译: gcc -Wall -Wextra -std=c11 -o atomic_operations atomic_operations.c
 */
#include <stdio.h>
#include <stdatomic.h>
#include <threads.h>

static atomic_int shared_counter = ATOMIC_VAR_INIT(0);
static atomic_flag spinlock = ATOMIC_FLAG_INIT;

int atomic_worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 100000; i++) {
        /* 原子自增，内存序：顺序一致 */
        atomic_fetch_add_explicit(&shared_counter, 1, memory_order_seq_cst);
    }
    printf("Worker %d done\n", id);
    return 0;
}

/* 自旋锁 */
void spinlock_acquire(void) {
    while (atomic_flag_test_and_set(&spinlock)) {
        /* 自旋等待 */
    }
}

void spinlock_release(void) {
    atomic_flag_clear(&spinlock);
}

int spinlock_worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 10; i++) {
        spinlock_acquire();
        printf("Worker %d in critical section\n", id);
        spinlock_release();
    }
    return 0;
}

int main(void) {
    thrd_t t1, t2;
    int id1 = 1, id2 = 2;
    
    thrd_create(&t1, atomic_worker, &id1);
    thrd_create(&t2, atomic_worker, &id2);
    thrd_join(t1, NULL);
    thrd_join(t2, NULL);
    
    printf("Atomic counter: %d (expected 200000)\n", atomic_load(&shared_counter));
    
    /* 测试自旋锁 */
    printf("\n--- Spinlock demo ---\n");
    thrd_create(&t1, spinlock_worker, &id1);
    thrd_create(&t2, spinlock_worker, &id2);
    thrd_join(t1, NULL);
    thrd_join(t2, NULL);
    
    return 0;
}
