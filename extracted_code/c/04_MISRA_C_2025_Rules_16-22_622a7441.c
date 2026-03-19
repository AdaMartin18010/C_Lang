/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 63
 * Language: c
 * Block ID: 622a7441
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 */
int* ptr = malloc(sizeof(int) * n);
*ptr = 42;  /* 如果malloc失败，未定义行为 */

/* 符合规范 */
int* ptr = malloc(sizeof(int) * n);
if (ptr != NULL) {
    *ptr = 42;
    free(ptr);
} else {
    /* 内存分配失败处理 */
}
