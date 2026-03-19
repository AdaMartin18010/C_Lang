/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 792
 * Language: c
 * Block ID: 7e0e231d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial11_init.c

// 使用未初始化变量
int risky_uninit() {
    int x;
    return x * 2;  // x未初始化!
}

// 安全初始化
int safe_init() {
    int x = 0;
    return x * 2;
}

// 条件初始化
int conditional_init(int flag) {
    int x;
    if (flag) {
        x = 10;
    } else {
        x = 20;
    }
    return x;  // Eva知道x已初始化
}
