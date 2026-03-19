/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\README.md
 * Line: 165
 * Language: c
 * Block ID: 85957097
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* c_language_skills.c - C语言完整技能矩阵 */
#include "skill_matrix.h"

/* 语法维度技能 */
skill_item_t c_syntax_skills[] = {
    {
        .id = 101,
        .name = "基础语法",
        .description = "变量、数据类型、运算符、控制流",
        .dimension = SKILL_DIM_SYNTAX,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_EXPERT,
        .priority = 10,
        .learning_hours = 20,
        .is_prerequisite = true
    },
    {
        .id = 102,
        .name = "指针与内存",
        .description = "指针算术、多级指针、函数指针",
        .dimension = SKILL_DIM_SYNTAX,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_EXPERT,
        .priority = 10,
        .learning_hours = 40,
        .is_prerequisite = true,
        .prereq_ids = (uint32_t[]){101},
        .prereq_count = 1
    },
    {
        .id = 103,
        .name = "高级预处理",
        .description = "宏技巧、条件编译、X-Macros",
        .dimension = SKILL_DIM_SYNTAX,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 7,
        .learning_hours = 15,
        .prereq_ids = (uint32_t[]){101},
        .prereq_count = 1
    },
    {
        .id = 104,
        .name = "位操作与位域",
        .description = "位运算、位域布局、掩码操作",
        .dimension = SKILL_DIM_SYNTAX,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 8,
        .learning_hours = 10
    },
    {
        .id = 105,
        .name = "复杂声明解析",
        .description = "右左法则、复杂类型声明",
        .dimension = SKILL_DIM_SYNTAX,
        .current_level = SKILL_LEVEL_AWARENESS,
        .target_level = SKILL_LEVEL_INTERMEDIATE,
        .priority = 5,
        .learning_hours = 8
    }
};

/* 系统编程维度 */
skill_item_t c_system_skills[] = {
    {
        .id = 201,
        .name = "内存管理",
        .description = "malloc/free、内存对齐、内存池",
        .dimension = SKILL_DIM_SYSTEM,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_EXPERT,
        .priority = 10,
        .learning_hours = 30,
        .is_prerequisite = true,
        .prereq_ids = (uint32_t[]){102},
        .prereq_count = 1
    },
    {
        .id = 202,
        .name = "进程管理",
        .description = "fork/exec、进程通信、信号处理",
        .dimension = SKILL_DIM_SYSTEM,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 8,
        .learning_hours = 25
    },
    {
        .id = 203,
        .name = "线程与并发",
        .description = "pthreads、同步原语、线程池",
        .dimension = SKILL_DIM_SYSTEM,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 9,
        .learning_hours = 35
    },
    {
        .id = 204,
        .name = "文件I/O",
        .description = "POSIX I/O、内存映射、异步I/O",
        .dimension = SKILL_DIM_SYSTEM,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 8,
        .learning_hours = 20
    },
    {
        .id = 205,
        .name = "网络编程",
        .description = "socket、TCP/UDP、IO多路复用",
        .dimension = SKILL_DIM_SYSTEM,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_INTERMEDIATE,
        .priority = 7,
        .learning_hours = 30
    }
};

/* 工具链维度 */
skill_item_t c_tools_skills[] = {
    {
        .id = 301,
        .name = "GCC/Clang",
        .description = "编译选项、优化级别、诊断信息",
        .dimension = SKILL_DIM_TOOLS,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 8,
        .learning_hours = 15
    },
    {
        .id = 302,
        .name = "Makefile/CMake",
        .description = "构建系统、依赖管理、跨平台构建",
        .dimension = SKILL_DIM_TOOLS,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 8,
        .learning_hours = 20
    },
    {
        .id = 303,
        .name = "GDB调试",
        .description = "断点、栈追踪、内存检查",
        .dimension = SKILL_DIM_DEBUGGING,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 9,
        .learning_hours = 20
    },
    {
        .id = 304,
        .name = "静态分析",
        .description = "Coverity、Clang Static Analyzer",
        .dimension = SKILL_DIM_TOOLS,
        .current_level = SKILL_LEVEL_AWARENESS,
        .target_level = SKILL_LEVEL_INTERMEDIATE,
        .priority = 6,
        .learning_hours = 10
    }
};

/* 性能优化维度 */
skill_item_t c_optimization_skills[] = {
    {
        .id = 401,
        .name = "算法优化",
        .description = "时间/空间复杂度、数据结构选择",
        .dimension = SKILL_DIM_OPTIMIZATION,
        .current_level = SKILL_LEVEL_INTERMEDIATE,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 9,
        .learning_hours = 40
    },
    {
        .id = 402,
        .name = "编译器优化",
        .description = "循环展开、向量化、内联",
        .dimension = SKILL_DIM_OPTIMIZATION,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 7,
        .learning_hours = 25
    },
    {
        .id = 403,
        .name = "SIMD编程",
        .description = "SSE/AVX/NEON intrinsics",
        .dimension = SKILL_DIM_OPTIMIZATION,
        .current_level = SKILL_LEVEL_AWARENESS,
        .target_level = SKILL_LEVEL_INTERMEDIATE,
        .priority = 6,
        .learning_hours = 30
    },
    {
        .id = 404,
        .name = "缓存优化",
        .description = "缓存友好布局、预取、避免伪共享",
        .dimension = SKILL_DIM_OPTIMIZATION,
        .current_level = SKILL_LEVEL_BASIC,
        .target_level = SKILL_LEVEL_ADVANCED,
        .priority = 8,
        .learning_hours = 20
    }
};
