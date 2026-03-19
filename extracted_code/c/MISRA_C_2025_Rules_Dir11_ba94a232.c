/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 96
 * Language: c
 * Block ID: ba94a232
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 解引用前必须检查指针非空 */

/* ❌ 违反 [待官方确认] */
void process(int *p) {
    *p = 42;  /* 未检查p是否为NULL */
}

/* ✅ 合规 [C23相关] */
#if __STDC_VERSION__ >= 202311L
void process_safe(int *p) {
    if (p != nullptr) {  /* C23 nullptr */
        *p = 42;
    }
}
#else
void process_safe(int *p) {
    if (p != NULL) {  /* C11/C17 */
        *p = 42;
    }
}
#endif
