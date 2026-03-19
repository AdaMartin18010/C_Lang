/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 741
 * Language: c
 * Block ID: d203e83e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：使用已释放内存（use-after-free）
void use_after_free_bug(void) {
    char *buf = malloc(100);
    strcpy(buf, "hello");
    free(buf);

    printf("%s\n", buf);  // ❌ 未定义行为！
}

// 检测方法：使用调试分配器
#ifdef DEBUG
void* debug_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size + 8);
    // 添加魔数/边界检测
    *(size_t*)((char*)ptr + size) = MAGIC;
    // 记录分配位置
    record_allocation(ptr, size, file, line);
    return ptr;
}

void debug_free(void *ptr) {
    // 检查魔数
    // 标记为释放但未真正释放
    mark_freed(ptr);
}
#endif
