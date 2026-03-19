/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 119
 * Language: c
 * Block ID: e17b565f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 */
void func(void) {
    jmp_buf env;
    if (setjmp(env) == 0) {
        /* ... */
    }
}
/* 在另一个函数中使用env */
void other_func(jmp_buf* env) {
    longjmp(*env, 1);  /* 违反规则 - 跨越函数边界 */
}

/* 符合规范 - 在同一函数中使用 */
void func(void) {
    jmp_buf env;
    if (setjmp(env) == 0) {
        /* 在同一函数中调用longjmp */
        if (error_condition) {
            longjmp(env, 1);
        }
    } else {
        /* 错误恢复 */
    }
}
