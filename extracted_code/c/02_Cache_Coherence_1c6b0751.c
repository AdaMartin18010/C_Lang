/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 762
 * Language: c
 * Block ID: 1c6b0751
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 无内存屏障，编译器/CPU可能重排序
int data_ready = 0;
int data = 0;

void thread1(void) {
    data = 42;
    data_ready = 1;  // 可能在data赋值前执行!
}

void thread2(void) {
    while (!data_ready);  // 可能看到ready但data未更新!
    printf("%d\n", data);
}

// ✅ 正确: 使用内存屏障
void thread1_fixed(void) {
    data = 42;
    smp_wmb();  // 确保data写完成后再写ready
    WRITE_ONCE(data_ready, 1);
}

void thread2_fixed(void) {
    while (!READ_ONCE(data_ready));
    smp_rmb();  // 确保ready读取完成后再读data
    printf("%d\n", data);
}
