/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 130
 * Language: c
 * Block ID: 330119ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 统一使用有符号 */
int a = -1;
int b = 1;
if (a < b) {  /* true */
    /* ... */
}

/* ✅ 合规 - 统一使用无符号 */
unsigned int x = 10;
unsigned int y = 20;
if (x >= y) {  /* 明确比较 */
    /* ... */
}

/* ✅ 合规 - 显式转换 */
int a = -1;
unsigned int b = 1;
if ((unsigned int)a < b) {  /* 显式 */
    /* ... */
}

/* ✅ 合规 - 使用size_t用于大小 */
size_t count = get_count();
for (size_t i = 0; i < count; i++) {
    /* ... */
}
