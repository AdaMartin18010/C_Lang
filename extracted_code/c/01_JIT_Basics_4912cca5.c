/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 84
 * Language: c
 * Block ID: 4912cca5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// JIT编译器核心组件架构

typedef struct {
    // 代码缓存
    CodeCache *code_cache;

    // IR（中间表示）管理
    IRBuilder *ir_builder;

    // 后端代码生成
    CodeGenerator *code_gen;

    // 优化器
    Optimizer *optimizer;

    // 运行时支持
    Runtime *runtime;

    // 调试信息
    DebugInfo *debug_info;
} JITCompiler;

// 代码缓存管理
typedef struct {
    void *base_addr;           // 可执行内存基址
    size_t capacity;           // 总容量
    size_t used;               // 已使用

    // 代码段管理
    GHashTable *code_blocks;   // 地址 -> CodeBlock

    // GC支持
    GList *code_gc_roots;
} CodeCache;

// 代码块
typedef struct {
    void *start;               // 起始地址
    size_t size;               // 大小
    uint64_t exec_count;       // 执行计数（用于GC）
    bool is_marked;            // GC标记

    // 元数据
    char *function_name;
    int optimization_level;
    void *source_info;
} CodeBlock;
