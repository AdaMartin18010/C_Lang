/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 481
 * Language: c
 * Block ID: c062ebe2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* if语句使用规范 */

/* ❌ 违反 [待官方确认] - 常量条件 */
if (1) {  /* 始终为真 */
    /* ... */
}

/* ❌ 违反 [待官方确认] - 赋值误用 */
if (x = 5) {  /* 赋值而非比较 */
    /* ... */
}

/* ✅ 合规 [待官方确认] */
if (condition) {  /* 变量条件 */
    /* ... */
}

/* ✅ 合规 [待官方确认] - 明确比较 */
if (x == 5) {  /* 比较 */
    /* ... */
}

/* ✅ 合规 [待官方确认] - 使用预处理 */
#ifdef DEBUG
    debug_log("message");
#endif
