/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 413
 * Language: c
 * Block ID: 86685efe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 异常处理需要栈展开信息
// .eh_frame段包含DWARF CFI (Call Frame Information)

// 简化的栈回溯实现
#include <execinfo.h>

void print_stack_trace(void) {
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **strings = backtrace_symbols(buffer, nptrs);

    printf("Stack trace:\n");
    for (int i = 0; i < nptrs; i++) {
        printf("  %s\n", strings[i]);
    }

    free(strings);
}

void level3(void) { print_stack_trace(); }
void level2(void) { level3(); }
void level1(void) { level2(); }
