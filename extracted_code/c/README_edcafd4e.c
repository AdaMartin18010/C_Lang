/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\README.md
 * Line: 250
 * Language: c
 * Block ID: edcafd4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 直接风格
int factorial(int n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}

// CPS风格（适合指称语义分析）
typedef void (*Cont)(int);

void factorial_cps(int n, Cont k) {
    if (n == 0) {
        k(1);
    } else {
        factorial_cps(n - 1, ^(int result) {
            k(n * result);
        });
    }
}
