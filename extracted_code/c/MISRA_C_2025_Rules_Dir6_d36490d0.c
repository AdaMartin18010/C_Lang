/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 573
 * Language: c
 * Block ID: d36490d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* typedef使用规范 */

/* ✅ 合规 [待官方确认] */
typedef uint32_t size_type;
typedef struct Node Node_t;
typedef int (*CompareFunc)(const void *, const void *);

/* [C23相关] - typeof [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* C23 typeof - 谨慎使用 */
    typeof(int) my_int;  /* 等同于int */

    /* 复杂场景 */
    typeof(void (*)(int)) callback_type;  /* 函数指针类型 */

    /* ❌ 避免过度复杂 [待官方确认] */
    /* typeof(typeof(int (*)())[10]) too_complex; */
#else
    /* C11/C17 - 使用typedef */
    typedef int my_int;
#endif
