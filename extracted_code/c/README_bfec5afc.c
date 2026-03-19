/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 403
 * Language: c
 * Block ID: bfec5afc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译时断言
static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
static_assert(alignof(double) == 8, "double alignment check");

// C23 静态断言消息可选
static_assert(sizeof(void*) == 8);  // 64 位系统检查

// 预处理诊断
#if __STDC_VERSION__ < 201112L
    #error "C11 or later required"
#endif

// 编译器警告控制 (GCC/Clang)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    int unused = 42;
#pragma GCC diagnostic pop
