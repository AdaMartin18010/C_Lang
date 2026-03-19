/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 543
 * Language: c
 * Block ID: 943b236a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 枚举使用规范 */

/* ✅ 合规 [待官方确认] */
enum Status {
    STATUS_OK = 0,
    STATUS_ERROR,
    STATUS_WARNING
};

enum Status s = STATUS_OK;  /* 使用枚举类型 */

/* ❌ 违反 [待官方确认] - 使用整数 */
enum Status s = 0;  /* 应使用枚举常量 */

/* [C23相关] - 枚举基础类型 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* C23允许指定枚举基础类型 */
    /* enum Status : uint8_t { ... }; */  /* [待确认] */
#endif
