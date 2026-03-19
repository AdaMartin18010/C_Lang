/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 631
 * Language: c
 * Block ID: d297f2a5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 大规模映射测试
void stress_test() {
    char *addrs[100];

    // 创建多个映射
    for (int i = 0; i < 100; i++) {
        addrs[i] = mmap(0, 4096, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addrs[i] == (char *)-1) {
            printf("mmap %d failed\n", i);
            exit(1);
        }

        // 写入触发实际分配
        addrs[i][0] = i;
    }

    // 验证
    for (int i = 0; i < 100; i++) {
        if (addrs[i][0] != i) {
            printf("verification failed at %d\n", i);
            exit(1);
        }
    }

    // 解除映射
    for (int i = 0; i < 100; i++) {
        munmap(addrs[i], 4096);
    }

    printf("stress test passed\n");
}
