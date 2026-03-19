/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 273
 * Language: c
 * Block ID: b8c82996
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内核缓存管理中的二级指针模式
// 用于在函数内修改外部指针

// 场景: 分配内存并初始化，需要返回指针
// 方法1: 返回指针（一级指针）
void *alloc_buffer(size_t size) {
    void *p = kmalloc(size, GFP_KERNEL);
    if (p)
        memset(p, 0, size);
    return p;
}

// 方法2: 二级指针参数（更灵活，可返回状态）
int alloc_buffer_pp(size_t size, void **out_ptr) {
    if (!out_ptr)
        return -EINVAL;  // 无效参数

    *out_ptr = kmalloc(size, GFP_KERNEL);
    if (!*out_ptr)
        return -ENOMEM;  // 内存不足

    memset(*out_ptr, 0, size);
    return 0;  // 成功
}

// 使用对比
void example(void) {
    // 方法1
    void *buf1 = alloc_buffer(100);
    if (buf1) {
        // 使用buf1
        kfree(buf1);
    }

    // 方法2 - 使用二级指针
    void *buf2 = NULL;
    int ret = alloc_buffer_pp(100, &buf2);
    if (ret == 0) {
        // 使用buf2
        kfree(buf2);
    }
}
