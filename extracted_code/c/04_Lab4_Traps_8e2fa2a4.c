/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 344
 * Language: c
 * Block ID: 8e2fa2a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/bttest.c (测试程序)
#include "kernel/types.h"
#include "user/user.h"

void f3() {
    backtrace();  // 应该看到f3, f2, f1, main的调用链
}

void f2() {
    f3();
}

void f1() {
    f2();
}

int main() {
    f1();
    exit(0);
}
