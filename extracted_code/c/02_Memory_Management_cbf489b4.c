/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2896
 * Language: c
 * Block ID: cbf489b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 调试配置: 最大安全性
 */
#ifdef DEBUG
    #define MALLOC(size) debug_malloc(size, __FILE__, __LINE__)
    #define FREE(ptr) debug_free(ptr, __FILE__, __LINE__)
    #define CALLOC(n, size) debug_calloc(n, size, __FILE__, __LINE__)
    #define REALLOC(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__)

    // 启用调试分配器功能
    #define MEM_DEBUG_FILL_PATTERN  // 填充特定模式检测未初始化
    #define MEM_DEBUG_GUARD_PAGES   // 保护页检测溢出
    #define MEM_DEBUG_TRACK_LEAKS   // 跟踪分配点
#else
    /*
     * 发布配置: 最大性能
     */
    #define MALLOC(size) malloc(size)
    #define FREE(ptr) free(ptr)
    #define CALLOC(n, size) calloc(n, size)
    #define REALLOC(ptr, size) realloc(ptr, size)
#endif
