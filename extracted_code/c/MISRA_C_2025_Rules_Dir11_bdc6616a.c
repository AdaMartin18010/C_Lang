/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 625
 * Language: c
 * Block ID: bdc6616a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 跳转语句限制 */

/* ❌ 违反 [待官方确认] - 向后goto */
start:
    if (condition) {
        goto start;  /* 向后跳转！ */
    }

/* ❌ 违反 [待官方确认] - setjmp/longjmp */
#include <setjmp.h>
jmp_buf env;

void func(void) {
    if (setjmp(env) == 0) {
        /* 首次进入 */
    } else {
        /* longjmp返回 - 复杂控制流 */
    }
}

/* ✅ 合规 [待官方确认] - 向前goto（资源清理） */
void process(void) {
    FILE *fp = fopen("file", "r");
    if (fp == NULL) {
        goto cleanup;  /* 向前跳转 */
    }
    /* 处理文件 */

cleanup:
    if (fp != NULL) {
        fclose(fp);
    }
    return;
}

/* ✅ 合规 [待官方确认] - 使用循环代替 */
while (condition) {
    if (retry_condition) {
        continue;  /* 向前到下一次迭代 */
    }
    /* 处理 */
}
