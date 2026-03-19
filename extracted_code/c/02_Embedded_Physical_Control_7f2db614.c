/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 511
 * Language: c
 * Block ID: 7f2db614
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 位图: 紧凑的状态/标志管理
// ============================================================

// 32个资源的占用状态 (1位表示一个资源)
static uint32_t resource_bitmap = 0;

// 分配一个资源 (返回0-31的索引，-1表示无可用)
int resource_alloc(void) {
    // __builtin_ffs: GCC内建函数，返回最低位1的位置 (1-32)，0表示无
    int bit = __builtin_ffs(~resource_bitmap) - 1;  // 找第一个0位
    if (bit < 0 || bit >= 32) return -1;

    resource_bitmap |= (1U << bit);
    return bit;
}

// 释放资源
void resource_free(int id) {
    if (id >= 0 && id < 32) {
        resource_bitmap &= ~(1U << id);
    }
}

// 检查资源是否占用
int resource_is_busy(int id) {
    return (resource_bitmap >> id) & 1U;
}

// 位图遍历所有占用资源
void resource_foreach_busy(void (*callback)(int id)) {
    uint32_t bm = resource_bitmap;
    while (bm) {
        int bit = __builtin_ffs(bm) - 1;  // 找最低位的1
        callback(bit);
        bm &= ~(1U << bit);  // 清除该位，继续遍历
    }
}
