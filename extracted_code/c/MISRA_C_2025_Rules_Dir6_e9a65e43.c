/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 605
 * Language: c
 * Block ID: e9a65e43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 确保类型大小正确 */

/* ✅ 合规 [待官方确认] - 编译时检查 */
#include <stdint.h>

static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
static_assert(sizeof(uintptr_t) >= sizeof(void *), "pointer size");

/* [C23相关] - 静态断言 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* static_assert在C23中是关键字 */
#else
    #define static_assert _Static_assert  /* C11 */
#endif
