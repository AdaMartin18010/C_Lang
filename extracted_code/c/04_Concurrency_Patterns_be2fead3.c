/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 737
 * Language: c
 * Block ID: be2fead3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原子CAS伪代码
bool atomic_compare_exchange(_Atomic int *ptr,
                              int *expected,
                              int desired) {
    if (*ptr == *expected) {
        *ptr = desired;
        return true;
    } else {
        *expected = *ptr;
        return false;
    }
}

// 实际使用
_Atomic int value = 0;
int expected = 0;
while (!atomic_compare_exchange_weak(&value, &expected, 1)) {
    expected = 0;  // 重试
}
