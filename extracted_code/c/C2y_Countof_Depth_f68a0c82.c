/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1568
 * Language: c
 * Block ID: f68a0c82
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 模式1：简单除法
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

// 模式2：带类型检查
#define ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof((a)[0])) / \
     (size_t)(!(sizeof(a) % sizeof((a)[0]))))

// 模式3：微软风格
#define _countof(a) (sizeof(a) / sizeof(*(a)))

// 模式4：GCC扩展
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
