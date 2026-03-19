/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 632
 * Language: c
 * Block ID: d2642319
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 锁竞争测试
void lock_contention_test(void)
{
    struct spinlock test_lock;
    initlock(&test_lock, "test");

    int shared_counter = 0;

    // 在多个CPU上并行运行
    for (int i = 0; i < NCPU; i++) {
        if (fork() == 0) {
            // 绑定到特定CPU
            cpu_bind(i);

            for (int j = 0; j < 1000000; j++) {
                acquire(&test_lock);
                shared_counter++;
                release(&test_lock);
            }

            exit(0);
        }
    }

    // 等待所有子进程
    for (int i = 0; i < NCPU; i++) {
        wait(0);
    }

    printf("Final counter: %d (expected: %d)\n",
           shared_counter, NCPU * 1000000);
}
