/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\README.md
 * Line: 517
 * Language: c
 * Block ID: 47e00ba5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 无日志数据结构（Log-free Data Structures）
// 使用指针切换和版本号实现原子更新

struct versioned_node {
    uint64_t version;      // 版本号（奇数=正在修改，偶数=稳定）
    void *data;
    struct versioned_node *next;
};

// 读操作
void* read_node(struct versioned_node *node) {
    uint64_t ver;
    void *result;

    do {
        ver = node->version;
        // 内存屏障
        result = node->data;
    } while (ver != node->version || (ver & 1));  // 重试如果在修改中

    return result;
}

// 写操作（持久化）
void write_node_persist(PMEMobjpool *pop, struct versioned_node *node, void *new_data) {
    // 1. 标记正在修改（版本变为奇数）
    node->version |= 1;
    pmem_persist(&node->version, sizeof(node->version));

    // 2. 写入新数据
    node->data = new_data;
    pmem_persist(&node->data, sizeof(node->data));

    // 3. 递增版本号（变为偶数，标记完成）
    node->version++;
    pmem_persist(&node->version, sizeof(node->version));
}
