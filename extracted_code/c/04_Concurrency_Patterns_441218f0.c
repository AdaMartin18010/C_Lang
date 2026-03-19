/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 533
 * Language: c
 * Block ID: 441218f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简化版RCU思想
struct Data {
    int value;
};

_Atomic(struct Data*) global_data;

// 读者 (无锁)
int read_data() {
    struct Data *ptr = atomic_load(&global_data);
    return ptr->value;  // 安全读取
}

// 写者
void write_data(int new_val) {
    struct Data *old = atomic_load(&global_data);
    struct Data *new = malloc(sizeof(struct Data));
    *new = *old;          // 复制
    new->value = new_val; // 修改
    atomic_store(&global_data, new);  // 原子更新
    // 延迟释放old (需要等待读者退出)
}
