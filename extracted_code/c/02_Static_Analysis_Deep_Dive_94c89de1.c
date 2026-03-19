/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 576
 * Language: c
 * Block ID: 94c89de1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 抑制特定警告
// cppcheck-suppress unusedFunction
static void helper_function(void) {
    // 只在特定情况下使用
}

// 抑制多个警告
// cppcheck-suppress [uninitvar, nullPointer]
int risky_function(int *p) {
    return *p + 1;
}

// 抑制函数内的特定行
void example() {
    char buf[10];
    // cppcheck-suppress bufferAccessOutOfBounds
    strcpy(buf, "this is too long");
}
